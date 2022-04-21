#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <iostream>
#include <vector>
#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <functional>

#include <sched.h>
#include <pthread.h> // Linux Only. https://man7.org/linux/man-pages/man3/pthread_setaffinity_np.3.html

class ThreadPool{
private:
    size_t num_threads_; // total # of threads
    std::vector<std::thread> worker_threads_; // worker threads

    std::condition_variable cond_var_;
    std::mutex mut_;
    
    std::queue<std::function<void()>> jobs_;

    bool flag_stop_all_;

public:

private:

public:
    ThreadPool(size_t num_thread);
    ~ThreadPool();

    void workerThread();

    void enqueueJob(std::function<void()> job);

    void notifyOne();

    bool setCPUAffinity(const std::thread& th, const int& cpu_num);
};

#endif