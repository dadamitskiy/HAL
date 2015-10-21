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

ThreadPool::ThreadPool()
{
	Init();
}

ThreadPool::~ThreadPool()
{

}

ThreadPool* ThreadPool::GetInstance()
{
	static ThreadPool threadPool;
	return &threadPool;
}

void ThreadPool::Init()
{
	// Get the number of concurrent threads supported.
	m_iNumThreads = std::thread::hardware_concurrency();
	m_bRun = true;

	for (unsigned int iter = 0; iter < m_iNumThreads; ++iter)
	{
		std::thread* thread = new std::thread(&ThreadPool::Execute, this);
		m_vThreads.push_back(thread);
	}
}

void ThreadPool::Shutdown()
{
	ClearWork();

	m_bRun = false;
	m_WorkCondition.notify_all();

	for (unsigned int iter = 0; iter < m_vThreads.size(); ++iter)
	{
		m_vThreads[iter]->join();
	}

	for (auto& threadIter : m_vThreads)
	{
		delete threadIter;
	}

	m_vThreads.clear();
}

void ThreadPool::Execute()
{
	while (m_bRun)
	{
		WorkerThread* work = nullptr;
		GetWork(&work);
		
		if (work != nullptr)
		{
			work->ExecuteWork();
			work->OnCompleteCallback();
		}
	}
}

void ThreadPool::PostWork(WorkerThread* InWorkerPtr)
{
	m_qWorkers.push(InWorkerPtr);
	m_WorkCondition.notify_all();
}

void ThreadPool::GetWork(WorkerThread** OutWorkerPtr)
{
	std::unique_lock<std::mutex> lock(m_QueueMutex);
	m_WorkCondition.wait(lock, [&](){ return (m_bRun == false) || (!m_qWorkers.empty()); });

	if (!m_qWorkers.empty())
	{
		(*OutWorkerPtr) = m_qWorkers.front();
		m_qWorkers.pop();
	}
	else
	{
		(*OutWorkerPtr) = nullptr;
	}

	lock.unlock();
}

void ThreadPool::ClearWork()
{
	m_QueueMutex.lock();

	while (!m_qWorkers.empty())
	{
		m_qWorkers.pop();
	}

	m_QueueMutex.unlock();
}
