#pragma once
#include <string>
#include <chrono>
#include <array>
#include <vector>
#include <cstdint>
#include <functional>

namespace holohash {

// Strong type pattern for type safety
template<typename T, typename Tag>
class StrongType {
    T value_;
public:
    explicit StrongType(const T& value) : value_(value) {}
    explicit StrongType(T&& value) : value_(std::move(value)) {}
    T& get() { return value_; }
    const T& get() const { return value_; }
    
    bool operator==(const StrongType& other) const {
        return value_ == other.value_;
    }
    
    bool operator!=(const StrongType& other) const {
        return !(*this == other);
    }
};

// Session parameters struct
struct SessionParams {
    std::string source_ip;
    std::string dest_ip;
    std::chrono::system_clock::time_point timestamp;
    std::vector<uint8_t> metadata;

    bool operator==(const SessionParams&) const = default;
};

// System state struct
struct SystemState {
    std::string content_hash;
    double cpu_load;
    uint64_t memory_usage;
    std::chrono::system_clock::time_point timestamp;
    std::vector<uint8_t> previous_nonce;

    bool operator==(const SystemState&) const = default;
};

// Type-safe wrappers
using Hash = StrongType<std::array<uint8_t, 32>, struct HashTag>;
using Nonce = StrongType<std::array<uint8_t, 16>, struct NonceTag>;
using Key = StrongType<std::array<uint8_t, 32>, struct KeyTag>;

} // namespace holohash

// Custom hash implementation for Key type
namespace std {
    template<>
    struct hash<holohash::Key> {
        size_t operator()(const holohash::Key& k) const {
            const auto& arr = k.get();
            size_t hash = 0;
            for (const auto& byte : arr) {
                hash = (hash * 31) + byte;
            }
            return hash;
        }
    };
}
