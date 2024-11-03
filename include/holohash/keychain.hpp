#pragma once
#include "types.hpp"
#include "hash.hpp"
#include "nonce.hpp"
#include "exceptions.hpp"
#include <unordered_map>

namespace holohash {

class Keychain {
public:
    Key generate_key(
        std::span<const uint8_t> input,
        const SessionParams& params,
        const SystemState& state
    ) {
        auto hash = HolographicHash::compute(input, params);
        auto nonce = EmergentNonce::generate(input, state);
        
        std::array<uint8_t, 32> key{};
        combine_hash_and_nonce(hash, nonce, key);
        
        auto key_obj = Key{key};
        store_key(key_obj, params, state);
        
        return key_obj;
    }
    
    bool validate_key(
        const Key& key,
        const SessionParams& params,
        const SystemState& state
    ) {
        auto it = key_store_.find(key);
        if (it == key_store_.end()) {
            return false;
        }
        
        const auto& [stored_params, stored_state] = it->second;
        return stored_params == params && stored_state == state;
    }

private:
    struct KeyData {
        SessionParams params;
        SystemState state;
    };
    
    std::unordered_map<Key, KeyData> key_store_;

    static void combine_hash_and_nonce(
        const Hash& hash,
        const Nonce& nonce,
        std::array<uint8_t, 32>& key
    ) {
        const auto& hash_data = hash.get();
        const auto& nonce_data = nonce.get();
        
        // Combine hash and nonce using XOR and rotation
        for (size_t i = 0; i < key.size(); ++i) {
            key[i] = hash_data[i] ^ nonce_data[i % nonce_data.size()];
            key[i] = std::rotl(key[i], i % 8);
        }
    }

    void store_key(const Key& key, const SessionParams& params, const SystemState& state) {
        key_store_[key] = KeyData{params, state};
    }
};

} // namespace holohash
