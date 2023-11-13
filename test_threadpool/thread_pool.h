#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <sched.h>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include <pthread.h>  // Linux Only. https://man7.org/linux/man-pages/man3/pthread_setaffinity_np.3.html

class ThreadPool {
 public:
  explicit ThreadPool(int num_thread);
  explicit ThreadPool(int num_thread, std::vector<int> cpu_affinity_numbers);
  ~ThreadPool();

  void enqueueJob(std::function<void()> job);
  void notifyOne();
  void notifyAll();

 private:
  bool AllocateCpuForThread(std::thread& th, const int& cpu_num);
  void RunWorkerThread();

 private:
  int num_threads_;
  std::vector<std::thread> worker_thread_list_;
  std::condition_variable condition_variable_;
  std::mutex mutex_;
  std::queue<std::function<void()>> job_queue_;

  std::atomic<bool> flag_stop_all_;
};

#endif