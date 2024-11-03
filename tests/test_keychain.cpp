#include <catch2/catch.hpp>
#include <holohash/core.hpp>
#include <vector>
#include <string>

using namespace holohash;

TEST_CASE("Keychain management", "[keychain]") {
    Keychain keychain;
    std::string input = "test data";
    std::vector<uint8_t> data(input.begin(), input.end());
    
    SessionParams params{
        "127.0.0.1",
        "192.168.1.1",
        std::chrono::system_clock::now(),
        {}
    };
    
    SystemState state{
        "content_hash",
        50.0,
        1024*1024,
        std::chrono::system_clock::now(),
        {}
    };

    SECTION("Generate and validate key") {
        auto key = keychain.generate_key(data, params, state);
        REQUIRE(keychain.validate_key(key, params, state));
    }

    SECTION("Invalid key validation") {
        auto key = keychain.generate_key(data, params, state);
        
        SessionParams different_params = params;
        different_params.source_ip = "192.168.1.2";
        
        REQUIRE_FALSE(keychain.validate_key(key, different_params, state));
    }

    SECTION("Different inputs produce different keys") {
        std::string input1 = "test data 1";
        std::string input2 = "test data 2";
        std::vector<uint8_t> data1(input1.begin(), input1.end());
        std::vector<uint8_t> data2(input2.begin(), input2.end());

        auto key1 = keychain.generate_key(data1, params, state);
        auto key2 = keychain.generate_key(data2, params, state);

        REQUIRE(key1.get() != key2.get());
    }
}
