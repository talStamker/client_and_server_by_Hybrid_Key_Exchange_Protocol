# client_and_server_by_Hybrid_Key_Exchange_Protocol
### This task solve maman 15 from חוברת קורס
## How to run it?
### donload crypto++ and connect it in vs
### you can donload hear:
https://github.com/weidai11/cryptopp
### download boost library and connect it to vs
### run server.py and after run client.cpp
## client
### i add to my client crypto wrapper This is the files:
### 1. AESWrapper.cpp
### 2. AESWrapper.h
### 3. Base64Wrapper.cpp
### 4. Base64Wrapper.h
### 5. cryptopp_wrapper.vcxproj
### 6. cryptopp_wrapper.vcxproj.user
### 7. RSAWrapper.cpp
### 8. RSAWrapper.h
## example files:
### Tal.txt
#### This is the file that the client want send to server.
#### You can change it's context and it's name but if you change the name change the third line in transfer.info
### transfer.info
#### First line: ip address:port 
#### You can change it but you will need to change it in server in server.py line: host: str = '127.0.0.1', and change the file port.info change it's context to new port.
#### Second line: the name of client as user in server.
#### Third line: The name of file that the client want send to server.
## bytesCodeCasting.cpp
#### This file is handle with the converting of byte code in different forms.
### functions:
#### std::string decodeByteCodeToHexString(const std::vector<uint8_t>& vec) - This function get vec and convert it to string that it's context is the byte code in hext
#### std::string bytesToHexString(const char* bytes, size_t length)- get char * and its length and the char* context is byte code and the function convert the byte code to hex and out string of the hexnumber.
#### std::string decodeByteCodeToString(const std::vector<uint8_t>& vec)- return a string from the byte code up to the null terminator
#### uint16_t toLittleEndian16(uint16_t value)- get number in 2 bytes and convert it to little endian in 2 bytes.
#### uint32_t toLittleEndian32(uint32_t value)- get number in 3 bytes and convert it to little endian in 3 bytes.
#### void encodeStringToByteCode(std::vector<uint8_t>& vec, std::string str, int stringSizeOnumberOfByte)- get vec,str that its context is byte code, stringSizeOnumberOfByte -length of byte code and enter to vec the byte code from str.
#### void encodeStringInArrToByteCode(std::vector<uint8_t>& vec, const char* str, int stringSizeOnumberOfByte)- get vec,str that its context is byte code, stringSizeOnumberOfByte -length of byte code and enter to vec the byte code from str.
#### uint8_t hexCharToByte(char c)- This function convert a hex character to it's integer value
#### encodeHexStringToByteCode(std::vector<uint8_t>& vec, const std::string& hexStr, int byteSize) - This function encode a 32-character hex string to a 16-byte binary format.
## bytesCodeCasting.h 
### library that bytesCodeCasting.cpp relizes
## 




