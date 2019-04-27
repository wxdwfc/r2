cmake_minimum_required(VERSION 3.2)

project(rtwo)

ADD_DEFINITIONS(-std=c++14)

## install dependies
#include(deps/CMakeLists.txt)
add_subdirectory(deps)

add_executable(a src/test.cc)

## tests
add_subdirectory(tests)

enable_testing()

add_test(NAME test COMMAND tests/coretest)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose
                  DEPENDS tests/coretest )
