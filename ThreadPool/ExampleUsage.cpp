/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Adamitskiy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "ThreadPool.h"
#include <chrono>

/*
 * Example class that will just determine number of prime numbers between 1 and n number.
 */
class ExampleUsage : public WorkerThread
{
public:

	int m_iRadix;
	int m_iPrimesFound;

	ExampleUsage(int InRadix)
	{
		m_iRadix = InRadix;
		m_iPrimesFound = 0;
	}

	void ExecuteWork()
	{
		// Quick function to find the number of prime numbers between 1 and m_iRadix to show thread execution.
		for (int i = 2; i < m_iRadix; ++i)
		{
			bool bIsPrime = true;

			for (int j = 2; j < (int)(m_iRadix / 2); ++j)
			{
				// If the divisor has grown to the size of an int.
				if (j >= i)
				{
					break;
				}

				// If it can be divided, it must not be a prime.
				if (i % j == 0)
				{
					bIsPrime = false;
					break;
				}
			}

			if (bIsPrime == true)
			{
				m_iPrimesFound++;
			}
		}
	}

	void OnCompleteCallback()
	{
		printf("The thread pool found %d primes between 1 and %d\n", m_iPrimesFound, m_iRadix);
	}
};

int main(int, const char*[])
{
	/*
	 * The thread pool will run work as it gets posted. We'll observe this in a simplistic way. 
	 */
	printf("Let's test how much faster running a thread pool is when compared to not having one at all. We will find the number of prime numbers between 1 and the following 4 numbers: %d, %d, %d, %d.\n\n", 13455, 13999, 13872, 13717);
	printf("We will first run our tasks asynchronously with the thread pool:\n\n");

	auto timerOnThreadPoolStart = std::chrono::steady_clock::now();

	ExampleUsage* example1 = new ExampleUsage(13455);
	ThreadPool::GetInstance()->PostWork(example1);

	ExampleUsage* example2 = new ExampleUsage(13999);
	ThreadPool::GetInstance()->PostWork(example2);

	ExampleUsage* example3 = new ExampleUsage(13872);
	ThreadPool::GetInstance()->PostWork(example3);

	ExampleUsage* example4 = new ExampleUsage(13717);
	ThreadPool::GetInstance()->PostWork(example4);

	ThreadPool::GetInstance()->Shutdown();

	// Clean up the dynamic memory.
	delete example1;
	delete example2;
	delete example3;
	delete example4;

	auto timerOnThreadPoolEnd = std::chrono::steady_clock::now();

	printf("\nDepending on which threads execution completes first, the outputing of resulting data may be out of order which is as expected.");
	printf("\n\nNow let's run the same test for the same numbers without multithreading.\n\n");
	
	auto timerOnNoThreadsStart = std::chrono::steady_clock::now();

	ExampleUsage* example5 = new ExampleUsage(13455);
	example5->ExecuteWork();
	example5->OnCompleteCallback();

	ExampleUsage* example6 = new ExampleUsage(13999);
	example6->ExecuteWork();
	example6->OnCompleteCallback();

	ExampleUsage* example7 = new ExampleUsage(13872);
	example7->ExecuteWork();
	example7->OnCompleteCallback();

	ExampleUsage* example8 = new ExampleUsage(13717);
	example8->ExecuteWork();
	example8->OnCompleteCallback();

	// Clean up the dynamic memory.
	delete example5;
	delete example6;
	delete example7;
	delete example8;

	auto timerOnNoThreadsEnd = std::chrono::steady_clock::now();

	auto threadsDiff = timerOnThreadPoolEnd - timerOnThreadPoolStart;
	printf("\n\nExecution time using thread pool: %f ms\n", std::chrono::duration<double, std::milli>(threadsDiff).count());

	auto noThreadsDiff = timerOnNoThreadsEnd - timerOnNoThreadsStart;
	printf("Execution time using no thread pool: %f ms\n\n", std::chrono::duration<double, std::milli>(noThreadsDiff).count());

	system("PAUSE");
	return 0;
}
