cmake_minimum_required(VERSION 3.2)

project(rtwo)

ADD_DEFINITIONS(-std=c++14)

## install dependies
include(deps/deps.cmake)

## tests
add_subdirectory(tests)

## examples
include(examples/examples.cmake)

enable_testing()

add_test(NAME test COMMAND tests/coretest)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose
                  DEPENDS tests/coretest )
