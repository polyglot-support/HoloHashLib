#pragma once
#include <stdexcept>
#include <string>

namespace holohash {

class HoloHashException : public std::runtime_error {
public:
    explicit HoloHashException(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidInputException : public HoloHashException {
public:
    explicit InvalidInputException(const std::string& msg) : HoloHashException(msg) {}
};

class NonceGenerationException : public HoloHashException {
public:
    explicit NonceGenerationException(const std::string& msg) : HoloHashException(msg) {}
};

class KeychainException : public HoloHashException {
public:
    explicit KeychainException(const std::string& msg) : HoloHashException(msg) {}
};

} // namespace holohash
