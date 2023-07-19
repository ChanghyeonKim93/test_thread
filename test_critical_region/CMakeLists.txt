set(INCLUDE_DIR ${INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set(SRC ${SRC} 
  )

include_directories(${INCLUDE_DIR})
add_executable(${CMAKE_PROJECT_NAME}_critical_region main.cpp ${SRC})
