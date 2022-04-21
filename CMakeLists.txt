cmake_minimum_required(VERSION 3.8.1)
project(example_threadpool)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -O2")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH 
    ${CMAKE_MODULE_PATH} 
    ${CMAKE_CURRENT_SOURCE_DIR}
) # ref: https://keunjun.blog/2018/10/07/cmake-10-%EB%AA%A8%EB%93%88/


find_package(cmake_modules)

set(includeDirs ${includeDirs} ${CMAKE_CURRENT_BINARY_DIR})
set(includeDirs ${includeDirs} include)
set(SRC ${SRC}
src/thread_pool.cpp)

include_directories( ${includeDirs} )
add_executable (example_threadpool src/main.cpp ${SRC})
