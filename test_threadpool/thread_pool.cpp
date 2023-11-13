#include "thread_pool.h"

#include <functional>
#include <future>
#include <iostream>
#include <sstream>
#include <string>

void PrintInfoImpl(const std::string& str, const std::string& func_str) {
  std::stringstream ss;
  ss << "Pid[" << std::this_thread::get_id() << "][INFO]\"" << func_str
     << "\": " << str << "\n";
  std::cerr << ss.str();
}
#define PrintInfo(str) PrintInfoImpl(str, std::string(__func__));

ThreadPool::ThreadPool(int num_threads_in_pool)
    : thread_pool_status_(ThreadPoolStatus::kRun) {
  worker_thread_list_.reserve(num_threads_in_pool);
  for (int index = 0; index < num_threads_in_pool; ++index) {
    PrintInfo("========== thread is generating... ==========");
    worker_thread_list_.emplace_back([this]() { RunProcessForWorkerThread(); });
  }
  std::cout << "[" << num_threads_in_pool << "] threads are generated!\n";
};

ThreadPool::ThreadPool(int num_threads_in_pool,
                       std::vector<int> cpu_affinity_numbers)
    : thread_pool_status_(ThreadPoolStatus::kRun) {
  if (num_threads_in_pool != cpu_affinity_numbers.size()) {
    throw std::runtime_error(
        "ThreadPool::ThreadPool : cpu_affinity_numbers.size() != num_thread");
  }

  worker_thread_list_.reserve(num_threads_in_pool);
  for (int index = 0; index < num_threads_in_pool; ++index) {
    PrintInfo("========== thread is generating... ==========");
    worker_thread_list_.emplace_back([this]() { RunProcessForWorkerThread(); });
    AllocateProcessorForEachThread(worker_thread_list_[index],
                                   cpu_affinity_numbers[index]);
  }
};

ThreadPool::~ThreadPool() {
  thread_pool_status_ = ThreadPoolStatus::kKill;
  NotifyAll();
  for (auto& worker_thread : worker_thread_list_) {
    PrintInfo("--- JOINING THE THREAD");
    worker_thread.join();
    PrintInfo("Done!");
  }
};

void ThreadPool::RunProcessForWorkerThread() {
  PrintInfo("The thread is initialized.");
  while (true) {
    std::unique_lock<std::mutex> local_lock(mutex_);
    condition_variable_.wait(local_lock, [this]() {
      return (!job_queue_.empty() ||
              thread_pool_status_ == ThreadPoolStatus::kKill);
    });

    if (thread_pool_status_ == ThreadPoolStatus::kKill) {
      PrintInfo(
          "The thread captured stop sign. The thread is going to join...");
      break;
    }

    // Get a job
    std::function<void()> new_job = std::move(job_queue_.front());
    job_queue_.pop();
    local_lock.unlock();

    // Do the job!
    new_job();
    PrintInfo("Job done!");
  }
  PrintInfo("The thread is end.");
};

void ThreadPool::EnqueueJob(std::function<void()> job) {
  if (thread_pool_status_ == ThreadPoolStatus::kKill) {
    throw std::runtime_error("Thread pool 사용 중지.");
  }

  size_t num_queued_jobs;
  mutex_.lock();
  job_queue_.push(std::move(job));
  num_queued_jobs = job_queue_.size();
  mutex_.unlock();
  // PrintInfo("Job queue size: " + std::to_string(num_queued_jobs));

  NotifyOne();
};

bool ThreadPool::AllocateProcessorForEachThread(std::thread& th,
                                                const int& cpu_num) {
  unsigned int num_max_threads = std::thread::hardware_concurrency();
  if (cpu_num >= num_max_threads) {
    std::cerr << "Exceed the maximum logical CPU number!";
    return false;
  }

  cpu_set_t cpuSet;
  CPU_ZERO(&cpuSet);
  CPU_SET(cpu_num, &cpuSet);
  int rc =
      pthread_setaffinity_np(th.native_handle(), sizeof(cpu_set_t), &cpuSet);
  if (rc != 0) {
    std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
    return false;
  }

  PrintInfo("The thread is running on CPU [" + std::to_string(cpu_num) + "]");
  return true;
};

void ThreadPool::NotifyOne() { condition_variable_.notify_one(); };
void ThreadPool::NotifyAll() { condition_variable_.notify_all(); };