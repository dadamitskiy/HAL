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

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>

/**
 * An abstract base class that the user will have to derive from in order to provide the thread pool with
 * something to do.
 */
class WorkerThread
{
public:

	virtual void ExecuteWork() = 0;
	virtual void OnCompleteCallback() { }
};

/**
 * The ThreadPool class accepts work tasks and then assigns the work to a thread that is idle.
 */
class ThreadPool
{
public:

	/* 
	 * Singleton interface. We will use a singleton here so a programmer can pass off work to a thread 
	 * pool with relative ease and have minimal dependencies in regards to what is actually being executed.
	 */
	static ThreadPool* GetInstance();

	/**
	 * Increases the work count, adds the WorkerThread to the queue, and posts the work.
	 *
	 * @param IN InWorkerPtr - Work to be added to the queue.
	 */
	void PostWork(WorkerThread* InWorkerPtr);

	/**
	 * Grabs a work item from the queue. Waits for one if none is posted.
	 *
	 * @param OUT OutWorkerPtr - Pointer to the memory address of the work pointer.
	 */
	void GetWork(WorkerThread** OutWorkerPtr);

	/* Removes all work from the work queue. */
	void ClearWork();

	/* Cleans up the allocated resources. */
	void Shutdown();

private:

	/* The number of spawned threads. */
	unsigned int m_iNumThreads;

	/* A mutex to protect the work queue. */
	std::mutex m_QueueMutex;

	/* Conditional variable to synchronize threads and run our atomic variable. */
	std::condition_variable m_WorkCondition;

	/* A list of worker threads. */
	std::queue<WorkerThread*> m_qWorkers;

	/* Storage of all the threads. */
	std::vector<std::thread*> m_vThreads;

	/* Atomic to keep track of the ThreadPool needing to run. */
	std::atomic_bool m_bRun;

	/* Constructor and Destructor */
	ThreadPool();
	~ThreadPool();

	/* Create threads and initialize internal members. */
	void Init();

	/* Execute the main execution function fo all threads. */
	void Execute();
};

#endif
