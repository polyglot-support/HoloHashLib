cmake_minimum_required(VERSION 3.20)
project(holohash VERSION 1.0.0 LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -msse4.2")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a+simd")
    endif()
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
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

# Add benchmark executable with maximum optimizations
add_executable(holohash_bench
    benchmarks/benchmark_main.cpp
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(holohash_bench PRIVATE -O3 -march=native -ffast-math)
elseif(MSVC)
    target_compile_options(holohash_bench PRIVATE /O2 /arch:AVX2 /fp:fast)
endif()

# Link Catch2
target_link_libraries(holohash_tests PRIVATE Catch2::Catch2)

# Enable testing
enable_testing()
add_test(NAME holohash_tests COMMAND holohash_tests)
