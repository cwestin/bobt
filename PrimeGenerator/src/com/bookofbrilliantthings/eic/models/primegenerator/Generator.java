package com.bookofbrilliantthings.eic.models.primegenerator;

import java.util.Iterator;

public abstract class Generator<T>
	implements Iterable<T>
{
	protected abstract void generate();
	
	protected void put(T t)
	{
		
	}
	
	@Override
	public Iterator<T> iterator()
	{
		// TODO Auto-generated method stub
		return null;
	}
}
