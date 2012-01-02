package com.bookofbrilliantthings.eic.models.primegenerator;

import java.util.Vector;

public class PrimeGenerator
	extends Generator<Long>
{
	private static final Vector<Long> primes = new Vector<Long>();
	static
	{
		primes.add(new Long(1));
		primes.add(new Long(2));
		primes.add(new Long(3));
	}
	
	public PrimeGenerator(int n)
	{
		super(n);
	}
	
	@Override
	protected void generate(Sink<Long> longSink)
	{
		int avail = 0;
		synchronized(primes)
		{
			avail = primes.size();
		}
		int count = 0;
		long lastPrime = 0;
		
		/* find and emit candidates until we're done */
		while(true)
		{
			/*
			  If the number of primes we have returned so far is less than the number
			  of pre-computed values available, use the next available value.
			 */
			if (count < avail)
			{
				lastPrime = primes.get(count).longValue();
				longSink.put(lastPrime);
				++count;
			}
			else
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
						/*
						   We've found a new prime, so add it to the shared vector of
						   pre-computed values.
						   
						   One or more other threads may have gotten there first, and may have
						   added other values already.  In that case, just take note of the
						   new count of available values; we'll revert to using the vector
						   at the top of this loop. 
						 */
						synchronized(primes)
						{
							int newAvail = primes.size();
							if (newAvail > avail)
								avail = newAvail;
							else
							{
								/* newAvail was the same, so we get to add the next value */
								primes.add(new Long(lastPrime));
								++avail;
							}
						}

						longSink.put(lastPrime);
						++count;
						break;
					}
				}
			}
		}
	}
}
