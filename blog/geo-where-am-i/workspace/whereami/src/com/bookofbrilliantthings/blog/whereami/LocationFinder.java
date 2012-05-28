package com.bookofbrilliantthings.blog.whereami;

import java.io.PrintStream;

import com.mongodb.DBObject;

/**
 * Simple command-line application for testing "Where am I?" queries using MongoGeo.java
 * 
 * @author cwestin
 * https://github.com/cwestin/bobt/blob/master/blog/geo-where-am-i/workspace/whereami/src/com/bookofbrilliantthings/blog/whereami/DataLoader.java
 */
public class LocationFinder
{
	/**
	 * Issue a query to find out where the given location is, and print out the result.
	 * 
	 * @param printStream where to print the result to
	 * @param mongoGeo a handle to the geo database
	 * @param lat latitude of the location
	 * @param lng longitude of the location
	 */
	public static void testLocation(PrintStream printStream, MongoGeo mongoGeo,
			double lat, double lng)
	{
		DBObject dbObject = mongoGeo.findContaining(lat, lng);
		if (dbObject == null)
		{
			printStream.println("(" + lat + ", " + lng + ") not found");
			return;
		}
		
		// for this exercise, assume we've only got states to deal with
		final String stateName = (String)dbObject.get("stateName");
		final String country = (String)dbObject.get("country");
		printStream.println("(" + lat + ", " + lng + ") is in " + stateName + ", " + country);
	}
	
	public static void main(String argv[])
	{
		MongoGeo mongoGeo = new MongoGeo("localhost", 27017);
		
		// query for some points
		testLocation(System.out, mongoGeo, -82, 40); // somewhere in OH? TODO 
		testLocation(System.out, mongoGeo, -83, 41); // somewhere in OH? TODO 
		testLocation(System.out, mongoGeo, -84, 42); // somewhere in OH? TODO 
		testLocation(System.out, mongoGeo, -85, 43); // somewhere in OH? TODO 
		testLocation(System.out, mongoGeo, -86, 44); // somewhere in OH? TODO 
		testLocation(System.out, mongoGeo, -87, 45); // somewhere in OH? TODO 
		testLocation(System.out, mongoGeo, -88, 46); // somewhere in OH? TODO 
		testLocation(System.out, mongoGeo, -89, 47); // somewhere in OH? TODO 
		testLocation(System.out, mongoGeo, -90, 48); // somewhere in OH? TODO 
		
		// clean up
		mongoGeo.close();
	}	
}
