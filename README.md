# client_and_server_by_Hybrid_Key_Exchange_Protocol
### This task solve maman 15 from חוברת קורס
## How to run it?
### donload crypto++ and connect it in vs
### you can donload hear:
https://github.com/weidai11/cryptopp
### download boost library and connect it to vs
### run server.py and after run client.cpp
## Client:
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
##### std::string runRegistrationRequest(TransferInfo& info, int retry_count = 0)- This function send the registration request it update the header of the request to the correct value and send the name that in transfer.info that in info and send the request and handle the response by calling handleServerResponse. This function out the id in hex number in string and if something get wrong it out empty string.:

### publicKeySending.cpp
#### elements:
##### RSAGenerating inherit from Message and have name and pubKey160Bytes
#### functions:
##### void EncryptedAESKeySendingResponse::hundlePrivateKey(RSAPrivateWrapper& rsapriv, static std::string base64key) const -This function write base64key in me.info and priv.key files.This function is of object EncryptedAESKeySendingResponse.
##### std::string EncryptedAESKeySendingResponse:: decryptAesKeyCipher(EncryptedAESKeySendingResponse& res, const char* response, size_t length, boost::asio::ip::tcp::socket& s, RSAPrivateWrapper& rsapriv, std::string privateKey) const- This function decryptthe cipher of AES key by the private key of RSA and return the decryption.This function is of object EncryptedAESKeySendingResponse.
##### std::string handleServerResponse(const char* response, size_t length, boost::asio::ip::tcp::socket& s, RSAPrivateWrapper& rsapriv, std::string base64key) - This function handle the response to the request that send public key. the response return if every things work good return code 1602 and return cipher of AES key. This function return AES key that the server send or blank string if something wrong.
##### std::string runPublicKeySending(TransferInfo& info)- This function send the server request that send public key and the server send cipher of AES key and the client decrypt it. This function send the request and return the AES key decryption and if something get wrong return empty string.

### NewClientConnect.cpp
#### functions:
##### void readMeInfo(const std::string& filename, TransferInfo& info) - This function read the file me.info and load the elements to info.
##### bool isValidIP(const std::string& ip)- This function get string and checks if the string in format of ip address.
##### std::string readTransferInfo(const std::string& filename, TransferInfo& info)- This function read the file transfer.info and load the elements to info
##### std::string runHundleNewClientConnect(TransferInfo& info)- This functionhandle with connection of new client he need to read the file tranfer.info send registration request and get id from server and send public key and get cipher of AES key from server and decrypt it. This function return the AES key decryption and if something get wrong return empty string.

### reconnectRequest.cpp
#### elements:
##### ReconnectRequest inherit from Message and have name element.
#### functions:
##### std::string handleServerResponse(TransferInfo& info,const char* response, size_t length, boost::asio::ip::tcp::socket& s, RSAPrivateWrapper& rsapriv,std::string privkeyBytes)- This function handle the response from server to reconnect request.If the sever send code 1606 reconnect request failed and the function will send registration request and public key sending request. If the code is 1605 the reconnect request succeededand this function will decrypt the cipher. This function return the AES key decryption and if something get wrong return empty string.
##### std::string runReconnectRequest(TransferInfo& info) -This function send reconnect request and handle the response to that. This function return the AES key decryption and if something get wrong return empty string.

