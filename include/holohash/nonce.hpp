#pragma once
#include "types.hpp"
#include "exceptions.hpp"
#include "platform.hpp"
#include <random>
#include <functional>

namespace holohash {

class EmergentNonce {
public:
    static Nonce generate(std::span<const uint8_t> input, const SystemState& state) {
        if (input.empty()) {
            throw NonceGenerationException("Input data cannot be empty");
        }

        std::array<uint8_t, 16> nonce{};
        
        // Mix system state into nonce
        mix_system_state(state, nonce);
        
        // Apply recursive transformation
        apply_recursive_transform(input, state.previous_nonce, nonce);
        
        return Nonce{nonce};
    }

private:
    static void mix_system_state(const SystemState& state, std::array<uint8_t, 16>& nonce) {
        // Mix content hash using SIMD
        if (!state.content_hash.empty()) {
            platform::simd_xor_block(nonce.data(), 
                reinterpret_cast<const uint8_t*>(state.content_hash.data()),
                std::min(state.content_hash.size(), size_t{8}));
        }

        // Mix CPU load and memory usage
        auto cpu = static_cast<uint8_t>(state.cpu_load * 255);
        auto mem = static_cast<uint64_t>(state.memory_usage);
        
        nonce[8] ^= cpu;
        
        // Use platform-specific optimizations for memory mixing
        for (size_t i = 0; i < 8; ++i) {
            nonce[8 + i] ^= static_cast<uint8_t>(mem >> (i * 8));
            nonce[8 + i] = platform::rotate_left(nonce[8 + i], i + 1);
        }
    }

    static void apply_recursive_transform(
        std::span<const uint8_t> input,
        std::span<const uint8_t> previous_nonce,
        std::array<uint8_t, 16>& nonce
    ) {
        std::mt19937 rng(std::hash<std::string>{}(std::string(input.begin(), input.end())));
        
        // Apply recursive transformation using SIMD when possible
        if (!previous_nonce.empty()) {
            platform::simd_xor_block(nonce.data(), previous_nonce.data(),
                std::min(previous_nonce.size(), nonce.size()));
        }
        
        // Process in cache-line sized chunks
        for (size_t i = 0; i < nonce.size(); i += platform::get_cache_line_size()) {
            size_t chunk_size = std::min(platform::get_cache_line_size(), nonce.size() - i);
            
            // Mix with input data
            std::array<uint8_t, 16> temp{};
            for (size_t j = 0; j < chunk_size; ++j) {
                for (size_t k = 0; k < input.size(); ++k) {
                    size_t idx = (k + rng()) % input.size();
                    temp[j] ^= input[idx];
                }
                temp[j] = platform::rotate_left(temp[j], 3);
            }
            
            platform::simd_xor_block(nonce.data() + i, temp.data(), chunk_size);
        }
    }
};

} // namespace holohash
