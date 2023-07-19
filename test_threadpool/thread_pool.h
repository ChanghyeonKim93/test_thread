#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <iostream>
#include <vector>
#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <utility> // std::move, std::forward
#include <functional> // std::function

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
    bool setThreadCPUAffinity(std::thread& th, const int& cpu_num);
    void workerThread();

public:
    ThreadPool(size_t num_thread);
    ThreadPool(size_t num_thread, std::vector<int> cpu_affinity_numbers);
    ~ThreadPool();
    void enqueueJob(std::function<void()> job);

    void notifyOne();
    void notifyAll();
};

#endif