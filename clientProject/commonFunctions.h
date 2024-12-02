#ifndef COMMON_FUNCTION_H
#define COMMON_FUNCTION_H
#pragma once
#include <boost/asio.hpp>
#include "RSAWrapper.h"
//HEADER ELEMENT
using boost::asio::ip::tcp;
const int VERSION_SIZE = 1;
const int CODE_SIZE = 2;
const int PAYLOAD_SIZE = 4;
const int ID_SIZE = 16;
const int VERSION = 3;
const int HEADER_SIZE = ID_SIZE + VERSION_SIZE + CODE_SIZE + PAYLOAD_SIZE;

//PAYLOAD ELEMENT
const int NAME_SIZE = 255;
const int PUBLIC_KEY_SIZE = 160;
const int RSA_ENCRYPTED_AES_KEY_SIZE = 128;
const int CONTENT_SIZE_SIZE = PAYLOAD_SIZE;
const int ORIG_FILE_SIZE_SIZE = PAYLOAD_SIZE;
const int PACKET_NUMBER_SIZE = CODE_SIZE;
const int TOTAL_PACKET_SIZE = CODE_SIZE;
const int FILE_NAME_SIZE = NAME_SIZE;
const int MESSAGE_CONTENT_SIZE = 730;
const int CHECKSUM_SIZE = PAYLOAD_SIZE;

const int HEADER_SIZE_SERVER = VERSION_SIZE + CODE_SIZE + PAYLOAD_SIZE;

//REQUEST CODE
const int REGISTRATION_REQUEST_CODE = 825;
const int PUBLIC_KEY_SENDING_REQUEST_CODE = 826;
const int RECONNECT_REQUEST_CODE = 827;
const int FILE_SENDING_REQUEST_CODE = 828;
const int CORRECT_CRC_CODE = 900;
const int INCORRECT_CRC_CODE_REPEAT = 901;
const int FATA_ERROR_CODE = 902;


//RESPONSE CODE
const int REGISTRATION_WAS_SUCCESS_CODE = 1600;
const int REGISTRATION_WAS_FAILED_CODE = 1601;
const int PRIVATE_KEY_ENCRYPTION_CODE  = 1602;
const int CORRECT_CRC_CODE_RESPONSE = 1603;
const int GENERAL_ERROR_CODE = 1607;
const int ACCEPT_CODE = 1604;

struct TransferInfo {
    std::string ip;
    int port;
    std::vector<uint8_t>  name;
    std::string pathFile;
    std::vector<uint8_t> id;
    std::string privateKey;
    TransferInfo() : name(NAME_SIZE, 0), id(ID_SIZE, 0) {} // Initialize vectors with 255 zeros
};

struct Message {
    std::vector<uint8_t> client_id;
    uint8_t version;
    uint16_t code;
    uint32_t payload_size;
    Message() : client_id(ID_SIZE, 0) {}
};
struct ResponseHeader {
    uint8_t version;
    uint16_t code;
    uint32_t payload_size;
};
struct  EncryptedAESKeySendingResponse : public ResponseHeader {
private:
    void hundlePrivateKey(RSAPrivateWrapper& rsapriv, static std::string privkeyBytes) const;
public:
    std::string client_id_hex;
    std::string cipher;
    std::string decryptAesKeyCipher(EncryptedAESKeySendingResponse& res, const char* response, size_t length, boost::asio::ip::tcp::socket& s, RSAPrivateWrapper& rsapriv, std::string privateKey) const;

};

void handlingHeaderResponse(const char* response, ResponseHeader& resHeader);
std::string loadClientId(const char* response);
void createBufferHeader(char* buffer, uint16_t code, uint32_t payload_size, std::vector<uint8_t>& client_id, uint8_t version);
void clear(char message[], int length);
void connect(TransferInfo& info, tcp::socket& s, tcp::resolver& resolver);
#endif