### fileSendingRequest.cpp
#### elements:
##### FileSendingRequest inherit from Message and have the fields: contentSize, origFileSize, packetNumber, totalPackets, fileName, messageContent.
##### CRCResponse inherit from Message and have the fields: clientIdHex, contentSize, fileName, checkSum.
##### uint32_t crc32_table[256]- global CRC32 table
#### functions:
##### std::string handleServerResponse(TransferInfo& info,const char* response, size_t length, boost::asio::ip::tcp::socket& s, RSAPrivateWrapper& rsapriv,std::string privkeyBytes)- This function handle the response from server to reconnect request.If the sever send code 1606 reconnect request failed and the function will send registration request and public key sending request. If the code is 1605 the reconnect request succeededand this function will decrypt the cipher. This function return the AES key decryption and if something get wrong return empty string.
##### std::string runReconnectRequest(TransferInfo& info) -This function send reconnect request and handle the response to that. This function return the AES key decryption and if something get wrong return empty string.
##### handleServerResponse(const char* response, size_t length,boost::asio::ip::tcp::socket& s,int checkSumClient,int consistantSizeClient,std::string fileNameClient)-This function handle the server response to file sending request. If the checksum that the client calculate suitable for the checksum that the server sent this function will return true else false.
##### void init_crc32_table() - This function init the CRC table
##### uint32_t calculate_crc32(const std::string& data) - This function calculate the CRC32 checksum of a string
##### std::string encryptMessageContent(std::string plaintext, std::string AESKeyBytes)- This function get plaintext and AESKeyBytes and return cipher of plaintext by AES encryption with theAES key AESKeyBytes.
##### std::string readFileToBuffer(const std::string& filePath) - This function get filePath and return its context.
##### bool runFileSendingRequest(TransferInfo& info, std::string AESKeyBytes)- This function run the request of file sending that send the file context cipher by AES encryption with AES key and the name of the file. It handle with the response to this request.If the checksum that the client calculate suitable for the checksum that the server sent this function will return true else false.

### correctCRCRequests.cpp
#### elements:
##### CRCRequest inherit from Message and have the fields: fileName.
##### CRCResponse inherit from ResponseHeader and have the fields: client_id_hex.
#### functions:
##### int getCode(const char* response, size_t length, TransferInfo& info , boost::asio::ip::tcp::socket& s, const std::string& nameStr) - This function handle the response from server and return the code that the server sent.
##### int runCRCRequest(TransferInfo& info, int code) - This function send CRC request and return the code that the server 

### client.h
#### This file have the functions signature of all function that handle with request to server and readTransferInfo, readMeInfo functions

### client.cpp
#### functions:
##### main- this function manage the client send the client requests according to this schemes
<img width="176" alt="image" src="https://github.com/user-attachments/assets/0f7eed58-bb50-4dc1-bcbd-b81feb70289c">
<img width="205" alt="image" src="https://github.com/user-attachments/assets/8db76ef0-e5bb-40ca-a488-032e39175f85"> 

## Server:
#### lets explain about the files of server:

### database_manager.py
##### This file handle the database of clients and files
#### functions:
##### def init_db() -> None:- This function init the table of clients and the table of files.
##### def register_client(name: str) -> bytes:- This function allocate uuid to the name that it got and insert the in line in table clients
##### def update_public_key(client_id: bytes, new_public_key: bytes) -> bool:- This function get public key and client_id and finde the line in table that it's id is client_id and update the public key of this line to what the function got.
##### def get_client_info(name: str) -> Union[Tuple[bytes, bytes], None]:- This function get name and and return the id and  public key of this name.
##### def get_id_by_name(name: str) -> bytes:- This function get name and return it's id
##### def delete_client_by_name(name: str) -> bool:- This function get name and delete frome table clients the line that have this name.
##### def update_AESkey(client_id: bytes, new_aes_key: bytes) -> bool:- This function update AES key to the line that have the id client_id in clients table.
##### def get_AesKey_by_id(client_id: bytes) -> bytes:- This function get id client_id and return it's AES key
##### def insert_file_record(id: bytes, fileName: str, pathName: str) -> None:- This function get id, fileName, pathName and insert them to line in table files.
##### def verify_file_by_client_id(client_id: bytes) -> bool:- This function get client_id and update it's Verified to 1 in files table.

### bytes_code_casting.py
##### This file is handle with the converting of byte code in different forms.
#### functions:
##### def bytes_4_little_endian_to_big_endian(data):-This function get 4 bytes of number in little endian and return the number in big endian.
##### def bytes_2_little_endian_to_big_endian(data):- This function get 2 bytes of number in little endian and return this number in big endian.
##### def decode_string_using_UTF_8(data):- This function decode data to string.
##### def ecode_string_using_UTF_8_And_remove_null_terminator(data):- This function encode the string data without null terminator.
##### def get_hex(data):-This function get bytecode data convert him to hext number and return it in strig.
##### def int_to_1_byte(data):- This function return an integer to a 1-byte binary representation
##### def encode_to_unsigned_little_endian_2_bytes(data):- This function encode unsigned little endian in 2 bytes.
##### def encode_to_unsigned_little_endian_4_bytes(data):- This function encode unsigned little endian in 4 bytes.
##### def convert_int_to_4_bytes(data):-This function return an integer to a 4-byte big-endian binary representation

