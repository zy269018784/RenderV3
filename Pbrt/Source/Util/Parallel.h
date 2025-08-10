#pragma once
#include <thread>
#include <vector>
#include <functional>
#include <condition_variable>
#include <mutex>
namespace Render {

    class ThreadPool;
    class ParallelJob {
    public:
        // ParallelJob Public Methods
        virtual ~ParallelJob() {  }

        virtual bool HaveWork() const = 0;
        virtual void RunStep(std::unique_lock<std::mutex>* lock) = 0;

        bool Finished() const { return !HaveWork() && activeWorkers == 0; }

        static ThreadPool* threadPool;

    private:
        // ParallelJob Private Members
        friend class ThreadPool;
        int activeWorkers = 0;
        ParallelJob* prev = nullptr, * next = nullptr;
        bool removed = false;
    };

    class ParallelForLoop1D : public ParallelJob {
    public:
        // ParallelForLoop1D Public Methods
        ParallelForLoop1D(int64_t startIndex, int64_t endIndex, int chunkSize,
            std::function<void(int64_t, int64_t)> func)
            : func(std::move(func)),
            nextIndex(startIndex),
            endIndex(endIndex),
            chunkSize(chunkSize) {}

        bool HaveWork() const { return nextIndex < endIndex; }

        void RunStep(std::unique_lock<std::mutex>* lock);

    private:
        // ParallelForLoop1D Private Members
        std::function<void(int64_t, int64_t)> func;
        int64_t nextIndex, endIndex;
        int chunkSize;
    };


    class ThreadPool {
    public:
        explicit ThreadPool(int nThreads);

        ~ThreadPool();

        size_t size() const { return threads.size(); }

        std::unique_lock<std::mutex> AddToJobList(ParallelJob* job);
        void RemoveFromJobList(ParallelJob* job);

        void WorkOrWait(std::unique_lock<std::mutex>* lock, bool isEnqueuingThread);
        bool WorkOrReturn();

        void Disable();
        void Reenable();

        void ForEachThread(std::function<void(void)> func);

        std::string ToString() const;

    private:
        // ThreadPool Private Methods
        void Worker();

        // ThreadPool Private Members
        std::vector<std::thread> threads;
        mutable std::mutex mutex;
        bool shutdownThreads = false;
        bool disabled = false;
        ParallelJob* jobList = nullptr;
        std::condition_variable jobListCondition;
    };
    void ParallelFor(int64_t start, int64_t end, std::function<void(int64_t, int64_t)> func);
    inline void ParallelFor(int64_t start, int64_t end, std::function<void(int64_t)> func) {
        ParallelFor(start, end, [&func](int64_t start, int64_t end) {
            for (int64_t i = start; i < end; ++i)
                func(i);
            });
    }
    int AvailableCores();
    void ParallelInit(int nThreads);
    void ParallelCleanup();
}