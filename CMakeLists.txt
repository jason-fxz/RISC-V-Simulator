cmake_minimum_required(VERSION 3.5.0)

project(RISC-V-Simulator VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -Wall -DDEBUG")

include_directories(src/include)

add_subdirectory(src)

add_executable(code src/main.cpp)

target_link_libraries(code simulator)
target_link_libraries(code naive_simulator)
