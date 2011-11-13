package com.bookofbrilliantthings.totg.itoa;

import java.io.IOException;
import java.util.Random;

public class Standard
{
	final static int N = 10000000;
	
	public static void main(String argv[])
	{
		Random rng = new Random(0xf0f0f0f0);
		NullWriter nullWriter = new NullWriter();
		
		try
		{
			long startTime = System.nanoTime();
			
			for(int i = 0; i < N; ++i)
			{
				int r = rng.nextInt();
				nullWriter.write(r + "\n");
			}
			
			long elapsedTime = System.nanoTime() - startTime;
			System.out.println("elapsed time in ns " + elapsedTime);
		}
		catch(IOException ioe)
		{
			System.out.println("caught " + ioe);
		}
	}
}
