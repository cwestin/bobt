package com.bookofbrilliantthings.blog.whereami;

import java.io.PrintStream;

import com.mongodb.DBObject;

/**
 * Simple command-line application for testing "Where am I?" queries using MongoGeo.java
 * 
 * @author cwestin
 * https://www.bookofbrilliantthings.com/blog/finding-your-location-with-geospatial-queries
 * https://github.com/cwestin/bobt/blob/master/blog/geo-where-am-i/workspace/whereami/src/com/bookofbrilliantthings/blog/whereami/DataLoader.java
 */
public class LocationFinder
{
	public static void printLocation(PrintStream printStream, double lng, double lat)
	{
		printStream.print("(" + lng + ", " + lat + ")");
	}
	
	/**
	 * Issue a query to find out where the given location is, and print out the result.
	 * 
	 * @param printStream where to print the result to
	 * @param mongoGeo a handle to the geo database
	 * @param lat latitude of the location
	 * @param lng longitude of the location
	 */
	public static void testLocation(PrintStream printStream, MongoGeo mongoGeo,
			double lng, double lat)
	{
		DBObject dbObject = mongoGeo.findContaining(lng, lat);
		if (dbObject == null)
		{
			printLocation(printStream, lng, lat);
			printStream.println(" not found");
			return;
		}
		
		// for this exercise, assume we've only got states to deal with
		final String stateName = (String)dbObject.get("stateName");
		final String country = (String)dbObject.get("country");
		printLocation(printStream, lng, lat);
		printStream.println(" is in " + stateName + ", " + country);
	}
	
	public static void main(String argv[])
	{
		MongoGeo mongoGeo = new MongoGeo("localhost", 27017);
		
		// query for some points; note Google Maps uses lat,lng, hence reversal here
		// https://maps.google.com/maps?q=43.707594,-114.433594&num=1&t=m&z=4
		testLocation(System.out, mongoGeo, -114.433594, 43.707594); // ID
		
		// https://maps.google.com/maps?q=40.713956,-83.583984&num=1&t=m&z=4
		testLocation(System.out, mongoGeo, -83.583984, 40.713956); // OH

		// https://maps.google.com/maps?q=31.353637,-92.548828&num=1&t=m&z=4
		testLocation(System.out, mongoGeo, -92.548828, 31.353637); // LA
		
		// https://maps.google.com/maps?q=25.482951,-90.087891&num=1&t=m&z=4
		testLocation(System.out, mongoGeo, -90.087891, 25.482951); // middle of gulf of mexico
		
		// clean up
		mongoGeo.close();
	}	
}
