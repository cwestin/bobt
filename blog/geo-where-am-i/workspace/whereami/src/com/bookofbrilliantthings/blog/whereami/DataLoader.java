package com.bookofbrilliantthings.blog.whereami;

import java.io.File;
import java.io.PrintStream;
import java.io.Serializable;
import java.util.HashMap;

import org.geotools.data.DataStore;
import org.geotools.data.DataStoreFinder;
import org.geotools.data.simple.SimpleFeatureCollection;
import org.geotools.data.simple.SimpleFeatureIterator;
import org.geotools.data.simple.SimpleFeatureSource;
import org.opengis.feature.simple.SimpleFeature;
import org.opengis.feature.simple.SimpleFeatureType;
import org.opengis.feature.type.AttributeDescriptor;
import org.opengis.feature.type.AttributeType;

import com.mongodb.BasicDBList;
import com.mongodb.BasicDBObject;

import com.vividsolutions.jts.geom.Coordinate;
import com.vividsolutions.jts.geom.MultiPolygon;
import com.vividsolutions.jts.geom.Point;

public class DataLoader
{
	static void printSchema(PrintStream printStream, SimpleFeatureSource featureSource)
	{
		final SimpleFeatureType featureType = featureSource.getSchema();
		printStream.println("FID\t");
		for (int i = 0; i < featureType.getAttributeCount(); i++)
		{
			final AttributeDescriptor attrDesc = featureType.getDescriptor(i);
			final String attrName = attrDesc.getLocalName();
			final AttributeType attrType = attrDesc.getType();
			printStream.println("attrName == " + attrName);
			printStream.println("attrType name " + attrType.getName());
		}
		printStream.println();
	}
	
	public static void main(String argv[])
	{
		/*
		 * Initial code taken from
		 * http://stackoverflow.com/questions/2044876/does-anyone-know-of-a-library-in-java-that-can-parse-esri-shapefiles
		 */
		File file = new File(Const.SHAPEFILE_NAME);
		assert file.exists();
		assert file.canRead();

		try
		{
			/*
			 * Attempt to find a GeoTools DataStore that can handle the shapefile
			 * Taken from demo/FirstProject.java
			 */
			HashMap<String, Serializable> connectParameters = new HashMap<String, Serializable>();
			connectParameters.put("url", file.toURI().toURL());
			DataStore dataStore = DataStoreFinder.getDataStore(connectParameters);	
			assert dataStore != null;
			final String[] typeNames = dataStore.getTypeNames();

			for(String typeName : typeNames)
				System.out.println("typeName " + typeName);
			System.out.println();

			final String typeName0 = typeNames[0];
			System.out.println("Reading content " + typeName0);
		
			final SimpleFeatureSource featureSource = dataStore.getFeatureSource(typeName0);

			// print out the list of features so we can see what's there
			printSchema(System.out, featureSource);

			final SimpleFeatureCollection collection = featureSource.getFeatures();
			final SimpleFeatureIterator iterator = collection.features();

			// set up our connection to MongoDB
			MongoGeo mongoGeo = new MongoGeo(Const.MONGODB_HOST, Const.MONGODB_PORT);
			
			// create the geospatial index
			mongoGeo.createGeoIndex();

			try
			{
				while(iterator.hasNext())
				{
					SimpleFeature feature = iterator.next();
					
					/*
					 * From previously printing out the schema, I've seen what look like the
					 * interesting attributes for each feature, and am going to ask for those.
					 */
					final String stateName = (String)feature.getAttribute("STATE_NAME");
					final String stateAbbr = (String)feature.getAttribute("STATE_ABBR");
					final MultiPolygon multiPolygon = (MultiPolygon)feature.getAttribute("the_geom");
					final int nPoints = multiPolygon.getNumPoints();
					final Point centroid = multiPolygon.getCentroid();
					System.out.println("STATE_NAME " + stateName + ", STATE_ABBR == " + stateAbbr +
							", nPoints == " + nPoints +
							", centroid == (" + centroid.getX() + ", " + centroid.getY() + ")");
					
					final Coordinate coordinates[] = multiPolygon.getCoordinates();
					assert coordinates.length == nPoints;
					
					// prepare a database object for insertion into MongoDB
					final BasicDBObject boundObj = new BasicDBObject();
					boundObj.put("stateAbbr", stateAbbr);
					boundObj.put("stateName", stateName);
					boundObj.put("country", "USA");

					final BasicDBList dbCentroid = new BasicDBList();
					dbCentroid.add(centroid.getX());
					dbCentroid.add(centroid.getY());

					boundObj.put("centroid", dbCentroid);
					
					final BasicDBList dbPoly = new BasicDBList();
					for(Coordinate coordinate : coordinates)
					{
						final BasicDBList dbPoint = new BasicDBList();
						dbPoint.add(coordinate.x);
						dbPoint.add(coordinate.y);

						dbPoly.add(dbPoint);
					}
					
					boundObj.put("poly", dbPoly);
					
					// insert the object
					mongoGeo.insertRegion(boundObj);
				}
			}
			finally
			{
				iterator.close();
				mongoGeo.close();
			}
		}
		catch(Throwable e)
		{
			throw new RuntimeException(e);
		}
	}
}
