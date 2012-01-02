package com.bookofbrilliantthings.eic.models.printprimes;

public class Main
{
	public static void main(String argv[])
	{
		LongPrinter longPrinter = new LongPrinter();
		PrimeGenerator.generate(longPrinter, 20);
	}
}
