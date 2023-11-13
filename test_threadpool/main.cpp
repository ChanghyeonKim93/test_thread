#include <chrono>
#include <iostream>

#include "thread_pool.h"

// std::chrono::high_resolution_clock::now();
// std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
void Work1(int t, int id) {
  printf("  work   %d start \n", id);
  std::this_thread::sleep_for(std::chrono::seconds(t));
  std::cout << "        " << std::this_thread::get_id()
            << " cpu num: " << sched_getcpu() << " " << id << " end after " << t
            << " seconds.\n";
};

void Work2(int t, int id, int a) {
  printf("  work2  %d start \n", id);
  std::this_thread::sleep_for(std::chrono::seconds(t));
  std::cout << "        " << std::this_thread::get_id() << " " << id
            << " end after " << t << " seconds.\n";
};

void Work3() {
  double a = 1.5;
  for (int i = 0; i < 2000000; ++i) {
    a *= 1.0;
  }
  std::cerr << "work 3 done\n";
};

void setThisProcessCPUAffinitiy(int num_cpu) {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(num_cpu, &mask);
  int result = sched_setaffinity(0, sizeof(mask), &mask);
};

int main() {
  int num_threads = 3;
  std::vector<int> select_cpu_nums(num_threads);
  select_cpu_nums[0] = 5;
  select_cpu_nums[1] = 7;
  select_cpu_nums[1] = 7;
  select_cpu_nums[1] = 7;

  // ThreadPool pool(num_threads, select_cpu_nums);
  std::unique_ptr<ThreadPool> thread_pool =
      std::make_unique<ThreadPool>(num_threads);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // std::vector<int> time_list{1, 2, 3, 4, 5, 4, 3, 2, 1};
  // for (int index = 0; index < 9; ++index) {
  //   pool.EnqueueJob(
  //       [time_list, index]() { Work2(time_list[index], index, index); });
  // }

  for (int index = 0; index < 48; ++index) {
    thread_pool->EnqueueJob([]() { Work3(); });
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  return 0;
}