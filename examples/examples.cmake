file(GLOB R_SOURCES "src/*.cc")

include_directories("src")
include_directories("deps/boost/include")
add_executable(routine examples/routine_example.cc ${R_SOURCES})
target_link_libraries(routine boost_coroutine boost_chrono boost_thread boost_context boost_system)
