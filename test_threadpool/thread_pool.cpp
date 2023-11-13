#include "thread_pool.h"

#include <iostream>
#include <sstream>
#include <string>

void PrintInfoThreadSafeWithPidImpl(const std::string& str,
                                    const std::string& func_str) {
  std::stringstream ss;
  ss << "Pid[" << std::this_thread::get_id() << "][INFO](";
  ss << func_str << "): " << str << "\n";
  std::cerr << ss.str();
}
#define PrintInfoThreadSafeWithPid(str) \
  PrintInfoThreadSafeWithPidImpl(str, std::string(__func__));

ThreadPool::ThreadPool(int num_thread)
    : num_threads_(num_thread), flag_stop_all_(false) {
  worker_thread_list_.reserve(num_threads_);
  for (int i = 0; i < num_threads_; ++i) {
    std::cout << "========== [" << i
              << "]-th thread is generating... ==========\n";
    worker_thread_list_.emplace_back([this]() { this->RunWorkerThread(); });
  }
  std::cout << "[" << num_threads_ << "] threads are generated!\n";
};

ThreadPool::ThreadPool(int num_thread, std::vector<int> cpu_affinity_numbers)
    : num_threads_(num_thread), flag_stop_all_(false) {
  if (num_threads_ != cpu_affinity_numbers.size()) {
    throw std::runtime_error(
        "ThreadPool::ThreadPool : cpu_affinity_numbers.size() != num_thread");
  }

  worker_thread_list_.reserve(num_threads_);
  for (int i = 0; i < num_threads_; ++i) {
    PrintInfoThreadSafeWithPid("========== thread is generating... ==========");

    worker_thread_list_.emplace_back([this]() { this->RunWorkerThread(); });
    this->AllocateCpuForThread(worker_thread_list_[i], cpu_affinity_numbers[i]);
  }
};

ThreadPool::~ThreadPool() {
  flag_stop_all_ = true;
  condition_variable_.notify_all();

  for (auto& t : worker_thread_list_) {
    PrintInfoThreadSafeWithPid("--- JOINING THE THREAD");
    t.join();
    PrintInfoThreadSafeWithPid("Done!");
  }
};

void ThreadPool::RunWorkerThread() {
  PrintInfoThreadSafeWithPid("The thread is initialized.");

  while (true) {
    std::unique_lock<std::mutex> local_lock(mutex_);
    condition_variable_.wait(local_lock, [this]() {
      return (!this->job_queue_.empty()) || flag_stop_all_;
    });
    if (flag_stop_all_ && this->job_queue_.empty()) {
      PrintInfoThreadSafeWithPid(
          "The thread captured stop sign. The thread is going to join...");
      break;
    }

    // Get a job
    std::function<void()> new_job = std::move(job_queue_.front());
    job_queue_.pop();
    local_lock.unlock();

    // Do the job!
    PrintInfoThreadSafeWithPid("Get the job.");
    new_job();
  }
  PrintInfoThreadSafeWithPid("The thread is end.");
};

void ThreadPool::enqueueJob(std::function<void()> job) {
  if (flag_stop_all_) {
    throw std::runtime_error("Thread pool 사용 중지.");
  }

  {
    std::lock_guard<std::mutex> lock(mutex_);
    job_queue_.push(std::move(job));
    PrintInfoThreadSafeWithPid("Job queue size: " +
                               std::to_string(job_queue_.size()));
  }
  condition_variable_.notify_one();

  PrintInfoThreadSafeWithPid("Notified");
};

bool ThreadPool::AllocateCpuForThread(std::thread& th, const int& cpu_num) {
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

  PrintInfoThreadSafeWithPid("The thread is running on CPU [" +
                             std::to_string(cpu_num) + "]");
  return true;
};

void ThreadPool::notifyOne() { condition_variable_.notify_one(); };
void ThreadPool::notifyAll() { condition_variable_.notify_all(); };