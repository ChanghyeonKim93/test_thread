#ifndef _VISUAL_ODOMETRY_H_
#define _VISUAL_ODOMETRY_H_
#include <iostream>
#include <thread>
#include <future>

#include "data_manager.h"

#include "bundle_adjuster.h"

class VisualOdometry
{
public:
  VisualOdometry(const std::shared_ptr<DataManager> &data_manager)
      : data_manager_(data_manager), bundle_adjuster_(std::make_unique<BundleAdjuster>(data_manager_))
  {
    // Run threads
    bundle_adjuster_->Run();
  }

  ~VisualOdometry() {}

  bool TrackStereoImages()
  {
    bool is_success = true;
    // TODO(@): implement track function

    return is_success;
  }

private:
  std::shared_ptr<DataManager> data_manager_;

  std::unique_ptr<BundleAdjuster> bundle_adjuster_;
};

#endif