cmake_minimum_required(VERSION 3.20)
project(holohash VERSION 1.0.0 LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings and basic optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
    # Ensure C++20 compatibility
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /O2 /std:c++20 /permissive-")
endif()

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

# Link against standard libraries
target_link_libraries(holohash_tests PRIVATE Catch2::Catch2)
target_link_libraries(holohash_bench PRIVATE)

# Enable testing
enable_testing()
add_test(NAME holohash_tests COMMAND holohash_tests)
