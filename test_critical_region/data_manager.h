#ifndef _DATA_MANAGER_H_
#define _DATA_MANAGER_H_
#include <iostream>
#include <unordered_map>
#include <vector>

#include <sstream>
#include <string>

#include <condition_variable>
#include <mutex>
struct Position {
  float x{0.0};
  float y{0.0};
  float z{0.0};
};
struct Pose {
  float x{0.0};
  float y{0.0};
  float yaw{0.0};
};

struct MapPoint {
  int id{-1};
  Position world_position;
  std::vector<int> observed_frame_id_list;
};

struct Frame {
  int id{-1};
  double timestamp;
  Pose pose;
  std::vector<int> observed_mappoint_id_list;
};

class DataManager;
using DataManagerPtr = std::shared_ptr<DataManager>;

class DataManager {
 public:
  DataManager() : mutex_(), last_mappoint_id_(-1), last_frame_id_(-1) {}
  ~DataManager() {}

 public:
  void RegisterMapPoint(const MapPoint& mappoint) {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    mappoint_pool_[mappoint.id] = mappoint;
  }
  void RegisterMapPoint(const std::vector<MapPoint>& mappoint_list) {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    for (const auto& mappoint : mappoint_list)
      mappoint_pool_[mappoint.id] = mappoint;
  }
  MapPoint GetMapPoint(const int mappoint_id) {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    if (mappoint_pool_.count(mappoint_id) == 0)
      throw std::runtime_error("no mappoint id in pool");

    return mappoint_pool_[mappoint_id];
  }
  std::vector<MapPoint> GetMapPoint(const std::vector<int>& mappoint_id_list) {
    const auto num_mappoint = mappoint_id_list.size();
    std::vector<MapPoint> mappoint_list;
    mappoint_list.resize(num_mappoint);

    std::lock_guard<std::mutex> lock_guard(mutex_);
    for (int index = 0; index < num_mappoint; ++index) {
      const auto& mappoint_id = mappoint_id_list[index];
      if (mappoint_pool_.count(mappoint_id) == 0)
        throw std::runtime_error("no mappoint id in pool");

      mappoint_list[index] = mappoint_pool_[mappoint_id];
    }
  }
  void UpdateMapPoint(const std::vector<MapPoint>& mappoint_list) {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    for (const auto& mappoint : mappoint_list) {
      if (mappoint_pool_.count(mappoint.id) == 0)
        throw std::runtime_error("no mappoint id in pool");

      mappoint_pool_[mappoint.id] = mappoint;
    }
  }

  MapPoint GenerateMapPointWithoutRegister() {
    MapPoint new_mappoint;
    new_mappoint.id = ++last_mappoint_id_;
    return new_mappoint;
  }

 private:
  int last_mappoint_id_;
  int last_frame_id_;

  std::unordered_map<int, MapPoint> mappoint_pool_;
  std::unordered_map<int, Frame> frame_pool_;

 private:
  std::mutex mutex_;
};

#endif