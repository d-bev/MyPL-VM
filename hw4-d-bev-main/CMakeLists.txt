
cmake_minimum_required(VERSION 3.0)

project(CPSC_326_HW_4)

set(CMAKE_GENERATOR "Unix Makefiles")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O0") 
set(CMAKE_BUILD_TYPE Debug)

include_directories("src")
include_directories("C:/mingw64/lib/gcc/x86_64-w64-mingw32/12.2.0/include/gtest")

# locate gtest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# create unit test executables
add_executable(ast_parser_tests tests/ast_parser_tests.cpp
  src/token.cpp src/mypl_exception.cpp src/lexer.cpp src/ast_parser.cpp)
target_link_libraries(ast_parser_tests ${GTEST_LIBRARIES} pthread)

add_executable(parser_syntax_tests tests/parser_syntax_tests.cpp
  src/token.cpp src/mypl_exception.cpp src/lexer.cpp src/ast_parser.cpp)
target_link_libraries(parser_syntax_tests ${GTEST_LIBRARIES} pthread)


# create mypl target
add_executable(mypl src/token.cpp src/mypl_exception.cpp src/lexer.cpp
  src/simple_parser.cpp src/ast_parser.cpp src/print_visitor.cpp src/mypl.cpp)
