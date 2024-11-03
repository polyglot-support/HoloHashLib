cmake_minimum_required(VERSION 3.20)
project(holohash VERSION 1.0.0 LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find Catch2 package
find_package(Catch2 REQUIRED)

# Add test executable
add_executable(holohash_tests
    tests/test_hash.cpp
    tests/test_nonce.cpp
    tests/test_keychain.cpp
    tests/test_security.cpp
)

# Add benchmark executable
add_executable(holohash_bench
    benchmarks/benchmark_main.cpp
)

# Set optimization flags for benchmarking
target_compile_options(holohash_bench PRIVATE -O3 -march=native)

# Link Catch2
target_link_libraries(holohash_tests PRIVATE Catch2::Catch2)

# Enable testing
enable_testing()
add_test(NAME holohash_tests COMMAND holohash_tests)
