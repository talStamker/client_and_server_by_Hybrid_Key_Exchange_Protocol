#ifndef BYTES_CODE_CASTING_H
#define BYTES_CODE_CASTING_H
#include <vector>
#include <string>

std::string decodeByteCodeToHexString(const std::vector<uint8_t>& vec);
std::string bytesToHexString(const char* bytes, size_t length);
std::string decodeByteCodeToString(const std::vector<uint8_t>& vec);
uint16_t toLittleEndian16(uint16_t value);
uint32_t toLittleEndian32(uint32_t value);
void encodeHexStringToByteCode(std::vector<uint8_t>& vec, const std::string& hexStr, int byteSize);
void encodeStringToByteCode(std::vector<uint8_t>& vec, std::string str, int stringSizeOnumberOfByte);
void encodeStringInArrToByteCode(std::vector<uint8_t>& vec, const char* str, int stringSizeOnumberOfByte);
#endif