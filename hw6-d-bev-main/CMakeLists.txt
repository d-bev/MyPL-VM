
project(CPSC_326_HW_6)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O0") 
set(CMAKE_BUILD_TYPE Debug)

include_directories("src")


# locate gtest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(vm_tests tests/vm_tests.cpp src/mypl_exception.cpp
  src/vm_instr.cpp src/vm.cpp)
target_link_libraries(vm_tests ${GTEST_LIBRARIES} pthread)

# create hw6 target
add_executable(hw6 src/mypl_exception.cpp src/vm_instr.cpp
  src/vm.cpp src/hw6.cpp)
  
 
