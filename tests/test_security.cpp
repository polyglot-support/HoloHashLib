#include <catch2/catch.hpp>
#include <holohash/core.hpp>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <bitset>
#include <unordered_set>

using namespace holohash;

// Utility function to count differing bits
size_t count_differing_bits(const std::array<uint8_t, 32>& a, const std::array<uint8_t, 32>& b) {
    size_t diff_count = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        diff_count += std::bitset<8>(a[i] ^ b[i]).count();
    }
    return diff_count;
}

// Generate random data
std::vector<uint8_t> generate_random_data(size_t size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    std::vector<uint8_t> data(size);
    for (auto& byte : data) {
        byte = static_cast<uint8_t>(dis(gen));
    }
    return data;
}

TEST_CASE("Avalanche Effect Testing", "[security]") {
    const size_t NUM_TESTS = 1000;
    const double MIN_AVALANCHE_RATIO = 0.45; // At least 45% of bits should change
    
    SessionParams params{
        "127.0.0.1",
        "192.168.1.1",
        std::chrono::system_clock::now(),
        {}
    };

    SECTION("Single bit changes cause significant output changes") {
        size_t total_bit_differences = 0;
        
        for (size_t i = 0; i < NUM_TESTS; ++i) {
            auto data = generate_random_data(64);
            auto original_hash = HolographicHash::compute(data, params);
            
            // Flip a random bit
            size_t byte_idx = i % data.size();
            data[byte_idx] ^= (1 << (i % 8));
            
            auto modified_hash = HolographicHash::compute(data, params);
            
            total_bit_differences += count_differing_bits(
                original_hash.get(), 
                modified_hash.get()
            );
        }
        
        double average_bit_difference_ratio = 
            static_cast<double>(total_bit_differences) / (NUM_TESTS * 256);
        
        REQUIRE(average_bit_difference_ratio >= MIN_AVALANCHE_RATIO);
    }
}

TEST_CASE("Collision Resistance Testing", "[security]") {
    const size_t NUM_TESTS = 10000;
    
    SessionParams params{
        "127.0.0.1",
        "192.168.1.1",
        std::chrono::system_clock::now(),
        {}
    };
    
    SECTION("No collisions in different inputs") {
        std::unordered_set<std::string> seen_hashes;
        
        for (size_t i = 0; i < NUM_TESTS; ++i) {
            auto data = generate_random_data(64);
            auto hash = HolographicHash::compute(data, params);
            
            // Convert hash to string for storage
            std::string hash_str(hash.get().begin(), hash.get().end());
            
            // Ensure we haven't seen this hash before
            REQUIRE(seen_hashes.find(hash_str) == seen_hashes.end());
            seen_hashes.insert(hash_str);
        }
    }
}

TEST_CASE("Key Independence Testing", "[security]") {
    Keychain keychain;
    const size_t NUM_TESTS = 1000;
    
    SECTION("Similar inputs produce independent keys") {
        auto base_data = generate_random_data(64);
        
        for (size_t i = 0; i < NUM_TESTS; ++i) {
            auto data1 = base_data;
            auto data2 = base_data;
            
            // Make small change to second data
            data2[i % data2.size()] ^= 1;
            
            SessionParams params1{
                "127.0.0.1",
                "192.168.1.1",
                std::chrono::system_clock::now(),
                {}
            };
            
            SessionParams params2 = params1;
            
            SystemState state{
                "content_hash",
                50.0,
                1024*1024,
                std::chrono::system_clock::now(),
                {}
            };
            
            auto key1 = keychain.generate_key(data1, params1, state);
            auto key2 = keychain.generate_key(data2, params2, state);
            
            // Keys should be significantly different
            size_t diff_bits = count_differing_bits(key1.get(), key2.get());
            REQUIRE(diff_bits >= 96); // At least 30% of bits should differ
        }
    }
}

TEST_CASE("Timing Attack Resistance", "[security]") {
    const size_t NUM_TESTS = 1000;
    
    SECTION("Key validation timing should be constant") {
        Keychain keychain;
        std::vector<uint8_t> data = generate_random_data(64);
        
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
        
        auto valid_key = keychain.generate_key(data, params, state);
        
        std::vector<double> valid_timings;
        std::vector<double> invalid_timings;
        
        for (size_t i = 0; i < NUM_TESTS; ++i) {
            // Test valid key timing
            auto start = std::chrono::high_resolution_clock::now();
            keychain.validate_key(valid_key, params, state);
            auto end = std::chrono::high_resolution_clock::now();
            valid_timings.push_back(std::chrono::duration<double>(end - start).count());
            
            // Test invalid key timing
            auto invalid_data = generate_random_data(64);
            auto invalid_key = keychain.generate_key(invalid_data, params, state);
            
            start = std::chrono::high_resolution_clock::now();
            keychain.validate_key(invalid_key, params, state);
            end = std::chrono::high_resolution_clock::now();
            invalid_timings.push_back(std::chrono::duration<double>(end - start).count());
        }
        
        // Calculate average timings
        double avg_valid = std::accumulate(valid_timings.begin(), valid_timings.end(), 0.0) / NUM_TESTS;
        double avg_invalid = std::accumulate(invalid_timings.begin(), invalid_timings.end(), 0.0) / NUM_TESTS;
        
        // Timing difference should be minimal (within 10%)
        double timing_ratio = avg_valid / avg_invalid;
        REQUIRE(timing_ratio >= 0.9);
        REQUIRE(timing_ratio <= 1.1);
    }
}
