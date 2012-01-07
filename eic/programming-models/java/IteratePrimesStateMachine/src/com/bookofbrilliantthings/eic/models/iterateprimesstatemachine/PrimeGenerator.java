package com.bookofbrilliantthings.eic.models.iterateprimesstatemachine;

import java.lang.Iterable;
import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.Vector;
import java.util.concurrent.atomic.AtomicReference;

public class PrimeGenerator
	implements Iterable<Long>
{
	private static final AtomicReference<Vector<Long>> primesRef =
		new AtomicReference<Vector<Long>>(new Vector<Long>());
	static
	{
		final Vector<Long> primes = primesRef.get();
		primes.add(new Long(1));
		primes.add(new Long(2));
		primes.add(new Long(3));
	}

	private final int n;
	private final Iter iter;
	
	public PrimeGenerator(int n)
	{
		this.n = n;
		iter = new Iter();
	}
	
	private class Iter
		implements Iterator<Long>
	{
		private boolean calledHas; /* hasNext() has been called */
		private Vector<Long> primes; /* we'll hang on to the vector we start with */
		private int avail; /* how many elements are available from the vector */
		private int count; /* how many have been handed out so far */
		private long lastPrime; /* last prime handed out */
		private Vector<Long> original;
		
		Iter()
		{
			calledHas = false;
			primes = primesRef.get();
			avail = primes.size();
			count = 0;
			lastPrime = 0;
		}
		
		@Override
		public boolean hasNext()
		{
			calledHas = true;

			/* we'll return more if we haven't returned as many as requested yet */
			if (count < n)
				return true;
			
			/*
			  We've seen all that we're going to.  If we computed more primes than
			  were originally available in the vector, we should try to save that work.
			 */
			if (original != null)
			{
				while(!primesRef.compareAndSet(original, primes))
				{
					/*
					 If the compareAndSet failed, someone else got there first.  We'll
					 check and see what they put there.    If the vector there has more
					 primes than we've found, we'll quit.  But if we've extended the vector
					 more than whoever else did, then we'll try again.  Whoever did the most
					 work wins.
					 */
					original = primesRef.get();
					if (original.size() >= avail)
						break;
				}
				
				/* don't try this again if the user call hasNext() again */
				original = null;
			}

			return false;
		}
	
		@Override
		public Long next()
		{
			if (!calledHas)
				throw new IllegalStateException("hasNext() has not been called");
			
			if (count >= n)
				throw new NoSuchElementException("hasNext() has returned false");
			
			calledHas = false;
			
			/* the simple case:  we're still in the vector of precomputed values */
			if (count < avail)
				return (lastPrime = primes.get(count++).longValue());

			/*
			   If we get here, we're going to add more elements to the vector; clone
			   the shared one.
			 */
			original = primes;
			primes = new Vector<Long>(original);
			
			/* find the next prime */
			while(true)
			{
				/* find the next prime; they can't be even, so only test odd numbers */
				lastPrime += 2;
			
				/*
				  Test this candidate.  We do this by trying to divide it by all the primes
				  less than it's square root.  i starts at one so we don't test for
				  divisibility by one, which is at index zero.
				 */
				for(int i = 1; i < avail; ++i)
				{
					/* the candidate divisor, which is itself prime */
					long divisor = primes.get(i).longValue();
					
					/* if this divides evenly, it's not prime */
					if (lastPrime % divisor == 0)
						break;
					
					/* if we're past the square root, we found another prime */
					if (divisor * divisor > lastPrime)
					{
						primes.add(new Long(lastPrime));
						++avail;
						++count;
						return lastPrime;
					}
				}
			}
		}
	
		@Override
		public void remove()
		{
			/* you can't remove elements from this generated list */
			throw new UnsupportedOperationException("cannot remove elements from a generated list");
		}
	}

	@Override
	public Iterator<Long> iterator()
	{
		return iter;
	}
}
