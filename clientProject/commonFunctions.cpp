#include <fstream>
#include <boost/asio.hpp>
#include <iostream>
#include "commonFunctions.h"
#include <iomanip>
#include "bytesCodeCasting.h"
#include <boost/asio.hpp>
#include "client.h"
using boost::asio::ip::tcp;

//hendle server response
void handlingHeaderResponse(const char* response,ResponseHeader& resHeader) {
    std::memcpy(&resHeader.version, response, VERSION_SIZE);
    std::memcpy(&resHeader.code, response + VERSION_SIZE, CODE_SIZE);
    std::memcpy(&resHeader.payload_size, response + VERSION_SIZE + CODE_SIZE, PAYLOAD_SIZE);
    // Convert from little-endian to host byte order
    resHeader.code = ntohs(*reinterpret_cast<uint16_t*>(const_cast<char*>(response + VERSION_SIZE)));
    resHeader.payload_size = ntohl(*reinterpret_cast<uint32_t*>(const_cast<char*>(response + VERSION_SIZE + CODE_SIZE)));
    resHeader.code = ntohs(resHeader.code);
    resHeader.payload_size = ntohl(resHeader.payload_size);
}

std::string loadClientId(const char* response) {
    char client_id[ID_SIZE];  // +1 for null-terminator
    std::memcpy(client_id, response + HEADER_SIZE_SERVER, ID_SIZE);
    std::string client_id_hex = bytesToHexString(client_id, ID_SIZE);
    return client_id_hex;
}
//clear buffer
void clear(char* message, int length) {
    std::memset(message, 0, length);  // Ensures the buffer is zero-padded
}
//create the header in buffer
void createBufferHeader(char* buffer, uint16_t code, uint32_t payload_size, std::vector<uint8_t>& client_id, uint8_t version) {
    uint16_t code_le = toLittleEndian16(code);
    uint32_t payload_size_le = toLittleEndian32(payload_size);
    clear(buffer, HEADER_SIZE + NAME_SIZE);  // Initialize buffer with zeroes
    std::memcpy(buffer, client_id.data(), ID_SIZE);
    buffer[ID_SIZE] = version;
    std::memcpy(buffer + ID_SIZE + VERSION_SIZE, &code_le, CODE_SIZE);
    std::memcpy(buffer + ID_SIZE + VERSION_SIZE + CODE_SIZE, &payload_size_le, PAYLOAD_SIZE);
}
//boost function
//connect
void connect(TransferInfo& info,tcp::socket&s, tcp::resolver &resolver) {
    std::cout << "Connecting to server... with port " << info.port << std::endl;
    boost::asio::connect(s, resolver.resolve(info.ip, std::to_string(info.port)));
    std::cout << "Connected to server\n";
}


