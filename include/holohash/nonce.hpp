#pragma once
#include "types.hpp"
#include "exceptions.hpp"
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
        // Mix content hash
        for (size_t i = 0; i < std::min(state.content_hash.size(), size_t{8}); ++i) {
            nonce[i] ^= state.content_hash[i];
        }

        // Mix CPU load and memory usage
        auto cpu = static_cast<uint8_t>(state.cpu_load * 255);
        auto mem = static_cast<uint64_t>(state.memory_usage);
        
        nonce[8] ^= cpu;
        for (size_t i = 0; i < 8; ++i) {
            nonce[8 + i] ^= static_cast<uint8_t>(mem >> (i * 8));
        }
    }

    static void apply_recursive_transform(
        std::span<const uint8_t> input,
        std::span<const uint8_t> previous_nonce,
        std::array<uint8_t, 16>& nonce
    ) {
        std::mt19937 rng(std::hash<std::string>{}(std::string(input.begin(), input.end())));
        
        // Apply recursive transformation using previous nonce
        for (size_t i = 0; i < nonce.size(); ++i) {
            uint8_t mixed = nonce[i];
            
            if (!previous_nonce.empty()) {
                mixed ^= previous_nonce[i % previous_nonce.size()];
            }
            
            // Mix with input data
            for (size_t j = 0; j < input.size(); ++j) {
                size_t idx = (j + rng()) % input.size();
                mixed ^= input[idx];
                mixed = std::rotl(mixed, 3);
            }
            
            nonce[i] = mixed;
        }
    }
};

} // namespace holohash
