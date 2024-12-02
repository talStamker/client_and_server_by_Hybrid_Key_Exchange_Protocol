#include <iostream>
#include <fstream>
#include <vector>
#include <boost/asio.hpp>
#include <cstdlib>  // For malloc and free
#include <cstring>  // For strlen
#include <cstdint>  // For uint32_t

#include "client.h"
#include "commonFunctions.h"
#include "Base64Wrapper.h"
#include "AESWrapper.h"
#include "bytesCodeCasting.h"

using boost::asio::ip::tcp;

struct FileSendingRequest : public Message {
    uint32_t contentSize;
    uint32_t origFileSize;
    uint16_t packetNumber;
    uint16_t totalPackets;
    std::vector<uint8_t> fileName;
    std::string messageContent;

    FileSendingRequest() : contentSize(0), origFileSize(0), packetNumber(0), totalPackets(0), fileName(255, 0) {}
};
struct CRCResponse :public ResponseHeader {
    std::string clientIdHex;
    uint32_t contentSize;
    std::vector<uint8_t> fileName;
    uint32_t checkSum;
    CRCResponse():clientIdHex(0), contentSize(0), fileName(255,0), checkSum(0){}
};

// CRC32 table
uint32_t crc32_table[256];
bool handleServerResponse(const char* response, size_t length,boost::asio::ip::tcp::socket& s,int checkSumClient,int consistantSizeClient,std::string fileNameClient) {
    CRCResponse res;
    handlingHeaderResponse(response, res);
   
    // Handle specific response codes
    if (res.code == CORRECT_CRC_CODE_RESPONSE) {

        res.clientIdHex = loadClientId(response);
        std::memcpy(&res.contentSize, response+HEADER_SIZE_SERVER+ID_SIZE, CONTENT_SIZE_SIZE);
        std::memcpy(&res.checkSum, response + HEADER_SIZE_SERVER + ID_SIZE + CONTENT_SIZE_SIZE + FILE_NAME_SIZE, CHECKSUM_SIZE);
        if (res.checkSum == static_cast<uint32_t>(checkSumClient) ) {
            s.close();
            return true;
        
        }else {
            std::cout << "Server responded with an error.\n";
            s.close();
            return false;
        }
    }
    else if (res.code == GENERAL_ERROR_CODE) {
        std::cout << "Server responded with an error.\n";
        s.close();
        return false;
    }
    return false;

}

void init_crc32_table() {
    uint32_t polynomial = 0xEDB88320;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (uint32_t j = 8; j > 0; j--) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            }
            else {
                crc >>= 1;
            }
        }
        crc32_table[i] = crc;
    }
}

// Function to calculate the CRC32 checksum of a string
uint32_t calculate_crc32(const std::string& data) {
    uint32_t crc = 0xFFFFFFFF;
    for (char c : data) {
        uint8_t byte = static_cast<uint8_t>(c);
        crc = (crc >> 8) ^ crc32_table[(crc ^ byte) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}
std::string encryptMessageContent(std::string plaintext, std::string AESKeyBytes) {
    if (AESKeyBytes.size() != AESWrapper::DEFAULT_KEYLENGTH) {
        std::cerr << "Key length must be 16 bytes" << std::endl;
        return "";
    }
    unsigned char key[AESWrapper::DEFAULT_KEYLENGTH] = { 0 };
    std::memcpy(key, AESKeyBytes.data(), AESWrapper::DEFAULT_KEYLENGTH);
    AESWrapper aes(key, AESWrapper::DEFAULT_KEYLENGTH);

    // 2. encrypt a message (plain text)
    std::string ciphertext = aes.encrypt(plaintext.c_str(), plaintext.length());
    return ciphertext;
}

std::string readFileToBuffer(const std::string& filePath) {
    // Open the file in binary mode
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return "";
    }

    // Get the file size
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Check if file size is valid
    if (fileSize <= 0) {
        std::cerr << "Invalid file size: " << fileSize << std::endl;
        return "";
    }

    // Read the file into a buffer
    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
        std::cerr << "Error reading file: " << filePath << std::endl;
        return "";
    }
    return std::string(buffer.begin(), buffer.end());
}


