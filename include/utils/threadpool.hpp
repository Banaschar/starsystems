#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <queue>
#include <cstdio>

class ThreadPool {
public:
    ThreadPool(int numThreads) {
        for (int i = 0; i < numThreads; i++) {
            pool_.push_back(new std::thread(&ThreadPool::workerFunction, this));
            //pool_.push_back(new std::thread([this]{workerFunction();}));
        }
    }

    ~ThreadPool() {
        if (!closed_)
            closePool();
    }

    void workerFunction() {
        std::function<void()> job;
        while(true) {
            {
                std::unique_lock<std::mutex> lock(queueMutex_);

                cond_.wait(lock, [this]{ return !queue_.empty() || shutdown_;});
                if (shutdown_ && queue_.empty())
                    return;

                job = queue_.front();
                queue_.pop();
            }
            job();
        }
    }

    void addJob(std::function<void()> job) {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            queue_.push(job);
        }
        cond_.notify_one();
    }

    void closePool() {
        shutdown_ = true;
        cond_.notify_all();

        for (std::thread *t : pool_)
            t->join();

        for (std::thread *t : pool_)
            delete t;

        pool_.clear();
        closed_ = true;
    }

private:
    std::vector<std::thread *> pool_;
    std::mutex queueMutex_; 
    std::condition_variable cond_;
    bool shutdown_ = false;
    bool closed_ = false;
    std::queue<std::function<void()>> queue_;
};