package com.bookofbrilliantthings.eic.models.printprimes;

import java.util.Vector;
import java.util.concurrent.atomic.AtomicReference;

public class PrimeGenerator
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
	
	public static void generate(LongSink longSink, int n)
	{
		Vector<Long> primes = primesRef.get();
		int avail = primes.size();
		int count = 0;
		long lastPrime = 0;
		for(; count < n; ++count)
		{
			/* if we're past all the available precomputed values, quit */
			if (count >= avail)
				break;
			
			lastPrime = primes.get(count).longValue();
			longSink.put(lastPrime);
		}
		
		/* if we've seen everything we wanted to, quit */
		if (count == n)
			return;
		
		/* since we're going to be adding new elements to the shared Vector<>, clone it */
		Vector<Long> original = primes;
		primes = new Vector<Long>(original);
		
		/* find and emit candidates until we're done */
		while(count < n)
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
					longSink.put(lastPrime);
					++count;
					break;
				}
			}
		}
		
		/* before we quit, replace the vector if appropriate */
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
				return;
		}
	}
}
