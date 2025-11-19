cmake_minimum_required(VERSION 3.20)

project(hw5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(hw5_headers INTERFACE)
target_include_directories(hw5_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(hw5_app src/main.cpp)
target_link_libraries(hw5_app PRIVATE hw5_headers)

include(CTest)

if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(hw5_tests tests/basic_tests.cpp)
    target_link_libraries(hw5_tests PRIVATE hw5_headers GTest::gtest_main)
    add_test(NAME hw5_tests COMMAND hw5_tests)
endif()

