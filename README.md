# HoloHash Library

A C++20 library implementing a one-way holographic hash function with emergent nonce mechanism. This library provides a unique approach to cryptographic hashing by incorporating context-sensitive key generation and dynamic nonce evolution.

## Features

- **One-Way Holographic Hash Function**: A non-invertible hashing algorithm that creates holographic-like hashes where each fragment contains partial information about the whole.
- **Emergent Nonce Generation**: Dynamic, context-aware nonce generation that evolves based on system state and content.
- **Dynamic Keychain Management**: Secure key generation and management with context sensitivity and forward secrecy.

## Requirements

- C++20 compatible compiler
- CMake 3.20 or higher
- Catch2 (for testing)

## Installation

```bash
git clone <repository-url>
cd holohash
mkdir build && cd build
cmake ..
cmake --build .
```

## Basic Usage

Here's a simple example of using the holographic hash function:

```cpp
#include <holohash/core.hpp>
#include <vector>
#include <string>

int main() {
    // Create input data
    std::string input = "Hello, World!";
    std::vector<uint8_t> data(input.begin(), input.end());

    // Set up session parameters
    holohash::SessionParams params{
        "127.0.0.1",                        // source IP
        "192.168.1.1",                      // destination IP
        std::chrono::system_clock::now(),   // timestamp
        {}                                  // additional metadata
    };

    // Generate hash
    auto hash = holohash::HolographicHash::compute(data, params);
    
    return 0;
}
```

## API Documentation

### Core Components

#### HolographicHash

```cpp
static Hash compute(std::span<const uint8_t> input, const SessionParams& params);
```
Computes a holographic hash of the input data using the provided session parameters.

#### EmergentNonce

```cpp
static Nonce generate(std::span<const uint8_t> input, const SystemState& state);
```
Generates a context-sensitive nonce based on input data and system state.

#### Keychain

```cpp
Key generate_key(std::span<const uint8_t> input, const SessionParams& params, const SystemState& state);
bool validate_key(const Key& key, const SessionParams& params, const SystemState& state);
```
Manages key generation and validation with context awareness.

### Data Types

#### SessionParams
```cpp
struct SessionParams {
    std::string source_ip;
    std::string dest_ip;
    std::chrono::system_clock::time_point timestamp;
    std::vector<uint8_t> metadata;
};
```

#### SystemState
```cpp
struct SystemState {
    std::string content_hash;
    double cpu_load;
    uint64_t memory_usage;
    std::chrono::system_clock::time_point timestamp;
    std::vector<uint8_t> previous_nonce;
};
```

## Advanced Usage Examples

### Key Generation with Context

```cpp
#include <holohash/core.hpp>

// Create keychain
holohash::Keychain keychain;

// Set up parameters
holohash::SessionParams params{
    "127.0.0.1", "192.168.1.1",
    std::chrono::system_clock::now(), {}
};

holohash::SystemState state{
    "content_hash", 45.5, 2048*1024,
    std::chrono::system_clock::now(), {}
};

// Generate and validate key
std::vector<uint8_t> data = {...};
auto key = keychain.generate_key(data, params, state);
bool is_valid = keychain.validate_key(key, params, state);
```

### Benchmarking

The library includes built-in benchmarking tools:

```cpp
#include <holohash/benchmark.hpp>

// Run benchmark
auto result = holohash::benchmark::run_benchmark(
    "Hash computation",
    1000,  // iterations
    1024,  // data size
    [&]() {
        HolographicHash::compute(data, params);
    }
);

// Print results
holohash::benchmark::print_result(result);
```

## Security Features

- **Avalanche Effect**: Small input changes cause significant output differences
- **Collision Resistance**: Designed to minimize hash collisions
- **Context Sensitivity**: Hash output depends on both data and session context
- **Forward Secrecy**: Keys are session-specific and non-replayable

## Testing

Run the test suite:

```bash
cd build
ctest --output-on-failure
```

## Performance

The library is optimized for:
- Fast hash computation
- Efficient nonce generation
- Quick key validation

Benchmark results are available in the `benchmarks/` directory.

## Contributing

Please read our contributing guidelines and code of conduct before submitting pull requests.

## License

[License details here]

## Contact

[Contact information here]
