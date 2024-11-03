#include <holohash/benchmark.hpp>
#include <random>

using namespace holohash;
using namespace holohash::benchmark;

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

void run_hash_benchmarks() {
    std::cout << "\n=== Hash Function Benchmarks ===\n";
    
    SessionParams params{
        "127.0.0.1",
        "192.168.1.1",
        std::chrono::system_clock::now(),
        {}
    };

    const std::vector<size_t> sizes = {64, 256, 1024, 4096, 16384};
    
    for (size_t size : sizes) {
        auto data = generate_random_data(size);
        
        auto result = run_benchmark(
            "Hash computation",
            1000,
            size,
            [&]() {
                HolographicHash::compute(data, params);
            }
        );
        
        print_result(result);
    }
}

void run_nonce_benchmarks() {
    std::cout << "\n=== Nonce Generation Benchmarks ===\n";
    
    SystemState state{
        "content_hash",
        50.0,
        1024*1024,
        std::chrono::system_clock::now(),
        {}
    };

    const std::vector<size_t> sizes = {64, 256, 1024, 4096, 16384};
    
    for (size_t size : sizes) {
        auto data = generate_random_data(size);
        
        auto result = run_benchmark(
            "Nonce generation",
            1000,
            size,
            [&]() {
                EmergentNonce::generate(data, state);
            }
        );
        
        print_result(result);
    }
}

void run_keychain_benchmarks() {
    std::cout << "\n=== Keychain Benchmarks ===\n";
    
    Keychain keychain;
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

    const std::vector<size_t> sizes = {64, 256, 1024, 4096, 16384};
    
    for (size_t size : sizes) {
        auto data = generate_random_data(size);
        
        auto result = run_benchmark(
            "Key generation and validation",
            1000,
            size,
            [&]() {
                auto key = keychain.generate_key(data, params, state);
                keychain.validate_key(key, params, state);
            }
        );
        
        print_result(result);
    }
}

int main() {
    try {
        run_hash_benchmarks();
        run_nonce_benchmarks();
        run_keychain_benchmarks();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
