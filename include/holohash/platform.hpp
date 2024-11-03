#pragma once
#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <bit>
#include <array>

namespace holohash {
namespace platform {

// Platform detection
#if defined(_MSC_VER)
    #define HOLOHASH_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define HOLOHASH_PLATFORM_MACOS
#elif defined(__linux__)
    #define HOLOHASH_PLATFORM_LINUX
#endif

// Architecture detection
#if defined(__x86_64__) || defined(_M_X64)
    #define HOLOHASH_ARCH_X64
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define HOLOHASH_ARCH_ARM64
#endif

// Memory alignment helpers
namespace detail {
    // Check if a number is a power of 2 at compile time
    constexpr bool is_power_of_2(std::size_t x) noexcept {
        return x != 0 && (x & (x - 1)) == 0;
    }

    // Convert pointer to integer safely
    constexpr std::uintptr_t to_uintptr(const void* ptr) noexcept {
        return reinterpret_cast<std::uintptr_t>(ptr);
    }
}

// Platform-independent alignment check
template<typename T>
constexpr bool is_aligned(const T* ptr, std::size_t alignment) noexcept {
    static_assert(std::is_object_v<T>, "T must be an object type");
    static_assert(!std::is_function_v<T>, "T cannot be a function type");
    
    if (!detail::is_power_of_2(alignment)) {
        return false;
    }

    if (ptr == nullptr) {
        return true;
    }

    const auto addr = detail::to_uintptr(ptr);
    return (addr & (alignment - 1)) == 0;
}

// Platform-independent byte rotation
constexpr uint8_t rotate_left(uint8_t value, unsigned int count) noexcept {
    const unsigned int mask = 7;
    count &= mask;
    return static_cast<uint8_t>((value << count) | (value >> ((-count) & mask)));
}

// Simple block operations
inline void simd_xor_block(uint8_t* dst, const uint8_t* src, size_t len) noexcept {
    // Process data in 8-byte chunks for better performance
    size_t chunks = len / 8;
    for (size_t i = 0; i < chunks; ++i) {
        uint64_t* dst64 = reinterpret_cast<uint64_t*>(dst + i * 8);
        const uint64_t* src64 = reinterpret_cast<const uint64_t*>(src + i * 8);
        *dst64 ^= *src64;
    }
    
    // Handle remaining bytes
    for (size_t i = chunks * 8; i < len; ++i) {
        dst[i] ^= src[i];
    }
}

// Cache line size detection
constexpr size_t get_cache_line_size() noexcept {
    return 64; // Most modern processors use 64-byte cache lines
}

} // namespace platform
} // namespace holohash
