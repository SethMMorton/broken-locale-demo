cmake_minimum_required (VERSION 3.14)
project (locale-demo)
enable_testing()
set(CMAKE_CXX_STANDARD 17)

# Grab gtest from the web
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(locale-demo locale-demo.cpp)
target_link_libraries(locale-demo GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(locale-demo)
