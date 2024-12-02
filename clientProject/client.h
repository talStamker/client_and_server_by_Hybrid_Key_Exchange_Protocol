#ifndef CLIENT_H
#define CLIENT_H
#include <vector>
#include <string>
#include <boost/asio.hpp>
#include "commonFunctions.h"
#include "RSAWrapper.h"


std::string runRegistrationRequest(TransferInfo& info, int retry_count );
std::string runPublicKeySending(TransferInfo& info);
std::string runHundleNewClientConnect(TransferInfo& info);
std::string readTransferInfo(const std::string& filename, TransferInfo& info);
void readMeInfo(const std::string& filename, TransferInfo& info);
std::string runReconnectRequest(TransferInfo& info);
bool runFileSendingRequest(TransferInfo& info, std::string AESKeyBytes);
int runCRCRequest(TransferInfo& info, int code);
#endif // CLIENT_H
