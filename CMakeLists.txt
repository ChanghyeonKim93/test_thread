cmake_minimum_required(VERSION 3.9)
project(test_thread)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O2")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(test_threadpool)
add_subdirectory(test_critical_region)