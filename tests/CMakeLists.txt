add_executable(runUnitTests tests/sample.cc)

target_link_libraries(runUnitTests gtest gtest_main)

add_test(NAME that-test-I-made COMMAND runUnitTests)
