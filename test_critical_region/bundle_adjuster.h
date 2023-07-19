#ifndef _BUNDLE_ADJUSTER_H_
#define _BUNDLE_ADJUSTER_H_
#include <iostream>
#include <thread>
#include <future>

#include "data_manager.h"
#include "thread_manager.h"

class BundleAdjuster
{
public:
  BundleAdjuster(const std::shared_ptr<DataManager> &data_manager)
      : data_manager_(data_manager)
  {
    terminate_signal_ = terminate_promise_.get_future();
  }
  ~BundleAdjuster()
  {
    terminate_promise_.set_value();
    if (thread_->joinable())
    {
      thread_->join();
      std::cerr << "BundleAdjuster thread joins successfully.\n";
    }
  }

  void Run()
  {
    thread_ = std::make_unique<std::thread>([&]()
                                            { ThreadProcess(terminate_signal_); });
  }

private:
  void ThreadProcess(const std::shared_future<void> &terminate_signal)
  {
    std::stringstream ss;
    ss << "Start thread: " << std::this_thread::get_id() << "\n";
    std::cerr << ss.str();

    constexpr int loop_period_in_ms = 200;
    while (true)
    {
      std::cout << "Run BA thread ! \n";
      // Check thread termination
      std::future_status terminate_status = terminate_signal.wait_for(std::chrono::microseconds(1));
      if (terminate_status == std::future_status::ready)
      {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(loop_period_in_ms));
    }
    std::cerr << "'BundleAdjuster' thread receives termination signal.\n";
  }

private:
  std::shared_ptr<DataManager> data_manager_;
  std::unique_ptr<std::thread> thread_;
  std::shared_future<void> terminate_signal_;
  std::promise<void> terminate_promise_;
};

#endif