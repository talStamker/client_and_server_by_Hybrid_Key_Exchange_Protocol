#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include "commonFunctions.h"
#include "bytesCodeCasting.h"
#include "client.h"
using boost::asio::ip::tcp;

void readMeInfo(const std::string& filename, TransferInfo& info) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string nameStr;
        std::getline(file, nameStr);
        encodeStringToByteCode(info.name, nameStr, NAME_SIZE);

        std::string idStr;
        std::getline(file, idStr);
        encodeHexStringToByteCode(info.id, idStr, ID_SIZE);

        std::ostringstream privateKeyStream;
        std::string line;
        while (std::getline(file, line)) {
            privateKeyStream << line;
        }
        info.privateKey = privateKeyStream.str();
        file.close();
    }
    else {
        std::cerr << "Unable to open " << filename << "\n";
    }
}
bool isValidIP(const std::string& ip) {
    // Basic validation for IP address format
    int dots = 0;
    for (char ch : ip) {
        if (ch == '.') {
            dots++;
        }
        else if (!isdigit(ch)) {
            return false;
        }
    }
    return dots == 3;
}

std::string readTransferInfo(const std::string& filename, TransferInfo& info) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string ip_port;
        std::getline(file, ip_port);

        // Split ip_port into ip and port
        size_t colon_pos = ip_port.find(':');
        if (colon_pos != std::string::npos) {
            info.ip = ip_port.substr(0, colon_pos);
            std::string port_str = ip_port.substr(colon_pos + 1);

            // Validate IP address
            if (!isValidIP(info.ip)) {
                std::cerr << "Invalid IP address: " << info.ip << "\n";
                return "";
            }

            // Validate port number
            try {
                info.port = std::stoi(port_str);
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Invalid port number: " << port_str << "\n";
                return "";
            }
        }
        else {
            std::cerr << "No colon found in IP:Port string\n";
            return "";
        }

        std::string nameStr;
        std::getline(file, nameStr);

        // Validate name length
        if (nameStr.length() > 100) {
            std::cerr << "Name is too long: " << nameStr << "\n";
            return "";
        }
        char* nameWithNullTerminator = new char[nameStr.length() + 1];
        strcpy_s(nameWithNullTerminator, nameStr.length() + 1, nameStr.c_str());
        encodeStringInArrToByteCode(info.name, nameWithNullTerminator, nameStr.length());
        std::getline(file, info.pathFile);
        file.close();
        return nameStr;
    }
    else {
        std::cerr << "Unable to open " << filename << "\n";
        return "";
    }
}

std::string runHundleNewClientConnect(TransferInfo& info) {
    std::string nameStr = readTransferInfo("transfer.info", info);
    if (nameStr.empty()) {
        return "";
    }
    int retry_count = 0;
    std::string clientId = runRegistrationRequest(info, retry_count);
    if (clientId.empty()) {
        return "";
    }

    readMeInfo("me.info", info);
    std::string AESKey = runPublicKeySending(info);
    if (AESKey.empty()) {
        return "";
    }
    return AESKey;
}