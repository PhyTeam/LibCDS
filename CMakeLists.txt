cmake_minimum_required(VERSION 2.8.12)
project(LibCDS)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive -Wno-unused-variable")
if(COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()


set(HEADER 
    include/priorityqueue.h
    include/concurrencyqueue.h
    include/blockingconcurrentqueue.h)

set(SOURCE
    src/main.cpp)

enable_testing()
add_subdirectory(thirdparty/googletest)

add_subdirectory(tests)

add_subdirectory(benchmark)


add_library(${PROJECT_NAME} ${HEADER} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${LIB})


