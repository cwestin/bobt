package com.bookofbrilliantthings.eic.models.printprimes;

public class LongPrinter
	implements LongSink
{
	@Override
	public void put(long i)
	{
		System.out.print(i);
		System.out.println();
	}
}
