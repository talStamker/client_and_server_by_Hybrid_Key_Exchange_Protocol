#include "bytesCodeCasting.h"
#include <iostream>
#include <iomanip>
#include <boost/asio.hpp>

//decode function
std::string decodeByteCodeToHexString(const std::vector<uint8_t>& vec) {
    std::ostringstream oss;
    for (auto byte : vec) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}
std::string bytesToHexString(const char* bytes, size_t length) {
    std::vector<uint8_t> vec(bytes, bytes + length);
    std::string str= decodeByteCodeToHexString(vec);
    return str;
}
std::string decodeByteCodeToString(const std::vector<uint8_t>& vec) {
    // Find the position of the null terminator
    auto null_pos = std::find(vec.begin(), vec.end(), '\0');

    // Create a string from the byte code up to the null terminator
    return std::string(vec.begin(), null_pos);
}

//encode function
uint16_t toLittleEndian16(uint16_t value) {
    return (value >> 8) | (value << 8);
}

uint32_t toLittleEndian32(uint32_t value) {
    return ((value >> 24) & 0x000000FF) |
        ((value >> 8) & 0x0000FF00) |
        ((value << 8) & 0x00FF0000) |
        ((value << 24) & 0xFF000000);
}

void encodeStringToByteCode(std::vector<uint8_t>& vec, std::string str, int stringSizeOnumberOfByte) {
    vec.resize(stringSizeOnumberOfByte, 0);
    std::memcpy(vec.data(), str.c_str(), std::min(str.size(), static_cast<std::size_t>(stringSizeOnumberOfByte)));
    //vec[std::min(str.size(), static_cast<std::size_t>(stringSizeOnumberOfByte - 1))] = '\0';
}
void encodeStringInArrToByteCode(std::vector<uint8_t>& vec, const char* str, int stringSizeOnumberOfByte) {
    vec.resize(stringSizeOnumberOfByte, 0);
    std::memcpy(vec.data(), str, std::min(std::strlen(str), static_cast<std::size_t>(stringSizeOnumberOfByte)));
}
// Function to convert a hex character to its integer value
uint8_t hexCharToByte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    throw std::invalid_argument("Invalid hex character");
}

// Function to encode a 32-character hex string to a 16-byte binary format
void encodeHexStringToByteCode(std::vector<uint8_t>& vec, const std::string& hexStr, int byteSize) {
    if (hexStr.size() != 2 * byteSize) {
        throw std::invalid_argument("Hex string length does not match the expected byte size");
    }
    vec.resize(byteSize);
    for (int i = 0; i < byteSize; ++i) {
        vec[i] = (hexCharToByte(hexStr[2 * i]) << 4) | hexCharToByte(hexStr[2 * i + 1]);
    }
}
