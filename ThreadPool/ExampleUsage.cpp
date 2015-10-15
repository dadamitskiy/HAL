
#include "ThreadPool.h"
#include <iostream>
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
		std::cout << "The thread pool found " << m_iPrimesFound << " primes between 1 and " << m_iRadix << std::endl;
	}
};

int main(int, const char*[])
{
	/*
	 * The thread pool will run work as it gets posted. We'll observe this in a simplistic way. 
	 */
	std::cout << "We will first run our tasks asynchronously with the thread pool:\n\n";

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

	std::cout << "\nDepending on how quickly our threads execute, the outputing of the data may be mixed together due to std::cout being called several times before it completes or they complete out of order.";
	std::cout << "\n\nNow let's run the same test for the same numbers without multithreading.\n\n";
	
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
	std::cout << "\n\nExecution time using thread pool: " << std::chrono::duration<double, std::milli>(threadsDiff).count() << " ms" << std::endl;

	auto noThreadsDiff = timerOnNoThreadsEnd - timerOnNoThreadsStart;
	std::cout << "Execution time using no threads: " << std::chrono::duration<double, std::milli>(noThreadsDiff).count() << " ms" << std::endl;

	system("PAUSE");
	return 0;
}
