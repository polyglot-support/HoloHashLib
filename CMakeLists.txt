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

# Fetch and configure Catch2
include(FetchContent)
FetchContent_Declare(
    Catch2
    URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.4.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(Catch2)

# Add test executable
add_executable(holohash_tests
    tests/test_hash.cpp
    tests/test_nonce.cpp
    tests/test_keychain.cpp
    tests/test_security.cpp
    tests/test_platform.cpp
)
target_link_libraries(holohash_tests PRIVATE Catch2::Catch2WithMain)

# Add benchmark executable
add_executable(holohash_bench
    benchmarks/benchmark_main.cpp
)

# Enable testing
enable_testing()
add_test(NAME holohash_tests COMMAND holohash_tests)
