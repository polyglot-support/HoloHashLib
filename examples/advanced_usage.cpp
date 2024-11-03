#include <holohash/core.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <iomanip>

// Utility function to print binary data in hex format
void print_hex(const std::string& label, const auto& data) {
    std::cout << label << ": ";
    for (const auto& byte : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << "\n";
}

// Example demonstrating avalanche effect
void demonstrate_avalanche_effect() {
    std::cout << "\n=== Demonstrating Avalanche Effect ===\n";
    
    // Create two similar inputs
    std::string input1 = "test data";
    std::string input2 = "test datA";  // Only last character is different
    
    SessionParams params{
        "127.0.0.1",
        "192.168.1.1",
        std::chrono::system_clock::now(),
        {}
    };
    
    // Compute hashes
    auto hash1 = HolographicHash::compute(
        std::vector<uint8_t>(input1.begin(), input1.end()),
        params
    );
    
    auto hash2 = HolographicHash::compute(
        std::vector<uint8_t>(input2.begin(), input2.end()),
        params
    );
    
    print_hex("Hash 1", hash1.get());
    print_hex("Hash 2", hash2.get());
}

// Example of context-sensitive key generation
void demonstrate_context_sensitivity() {
    std::cout << "\n=== Demonstrating Context Sensitivity ===\n";
    
    Keychain keychain;
    std::string input = "sensitive data";
    std::vector<uint8_t> data(input.begin(), input.end());
    
    // Create two different contexts
    SessionParams params1{
        "192.168.1.1",
        "10.0.0.1",
        std::chrono::system_clock::now(),
        {}
    };
    
    SessionParams params2{
        "192.168.1.1",
        "10.0.0.2",  // Different destination
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
    
    // Generate keys in different contexts
    auto key1 = keychain.generate_key(data, params1, state);
    auto key2 = keychain.generate_key(data, params2, state);
    
    print_hex("Key 1", key1.get());
    print_hex("Key 2", key2.get());
    
    // Validate keys
    std::cout << "Key 1 valid in context 1: " 
              << keychain.validate_key(key1, params1, state) << "\n";
    std::cout << "Key 1 valid in context 2: " 
              << keychain.validate_key(key1, params2, state) << "\n";
}

// Example of emergent nonce evolution
void demonstrate_nonce_evolution() {
    std::cout << "\n=== Demonstrating Nonce Evolution ===\n";
    
    std::string input = "evolving data";
    std::vector<uint8_t> data(input.begin(), input.end());
    
    SystemState state{
        "initial_hash",
        30.0,
        512*1024,
        std::chrono::system_clock::now(),
        {}
    };
    
    // Generate initial nonce
    auto nonce1 = EmergentNonce::generate(data, state);
    print_hex("Initial Nonce", nonce1.get());
    
    // Evolve system state
    state.cpu_load = 45.0;
    state.memory_usage = 768*1024;
    state.previous_nonce = std::vector<uint8_t>(nonce1.get().begin(), nonce1.get().end());
    
    // Generate evolved nonce
    auto nonce2 = EmergentNonce::generate(data, state);
    print_hex("Evolved Nonce", nonce2.get());
}

// Example of performance benchmarking
void run_performance_benchmark() {
    std::cout << "\n=== Running Performance Benchmark ===\n";
    
    using namespace holohash::benchmark;
    
    std::vector<uint8_t> data(1024, 'x');  // 1KB of data
    SessionParams params{
        "127.0.0.1",
        "192.168.1.1",
        std::chrono::system_clock::now(),
        {}
    };
    
    auto result = run_benchmark(
        "Hash computation (1KB)",
        1000,
        data.size(),
        [&]() {
            HolographicHash::compute(data, params);
        }
    );
    
    print_result(result);
}

int main() {
    try {
        std::cout << "HoloHash Advanced Usage Examples\n"
                  << "================================\n";
        
        demonstrate_avalanche_effect();
        demonstrate_context_sensitivity();
        demonstrate_nonce_evolution();
        run_performance_benchmark();
        
        return 0;
    } catch (const HoloHashException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
