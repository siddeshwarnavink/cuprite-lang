cmake_minimum_required(VERSION 3.10)
project(cuprite VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC_FILES "src/*.c")

add_library(mylib ${SRC_FILES})
target_include_directories(mylib PUBLIC src)

add_executable(cuprite ${SRC_FILES})
target_link_libraries(cuprite PRIVATE mylib)

enable_testing()

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_FILES "tests/*.cpp")

add_executable(test_mylib ${TEST_FILES})
target_link_libraries(test_mylib PRIVATE mylib gtest gtest_main)
target_compile_options(test_mylib PRIVATE -Wno-write-strings)

add_test(NAME cupriteTest COMMAND test_mylib)
