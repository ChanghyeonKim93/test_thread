#include <iostream>
#include <vector>

#include "data_manager.h"

#include "visual_odometry.h"
#include "bundle_adjuster.h"

int main()
{
  printf("Program starts.\n");

  std::shared_ptr<DataManager> data_manager = std::make_shared<DataManager>();

  std::unique_ptr<VisualOdometry> vo = std::make_unique<VisualOdometry>(data_manager);
  std::unique_ptr<BundleAdjuster> ba = std::make_unique<BundleAdjuster>(data_manager);

  std::shared_ptr<ThreadManager> tm = std::make_shared<ThreadManager>();

  vo->Run(tm->GetTerminateSignal());
  ba->Run(tm->GetTerminateSignal());

  MapPoint mp = data_manager->GenerateMapPointWithoutRegister();
  data_manager->RegisterMapPoint(mp);
  auto mp_in_pool = data_manager->GetMapPoint(0);

  // std::this_thread::sleep_for(std::chrono::milliseconds(100));

  std::cout << " program ends...\n ";

  return 0;
}