cmake_minimum_required(VERSION 3.14)
project(DimensionalAnalysisLibrary CXX)

# Policy to suppress the FetchContent timestamp warning we saw earlier
cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Explicitly find the Threads package
find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

include_directories(include)

# 2. Build the demo
add_executable(engine_demo src/main.cpp)
# Now Threads::Threads is a recognized target
target_link_libraries(engine_demo PRIVATE Threads::Threads)

# 3. Build the tests
add_executable(engine_tests tests/unit_tests.cpp)
target_link_libraries(engine_tests PRIVATE GTest::gtest_main Threads::Threads)