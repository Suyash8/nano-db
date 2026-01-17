cmake_minimum_required(VERSION 3.15)
project(nano-db)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723
)
FetchContent_MakeAvailable(googletest)

add_executable(nanodb_tests 
    tests/test_main.cpp
    tests/test_simple_ts.cpp
)

target_include_directories(nanodb_tests PRIVATE include)

target_link_libraries(nanodb_tests gtest_main)