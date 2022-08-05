cmake_minimum_required(VERSION 3.14)

set(PROJECT test)

project(${PROJECT})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wpedantic")

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        main
)

set(BUILD_GMOCK OFF)

FetchContent_MakeAvailable(googletest)

set(SOURCES
    testcases/info.cpp
    testcases/accessors.cpp
    testcases/constructors.cpp
)

add_executable(${PROJECT} ${SOURCES})

target_include_directories(${PROJECT} PRIVATE ../)

target_link_libraries(${PROJECT} gtest_main)
