package com.bookofbrilliantthings.blog.whereami;

/**
 * Shared constants for the DataLoader and LocationFinder applications.
 * 
 * @author cwestin
 * https://github.com/cwestin/bobt/blob/master/blog/geo-where-am-i/workspace/whereami/src/com/bookofbrilliantthings/blog/whereami/Const.java
 */
public class Const
{
	// file downloaded from USGS - doesn't include boundaries for coastal states
	//final static String SHAPEFILE_NAME = "C:/extracted/state_bounds/state_bounds.shp";

	// sample data file from the geotools library download
	final static String SHAPEFILE_NAME = "C:/extracted/geotools-project/geotools-2.7.4/" +
		"modules/library/sample-data/src/main/resources/org/geotools/test-data/shapes/statepop.shp";

	// connecting to MongoDB
	final static String MONGODB_HOST = "localhost";
	final static int MONGODB_PORT = 27017;
}
