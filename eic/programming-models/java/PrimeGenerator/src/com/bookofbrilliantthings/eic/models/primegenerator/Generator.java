/* Copyright 2011 Chris Westin.  All rights reserved. */
/*
  Example for discussion from "Programming Models," from
  "The Book of Brilliant Things:"
  https://www.bookofbrilliantthings.com/eic/programming-models
 */

package com.bookofbrilliantthings.eic.models.primegenerator;

import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.concurrent.SynchronousQueue;

public abstract class Generator<T>
	implements Iterable<T>
{
	private SynchronousQueue<T> synchronousQueue;
	private QueueSink queueSink;
	private Iterator<T> iter;
	
	private class QueueSink
		implements Sink<T>
	{
		@Override
		public void put(T t)
		{
			try
			{
				synchronousQueue.put(t);
			}
			catch(InterruptedException ie)
			{
				/*
				  We can't get here.  The InterruptedException can only occur if someone
				  fires a Notify at this Thread, and that's not possible, because it
				  never escapes the Generator class, and the Generator class itself never
				  does that.
				 */
				throw new IllegalStateException("internal error", ie);
			}
		}
	}
	
	protected abstract void generate(Sink<T> sink);
	
	private class GeneratorThread
		extends Thread
	{
		@Override
		public void run()
		{
			generate(queueSink);
		}
	}
	
	private void commonInit()
	{
		synchronousQueue = new SynchronousQueue<T>();
		queueSink = new QueueSink();

		/*
		  Start generating values.  The use of the SynchronousQueue will cause this
		  to block until the Iterator is used to pull values out.
		 */
		Thread generator = new GeneratorThread();
		generator.setDaemon(true);
		generator.start();
	}
	
	public Generator()
	{
		iter = new Unbounded();
		commonInit();
	}
	
	public Generator(int n)
	{
		iter = new Bounded(n);
		commonInit();
	}
	
	private class Unbounded
		implements Iterator<T>
	{
		protected boolean hasCalled;
		
		Unbounded()
		{
			hasCalled = false;
		}
		
		@Override
		public boolean hasNext()
		{
			hasCalled = true;
			return true;
		}

		@Override
		public T next()
		{
			if (!hasCalled)
				throw new IllegalStateException("hasNext() has not been called");
			
			hasCalled = false;

			try
			{
				return synchronousQueue.take();
			}
			catch(InterruptedException ie)
			{
				/*
				  We can't get here.  The InterruptedException can only occur if someone
				  fires a Notify at this Thread, and that's not possible, because it
				  never escapes the Generator class, and the Generator class itself never
				  does that.
				 */
				throw new IllegalStateException("internal error", ie);
			}
		}

		@Override
		public void remove()
		{
			throw new UnsupportedOperationException("cannot remove elements from a generated list");
		}
	}
	
	private class Bounded
		extends Unbounded
	{
		private int n;
		private int count;
		
		Bounded(int n)
		{
			this.n = n;
			count = 0;
		}
		
		@Override
		public boolean hasNext()
		{
			hasCalled = true;
			
			if (count < n)
				return true;
			
			return false;
		}

		@Override
		public T next()
		{
			if (count >= n)
				throw new NoSuchElementException("hasNext() has returned false");
			
			T rv = super.next();
			++count;
			hasCalled = false;
			return rv;
		}
	}
	
	@Override
	public Iterator<T> iterator()
	{
		return iter;
	}
}