bool runFileSendingRequest(TransferInfo& info, std::string AESKeyBytes) {
    const int max_length = MESSAGE_CONTENT_SIZE;
    try {
        FileSendingRequest req;
        req.client_id = info.id;
        req.version = VERSION;
        req.code = 828;
        req.payload_size = CONTENT_SIZE_SIZE + ORIG_FILE_SIZE_SIZE + PACKET_NUMBER_SIZE + TOTAL_PACKET_SIZE + FILE_NAME_SIZE;
        std::string content = readFileToBuffer(info.pathFile);
        if (content == "") {
            return false;
        }
        init_crc32_table();
        int  crc_checksum = calculate_crc32(content);
        std::string encryptedContent = encryptMessageContent(content, AESKeyBytes);
        if (encryptedContent == "") {
            return false;
        }
        req.contentSize = toLittleEndian32( encryptedContent.size());
        req.origFileSize = toLittleEndian32(content.size());
        encodeStringToByteCode(req.fileName, info.pathFile, FILE_NAME_SIZE);
        boost::asio::io_context io_context;
        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        connect(info, s, resolver);
        const size_t packetSize = MESSAGE_CONTENT_SIZE;
        size_t totalPackets = encryptedContent.size() / packetSize + (encryptedContent.size() % packetSize != 0);
        
        for (size_t packetNumber = 1; packetNumber <= totalPackets; ++packetNumber) {

            size_t start = (packetNumber - 1) * packetSize;
            size_t end = std::min(start + packetSize, encryptedContent.size());
            req.packetNumber = toLittleEndian16( packetNumber);
            req.totalPackets = toLittleEndian16(totalPackets);
            req.messageContent = encryptedContent.substr(start, end - start);

            // Update payload size to include the size of messageContent for this packet
            req.payload_size = CONTENT_SIZE_SIZE + ORIG_FILE_SIZE_SIZE + PACKET_NUMBER_SIZE + TOTAL_PACKET_SIZE + FILE_NAME_SIZE + req.messageContent.size();
            // Allocate memory for the buffer
            char* buffer = (char*)malloc(HEADER_SIZE + req.payload_size);
            clear(buffer, HEADER_SIZE + req.payload_size);
            if (buffer == nullptr) {
                std::cerr << "Memory allocation failed" << std::endl;
                return "";
            }
            createBufferHeader(buffer, req.code, req.payload_size, req.client_id, req.version);
            
            int counterBytes = HEADER_SIZE;
            std::memcpy(buffer + counterBytes, &(req.contentSize), CONTENT_SIZE_SIZE);
            counterBytes += CONTENT_SIZE_SIZE;
            std::memcpy(buffer + counterBytes, &(req.origFileSize), ORIG_FILE_SIZE_SIZE);
            counterBytes += ORIG_FILE_SIZE_SIZE;
            std::memcpy(buffer + counterBytes, &(req.packetNumber), PACKET_NUMBER_SIZE);
            counterBytes += PACKET_NUMBER_SIZE;
            std::memcpy(buffer + counterBytes, &(req.totalPackets), TOTAL_PACKET_SIZE);
            counterBytes += TOTAL_PACKET_SIZE;
            std::memcpy(buffer + counterBytes, req.fileName.data(), FILE_NAME_SIZE);
            counterBytes += FILE_NAME_SIZE;
            std::memcpy(buffer + counterBytes, req.messageContent.data(), req.messageContent.size());
            boost::asio::write(s, boost::asio::buffer(buffer, HEADER_SIZE + req.payload_size));
            // Free the allocated memory
            free(buffer);
        }
        char reply[4096];
        boost::system::error_code error;
        size_t reply_length = boost::asio::read(s, boost::asio::buffer(reply, 4096), error);
        if (error == boost::asio::error::eof) {
            std::cout << "Connection closed by server\n";
        }
        else if (error) {
            throw boost::system::system_error(error);
        }
        std::cout << "Response received\n";
        return handleServerResponse(reply, reply_length, s, static_cast<uint32_t>(crc_checksum), encryptedContent.size(), info.pathFile);
        
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return false;
    }
}