### common_functions.py
#### elements:
##### Header have the fields: version, code, payload_size- all the response have this fields and inherit from this.
#### functions:
##### def create_header_response(version: int, code: int, payload_size: int) -> bytes:- This function create the header of all response from server
##### def create_general_failed_response():- This function create general failed response that happend in server.
##### def send_and_close(client_socket:socket.socket,response:bytes):- This function send to client the response and close the socket.

### registration_response.py
#### elements:
##### RegistrationResponse inherit from Header and have the field client_id
#### functions:
##### def create_response(request: RegistrationResponse) -> bytes:- This function get the object and create the response in bytes.
##### def run_registration_response(client_socket: socket.socket) -> None:- This function get the registration request from client and return send him the response to that.

### encrypted_AES_key_response.py
#### elements:
##### EncryptedAESKeyResponse inherit from Header
#### functions:
##### def encrypt_aes_key_with_rsa(public_key_bytes: bytes, aes_key: bytes) -> bytes:- This function encrypt AES key with the public RSA key.
##### def create_response(request: EncryptedAESKeyResponse) -> bytes:-  This function get the object and create the response in bytes.
##### def run_aes_key_generating_response(client_socket: socket.socket) -> None:- This function get request sending public key and handle it create AES key and encrypt it with public key and send it to client.

### reconnect_AEA_key_response.py
#### elements:
##### RequestDeniedResponse inherit from Header and has the field client_id.
#### functions:
##### def request_denied(request: RequestDeniedResponse) -> bytes:- This function return response in bytes of the object it get.
##### def is_valid_rsa_public_key(binary_key: bytes) -> bool:- This function return true if the public key of RSA is valid and false if not.
##### def rerun_aes_key_generating_response(client_socket: socket.socket, client_id: str) -> None:- This function get reconnect request check if the public key that it get valid. If yes create AES key and send to client the cipher of it by the public key and update the database. If not send to client request_denied

### file_recived_correct_with_CRC.py
#### elements:
##### CorrectCRCResponse inherit from Header and has the fields: client_id, content_size, file_name, check_sum
#### functions:
##### def calculate_crc32(byte_data):- This function calculate the checksum of byte_data.
##### def aes_decrypt(cipher_bytes, key):- This function decrypt cipher_bytes with the AES key by the decryption AES.
##### def create_response(request: CorrectCRCResponse) -> bytes: This function get the object and return the response to sending file request in bytes.
##### def handle_data(data,payload_size):- This function handle the data and divide it to number of variable.
##### def get_complete_cipher(client_socket,complete_cipher,packet_number ,total_packets,file_name):- This function get the packets from client and return the cipher
##### handle_file_sending_request(client_socket:socket.socket, client_id, payload_size):- This function handle file sending request and response the client the checksum

### crc_respones.py
#### elements:
##### CRCResponse inherit from Header and has the fields: client_id
#### functions:
##### def create_response(request: CRCResponse) -> bytes:  This function get the object and return the response to CRC request in bytes.
##### def run_crc_response(client_socket: socket.socket,code,client_id) -> None:- This function get the request from client if the client send the checksum correct it response accept response and change the database in the correct place Verify to 1.If the check sum not correct and the client send fatal error after it sent 3 times attempt, it return accept response.

### server.py 
#### functions:
##### read_port() -> int:- This function read from port.info file the port and return it.
##### def decode_client_id(data: bytes) -> str:- This function decode client id from data.
##### def start_server(host: str, port: int) -> None:- This function handle the server response according to the code that it get from client  and call the correct function that handle this. This function wait for request from client and handle them.



