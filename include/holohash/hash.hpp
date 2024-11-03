#pragma once
#include "types.hpp"
#include "exceptions.hpp"
#include <span>
#include <array>
#include <bit>
#include <random>

namespace holohash {

class HolographicHash {
public:
    static Hash compute(std::span<const uint8_t> input, const SessionParams& params) {
        if (input.empty()) {
            throw InvalidInputException("Input data cannot be empty");
        }

        std::array<uint8_t, 32> result{};
        
        // Initialize with session parameters
        auto init_vector = initialize_vector(params);
        
        // Apply holographic transformation
        apply_holographic_transform(input, init_vector, result);
        
        // Additional mixing rounds for better diffusion
        for (int round = 0; round < 4; ++round) {
            mix_round(result);
        }
        
        return Hash{result};
    }

private:
    static std::array<uint8_t, 16> initialize_vector(const SessionParams& params) {
        std::array<uint8_t, 16> iv{};
        
        // Mix session parameters into initialization vector
        auto hash_component = [&](const auto& data, size_t offset) {
            for (size_t i = 0; i < data.size() && i < 4; ++i) {
                iv[offset + i] ^= data[i];
                iv[offset + i] = std::rotl(iv[offset + i], 3);
            }
        };

        hash_component(params.source_ip, 0);
        hash_component(params.dest_ip, 4);
        
        // Include timestamp with better mixing
        auto ts = params.timestamp.time_since_epoch().count();
        for (size_t i = 0; i < 8; ++i) {
            iv[8 + i] ^= static_cast<uint8_t>(ts >> (i * 8));
            iv[8 + i] = std::rotl(iv[8 + i], i + 1);
        }

        return iv;
    }

    static void apply_holographic_transform(
        std::span<const uint8_t> input,
        const std::array<uint8_t, 16>& iv,
        std::array<uint8_t, 32>& result
    ) {
        std::mt19937_64 rng(std::bit_cast<uint64_t>(iv.data()));
        
        // Initialize result with input data
        for (size_t i = 0; i < result.size(); ++i) {
            size_t idx = i % input.size();
            result[i] = input[idx];
        }
        
        // Multiple rounds of mixing for better diffusion
        for (size_t round = 0; round < 8; ++round) {
            for (size_t i = 0; i < result.size(); ++i) {
                uint8_t mix = result[i];
                
                // Mix with random input bytes
                for (size_t j = 0; j < 4; ++j) {
                    size_t idx = (rng() % input.size());
                    mix ^= input[idx];
                    mix = std::rotl(mix, 3);
                    mix += iv[j % iv.size()];
                }
                
                // Mix with neighboring bytes
                mix ^= result[(i + 1) % result.size()];
                mix = std::rotl(mix, 2);
                mix ^= result[(i + result.size() - 1) % result.size()];
                
                result[i] = mix;
            }
        }
    }

    static void mix_round(std::array<uint8_t, 32>& data) {
        for (size_t i = 0; i < data.size(); ++i) {
            // Mix with neighbors using rotation and XOR
            uint8_t prev = data[(i + data.size() - 1) % data.size()];
            uint8_t next = data[(i + 1) % data.size()];
            
            data[i] = std::rotl(data[i], 3) ^ prev;
            data[i] = std::rotl(data[i], 2) ^ next;
            data[i] = std::rotl(data[i], 1);
        }
    }
};

} // namespace holohash
