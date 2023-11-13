#include <chrono>
#include <iostream>

#include "thread_pool.h"

// std::chrono::high_resolution_clock::now();
// std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
void setThisProcessCPUAffinitiy(int num_cpu) {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(num_cpu, &mask);
  int result = sched_setaffinity(0, sizeof(mask), &mask);
};

struct Feature {
  float x{0.0f};
  float y{0.0f};
  float response{0.0f};
  int octave{0};
};

std::vector<Feature> FindFeatures(const int num_features,
                                  const int fast_threshold,
                                  const int num_scale_level,
                                  const float scale_factor) {
  std::vector<Feature> feature_list;
  feature_list.resize(num_features);
  return feature_list;
};

int main() {
  int num_threads = 3;
  std::vector<int> select_cpu_nums(num_threads);
  select_cpu_nums[0] = 0;
  select_cpu_nums[1] = 1;
  select_cpu_nums[2] = 2;
  select_cpu_nums[3] = 3;

  // ThreadPool pool(num_threads, select_cpu_nums);
  std::unique_ptr<ThreadPool> thread_pool =
      std::make_unique<ThreadPool>(num_threads);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  using FeatureList = std::vector<Feature>;
  std::vector<std::future<FeatureList>> vector_of_future_feature_list;
  vector_of_future_feature_list.reserve(3);

  vector_of_future_feature_list.emplace_back(
      thread_pool->EnqueueJobAndGetFuture(FindFeatures, 100, 20, 8, 0.9f));
  vector_of_future_feature_list.emplace_back(
      thread_pool->EnqueueJobAndGetFuture(FindFeatures, 200, 20, 8, 0.9f));
  vector_of_future_feature_list.emplace_back(
      thread_pool->EnqueueJobAndGetFuture(FindFeatures, 300, 20, 8, 0.9f));

  FeatureList all_feature_list;
  for (size_t index = 0; index < 3; ++index) {
    auto feature_list = vector_of_future_feature_list[index].get();
    for (size_t feature_index = 0; feature_index < feature_list.size();
         ++feature_index) {
      all_feature_list.push_back(feature_list.at(feature_index));
    }
  }
  std::cerr << "all_feature_list.size() : " << all_feature_list.size()
            << std::endl;

  return 0;
}