
project(CPSC_326_HW1)

cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O0") 
set(CMAKE_BUILD_TYPE Debug)

include_directories("src")

# create mypl target
add_executable(mypl src/mypl.cpp)
  
 
