add_executable(coretest ../tests/main.cc)
target_link_libraries(coretest gtest gtest_main ibverbs)
