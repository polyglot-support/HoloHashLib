#include <catch2/catch.hpp>
#include <holohash/platform.hpp>
#include <vector>

using namespace holohash::platform;

TEST_CASE("Alignment check functionality", "[platform]") {
    // Test aligned pointer
    alignas(64) std::array<char, 128> aligned_data{};
    REQUIRE(is_aligned(aligned_data.data(), 64));
    REQUIRE(is_aligned(aligned_data.data(), 32));
    REQUIRE(is_aligned(aligned_data.data(), 16));
    REQUIRE(is_aligned(aligned_data.data(), 8));

    // Test non power-of-2 alignment
    REQUIRE_FALSE(is_aligned(aligned_data.data(), 3));
    REQUIRE_FALSE(is_aligned(aligned_data.data(), 7));
    
    // Test nullptr
    REQUIRE(is_aligned(static_cast<int*>(nullptr), 8));
    
    // Test various pointer types
    int x = 0;
    REQUIRE(is_aligned(&x, sizeof(int)));
    
    double d = 0.0;
    REQUIRE(is_aligned(&d, sizeof(double)));
}
