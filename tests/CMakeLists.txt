#tests
include_directories(${CMAKE_SOURCE_DIR}/include)

function(build_test test_name)
    include_directories(${SRC_DIR})
    add_executable(${test_name}_test ${ARGN})

    target_link_libraries(${test_name}_test pthread gtest gmock gtest_main)
    add_test(${test_name} ./${test_name}_test)
endfunction()

build_test(priority_queue priority_queue.t.cpp)
