# client_and_server_by_Hybrid_Key_Exchange_Protocol
### This task solve maman 15 from חוברת קורס
## How to run it?
### donload crypto++ and connect it in vs
### you can donload hear:
https://github.com/weidai11/cryptopp
### download boost library and connect it to vs
### run server.py and after run client.cpp
## client
#### I add to my client crypto wrapper This is the files:
##### 1. AESWrapper.cpp
##### 2. AESWrapper.h
##### 3. Base64Wrapper.cpp
##### 4. Base64Wrapper.h
##### 5. cryptopp_wrapper.vcxproj
##### 6. cryptopp_wrapper.vcxproj.user
##### 7. RSAWrapper.cpp
##### 8. RSAWrapper.h
### example files:
#### Tal.txt
##### This is the file that the client want send to server.
##### You can change it's context and it's name but if you change the name change the third line in transfer.info
#### transfer.info
##### First line: ip address:port 
##### You can change it but you will need to change it in server in server.py line: host: str = '127.0.0.1', and change the file port.info change it's context to new port.
##### Second line: the name of client as user in server.
##### Third line: The name of file that the client want send to server.
### bytesCodeCasting.h 
#### library that bytesCodeCasting.cpp relizes
### bytesCodeCasting.cpp
##### This file is handle with the converting of byte code in different forms.
#### functions:
##### std::string decodeByteCodeToHexString(const std::vector<uint8_t>& vec) - This function get vec and convert it to string that it's context is the byte code in hext
##### std::string bytesToHexString(const char* bytes, size_t length)- get char * and its length and the char* context is byte code and the function convert the byte code to hex and out string of the hexnumber.
##### std::string decodeByteCodeToString(const std::vector<uint8_t>& vec)- return a string from the byte code up to the null terminator
##### uint16_t toLittleEndian16(uint16_t value)- get number in 2 bytes and convert it to little endian in 2 bytes.
##### uint32_t toLittleEndian32(uint32_t value)- get number in 3 bytes and convert it to little endian in 3 bytes.
##### void encodeStringToByteCode(std::vector<uint8_t>& vec, std::string str, int stringSizeOnumberOfByte)- get vec,str that its context is byte code, stringSizeOnumberOfByte -length of byte code and enter to vec the byte code from str.
##### void encodeStringInArrToByteCode(std::vector<uint8_t>& vec, const char* str, int stringSizeOnumberOfByte)- get vec,str that its context is byte code, stringSizeOnumberOfByte -length of byte code and enter to vec the byte code from str.
##### uint8_t hexCharToByte(char c)- This function convert a hex character to it's integer value
##### encodeHexStringToByteCode(std::vector<uint8_t>& vec, const std::string& hexStr, int byteSize) - This function encode a 32-character hex string to a 16-byte binary format.
### commonFunctions.h
#### elements:
##### TransferInfo- will be in use for the most of code it update its feilds by the file transfer.info and update in the handle of requests to server and responses from server.
##### Message- all request to server have it's fields as header of the request.
##### ResponseHeader- all response from server have it's fields as header of the request.
##### EncryptedAESKeySendingResponse inherit from ResponseHeader - when the server response is AES key encrypted by public key of RSA he send client_id_hex,
 cipher.
##### About it's static functions i will explain when i will explain the files that realize it.
### commonFunctions.cpp
#### functions:
##### void handlingHeaderResponse(const char* response,ResponseHeader& resHeader)-  get resHeader object and update it's fields acording to the response
##### std::string loadClientId(const char* response)- return client id in hexnumber in string by read it from response
##### void clear(char* message, int length)- clear message
##### void createBufferHeader(char* buffer, uint16_t code, uint32_t payload_size, std::vector<uint8_t>& client_id, uint8_t version)- update buffer according to the elements of header that it get it update buffer that its begining will be this elements
##### void connect(TransferInfo& info,tcp::socket&s, tcp::resolver &resolver)- wrapper of function connect of boost
### registrationRequest.cpp
#### elements:
##### RegistrationRequest inherit from Message and have the field name- the client want send request to server that have the name of user and the server need to write him in its database.
##### RegistrationResponse inherit from ResponseHeader and have the field client_id_hex- the server response and send the id that he gave to the user.
##### std::string handleServerResponse(const char* response, size_t length, TransferInfo& info, int& retry_count, boost::asio::ip::tcp::socket& s, const std::string& nameStr) - This function handle the response that the client can get to the registration request. code 1600 the registration succeeded and the server send id, in this function we open new file me.info  and the first line we write the name the second line we write the id. code 1601 the registration failed and this function try to send registration request more time untill it get 3 failures. if it get failure in the third time it print fatal error. This function out the id in hex number in string and if something get wrong it out empty string.
##### std::string runRegistrationRequest(TransferInfo& info, int retry_count = 0)- This function send the registration request it update the header of the request to the correct value and send the name that in transfer.info that in info and send the request and handle the response by calling handleServerResponse. This function out the id in hex number in string and if something get wrong it out empty string.





