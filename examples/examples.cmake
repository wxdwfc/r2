file(GLOB R_SOURCES "src/*.cc" "src/rpc/*.cc" "src/msg/*.cc")

include_directories("src")
include_directories("deps/boost/include")

set(apps
    routine
    rpc
    timer
    )

add_executable(routine examples/routine_example.cc ${R_SOURCES})
add_executable(timer examples/timer_cycle.cc)
add_executable(rpc examples/rpc_example.cc ${R_SOURCES})

foreach(app ${apps})
  target_link_libraries(${app} boost_coroutine boost_chrono boost_thread boost_context boost_system ibverbs ssmalloc pthread)
  add_dependencies(${app} libboost1.61 ralloc)
endforeach(app)

