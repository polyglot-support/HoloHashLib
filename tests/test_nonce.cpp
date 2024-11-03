#include <catch2/catch.hpp>
#include <holohash/core.hpp>
#include <vector>
#include <string>

using namespace holohash;

TEST_CASE("EmergentNonce generation", "[nonce]") {
    std::string input = "test data";
    SystemState state{
        "content_hash",
        50.0,
        1024*1024,
        std::chrono::system_clock::now(),
        {}
    };

    SECTION("Generate nonce with valid input") {
        std::vector<uint8_t> data(input.begin(), input.end());
        REQUIRE_NOTHROW(EmergentNonce::generate(data, state));
    }

    SECTION("Empty input throws exception") {
        std::vector<uint8_t> empty_data;
        REQUIRE_THROWS_AS(
            EmergentNonce::generate(empty_data, state),
            NonceGenerationException
        );
    }

    SECTION("Different states produce different nonces") {
        std::vector<uint8_t> data(input.begin(), input.end());
        
        SystemState state1 = state;
        SystemState state2 = state;
        state2.cpu_load = 75.0;

        auto nonce1 = EmergentNonce::generate(data, state1);
        auto nonce2 = EmergentNonce::generate(data, state2);

        REQUIRE(nonce1.get() != nonce2.get());
    }
}
