#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm> // For std::min
#include <iomanip>
#include "client.h"
#include "commonFunctions.h"
#include "bytesCodeCasting.h"

int main(int argc, char* argv[]) {
    TransferInfo info;
    try {
        std::string AESKeyBytes;
        if (!std::ifstream("me.info")) {
            AESKeyBytes = runHundleNewClientConnect(info);
            if (AESKeyBytes.empty()) {
                return 0;
            }
        }
        else {
            std::string nameStr = readTransferInfo("transfer.info", info);
            readMeInfo("me.info", info);
            AESKeyBytes = runReconnectRequest(info);
            if (AESKeyBytes.empty()) {
                return 0;
            }
        }
        int counter;
            for (counter = 0; counter < 3; counter++)
            {
                bool isSendingFileCorrect = runFileSendingRequest(info, AESKeyBytes);
                if (isSendingFileCorrect) {
                    int codeResponse= runCRCRequest(info, CORRECT_CRC_CODE);
                    return 0;
                }
                else {
                    int codeResponse = runCRCRequest(info, INCORRECT_CRC_CODE_REPEAT);
                }
            }
            if (counter == 3) {
                int codeResponse = runCRCRequest(info, FATA_ERROR_CODE);
            }
        else {
            return 0;
        }   
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
