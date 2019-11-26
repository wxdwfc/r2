file(GLOB TSOURCES  "tests/test_srop.cc")
add_executable(coretest ${TSOURCES} )

find_library(boost_serialization_lib NAMES boost_serialization PATHS ./deps/boost PATH_SUFFIXES lib
               NO_DEFAULT_PATH)

if(NOT boost_serialization_lib)

	set(boost_serialization_lib "")

endif()

target_link_libraries(coretest gtest gtest_main ${rocksdb_lib} ${mkl_rt_lib} ${boost_serialization_lib} ${jemalloc_lib} ibverbs boost_coroutine boost_chrono boost_thread boost_context boost_system r2)
add_dependencies(coretest jemalloc )

file(GLOB T_WO_SOURCES  "tests/test_list.cc" "tests/test_rdtsc.cc" "tests/test_ssched.cc")
add_executable(coretest_wo_rdma ${T_WO_SOURCES} "src/logging.cc")
target_link_libraries(coretest_wo_rdma gtest gtest_main r2 boost_context boost_system boost_coroutine boost_thread)
