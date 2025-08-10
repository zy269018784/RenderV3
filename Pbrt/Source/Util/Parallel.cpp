#include <Util/Parallel.h>

namespace Render {
    void ParallelForLoop1D::RunStep(std::unique_lock<std::mutex>* lock) {
        // Determine the range of loop iterations to run in this step
        int64_t indexStart = nextIndex;
        int64_t indexEnd = std::min(indexStart + chunkSize, endIndex);
        nextIndex = indexEnd;

        // Remove job from list if all work has been started
        if (!HaveWork())
            threadPool->RemoveFromJobList(this);

        // Release lock and execute loop iterations in _[indexStart, indexEnd)_
        lock->unlock();
        func(indexStart, indexEnd);
    }
    ThreadPool* ParallelJob::threadPool;

	ThreadPool::ThreadPool(int nThreads) {
		for (int i = 0; i < nThreads - 1; ++i)
			threads.push_back(std::thread(&ThreadPool::Worker, this));
	}

    ThreadPool::~ThreadPool() {
        if (threads.empty())
            return;
        {
            std::lock_guard<std::mutex> lock(mutex);
            shutdownThreads = true;
            jobListCondition.notify_all();
        }

        for (std::thread& thread : threads)
            thread.join();
    }

    void ThreadPool::Worker() {
        std::unique_lock<std::mutex> lock(mutex);
        while (!shutdownThreads)
            WorkOrWait(&lock, false);
    }


    std::unique_lock<std::mutex> ThreadPool::AddToJobList(ParallelJob* job) {
        std::unique_lock<std::mutex> lock(mutex);
        // Add _job_ to head of _jobList_
        if (jobList)
            jobList->prev = job;
        job->next = jobList;
        jobList = job;

        jobListCondition.notify_all();
        return lock;
    }

    void ThreadPool::WorkOrWait(std::unique_lock<std::mutex>* lock, bool isEnqueuingThread) {
        // Return if this is a worker thread and the thread pool is disabled
        if (!isEnqueuingThread && disabled) {
            jobListCondition.wait(*lock);
            return;
        }

        ParallelJob* job = jobList;
        while (job && !job->HaveWork())
            job = job->next;
        if (job) {
            // Execute work for _job_
            job->activeWorkers++;
            job->RunStep(lock);
            // Handle post-job-execution details
            lock->lock();
            job->activeWorkers--;
            if (job->Finished())
                jobListCondition.notify_all();

        }
        else
            // Wait for new work to arrive or the job to finish
            jobListCondition.wait(*lock);
    }

    void ThreadPool::RemoveFromJobList(ParallelJob* job) {

        if (job->prev)
            job->prev->next = job->next;
        else {
            jobList = job->next;
        }
        if (job->next)
            job->next->prev = job->prev;

        job->removed = true;
    }

    bool ThreadPool::WorkOrReturn() {
        std::unique_lock<std::mutex> lock(mutex);

        ParallelJob* job = jobList;
        while (job && !job->HaveWork())
            job = job->next;
        if (!job)
            return false;

        // Execute work for _job_
        job->activeWorkers++;
        job->RunStep(&lock);
        lock.lock();
        job->activeWorkers--;
        if (job->Finished())
            jobListCondition.notify_all();

        return true;
    }

    void ThreadPool::ForEachThread(std::function<void(void)> func) {
       //Barrier* barrier = new Barrier(threads.size() + 1);
       //
       //ParallelFor(0, threads.size() + 1, [barrier, &func](int64_t) {
       //    func();
       //    if (barrier->Block())
       //        delete barrier;
       //    });
    }

    void ThreadPool::Disable() {
        disabled = true;
    }

    void ThreadPool::Reenable() {
        disabled = false;
    }

    int AvailableCores() {
        return std::max<int>(1, std::thread::hardware_concurrency());
    }

    int RunningThreads() {
        return ParallelJob::threadPool ? (1 + ParallelJob::threadPool->size()) : 1;
    }
    void ParallelFor(int64_t start, int64_t end, std::function<void(int64_t, int64_t)> func) {
        if (start == end)
            return;
        // Compute chunk size for parallel loop
        int64_t chunkSize = std::max<int64_t>(1, (end - start) / (8 * RunningThreads()));

        // Create and enqueue _ParallelForLoop1D_ for this loop
        ParallelForLoop1D loop(start, end, chunkSize, std::move(func));
        std::unique_lock<std::mutex> lock = ParallelJob::threadPool->AddToJobList(&loop);

        // Help out with parallel loop iterations in the current thread
        while (!loop.Finished())
            ParallelJob::threadPool->WorkOrWait(&lock, true);
    }

    void ParallelInit(int nThreads) {
        if (nThreads <= 0)
            nThreads = AvailableCores();
        ParallelJob::threadPool = new ThreadPool(nThreads);
    }

    void ParallelCleanup() {
        delete ParallelJob::threadPool;
        ParallelJob::threadPool = nullptr;
    }
}