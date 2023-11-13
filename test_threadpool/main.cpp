#include <chrono>
#include <iostream>

#include "thread_pool.h"

void work(int t, int id) {
  printf("  work   %d start \n", id);
  std::this_thread::sleep_for(std::chrono::seconds(t));
  std::cout << "        " << std::this_thread::get_id()
            << " cpu num: " << sched_getcpu() << " " << id << " end after " << t
            << " seconds.\n";
};
void work2(int t, int id, int a) {
  printf("  work2  %d start \n", id);
  std::this_thread::sleep_for(std::chrono::seconds(t));
  std::cout << "        " << std::this_thread::get_id() << " " << id
            << " end after " << t << " seconds.\n";
};

void workIteration() {
  unsigned long a = 0;
  for (int i = 0; i < 100000000; ++i) {
    a += 1;
  }
};

void setThisProcessCPUAffinitiy(int num_cpu) {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(num_cpu, &mask);
  int result = sched_setaffinity(0, sizeof(mask), &mask);
};

int main() {
  printf("!!!!! Program starts.\n");

  int num_threads = 2;
  std::vector<int> select_cpu_nums(num_threads);
  select_cpu_nums[0] = 5;
  select_cpu_nums[1] = 7;

  ThreadPool pool(num_threads, select_cpu_nums);
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < 9; ++i) {
    pool.enqueueJob([i]() { workIteration(); });
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "!!!!! program ends...\n";

  return 0;
}