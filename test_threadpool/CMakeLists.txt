cmake_minimum_required(VERSION 3.9)
project(test_threadpool)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O2")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

set(INCLUDE_DIR ${INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set(SRC ${SRC}
 thread_pool.cpp)

include_directories(${INCLUDE_DIR})

add_executable(${CMAKE_PROJECT_NAME}_thread_pool main.cpp ${SRC})
add_executable(${CMAKE_PROJECT_NAME}_test_enqueue_and_future test_enqueue_and_future.cc ${SRC})
