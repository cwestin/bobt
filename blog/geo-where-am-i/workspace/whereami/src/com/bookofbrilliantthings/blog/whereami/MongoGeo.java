package com.bookofbrilliantthings.blog.whereami;

import java.util.Iterator;

import com.mongodb.BasicDBList;
import com.mongodb.BasicDBObject;
import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.DBObject;
import com.mongodb.Mongo;
import com.mongodb.ReadPreference;
import com.mongodb.WriteConcern;

class MongoGeo
{
	final public static String CENTROID = "centroid";
	final public static String POLY = "poly";
	
	final private static String DBNAME = "geobox";
	final private static String COLLECTIONNAME = "bounds";

	private Mongo mongo;
	private DB geoDb;
	private DBCollection boundsCollection;
	
	public MongoGeo(String hostName, int port)
	{
		// connect to MongoDB
		try
		{
			mongo = new Mongo(hostName, port);
			WriteConcern writeConcern = new WriteConcern(1, 2000);
			mongo.setWriteConcern(writeConcern);
		}
		catch(Exception e)
		{
			throw new RuntimeException(e);
		}

		// get our hands on the bounds collection
		geoDb = mongo.getDB(DBNAME);
		geoDb.setReadPreference(ReadPreference.SECONDARY);
		boundsCollection = geoDb.getCollection(COLLECTIONNAME);
	}
	
	public void close()
	{
		boundsCollection = null;
		geoDb = null;
		mongo.close();
		mongo = null;
	}

	public void createGeoIndex()
	{
		// create the geospatial index we need
		final BasicDBObject boundsIndex = new BasicDBObject();
		boundsIndex.put("centroid", "2d");
		boundsCollection.createIndex(boundsIndex);
	}

	public void insertRegion(DBObject dbObject)
	{
		/*
		 * Validate: the only things that have to exist for this to work are the centroid
		 * and the poly.
		 * 
		 * We don't check anything else, because the user might vary it based on
		 * countries, regions, cantons, city limits, etc, etc.
		 */
		final Object objCentroid = dbObject.get(CENTROID);
		if (!(objCentroid instanceof BasicDBList))
			throw new IllegalArgumentException("the \"" + CENTROID + "\" must be a list");
		final BasicDBList listCentroid = (BasicDBList)objCentroid;
		if (listCentroid.size() != 2)
			throw new IllegalArgumentException("the \"" + CENTROID + "\" must have two elements");
		
		Object objPoly = dbObject.get(POLY);
		if (!(objPoly instanceof BasicDBList))
			throw new IllegalArgumentException("the \"" + POLY + "\" must be a list of points");

		// insert the bounds into the collection
		boundsCollection.insert(dbObject);
	}

	private static boolean pointInPoly(double x, double y, BasicDBList polyList)
	{
		/*
		 * This is easiest to do if we have the polygon's vertices in native arrays, so
		 * we'll get them out first
		 */
		final int nPoint = polyList.size();
		final double polyX[] = new double[nPoint];
		final double polyY[] = new double[nPoint];
		final Iterator<Object> pointIterator = polyList.iterator();
		int pointIndex = 0;
		while(pointIterator.hasNext())
		{
			final BasicDBList point = (BasicDBList)pointIterator.next();
			polyX[pointIndex] = (Double)point.get(0);
			polyY[pointIndex] = (Double)point.get(1);
			
			++pointIndex;
		}

		/*
		 * Now we can easily apply the test to see if the point is in the polygon.  We'll use
		 * the algorithm by Lascha Lagidse found here http://alienryderflex.com/polygon/ .
		 */
		int j = nPoint - 1;
		boolean oddNodes = false;
		for(int i = 0; i < nPoint; j = i, ++i)
		{
			if ((((polyY[i] < y) && (polyY[j] >= y)) || ((polyY[j] < y) && (polyY[i] >= y)))
					&& ((polyX[i] <= x) || (polyX[j] <= x)))
			{
				oddNodes ^= (polyX[i] + (y - polyY[i])/(polyY[j] - polyY[i])*(polyX[j] - polyX[i]) < x);
			}
		}
		
		return oddNodes;
	}
	
	public DBObject findContaining(double lat, double lng)
	{
		// these are cleared by close()
		assert boundsCollection != null;
		assert geoDb != null;
		
		// construct the geoNear command
		final BasicDBObject geoNearCmd = new BasicDBObject();
		geoNearCmd.put("geoNear", COLLECTIONNAME); // geoNear on COLLECTIONNAME
		
		final BasicDBList nearPoint = new BasicDBList();
		nearPoint.add(lat);
		nearPoint.add(lng);
		
		geoNearCmd.put("near", nearPoint);
		geoNearCmd.put("num", 10); // max number to return
		
		final DBObject cmdResult = geoDb.command(geoNearCmd);
		final BasicDBList resultsList = (BasicDBList)cmdResult.get("results");
		final Iterator<Object> resultIterator = resultsList.iterator();
		while(resultIterator.hasNext())
		{
			// reach in and get the polygon
			final DBObject result = (DBObject)resultIterator.next();
			final DBObject resultObj = (DBObject)result.get("obj");
			final BasicDBList polyList = (BasicDBList)resultObj.get("poly");
			
			// test to see if the point is inside the poly
			if (pointInPoly(lat, lng, polyList))
				return resultObj;
		}

		// if we got here, we didn't find any containing polygons
		return null;
	}
}