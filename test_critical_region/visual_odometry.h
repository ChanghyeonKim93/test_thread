#ifndef _VISUAL_ODOMETRY_H_
#define _VISUAL_ODOMETRY_H_
#include <iostream>
#include <thread>
#include <future>

#include "data_manager.h"
#include "thread_manager.h"

class VisualOdometry
{
public:
  VisualOdometry(const std::shared_ptr<DataManager> &data_manager)
      : data_manager_(data_manager) {}
  ~VisualOdometry()
  {
    if (thread_->joinable())
    {
      thread_->join();
      std::cerr << "VisualOdometry thread joins successfully.\n";
    }
  }

  void Run(const std::shared_future<void> &terminate_signal)
  {
    thread_ = std::make_unique<std::thread>([&]()
                                            { ThreadProcess(terminate_signal); });
  }

private:
  void ThreadProcess(const std::shared_future<void> &terminate_signal)
  {
    std::stringstream ss;
    ss << "Start thread: " << std::this_thread::get_id() << "\n";
    std::cerr << ss.str();

    while (true)
    {
      std::cout << "Run VO thread\n";

      // Check thread termination
      std::future_status terminate_status = terminate_signal.wait_for(std::chrono::microseconds(1));
      if (terminate_status == std::future_status::ready)
      {
        break;
      }
    }
    std::cerr << "'VisualOdometry' thread receives termination signal.\n";
  }

private:
  std::shared_ptr<DataManager> data_manager_;

  std::unique_ptr<std::thread> thread_;
};

#endif