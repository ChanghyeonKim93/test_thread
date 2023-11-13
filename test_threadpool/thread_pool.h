#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include <pthread.h>  // Linux Only. https://man7.org/linux/man-pages/man3/pthread_setaffinity_np.3.html
#include <sched.h>

class ThreadPool {
 public:
  enum ThreadPoolStatus { kRun = 0, kKill = 1 };

 public:
  explicit ThreadPool(int num_thread);
  explicit ThreadPool(int num_thread, std::vector<int> cpu_affinity_numbers);
  ~ThreadPool();

  template <typename Func, typename... Args>
  std::future<typename std::invoke_result<Func, Args...>::type>
  EnqueueJobAndGetFuture(Func&& func, Args&&... args);

  void EnqueueJob(std::function<void()> job);
  void NotifyOne();
  void NotifyAll();

 private:
  bool AllocateProcessorForEachThread(std::thread& th, const int& cpu_num);
  void RunProcessForWorkerThread();

 private:
  std::vector<std::thread> worker_thread_list_;
  std::queue<std::function<void()>> job_queue_;

  std::mutex mutex_;
  std::condition_variable condition_variable_;
  std::atomic<ThreadPoolStatus> thread_pool_status_;
};

template <typename Func, typename... Args>
std::future<typename std::invoke_result<Func, Args...>::type>  // from C++17
ThreadPool::EnqueueJobAndGetFuture(Func&& func, Args&&... args) {
  using ReturnType = typename std::invoke_result<Func, Args...>::type;

  auto task_ptr = std::make_shared<std::packaged_task<ReturnType()>>(
      std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
  std::future<ReturnType> future_result = task_ptr->get_future();

  mutex_.lock();
  job_queue_.push([task_ptr]() { (*task_ptr)(); });
  mutex_.unlock();

  NotifyOne();

  return future_result;
}

#endif