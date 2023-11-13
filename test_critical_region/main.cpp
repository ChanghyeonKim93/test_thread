#include <iostream>
#include <vector>

#include "data_manager.h"

#include "visual_odometry.h"

int main()
{
  printf("Main Node starts.\n");

  std::shared_ptr<DataManager> data_manager = std::make_shared<DataManager>();
  std::unique_ptr<VisualOdometry> vo = std::make_unique<VisualOdometry>(data_manager);

  Landmark mp = data_manager->GenerateMapPointWithoutRegister();
  data_manager->RegisterMapPoint(mp);
  auto mp_in_pool = data_manager->GetMapPoint(0);

  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  std::cout << "Main Node ends\n";

  return 0;
}