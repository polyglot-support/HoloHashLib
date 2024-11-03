#include <holohash/core.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace holohash;

int main() {
    try {
        // Create sample input data
        std::string input = "Hello, World!";
        std::vector<uint8_t> data(input.begin(), input.end());

        // Create session parameters
        SessionParams params{
            "127.0.0.1",
            "192.168.1.1",
            std::chrono::system_clock::now(),
            {}
        };

        // Create system state
        SystemState state{
            "sample_content_hash",
            45.5,
            2048*1024,
            std::chrono::system_clock::now(),
            {}
        };

        // Create keychain
        Keychain keychain;

        // Generate and validate a key
        auto key = keychain.generate_key(data, params, state);
        bool is_valid = keychain.validate_key(key, params, state);

        std::cout << "Key validation result: " << (is_valid ? "valid" : "invalid") << std::endl;

        // Demonstrate hash computation
        auto hash = HolographicHash::compute(data, params);
        
        // Demonstrate nonce generation
        auto nonce = EmergentNonce::generate(data, state);

        std::cout << "Successfully generated cryptographic components" << std::endl;

    } catch (const HoloHashException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
