#ifndef _DATA_MANAGER_H_
#define _DATA_MANAGER_H_
#include <iostream>
#include <vector>
#include <unordered_map>

#include <string>
#include <sstream>

#include <mutex>
#include <condition_variable>
struct Position
{
  float x{0.0};
  float y{0.0};
  float z{0.0};
};
struct Pose
{
  float x{0.0};
  float y{0.0};
  float yaw{0.0};
};

struct MapPoint
{
  int id{-1};
  Position world_position;
  std::vector<int> observed_frame_id_list;
};
struct Frame
{
  int id{-1};
  Pose pose;
  std::vector<int> observed_mappoint_id_list;
};

class DataManager
{
public:
  DataManager()
      : mutex_(), last_mappoint_id_(-1), last_frame_id_(-1) {}
  ~DataManager() {}

public:
  void RegisterMapPoint(const MapPoint &mappoint)
  {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    MapPoint mappoint_new = mappoint;
    mappoint_new.id = ++last_mappoint_id_;
    mappoint_pool_[mappoint_new.id] = mappoint_new;
  }
  void RegisterMapPoint(const std::vector<MapPoint> &mappoint_list)
  {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    for (const auto &mappoint : mappoint_list)
    {
      MapPoint mappoint_new = mappoint;
      mappoint_new.id = ++last_mappoint_id_;
      mappoint_pool_[mappoint_new.id] = mappoint_new;
    }
  }
  MapPoint GetMapPoint(const int mappoint_id)
  {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    if (mappoint_pool_.count(mappoint_id) == 0)
      throw std::runtime_error("no mappoint id in pool");

    return mappoint_pool_[mappoint_id];
  }
  std::vector<MapPoint> GetMapPoint(const std::vector<int> &mappoint_id_list)
  {
    const auto num_mappoint = mappoint_id_list.size();
    std::vector<MapPoint> mappoint_list;
    mappoint_list.resize(num_mappoint);

    std::lock_guard<std::mutex> lock_guard(mutex_);
    for (size_t index = 0; index < num_mappoint; ++index)
    {
      const auto &mappoint_id = mappoint_id_list[index];
      if (mappoint_pool_.count(mappoint_id) == 0)
        throw std::runtime_error("no mappoint id in pool");

      mappoint_list[index] = mappoint_pool_[mappoint_id];
    }
  }
  void UpdateMapPoint(const std::vector<MapPoint> &mappoint_list)
  {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    for (const auto &mappoint : mappoint_list)
    {
      if (mappoint_pool_.count(mappoint.id) == 0)
        throw std::runtime_error("no mappoint id in pool");

      mappoint_pool_[mappoint.id] = mappoint;
    }
  }

  MapPoint GenerateMapPointWithoutRegister()
  {
    return MapPoint();
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