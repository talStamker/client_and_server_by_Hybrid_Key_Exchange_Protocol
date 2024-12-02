#include <iostream>
#include <fstream>
#include "client.h"
#include "commonFunctions.h"
#include "Base64Wrapper.h"
using boost::asio::ip::tcp;

struct ReconnectRequest : public Message {
    std::vector<uint8_t>  name;
    ReconnectRequest() : name(NAME_SIZE, 0) {}
};


std::string handleServerResponse(TransferInfo& info,const char* response, size_t length, boost::asio::ip::tcp::socket& s, RSAPrivateWrapper& rsapriv,std::string privkeyBytes) {
    EncryptedAESKeySendingResponse res;
    handlingHeaderResponse(response, res);
    // Handle specific response codes
    if (res.code == 1606) {
        res.client_id_hex = loadClientId(response);
        std::string AESKey=runHundleNewClientConnect(info);
            s.close();
            return AESKey;
    }
    else if (res.code == 1605) {
        return res.decryptAesKeyCipher(res, response, length, s, rsapriv, privkeyBytes);
    }
    else {
            std::cerr << "Fatal error: server responded with an error after 3 attempts. The username might be taken.\n";
            s.close();
            return "";
    }
}

std::string runReconnectRequest(TransferInfo& info) {
    const int max_length = 1024;
    try {
        // Set default values for RegistrationRequest
        ReconnectRequest req;
        req.client_id = info.id;
        req.version = VERSION;//version 3
        req.code = 827;
        req.payload_size = NAME_SIZE;// Set payload size to the length of info.name
        req.name = info.name;

        // Create a buffer to send
        char buffer[HEADER_SIZE + NAME_SIZE];
        createBufferHeader(buffer, req.code, req.payload_size, req.client_id, req.version);
        std::memcpy(buffer + HEADER_SIZE, req.name.data(), NAME_SIZE);
        // Print the buffer content

       std::string privkeyBytes= Base64Wrapper::decode(info.privateKey);
       RSAPrivateWrapper rsapriv= RSAPrivateWrapper(privkeyBytes);
        boost::asio::io_context io_context;
        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        connect(info, s, resolver);
        // Send the buffer
        boost::asio::write(s, boost::asio::buffer(buffer, HEADER_SIZE + NAME_SIZE));

        // Receive the response from the server
        char reply[max_length];
        boost::system::error_code error;
        size_t reply_length = boost::asio::read(s, boost::asio::buffer(reply, max_length), error);
        if (error == boost::asio::error::eof) {
            std::cout << "Connection closed by server\n";
        }
        else if (error) {
            throw boost::system::system_error(error);
        }

        // Handle the server response
        return handleServerResponse(info,reply, reply_length ,s, rsapriv, privkeyBytes);
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return "";
    }
}
