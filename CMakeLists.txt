cmake_minimum_required(VERSION 3.14)

project(laba_5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
add_executable(laba5
    include/linked_list.h
    include/my_memory_resource.h
    src/main.cpp
)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests 
    tests/main_test.cpp
    tests/my_memory_resource_test.cpp
    tests/linked_list_test.cpp
    include/linked_list.h
    include/my_memory_resource.h
)

target_link_libraries(tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)