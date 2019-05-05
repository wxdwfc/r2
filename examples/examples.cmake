file(GLOB R_SOURCES "src/*.cc")

include_directories("src")
include_directories("deps/boost/include")

## routine example
add_executable(routine examples/routine_example.cc ${R_SOURCES})
target_link_libraries(routine boost_coroutine boost_chrono boost_thread boost_context boost_system)

add_executable(timer examples/timer_cycle.cc)
add_dependencies(timer libboost1.61)
add_dependencies(routine libboost1.61)

