#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include <holohash/core.hpp>
#include <vector>
#include <string>

using namespace holohash;

TEST_CASE("HolographicHash basic functionality", "[hash]") {
    std::string input = "test data";
    SessionParams params{
        "127.0.0.1",
        "192.168.1.1",
        std::chrono::system_clock::now(),
        {}
    };

    SECTION("Compute hash with valid input") {
        std::vector<uint8_t> data(input.begin(), input.end());
        REQUIRE_NOTHROW(HolographicHash::compute(data, params));
    }

    SECTION("Empty input throws exception") {
        std::vector<uint8_t> empty_data;
        REQUIRE_THROWS_AS(
            HolographicHash::compute(empty_data, params),
            InvalidInputException
        );
    }

    SECTION("Different inputs produce different hashes") {
        std::string input1 = "test data 1";
        std::string input2 = "test data 2";
        std::vector<uint8_t> data1(input1.begin(), input1.end());
        std::vector<uint8_t> data2(input2.begin(), input2.end());

        auto hash1 = HolographicHash::compute(data1, params);
        auto hash2 = HolographicHash::compute(data2, params);

        REQUIRE(hash1.get() != hash2.get());
    }
}
