#include <iostream>
#include <fstream>
#include "client.h"
#include "commonFunctions.h"
#include "bytesCodeCasting.h"

using boost::asio::ip::tcp;

struct CRCRequest : public Message {
    std::vector<uint8_t>  fileName;
    CRCRequest() : fileName(NAME_SIZE, 0) {}
};
struct CRCResponse : public ResponseHeader {
    std::string client_id_hex;
};


int getCode(const char* response, size_t length, TransferInfo& info , boost::asio::ip::tcp::socket& s, const std::string& nameStr) {
    CRCResponse res;
    handlingHeaderResponse(response, res);
    // Handle specific response codes
    return res.code;

}

int runCRCRequest(TransferInfo& info, int code) {
    const int max_length = 1024;
    try {
        // Set default values for RegistrationRequest
        CRCRequest req;
        req.client_id =info.id ;
        req.version = VERSION;//version 3
        req.code = code;
        req.payload_size = NAME_SIZE;// Set payload size to the length of info.name
        encodeStringToByteCode(req.fileName, info.pathFile, FILE_NAME_SIZE);

        // Create a buffer to send
        char buffer[HEADER_SIZE + NAME_SIZE];
        createBufferHeader(buffer, req.code, req.payload_size, req.client_id, req.version);
        std::memcpy(buffer + HEADER_SIZE, req.fileName.data(), NAME_SIZE);

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
        return getCode(reply, reply_length, info, s, decodeByteCodeToString(info.name));
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 0;
    }
}
