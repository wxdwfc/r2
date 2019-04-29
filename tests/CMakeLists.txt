include_directories(../deps)

file(GLOB SOURCES  "../src/*")
file(GLOB MSOURCES  "../src/msg/*")
file(GLOB TSOURCES  "../tests/*.cc")
add_executable(coretest "${TSOURCES}" "${SOURCES}" "${MSOURCES}")

target_link_libraries(coretest gtest gtest_main ibverbs ssmalloc
                               boost_coroutine boost_chrono boost_thread boost_context boost_system
                               )
