#include "Base64Wrapper.h"
#include "RSAWrapper.h"
#include "AESWrapper.h"
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <iomanip>
#include "client.h"
#include "commonFunctions.h"
#include "bytesCodeCasting.h"


using boost::asio::ip::tcp;
struct RSAGenerating : public Message {
    std::vector<uint8_t>  name;
    std::string  pubKey160Bytes;
    RSAGenerating(): name(NAME_SIZE, 0) {}
};

//void  hexify(const unsigned char* buffer, unsigned int length) 
//{
//    std::ios::fmtflags f(std::cout.flags());
//    std::cout << std::hex;
//    for (size_t i = 0; i < length; i++)
//        std::cout << std::setfill('0') << std::setw(2) << (0xFF & buffer[i]) << (((i + 1) % 16 == 0) ? "\n" : " ");
//    std::cout << std::endl;
//    std::cout.flags(f);
//}

void EncryptedAESKeySendingResponse::hundlePrivateKey(RSAPrivateWrapper& rsapriv, static std::string base64key) const {
    // 4. get the private key and encode it as base64 (base64 in not necessary for an RSA decryptor.)
    //static std::string privkeyBytes = rsapriv.getPrivateKey();
    //std::string base64key = Base64Wrapper::encode(privkeyBytes);
    std::string filePath = "me.info"; // Replace with your file path
    std::ofstream meFile(filePath, std::ios::app); // Open in append mode

    if (meFile.is_open()) {
        meFile << base64key; // Write the new line
        meFile.close();
    }
    else {
        std::cerr << "Unable to open file for writing\n";
    }
    std::ofstream privfile("priv.key");
    if (privfile.is_open()) {
        privfile << base64key;
        privfile.close();
    }
    else {
        std::cout << "priv.key file created with name and UUID.\n";
    }
}
std::string EncryptedAESKeySendingResponse:: decryptAesKeyCipher(EncryptedAESKeySendingResponse& res, const char* response, size_t length, boost::asio::ip::tcp::socket& s, RSAPrivateWrapper& rsapriv, std::string privateKey) const{
    res.client_id_hex = loadClientId(response);
    // 3. create an RSA encryptor
    std::string cipher(response + HEADER_SIZE_SERVER + ID_SIZE, length - HEADER_SIZE_SERVER - ID_SIZE);

    // 4. get the private key and encode it as base64 (base64 in not necessary for an RSA decryptor.)
    std::string base64key = Base64Wrapper::encode(privateKey);;
    hundlePrivateKey(rsapriv, base64key);

    // 5. create another RSA decryptor using an existing private key (decode the base64 key to an std::string first)
    RSAPrivateWrapper rsapriv_other(Base64Wrapper::decode(base64key));

    std::string decrypted = rsapriv_other.decrypt(cipher);		// 6. you can decrypt an std::string or a const char* buffer

    s.close();
    return decrypted;
}



std::string handleServerResponse(const char* response, size_t length, boost::asio::ip::tcp::socket& s, RSAPrivateWrapper& rsapriv, std::string base64key) {
    EncryptedAESKeySendingResponse res;
    handlingHeaderResponse(response, res);

    if (res.code == 1602) {
       
        return res.decryptAesKeyCipher(res,response, length,  s,  rsapriv, base64key);
    }
    else {
        return "";
    }
}

std::string runPublicKeySending(TransferInfo& info) {
    const int max_length = 1024;
    try {
        RSAGenerating req;
        req.client_id = info.id;
        req.version = VERSION; // version 3
        req.code = 826;
        req.payload_size = NAME_SIZE + PUBLIC_KEY_SIZE; // Set payload size to the length of 255+160
        req.name = info.name;

        RSAPrivateWrapper rsapriv;
        std::string pubkey = rsapriv.getPublicKey();
        const int PAYLOAD_SIZE_FIELDS = NAME_SIZE + PUBLIC_KEY_SIZE;
        char buffer[HEADER_SIZE + PAYLOAD_SIZE_FIELDS];
        createBufferHeader(buffer, req.code, req.payload_size, req.client_id, req.version);
        std::memcpy(buffer + HEADER_SIZE, req.name.data(), NAME_SIZE);
        std::memcpy(buffer + HEADER_SIZE + NAME_SIZE, pubkey.c_str(), PUBLIC_KEY_SIZE);
        boost::asio::io_context io_context;
        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        connect(info, s, resolver);
        boost::asio::write(s, boost::asio::buffer(buffer, HEADER_SIZE + PAYLOAD_SIZE_FIELDS));
        char reply[4096];
        boost::system::error_code error;
        size_t reply_length = boost::asio::read(s, boost::asio::buffer(reply, 4096), error);
        if (error == boost::asio::error::eof) {
            std::cout << "Connection closed by server\n";
        }
        else if (error) {
            throw boost::system::system_error(error);
        }
        std::string base64key = rsapriv.getPrivateKey();
        return handleServerResponse(reply, reply_length ,s, rsapriv, base64key);
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return "";
}
