
project(CPSC_326_HW_5)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O0") 
set(CMAKE_BUILD_TYPE Debug)

include_directories("src")


# locate gtest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# create unit test executables
add_executable(semantic_checker_tests tests/semantic_checker_tests.cpp
  src/token.cpp src/mypl_exception.cpp src/lexer.cpp src/ast_parser.cpp
  src/symbol_table.cpp src/semantic_checker.cpp)
target_link_libraries(semantic_checker_tests ${GTEST_LIBRARIES} pthread)

# create mypl target
add_executable(mypl src/token.cpp src/mypl_exception.cpp src/lexer.cpp
  src/simple_parser.cpp src/ast_parser.cpp src/print_visitor.cpp
  src/symbol_table.cpp src/semantic_checker.cpp src/mypl.cpp)
  
 
