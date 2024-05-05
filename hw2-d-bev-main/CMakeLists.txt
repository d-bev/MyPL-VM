
project(CPSC_326_HW2)

cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O0") 
set(CMAKE_BUILD_TYPE Debug)

include_directories("src")

# locate gtest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# create unit test executables
add_executable(token_tests tests/token_tests.cpp src/token.cpp)
target_link_libraries(token_tests ${GTEST_LIBRARIES} pthread)

add_executable(lexer_tests tests/lexer_tests.cpp src/token.cpp
  src/mypl_exception.cpp src/lexer.cpp)
target_link_libraries(lexer_tests ${GTEST_LIBRARIES} pthread)

# create mypl target
add_executable(mypl src/token.cpp src/mypl_exception.cpp
  src/lexer.cpp src/mypl.cpp)
  
 
