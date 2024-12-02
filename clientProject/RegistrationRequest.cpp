#include <iostream>
#include <fstream>
#include "client.h"
#include "commonFunctions.h"
#include "bytesCodeCasting.h"

using boost::asio::ip::tcp;

struct RegistrationRequest : public Message {
    std::vector<uint8_t>  name;
    RegistrationRequest() : name(NAME_SIZE, 0){}
};
struct RegistrationResponse : public ResponseHeader {
    std::string client_id_hex;
};

std::string handleServerResponse(const char* response, size_t length, TransferInfo& info, int& retry_count, boost::asio::ip::tcp::socket& s, const std::string& nameStr) {
    if (length < HEADER_SIZE_SERVER) {
        std::cerr << "Incomplete response received\n";
        return "";
    }
    RegistrationResponse res;
    handlingHeaderResponse(response,res);
    // Handle specific response codes
    if (res.code == 1600) {
        res.client_id_hex = loadClientId(response);
        std::ofstream outfile("me.info");
        if (outfile.is_open()) {
            outfile << nameStr << "\n" << res.client_id_hex << "\n";
            outfile.close();
            s.close();
            return res.client_id_hex;
        }
        else {
            std::cout << "me.info not open correct.\n";
            return "";
        }
    }
    else if (res.code == 1601) {
        if (retry_count < 2) {
            std::cout << "Server responded with an error 1601. Retrying...\n";
            s.close();
            retry_count++;
            return runRegistrationRequest(info, retry_count);
        }
        else {
            std::cerr << "Fatal error: server responded with an error after 3 attempts. The username might be taken.\n";
            s.close();
            return "";
        }
    }

}

std::string runRegistrationRequest(TransferInfo& info, int retry_count = 0) {
    const int max_length = 1024;
    try {
        // Set default values for RegistrationRequest
        RegistrationRequest req;
        req.client_id[0]='\0';
        req.version = VERSION;//version 3
        req.code = 825;
        req.payload_size = NAME_SIZE;// Set payload size to the length of info.name
        req.name=info.name;        

        // Create a buffer to send
        char buffer[HEADER_SIZE + NAME_SIZE];
        createBufferHeader(buffer, req.code, req.payload_size, req.client_id, req.version);
        std::memcpy(buffer + HEADER_SIZE, req.name.data(), NAME_SIZE);

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
        return handleServerResponse(reply, reply_length,  info, retry_count, s, decodeByteCodeToString(info.name));
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return "";
    }
}
