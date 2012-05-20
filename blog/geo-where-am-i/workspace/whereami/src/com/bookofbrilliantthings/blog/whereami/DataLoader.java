package com.bookofbrilliantthings.blog.whereami;

import java.io.File;
import java.util.HashMap;

import org.geotools.data.FeatureSource;
import org.geotools.data.FileDataStore;
import org.geotools.data.FileDataStoreFinder;
import org.geotools.feature.FeatureCollection;
import org.geotools.feature.FeatureIterator;
import org.opengis.feature.Feature;

public class DataLoader
{
	public static void Main(String argv[])
	{
		/*
		 * Initial code taken from
		 * http://stackoverflow.com/questions/2044876/does-anyone-know-of-a-library-in-java-that-can-parse-esri-shapefiles
		 */
		File file = new File("C:/extracted/state_bounds/state_bounds.shp");

		try
		{
			FileDataStore fileDataStore = FileDataStoreFinder.getDataStore(file);
			String[] typeNames = fileDataStore.getTypeNames();
			String typeName = typeNames[0];
		
			System.out.println("Reading content " + typeName);
		
			FeatureSource featureSource = fileDataStore.getFeatureSource(typeName);
			FeatureCollection collection = featureSource.getFeatures();
			FeatureIterator iterator = collection.features();
		
		
			try
			{
				while(iterator.hasNext())
				{
					Feature feature = iterator.next();
					Geometry sourceGeometry = feature.getDefaultGeometry();
				}
			}
			finally
			{
				iterator.close();
			}
		}
		catch(Throwable e)
		{
			throw new RuntimeException(e);
		}
	}
}
