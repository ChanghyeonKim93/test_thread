set(INCLUDE_DIR ${INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set(SRC ${SRC} thread_pool.cpp)

include_directories(${INCLUDE_DIR})
add_executable(${CMAKE_PROJECT_NAME}_thread_pool main.cpp ${SRC})
