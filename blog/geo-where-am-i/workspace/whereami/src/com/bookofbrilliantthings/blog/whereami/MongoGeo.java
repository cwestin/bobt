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

/**
 * MongoGeo establishes and maintains database access handles for a simple
 * geospatial database that can be used to find regions that contain specified points.
 * MongoDB is used for persistence.
 * 
 * The query mechanism requires the use of a property named MongoGeo.POLY to hold
 * a list of points represented as an array of two element arrays representing
 * latitude and longitude of a polygon, and MongoGeo.CENTROID to hold a two element array
 * that is the he centroid of the polygon.  (This is currently required to be pre-computed,
 * but a future enhancement could be to do this if it is not supplied.)
 * 
 * There are methods for loading the database, as well as methods for querying it.
 * 
 * @author cwestin
 * https://www.bookofbrilliantthings.com/blog/finding-your-location-with-geospatial-queries
 * https://github.com/cwestin/bobt/blob/master/blog/geo-where-am-i/workspace/whereami/src/com/bookofbrilliantthings/blog/whereami/MongoGeo.java
 */
class MongoGeo
{
	// constants for the database schema
	final public static String CENTROID = "centroid";
	final public static String POLY = "poly";
	
	// constants for the db and collection names
	final private static String DBNAME = "geobox";
	final private static String COLLECTIONNAME = "bounds";

	// max number of polygons to request from query and check
	final private static int N_TO_CHECK = 10;
	
	// handles to the database
	private Mongo mongo;
	private DB geoDb;
	private DBCollection boundsCollection;
	
	/**
	 * Set up the connection to MongoDB
	 * 
	 * @param hostName the host to use
	 * @param port the port to use
	 */
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
	
	/**
	 * Close the connection to MongoDB. It is an error to issue any more requests
	 * to this class after calling this.
	 */
	public void close()
	{
		boundsCollection = null;
		geoDb = null;
		mongo.close();
		mongo = null;
	}

	/**
	 * Create the geospatial index required for future queries. Should only be
	 * used once.
	 */
	public void createGeoIndex()
	{
		// create the geospatial index we need
		final BasicDBObject boundsIndex = new BasicDBObject();
		boundsIndex.put("centroid", "2d");
		boundsCollection.createIndex(boundsIndex);
	}

	/**
	 * Insert a polygonally bounded region into the geo database.
	 * 
	 * This does some lightweight checking, making sure that the object has
	 * a CENTROID field made up of a two-item list (a point), and a POLY field
	 * made up of a list (should be points).  Other than that, the application is
	 * free to use any other fields for any other purpose.
	 * 
	 * @param dbObject the database object to insert
	 */
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

	/**
	 * Test to see if the specified point is contained by the given polygon.
	 * 
	 * The polygon is represented as a list of points in the POLY field.
	 * 
	 * @param x the x co-ordinate of the point
	 * @param y the y co-ordinate of the point
	 * @param polyList the polygon
	 * @return true if the point is within the polygon, false otherwise; if the point is
	 *   on the boundary, may return either
	 */
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
	
	/**
	 * Query the geo database to find a polygon containing the specified point.
	 * 
	 * Assumes the polygons don't overlap.  Queries for a small set of polygons whose
	 * centroids are nearest the given point, ordered by distance.  Then tests each of
	 * those to see if the given point is within them.  The tests stop at the first
	 * containing polygon, and its original containing database object is returned.
	 * 
	 * @param lng the longitude of the point
	 * @param lat the latitude of the point
	 * @return the database object with the containing polygon, if one is found, null otherwise
	 */
	public DBObject findContaining(double lng, double lat)
	{
		// these are cleared by close()
		assert boundsCollection != null;
		assert geoDb != null;
		
		// construct the geoNear command
		final BasicDBObject geoNearCmd = new BasicDBObject();
		geoNearCmd.put("geoNear", COLLECTIONNAME); // geoNear on COLLECTIONNAME
		
		final BasicDBList nearPoint = new BasicDBList();
		nearPoint.add(lng);
		nearPoint.add(lat);
		
		geoNearCmd.put("near", nearPoint);
		geoNearCmd.put("num", N_TO_CHECK); // max number to return
		
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
			if (pointInPoly(lng, lat, polyList))
				return resultObj;
		}

		// if we got here, we didn't find any containing polygons
		return null;
	}
}
