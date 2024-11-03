#pragma once
#include <cstdint>
#include <type_traits>
#include <bit>

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

// SIMD support detection
#if defined(HOLOHASH_ARCH_X64)
    #include <immintrin.h>
    #define HOLOHASH_SIMD_SSE2
    #if defined(__AVX2__)
        #define HOLOHASH_SIMD_AVX2
    #endif
#elif defined(HOLOHASH_ARCH_ARM64)
    #include <arm_neon.h>
    #define HOLOHASH_SIMD_NEON
#endif

// Memory alignment helpers
inline constexpr bool is_aligned(const void* ptr, std::size_t alignment) noexcept {
    return (reinterpret_cast<std::uintptr_t>(ptr) % alignment) == 0;
}

// Platform-independent byte rotation
inline uint8_t rotate_left(uint8_t value, unsigned int count) {
    count &= 7;
    return static_cast<uint8_t>((value << count) | (value >> ((-count) & 7)));
}

// SIMD-optimized operations
#if defined(HOLOHASH_SIMD_AVX2)
inline void simd_xor_block(uint8_t* dst, const uint8_t* src, size_t len) {
    // Process 32-byte blocks with AVX2
    while (len >= 32 && is_aligned(dst, 32) && is_aligned(src, 32)) {
        __m256i a = _mm256_load_si256(reinterpret_cast<const __m256i*>(dst));
        __m256i b = _mm256_load_si256(reinterpret_cast<const __m256i*>(src));
        _mm256_store_si256(reinterpret_cast<__m256i*>(dst), _mm256_xor_si256(a, b));
        dst += 32;
        src += 32;
        len -= 32;
    }
    
    // Process remaining bytes with SSE2
    while (len >= 16 && is_aligned(dst, 16) && is_aligned(src, 16)) {
        __m128i a = _mm_load_si128(reinterpret_cast<const __m128i*>(dst));
        __m128i b = _mm_load_si128(reinterpret_cast<const __m128i*>(src));
        _mm_store_si128(reinterpret_cast<__m128i*>(dst), _mm_xor_si128(a, b));
        dst += 16;
        src += 16;
        len -= 16;
    }
    
    // Handle remaining bytes
    while (len--) {
        *dst++ ^= *src++;
    }
}
#elif defined(HOLOHASH_SIMD_NEON)
inline void simd_xor_block(uint8_t* dst, const uint8_t* src, size_t len) {
    // Process 16-byte blocks with NEON
    while (len >= 16) {
        uint8x16_t a = vld1q_u8(dst);
        uint8x16_t b = vld1q_u8(src);
        vst1q_u8(dst, veorq_u8(a, b));
        dst += 16;
        src += 16;
        len -= 16;
    }
    
    // Handle remaining bytes
    while (len--) {
        *dst++ ^= *src++;
    }
}
#else
inline void simd_xor_block(uint8_t* dst, const uint8_t* src, size_t len) {
    // Fallback implementation for platforms without SIMD
    while (len--) {
        *dst++ ^= *src++;
    }
}
#endif

// Cache line size detection
inline constexpr size_t get_cache_line_size() {
#if defined(HOLOHASH_ARCH_X64)
    return 64; // Most x86_64 processors use 64-byte cache lines
#elif defined(HOLOHASH_ARCH_ARM64)
    return 64; // Most ARM64 processors also use 64-byte cache lines
#else
    return 64; // Default assumption
#endif
}

} // namespace platform
} // namespace holohash
