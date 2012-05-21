package com.bookofbrilliantthings.blog.whereami;

import java.io.File;
import java.io.Serializable;
import java.util.HashMap;

import org.geotools.data.DataStore;
import org.geotools.data.DataStoreFinder;
import org.geotools.data.FeatureSource;
import org.geotools.data.FileDataStore;
import org.geotools.data.FileDataStoreFinder;
import org.geotools.data.simple.SimpleFeatureCollection;
import org.geotools.data.simple.SimpleFeatureIterator;
import org.geotools.data.simple.SimpleFeatureSource;
import org.geotools.feature.FeatureCollection;
import org.geotools.feature.FeatureIterator;
import org.opengis.feature.Feature;
import org.opengis.feature.GeometryAttribute;
import org.opengis.feature.simple.SimpleFeature;
import org.opengis.geometry.Geometry;

public class DataLoader
{
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
			String[] typeNames = dataStore.getTypeNames();
			String typeName = typeNames[0];
		
			System.out.println("Reading content " + typeName);
		
			SimpleFeatureSource featureSource = dataStore.getFeatureSource(typeName);
			SimpleFeatureCollection collection = featureSource.getFeatures();
			SimpleFeatureIterator iterator = collection.features();
		
		
			try
			{
				while(iterator.hasNext())
				{
					SimpleFeature feature = iterator.next();
					//Geometry sourceGeometry = (Geometry)feature.getDefaultGeometry();
					System.out.println(feature.getID());
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
