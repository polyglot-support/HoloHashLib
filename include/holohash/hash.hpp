#pragma once
#include "types.hpp"
#include "exceptions.hpp"
#include "platform.hpp"
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
                iv[offset + i] = platform::rotate_left(iv[offset + i], 3);
            }
        };

        hash_component(params.source_ip, 0);
        hash_component(params.dest_ip, 4);
        
        // Include timestamp with better mixing
        auto ts = params.timestamp.time_since_epoch().count();
        for (size_t i = 0; i < 8; ++i) {
            iv[8 + i] ^= static_cast<uint8_t>(ts >> (i * 8));
            iv[8 + i] = platform::rotate_left(iv[8 + i], i + 1);
        }

        return iv;
    }

    static void apply_holographic_transform(
        std::span<const uint8_t> input,
        const std::array<uint8_t, 16>& iv,
        std::array<uint8_t, 32>& result
    ) {
        std::mt19937_64 rng(std::bit_cast<uint64_t>(iv.data()));
        
        // Initialize result with input data using SIMD when possible
        for (size_t i = 0; i < result.size(); i += platform::get_cache_line_size()) {
            size_t chunk_size = std::min(platform::get_cache_line_size(), result.size() - i);
            for (size_t j = 0; j < chunk_size; ++j) {
                result[i + j] = input[(i + j) % input.size()];
            }
        }
        
        // Multiple rounds of mixing for better diffusion
        for (size_t round = 0; round < 8; ++round) {
            // Process data in cache-line sized chunks
            for (size_t i = 0; i < result.size(); i += platform::get_cache_line_size()) {
                size_t chunk_size = std::min(platform::get_cache_line_size(), result.size() - i);
                
                // Mix with random input bytes using SIMD
                std::array<uint8_t, 32> temp{};
                for (size_t j = 0; j < chunk_size; ++j) {
                    size_t idx = (rng() % input.size());
                    temp[j] = input[idx];
                }
                platform::simd_xor_block(result.data() + i, temp.data(), chunk_size);
                
                // Apply rotations and additional mixing
                for (size_t j = 0; j < chunk_size; ++j) {
                    result[i + j] = platform::rotate_left(result[i + j], 3);
                    result[i + j] += iv[j % iv.size()];
                }
            }
            
            // Mix with neighboring bytes
            mix_round(result);
        }
    }

    static void mix_round(std::array<uint8_t, 32>& data) {
        // Process in cache-line sized chunks for better cache utilization
        for (size_t i = 0; i < data.size(); i += platform::get_cache_line_size()) {
            size_t chunk_size = std::min(platform::get_cache_line_size(), data.size() - i);
            
            for (size_t j = 0; j < chunk_size; ++j) {
                size_t idx = i + j;
                uint8_t prev = data[(idx + data.size() - 1) % data.size()];
                uint8_t next = data[(idx + 1) % data.size()];
                
                data[idx] = platform::rotate_left(data[idx], 3) ^ prev;
                data[idx] = platform::rotate_left(data[idx], 2) ^ next;
                data[idx] = platform::rotate_left(data[idx], 1);
            }
        }
    }
};

} // namespace holohash
