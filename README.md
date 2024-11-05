# Server code:

# Bytes Code Casting

## Overview
This script provides various functions for encoding and decoding binary data, including converting between different endian formats, decoding strings, and converting integers to byte representations.

## Installation
1. **Clone the repository**:
   ```sh
   git clone https://github.com/yourusername/bytes_code_casting.git
### `common_function.py`
This file contains variables and functions that assist in handling various responses and requests within the server application.

- **Imports**:
  - `import struct`: Provides functions to convert between Python values and C structs represented as Python bytes objects.
  - `from typing import Union`: Allows for type hinting, making the code more readable and maintainable.
  - `import socket`: Provides access to the BSD socket interface.

- **Request Codes**:
  - `REGISTRATION_REQUEST_CODE`: Code for registration requests (825).
  - `PUBLIC_KEY_SENDING_REQUEST_CODE`: Code for public key sending requests (826).
  - `RECONNECT_REQUEST_CODE`: Code for reconnection requests (827).
  - `FILE_SENDING_REQUEST_CODE`: Code for file sending requests (828).
  - `CORRECT_CRC_CODE`: Code for correct CRC (900).
  - `INCORRECT_CRC_CODE_REPEAT`: Code for incorrect CRC, repeat request (901).
  - `FATA_ERROR_CODE`: Code for fatal errors (902).

- **Response Codes**:
  - `REGISTRATION_WAS_SUCCESS_CODE`: Code indicating registration was successful (1600).
  - `REGISTRATION_WAS_FAILED_CODE`: Code indicating registration failed (1601).
  - `PRIVATE_KEY_ENCRYPTION_CODE`: Code for private key encryption (1602).
  - `CORRECT_CRC_CODE_RESPONSE`: Code for correct CRC response (1603).
  - `GENERAL_ERROR_CODE`: Code for general errors (1607).
  - `ACCEPT_CODE`: Code indicating acceptance (1604).

- **Header Sizes**:
  - `ID_SIZE`: Size of the ID field (16 bytes).
  - `VERSION_SIZE`: Size of the version field (1 byte).
  - `CODE_SIZE`: Size of the code field (2 bytes).
  - `PAYLOAD_SIZE`: Size of the payload size field (4 bytes).
  - `HEADER_SIZE`: Total size of the header (23 bytes).

- **Payload Sizes**:
  - `NAME_SIZE`: Size of the name field (255 bytes).
  - `PUBLIC_KEY_SIZE`: Size of the public key field (160 bytes).
  - `CONTENT_SIZE_SIZE`: Size of the content size field (4 bytes).
  - `ORIG_FILE_SIZE_SIZE`: Size of the original file size field (4 bytes).
  - `PACKET_NUMBER_SIZE`: Size of the packet number field (2 bytes).
  - `TOTAL_PACKET_SIZE`: Size of the total packet size field (2 bytes).
  - `FILE_NAME_SIZE`: Size of the file name field (255 bytes).
  - `MESSAGE_CONTENT_SIZE`: Size of the message content field (730 bytes).
  - `CHECKSUM_SIZE`: Size of the checksum field (4 bytes).
  - `DEFAULT_PORT`: Default port number (1256).

- **Functions**:
  - `create_header_response(version: int, code: int, payload_size: int) -> bytes`: Creates a header response by converting the elements to binary code.
    ```python
    def create_header_response(version: int, code: int, payload_size: int) -> bytes:
        version_bytes: bytes = struct.pack('B', version)
        code_bytes: bytes = struct.pack('<H', code)  # '<H' for little-endian unsigned short
        payload_size_bytes: bytes = struct.pack('<I', payload_size)  # '<I' for little-endian unsigned int
        return version_bytes + code_bytes + payload_size_bytes
    ```

  - `create_general_failed_response()`: Creates a general failed response with a specific error code.
    ```python
    def create_general_failed_response():
        return create_header_response(VERSION, GENERAL_ERROR_CODE, FAILED_PAYLOAD_SIZE)
    ```

  - `send_and_close(client_socket: socket.socket, response: bytes)`: Sends a response to the client and closes the socket.
    ```python
    def send_and_close(client_socket: socket.socket, response: bytes):
        client_socket.send(response)
        client_socket.close()
    ```

- **Classes**:
  - `Header`: Represents the header for responses.
    ```python
    class Header:
        def __init__(self, code: int, payload_size: int) -> None:
            self.version: int = VERSION
            self.code: int = code
            self.payload_size: int = payload_size
    ```### `crc_responses.py`
This file handles CRC (Cyclic Redundancy Check) requests after a file has been sent with AES encryption. It checks the request codes and handles them appropriately to generate the corresponding responses.

- **Imports**:
  - `import socket`: Provides access to the BSD socket interface.
  - `from database_manager import register_client, verify_file_by_client_id`: Imports functions to register a client and verify a file by client ID.
  - `from common_functions import *`: Imports common functions and constants used across the project.
  - `import binascii`: Contains methods to convert between binary and ASCII.

- **Classes**:
  - `CRCResponse(Header)`: Represents a CRC response.
    ```python
    class CRCResponse(Header):
        def __init__(self, code: int, payload_size: int, client_id: bytes) -> None:
            super().__init__(code, payload_size)
            self.client_id: bytes = client_id
    ```

- **Functions**:
  - `create_response(request: CRCResponse) -> bytes`: Creates a header for the request and combines all parts into the response.
    ```python
    def create_response(request: CRCResponse) -> bytes:
        header_response: bytes = create_header_response(request.version, request.code, request.payload_size)
        # Combine all parts into the response
        client_id = request.client_id
        response: bytes = header_response + client_id
        return response
    ```

  - `run_crc_response(client_socket: socket.socket, code, client_id) -> None`: Handles the CRC response by checking the request code and generating the appropriate response.
    ```python
    def run_crc_response(client_socket: socket.socket, code, client_id) -> None:
        data: bytes = client_socket.recv(NAME_SIZE)
        if len(data) < NAME_SIZE:
            print("Incomplete name data received")
            return
        name_data: bytes = data[:NAME_SIZE]
        name: str = name_data.decode('utf-8').rstrip('\x00')
        if code == CORRECT_CRC_CODE:
            print(f"Server accept the file {name}")
            request = CRCResponse(ACCEPT_CODE, ID_SIZE, client_id)
            verify_file_by_client_id(client_id)
            response: bytes = create_response(request)
            client_socket.sendall(response)
            client_socket.shutdown(socket.SHUT_WR)
        elif code == FATA_ERROR_CODE:
            print(f"Fatal error client try sent 3 times to server but there are miss knowledge")
            request = CRCResponse(ACCEPT_CODE, ID_SIZE, client_id)
            response: bytes = create_response(request)
            client_socket.sendall(response)
            client_socket.shutdown(socket.SHUT_WR)
    ```

- **Purpose**:
  - This file is specifically designed to handle CRC requests after a file has been sent. It verifies the integrity of the file using CRC and generates appropriate responses based on the request code. If the CRC is correct, it accepts the file and verifies it by client ID. If there is a fatal error, it handles the error accordingly.

This file is crucial for ensuring the integrity of files sent to the server and handling any errors that may occur during the process.
### `database_manager.py`
This file handles the database operations for managing clients and files. It uses SQLite to store and retrieve data related to clients and their files.

- **Imports**:
  - `import sqlite3`: Interacts with the SQLite database.
  - `import uuid`: Generates unique identifiers.
  - `from typing import List, Tuple, Union`: Provides type hints for better code readability.
  - `import binascii`: Converts between binary and ASCII.

- **Functions**:
  - `init_db() -> None`: Initializes the database and creates tables if they do not exist.
    ```python
    def init_db() -> None:
        """Initialize the database and create tables if they do not exist."""
        conn = sqlite3.connect('defensive.db')  # Connect to the SQLite database
        cursor = conn.cursor()  # Create a cursor object to execute SQL commands

        # Create the clients table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS clients (
                ID BLOB(16) PRIMARY KEY,  -- 16 bytes
                Name TEXT(255) UNIQUE,    -- string of 255 characters
                PublicKey BLOB(160),     -- 160 bytes
                LastSeen DATETIME,       -- Date and hour
                AESKey BLOB(32)          -- 256 bits (32 bytes)
            )
        ''')

        # Create the files table
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS files (
                ID BLOB(16),
                FileName TEXT(255),
                PathName TEXT(255),
                Verified BOOLEAN
            )
        ''')

        conn.commit()  # Commit the transaction
        conn.close()  # Close the connection
    ```

  - `register_client(name: str) -> bytes`: Registers a new client in the database.
    ```python
    def register_client(name: str) -> bytes:
        """Register a new client in the database."""
        conn = sqlite3.connect('defensive.db')  # Connect to the SQLite database
        cursor = conn.cursor()  # Create a cursor object to execute SQL commands
        try:
            client_id: bytes = uuid.uuid4().bytes  # Generate a UUID and get its 16-byte representation
            public_key: bytes = b'\x00' * 160  # 160 bytes of zeros
            last_seen: str = None  # Default to None for DATETIME
            aeskey: bytes = b'\x00' * 32  # 256 bits (32 bytes) of zeros

            cursor.execute('''
                INSERT INTO clients (ID, Name, PublicKey, LastSeen, AESKey) 
                VALUES (?, ?, ?, ?, ?)
            ''', (client_id, name, public_key, last_seen, aeskey))

            conn.commit()  # Commit the transaction
            return client_id  # Return the 16-byte UUID
        except sqlite3.IntegrityError as e:
            print(f"Error: {e}")
            return b""  # Return an empty byte string on error
        finally:
            conn.close()  # Close the connection
    ```

  - `update_public_key(client_id: bytes, new_public_key: bytes) -> bool`: Updates the public key of an existing client.
    ```python
    def update_public_key(client_id: bytes, new_public_key: bytes) -> bool:
        """Update the public key of an existing client."""
        conn = sqlite3.connect('defensive.db')  # Connect to the SQLite database
        cursor = conn.cursor()  # Create a cursor object to execute SQL commands
        try:
            cursor.execute('''
                UPDATE clients
                SET PublicKey = ?
                WHERE ID = ?
            ''', (new_public_key, client_id))

            conn.commit()  # Commit the transaction
            return cursor.rowcount > 0  # Return True if a row was updated, False otherwise
        except sqlite3.Error as e:


This file is essential for defining the structure and handling of various requests and responses within the server application.
### `encrypted_AES_key_response.py`
This file handles requests where the client sends a public key to the server and expects the server to respond with an encrypted AES key. It ensures secure communication by encrypting the AES key with the client's public key.

- **Imports**:
  - `import socket`: Provides access to the BSD socket interface.
  - `from Crypto.PublicKey import RSA`: Imports RSA key handling from the PyCryptodome library.
  - `from Crypto.Cipher import PKCS1_OAEP`: Imports the OAEP cipher for RSA encryption.
  - `from Crypto.Random import get_random_bytes`: Generates random bytes for the AES key.
  - `import binascii`: Contains methods to convert between binary and ASCII.
  - `from database_manager import update_public_key, get_id_by_name, get_client_info, update_AESkey`: Imports functions to manage client data.
  - `from common_functions import *`: Imports common functions and constants used across the project.

- **Classes**:
  - `EncryptedAESKeyResponse(Header)`: Represents a response containing an encrypted AES key.
    ```python
    class EncryptedAESKeyResponse(Header):
        def __init__(self, code: int, payload_size: int, client_id: bytes, cipher: bytes) -> None:
            super().__init__(code, payload_size)
            self.client_id: bytes = client_id
            self.cipher: bytes = cipher
    ```

- **Functions**:
  - `encrypt_aes_key_with_rsa(public_key_bytes: bytes, aes_key: bytes) -> bytes`: Encrypts the AES key with the provided RSA public key.
    ```python
    def encrypt_aes_key_with_rsa(public_key_bytes: bytes, aes_key: bytes) -> bytes:
        # Load the public key from the provided bytes
        public_key = RSA.import_key(public_key_bytes)
        cipher_rsa = PKCS1_OAEP.new(public_key)

        # Encrypt the AES key
        encrypted_aes_key = cipher_rsa.encrypt(aes_key)
        return encrypted_aes_key
    ```

  - `create_response(request: EncryptedAESKeyResponse) -> bytes`: Creates a response containing the encrypted AES key.
    ```python
    def create_response(request: EncryptedAESKeyResponse) -> bytes:
        header_response: bytes = create_header_response(request.version, request.code, request.payload_size)
        client_id: bytes = request.client_id
        print("cipher hex: ", binascii.hexlify(request.cipher))
        cipher: bytes = request.cipher  # Use the cipher directly without converting to bytes again
        response: bytes = header_response + client_id + cipher
        return response
    ```

  - `run_aes_key_generating_response(client_socket: socket.socket) -> None`: Handles the request to generate and encrypt an AES key, then sends the response to the client.
    ```python
    def run_aes_key_generating_response(client_socket: socket.socket) -> None:
        # Receive the data
        data: bytes = client_socket.recv(NAME_SIZE + PUBLIC_KEY_SIZE)
        print(len(data))
        if len(data) < NAME_SIZE + PUBLIC_KEY_SIZE:
            print("Incomplete data received")
            return

        name: str = data[:NAME_SIZE].decode('utf-8').rstrip('\x00')
        print(f"Name: {name}")
        client_id = get_id_by_name(name)
        public_key_bytes: bytes = data[NAME_SIZE:NAME_SIZE + PUBLIC_KEY_SIZE]
        print(f'Received public key (bytes): {binascii.hexlify(public_key_bytes)}')
        update_public_key(client_id, public_key_bytes)
        aes_key: bytes = get_random_bytes(16)  # AES key of 16 bytes
        update_AESkey(client_id, aes_key)
        print(binascii.hexlify(aes_key))
        encrypted_aes_key: bytes = encrypt_aes_key_with_rsa(public_key_bytes, aes_key)

        # Create the response with the encrypted AES key directly
        request = EncryptedAESKeyResponse(1602, ID_SIZE + len(encrypted_aes_key), client_id, encrypted_aes_key)
        response: bytes = create_response(request)

        # Send the response to the client
        client_socket.send(response)
        client_socket.close()
    ```

- **Purpose**:
  - This file is responsible for securely generating and encrypting an AES key using the client's public key. It ensures that the AES key is transmitted securely to the client, maintaining the confidentiality and integrity of the communication.

This file is crucial for handling secure key exchange between the server and the client.
import socket
import os
import zlib
from importlib.metadata import version
from bytes_code_casting import *
from database_manager import get_AesKey_by_id,insert_file_record
from common_functions import *
from Crypto.PublicKey import RSA
from Crypto.Cipher import AES
from Crypto.Util.Padding import unpad

#File received ok with CRC class
class CorrectCRCResponse(Header):
    def __init__(self, code: int, payload_size: int, client_id: bytes, content_size: bytes, file_name: bytes, check_sum: int) -> None:
        super().__init__(code, payload_size)
        self.client_id: bytes = client_id
        self.content_size: bytes = content_size
        self.file_name: bytes = file_name
        self.check_sum: int = check_sum
#This function calculate the checksum and return the correct bytes code in 4 bytes BY GETTING the str in bytes code
def calculate_crc32(byte_data):
    # Convert the string to bytes
    # Calculate the CRC32 checksum
    checksum = zlib.crc32(byte_data)& 0xFFFFFFFF
    return checksum
# This function get encrypted file and
# AES key that storage in database in this client_id
# Out the string of plaintext after decryption
def aes_decrypt(cipher_bytes, key):
    iv = bytes([0] * AES.block_size)  # Initialization vector (IV) should not be fixed in practical use
    cipher = AES.new(key, AES.MODE_CBC, iv)
    decrypted = unpad(cipher.decrypt(cipher_bytes), AES.block_size)
    return decrypted
# create response on bytes
# create the response of file received OK with CRC.
# response code 1603
def create_response(request: CorrectCRCResponse) -> bytes:
    header_response: bytes = create_header_response(request.version, request.code, request.payload_size)
    client_id: bytes = request.client_id
    content_size: bytes = request.content_size  # Use the cipher directly without converting to bytes again
    file_name:bytes=request.file_name
    check_sum:bytes=encode_to_unsigned_little_endian_4_bytes(request.check_sum)
    response: bytes = header_response + client_id + content_size+file_name+check_sum
    return response
#handele with the fist packet of payload od request 228
def handle_data(data,payload_size):
    priv_size = 0
    next_size = CONTENT_SIZE_SIZE
    content_size_data=data[priv_size:next_size]
    content_size = bytes_4_little_endian_to_big_endian( content_size_data)
    priv_size = next_size
    next_size += ORIG_FILE_SIZE_SIZE
    orig_file_size = bytes_4_little_endian_to_big_endian( data[priv_size:next_size])
    priv_size = next_size
    next_size += PACKET_NUMBER_SIZE
    packet_number = bytes_2_little_endian_to_big_endian( data[priv_size:next_size])
    priv_size = next_size
    next_size += TOTAL_PACKET_SIZE
    # print(f"packet number: {packet_number}")

    total_packets = bytes_2_little_endian_to_big_endian( data[priv_size:next_size])
    priv_size = next_size
    next_size += FILE_NAME_SIZE
    # print(f"total packet: {total_packets}")
    file_name_data=data[priv_size:next_size]
    file_name = ecode_string_using_UTF_8_And_remove_null_terminator(file_name_data)
    priv_size = next_size
    next_size = priv_size + payload_size
    # print(f"file name: {file_name}")

    complete_cipher = data[priv_size:]
    return content_size_data,content_size,orig_file_size,packet_number,total_packets,file_name_data,file_name,complete_cipher
#continue load elements in packets
def get_complete_cipher(client_socket,complete_cipher,packet_number ,total_packets,file_name):
    packet_number_place=CONTENT_SIZE_SIZE+ORIG_FILE_SIZE_SIZE
    complete_cipher_place=packet_number_place+PACKET_NUMBER_SIZE+TOTAL_PACKET_SIZE+ FILE_NAME_SIZE
    payload_size_place=ID_SIZE + VERSION_SIZE + CODE_SIZE

    while packet_number < total_packets:
        header = client_socket.recv(HEADER_SIZE)
        if not header:
            break
        payload_size = bytes_4_little_endian_to_big_endian( header[payload_size_place:payload_size_place + PAYLOAD_SIZE])
        payload = client_socket.recv(payload_size)
        complete_cipher += payload[complete_cipher_place:]
        packet_number = bytes_2_little_endian_to_big_endian( payload[packet_number_place:packet_number_place+PACKET_NUMBER_SIZE])
        print(
            f"Receiving packet {packet_number}/{total_packets} for file {file_name} ")

    return complete_cipher

def handle_file_sending_request(client_socket:socket.socket, client_id, payload_size):
    data = client_socket.recv(payload_size)
    content_size_data, content_size, orig_file_size, packet_number, total_packets, file_name_data, file_name,complete_cipher=handle_data(data,payload_size)
    complete_cipher=get_complete_cipher(client_socket,complete_cipher,packet_number,total_packets,file_name)
    client_id_hex=get_hex(client_id)
    # Print all elements in the buffer
    print(f"Server get code  request {FILE_SENDING_REQUEST_CODE} with the file name {file_name} and the cipher {get_hex(complete_cipher)}")
    client_dir = get_hex(client_id)
    try:
        AESKey = get_AesKey_by_id(client_id)
        if not AESKey:
            print(f"Server failed load public key from its database.\nThis happened because one of requests code: {REGISTRATION_REQUEST_CODE}, {PUBLIC_KEY_SENDING_REQUEST_CODE},{RECONNECT_REQUEST_CODE} NOT passed")
            req = Header(GENERAL_ERROR_CODE, FAILED_PAYLOAD_SIZE)
            response=create_general_failed_response()
            send_and_close(client_socket, response)
        decrypted_content = aes_decrypt(complete_cipher, AESKey)
        # Create directory if it does not exist
        if not os.path.exists(client_dir):
            os.makedirs(client_dir)

        # Write the decrypted content to the file in the directory
        file_path = os.path.join(client_dir, file_name)
        with open(file_path, 'wb') as f:
            f.write(decrypted_content)
        # Insert the file record into the database
        try:
            check_sum=calculate_crc32(decrypted_content)
            req = CorrectCRCResponse(CORRECT_CRC_CODE_RESPONSE, ID_SIZE+CONTENT_SIZE_SIZE+FILE_NAME_SIZE+CHECKSUM_SIZE,client_id, content_size_data, file_name_data,check_sum)
            insert_file_record(file_name, client_id, file_path)
            response=create_response(req)
            print(f"Server got decryption: ",decrypted_content.decode('unicode_escape'), f"and build the file in folder name {get_hex(client_id)}\n",f" and calculate the checksum and got {check_sum}")
            send_and_close(client_socket,response)
        except Exception as e:
            print(
                f" The  path to the file {file_name} with {file_path} that client {client_id_hex} sent is accepts correct.\nBut the server have an issue with its database so please re-register.\nerror{e}")
            response = create_general_failed_response()
            send_and_close(client_socket, response)
    except Exception as e:
        print(f" The  path to the file {file_name} client sent is not found. Please check your file path, and the disk. error.\n{e}" )
        response = create_general_failed_response()
        send_and_close(client_socket, response)
### `reconnect_AES_response.py`
This file handles client reconnection requests. It verifies the client's public key and, if valid, generates a new AES key for the client. If the public key is invalid or the client does not exist, it denies the request.

- **Imports**:
  - `from database_manager import get_client_info, delete_client_by_name, update_AESkey`: Imports functions to manage client data.
  - `from common_functions import *`: Imports common functions and constants used across the project.
  - `import binascii`: Contains methods to convert between binary and ASCII.
  - `import socket`: Provides access to the BSD socket interface.
  - `from cryptography.hazmat.primitives import serialization`: Provides functions for serializing and deserializing keys.
  - `from cryptography.hazmat.primitives.asymmetric import rsa`: Imports RSA key handling.
  - `from cryptography.hazmat.backends import default_backend`: Provides the default cryptographic backend.
  - `from Crypto.Random import get_random_bytes`: Generates random bytes for the AES key.
  - `from encrypted_AES_key_response import EncryptedAESKeyResponse, create_response, encrypt_aes_key_with_rsa`: Imports functions and classes for handling encrypted AES key responses.

- **Classes**:
  - `RequestDeniedResponse(Header)`: Represents a response for denied requests.
    ```python
    class RequestDeniedResponse(Header):
        def __init__(self, code: int, payload_size: int, client_id: bytes) -> None:
            super().__init__(code, payload_size)
            self.client_id: bytes = client_id
    ```

- **Functions**:
  - `request_denied(request: RequestDeniedResponse) -> bytes`: Creates a response for denied requests.
    ```python
    def request_denied(request: RequestDeniedResponse) -> bytes:
        # Create the header response
        header_response: bytes = create_header_response(request.version, request.code, request.payload_size)
        # Combine the header response with the client ID
        response: bytes = header_response + request.client_id
        return response
    ```

  - `is_valid_rsa_public_key(binary_key: bytes) -> bool`: Validates an RSA public key.
    ```python
    def is_valid_rsa_public_key(binary_key: bytes) -> bool:
        try:
            # Load the binary key as an RSA public key
            public_key = serialization.load_der_public_key(binary_key, backend=default_backend())

            # Check if the key is an instance of RSAPublicKey
            if isinstance(public_key, rsa.RSAPublicKey):
                return True
            else:
                return False
        except (ValueError, binascii.Error):
            # If there is an error in loading the key, it is not valid
            return False
    ```

  - `rerun_aes_key_generating_response(client_socket: socket.socket, client_id: str) -> None`: Handles the reconnection request, verifies the public key, and generates a new AES key if valid.
    ```python
    def rerun_aes_key_generating_response(client_socket: socket.socket, client_id: str) -> None:
        # Receive the data
        data: bytes = client_socket.recv(NAME_SIZE)
        if len(data) < NAME_SIZE:
            print("Incomplete data received")
            return

        name: str = data[:NAME_SIZE].decode('utf-8').rstrip('\x00')
        print(f" {name} try reconnect server.")

        client_info = get_client_info(name)
        incorrect_request = RequestDeniedResponse(1606, ID_SIZE, binascii.unhexlify(client_id))

        if client_info:
            client_id_table, public_key = client_info
            print(f"Server find {name}, that ID: {binascii.hexlify(client_id_table)}, Public Key: {binascii.hexlify(public_key)}")
            if binascii.hexlify(client_id_table).decode('utf-8') == client_id and is_valid_rsa_public_key(public_key):
                aes_key: bytes = get_random_bytes(16)  # AES key of 16 bytes
                update_AESkey(client_id_table, aes_key)
                print(f"Server create aes key  {binascii.hexlify(aes_key)}")
                encrypted_aes_key: bytes = encrypt_aes_key_with_rsa(public_key, aes_key)
                correct_request = EncryptedAESKeyResponse(1605, ID_SIZE + len(encrypted_aes_key), client_id_table, encrypted_aes_key)
                response: bytes = create_response(correct_request)
            else:
                delete_client_by_name(name)
                print(f"The server send error because this user {name} have an incorrect public key or he did not get id not exist")
                response: bytes = request_denied(incorrect_request)
        else:
            response: bytes = request_denied(incorrect_request)
            print(f"The server send error because this user {name} not exist")
        client_socket.send(response)
        client_socket.close()
    ```

- **Purpose**:
  - This file is responsible for handling client reconnection requests. It verifies the client's public key and, if valid, generates a new AES key for secure communication. If the public key is invalid or the client does not exist, it denies the request and informs the client.

This file is crucial for maintaining secure communication and ensuring that only valid clients can reconnect and receive new AES keys.
### `registration_response.py`
This file handles client registration requests. It checks if the client's name already exists in the server's database and registers the client if the name is not found.

- **Imports**:
  - `import socket`: Provides access to the BSD socket interface.
  - `from database_manager import register_client`: Imports the function to register a client in the database.
  - `from common_functions import *`: Imports common functions and constants used across the project.
  - `import binascii`: Contains methods to convert between binary and ASCII.

- **Classes**:
  - `RegistrationResponse(Header)`: Represents a response to a registration request.
    ```python
    class RegistrationResponse(Header):
        def __init__(self, code: int, payload_size: int, client_id: bytes) -> None:
            super().__init__(code, payload_size)
            self.client_id: bytes = client_id
    ```

- **Functions**:
  - `create_response(request: RegistrationResponse) -> bytes`: Creates a response for the registration request.
    ```python
    def create_response(request: RegistrationResponse) -> bytes:
        # Create the header response
        header_response: bytes = create_header_response(request.version, request.code, request.payload_size)
        # Combine all parts into the response
        client_id = request.client_id
        response: bytes = header_response + client_id
        return response
    ```

  - `run_registration_response(client_socket: socket.socket) -> None`: Handles the registration request, checks if the name exists, and registers the client if it does not.
    ```python
    def run_registration_response(client_socket: socket.socket) -> None:
        # Receive the data
        data: bytes = client_socket.recv(NAME_SIZE)
        if len(data) < NAME_SIZE:
            print("Incomplete name data received")
            return
        name_data: bytes = data[:NAME_SIZE]
        name: str = name_data.decode('utf-8').rstrip('\x00')
        client_id: bytes = register_client(name)
        if client_id == b"":
            print(f"Name: {name} already exist.")
            request = RegistrationResponse(REGISTRATION_WAS_FAILED_CODE, 0, b"")
        else:
            print(f"Name: {name} have registration successful.")
            request = RegistrationResponse(REGISTRATION_WAS_SUCCESS_CODE, ID_SIZE, client_id)
        response: bytes = create_response(request)
        client_socket.sendall(response)
        client_socket.shutdown(socket.SHUT_WR)
    ```

- **Purpose**:
  - This file is responsible for handling client registration requests. It ensures that each client has a unique name in the server's database. If the name already exists, it sends a failure response. If the name is new, it registers the client and sends a success response.

This file is crucial for managing client registrations and ensuring that each client has a unique identifier in the server's database.
import socket
import os
from registration_response import run_registration_response
from encrypted_AES_key_response import run_aes_key_generating_response
from reconnect_AEA_key_response import rerun_aes_key_generating_response
from database_manager import init_db
from file_recived_correct_with_CRC import handle_file_sending_request
from common_functions import *
import struct
import binascii
from typing import Union
from crc_respones import run_crc_response

def read_port() -> int:
    file_path = os.path.join(os.getcwd(), 'port.info')
    if os.path.exists(file_path):
        with open(file_path, 'r') as file:
            return int(file.read().strip())
    else:
        print("File 'port.info' not found. Using default port: 1256")
        return DEFAULT_PORT

def decode_client_id(data: bytes) -> str:
    client_id: bytes = data[:ID_SIZE]
    try:
        # Try to decode as a hex string
        hex_client_id: str = binascii.hexlify(client_id).decode('utf-8')
        return hex_client_id
    except Exception as e:
        # If decoding as hex fails, decode as a regular string
        client_id_str: str = client_id.decode('utf-8').rstrip('\x00')
        print(f"Client ID (string): {client_id_str}")
        return client_id_str

def start_server(host: str, port: int) -> None:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((host, port))
        server_socket.listen(1)
        print(f"Server listening on {host}:{port}")

        while True:
            client_socket, client_address = server_socket.accept()
            with client_socket:
                print(f"Connection from {client_address}")
                data: bytes = client_socket.recv(HEADER_SIZE)
                if len(data) < HEADER_SIZE:
                    print("Incomplete header received")
                    continue

                # Extract header
                hex_client_id: str = decode_client_id(data)
                version: int = struct.unpack('B', data[ID_SIZE:ID_SIZE + VERSION_SIZE])[0]
                code: int = struct.unpack('!H', data[ID_SIZE + VERSION_SIZE:ID_SIZE + VERSION_SIZE + CODE_SIZE])[0]
                payload_size: int = struct.unpack('!I', data[ID_SIZE + VERSION_SIZE + CODE_SIZE:ID_SIZE + VERSION_SIZE + CODE_SIZE + PAYLOAD_SIZE])[0]
                client_id: bytes = data[:ID_SIZE]
                print(f"Server got request {code} from client ID (in hex): {hex_client_id}")

                # Handle specific request types
                if code == REGISTRATION_REQUEST_CODE:
                    run_registration_response(client_socket)
                elif code == PUBLIC_KEY_SENDING_REQUEST_CODE:
                    run_aes_key_generating_response(client_socket)
                elif code == RECONNECT_REQUEST_CODE:
                    rerun_aes_key_generating_response(client_socket, hex_client_id)
                elif code == FILE_SENDING_REQUEST_CODE:
                    handle_file_sending_request(client_socket, client_id, payload_size)
                elif code == CORRECT_CRC_CODE or code == FATA_ERROR_CODE:
                    run_crc_response(client_socket, code, client_id)

if __name__ == '__main__':
    init_db()
    host: str = '127.0.0.1'
    port: int = read_port()
    start_server(host, port)
### `bytesCodeCasting.cpp`
This file provides functions for encoding and decoding data, which are essential for making requests in byte code. It includes functions to convert between different data formats, such as strings, hex, and binary.

- **Includes**:
  - `#include "bytesCodeCasting.h"`: Includes the header file for function declarations.
  - `#include <iostream>`: Provides input and output stream objects.
  - `#include <iomanip>`: Provides I/O manipulators.
  - `#include <boost/asio.hpp>`: Includes Boost.Asio for network programming.

- **Decode Functions**:
  - `std::string decodeByteCodeToHexString(const std::vector<uint8_t>& vec)`: Converts a vector of bytes to a hex string.
    ```cpp
    std::string decodeByteCodeToHexString(const std::vector<uint8_t>& vec) {
        std::ostringstream oss;
        for (auto byte : vec) {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        return oss.str();
    }
    ```

  - `std::string bytesToHexString(const char* bytes, size_t length)`: Converts a byte array to a hex string.
    ```cpp
    std::string bytesToHexString(const char* bytes, size_t length) {
        std::vector<uint8_t> vec(bytes, bytes + length);
        std::string str = decodeByteCodeToHexString(vec);
        return str;
    }
    ```

  - `std::string decodeByteCodeToString(const std::vector<uint8_t>& vec)`: Converts a vector of bytes to a string, stopping at the null terminator.
    ```cpp
    std::string decodeByteCodeToString(const std::vector<uint8_t>& vec) {
        // Find the position of the null terminator
        auto null_pos = std::find(vec.begin(), vec.end(), '\0');

        // Create a string from the byte code up to the null terminator
        return std::string(vec.begin(), null_pos);
    }
    ```

- **Encode Functions**:
  - `uint16_t toLittleEndian16(uint16_t value)`: Converts a 16-bit integer to little-endian format.
    ```cpp
    uint16_t toLittleEndian16(uint16_t value) {
        return (value >> 8) | (value << 8);
    }
    ```

  - `uint32_t toLittleEndian32(uint32_t value)`: Converts a 32-bit integer to little-endian format.
    ```cpp
    uint32_t toLittleEndian32(uint32_t value) {
        return ((value >> 24) & 0x000000FF) |
               ((value >> 8) & 0x0000FF00) |
               ((value << 8) & 0x00FF0000) |
               ((value << 24) & 0xFF000000);
    }
    ```

  - `void encodeStringToByteCode(std::vector<uint8_t>& vec, std::string str, int stringSizeOnumberOfByte)`: Encodes a string into a vector of bytes.
    ```cpp
    void encodeStringToByteCode(std::vector<uint8_t>& vec, std::string str, int stringSizeOnumberOfByte) {
        vec.resize(stringSizeOnumberOfByte, 0);
        std::memcpy(vec.data(), str.c_str(), std::min(str.size(), static_cast<std::size_t>(stringSizeOnumberOfByte)));
    }
    ```

  - `void encodeStringInArrToByteCode(std::vector<uint8_t>& vec, const char* str, int stringSizeOnumberOfByte)`: Encodes a C-string into a vector of bytes.
    ```cpp
    void encodeStringInArrToByteCode(std::vector<uint8_t>& vec, const char* str, int stringSizeOnumberOfByte) {
        vec.resize(stringSizeOnumberOfByte, 0);
        std::memcpy(vec.data(), str, std::min(std::strlen(str), static_cast<std::size_t>(stringSizeOnumberOfByte)));
    }
    ```

  - `uint8_t hexCharToByte(char c)`: Converts a hex character to its integer value.
    ```cpp
    uint8_t hexCharToByte(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        throw std::invalid_argument("Invalid hex character");
    }
    ```

  - `void encodeHexStringToByteCode(std::vector<uint8_t>& vec, const std::string& hexStr, int byteSize)`: Encodes a hex string to a binary format.
    ```cpp
    void encodeHexStringToByteCode(std::vector<uint8_t>& vec, const std::string& hexStr, int byteSize) {
        if (hexStr.size() != 2 * byteSize) {
            throw std::invalid_argument("Hex string length does not match the expected byte size");
        }
        vec.resize(byteSize);
        for (int i = 0; i < byteSize; ++i) {
            vec[i] = (hexCharToByte(hexStr[2 * i]) << 4) | hexCharToByte(hexStr[2 * i + 1]);
        }
    }
    ```

- **Purpose**:
  - This file is essential for converting data between different formats, which is necessary for making requests in byte code. It includes functions for encoding and decoding strings, hex values, and binary data, ensuring that data is correctly formatted for transmission.

This file is crucial for handling data conversion and ensuring that requests are properly encoded and decoded.
### `commonFunction.cpp`
This file contains utility functions that are useful for handling various client requests and responses. It includes functions for handling server responses, clearing buffers, creating headers, and establishing connections using Boost.Asio.

- **Includes**:
  - `#include <fstream>`: Provides file stream classes.
  - `#include <boost/asio.hpp>`: Includes Boost.Asio for network programming.
  - `#include <iostream>`: Provides input and output stream objects.
  - `#include "commonFunctions.h"`: Includes the header file for function declarations.
  - `#include <iomanip>`: Provides I/O manipulators.
  - `#include "bytesCodeCasting.h"`: Includes the header file for byte code casting functions.
  - `#include "client.h"`: Includes the header file for client-related functions.

- **Functions**:
  - `void handlingHeaderResponse(const char* response, ResponseHeader& resHeader)`: Handles the server response by extracting and converting the header fields.
    ```cpp
    void handlingHeaderResponse(const char* response, ResponseHeader& resHeader) {
        std::memcpy(&resHeader.version, response, VERSION_SIZE);
        std::memcpy(&resHeader.code, response + VERSION_SIZE, CODE_SIZE);
        std::memcpy(&resHeader.payload_size, response + VERSION_SIZE + CODE_SIZE, PAYLOAD_SIZE);
        // Convert from little-endian to host byte order
        resHeader.code = ntohs(*reinterpret_cast<uint16_t*>(const_cast<char*>(response + VERSION_SIZE)));
        resHeader.payload_size = ntohl(*reinterpret_cast<uint32_t*>(const_cast<char*>(response + VERSION_SIZE + CODE_SIZE)));
        resHeader.code = ntohs(resHeader.code);
        resHeader.payload_size = ntohl(resHeader.payload_size);
    }
    ```

  - `std::string loadClientId(const char* response)`: Loads the client ID from the server response and converts it to a hex string.
    ```cpp
    std::string loadClientId(const char* response) {
        char client_id[ID_SIZE];  // +1 for null-terminator
        std::memcpy(client_id, response + HEADER_SIZE_SERVER, ID_SIZE);
        std::string client_id_hex = bytesToHexString(client_id, ID_SIZE);
        return client_id_hex;
    }
    ```

  - `void clear(char* message, int length)`: Clears the buffer by setting all bytes to zero.
    ```cpp
    void clear(char* message, int length) {
        std::memset(message, 0, length);  // Ensures the buffer is zero-padded
    }
    ```

  - `void createBufferHeader(char* buffer, uint16_t code, uint32_t payload_size, std::vector<uint8_t>& client_id, uint8_t version)`: Creates a header in the buffer with the specified code, payload size, client ID, and version.
    ```cpp
    void createBufferHeader(char* buffer, uint16_t code, uint32_t payload_size, std::vector<uint8_t>& client_id, uint8_t version) {
        uint16_t code_le = toLittleEndian16(code);
        uint32_t payload_size_le = toLittleEndian32(payload_size);
        clear(buffer, HEADER_SIZE + NAME_SIZE);  // Initialize buffer with zeroes
        std::memcpy(buffer, client_id.data(), ID_SIZE);
        buffer[ID_SIZE] = version;
        std::memcpy(buffer + ID_SIZE + VERSION_SIZE, &code_le, CODE_SIZE);
        std::memcpy(buffer + ID_SIZE + VERSION_SIZE + CODE_SIZE, &payload_size_le, PAYLOAD_SIZE);
    }
    ```

  - `void connect(TransferInfo& info, tcp::socket& s, tcp::resolver& resolver)`: Establishes a connection to the server using Boost.Asio.
    ```cpp
    void connect(TransferInfo& info, tcp::socket& s, tcp::resolver& resolver) {
        std::cout << "Connecting to server... with port " << info.port << std::endl;
        boost::asio::connect(s, resolver.resolve(info.ip, std::to_string(info.port)));
        std::cout << "Connected to server\n";
    }
    ```

- **Purpose**:
  - This file provides essential utility functions for handling client-server communication. It includes functions for processing server responses, managing buffers, creating request headers, and establishing network connections. These functions are crucial for ensuring smooth and efficient communication between the client and the server.

This file is a key component for managing client requests and responses, making it easier to handle various operations required for client-server interactions.
### `registrationRequest.cpp`
This file handles the client-side registration request and response. It sends a registration request to the server and processes the server's response, including retrying the request if necessary.

- **Includes**:
  - `#include <iostream>`: Provides input and output stream objects.
  - `#include <fstream>`: Provides file stream classes.
  - `#include "client.h"`: Includes the header file for client-related functions.
  - `#include "commonFunctions.h"`: Includes the header file for common utility functions.
  - `#include "bytesCodeCasting.h"`: Includes the header file for byte code casting functions.

- **Structures**:
  - `struct RegistrationRequest`: Represents a registration request message.
    ```cpp
    struct RegistrationRequest : public Message {
        std::vector<uint8_t> name;
        RegistrationRequest() : name(NAME_SIZE, 0) {}
    };
    ```

  - `struct RegistrationResponse`: Represents a registration response message.
    ```cpp
    struct RegistrationResponse : public ResponseHeader {
        std::string client_id_hex;
    };
    ```

- **Functions**:
  - `std::string handleServerResponse(const char* response, size_t length, TransferInfo& info, int& retry_count, boost::asio::ip::tcp::socket& s, const std::string& nameStr)`: Handles the server's response to the registration request.
    ```cpp
    std::string handleServerResponse(const char* response, size_t length, TransferInfo& info, int& retry_count, boost::asio::ip::tcp::socket& s, const std::string& nameStr) {
        if (length < HEADER_SIZE_SERVER) {
            std::cerr << "Incomplete response received\n";
            return "";
        }
        RegistrationResponse res;
        handlingHeaderResponse(response, res);
        // Handle specific response codes
        if (res.code == 1600) {
            res.client_id_hex = loadClientId(response);
            std::ofstream outfile("me.info");
            if (outfile.is_open()) {
                outfile << nameStr << "\n" << res.client_id_hex << "\n";
                outfile.close();
                s.close();
                return res.client_id_hex;
            } else {
                std::cout << "me.info not open correct.\n";
                return "";
            }
        } else if (res.code == 1601) {
            if (retry_count < 2) {
                std::cout << "Server responded with an error 1601. Retrying...\n";
                s.close();
                retry_count++;
                return runRegistrationRequest(info, retry_count);
            } else {
                std::cerr << "Fatal error: server responded with an error after 3 attempts. The username might be taken.\n";
                s.close();
                return "";
            }
        }
    }
    ```

  - `std::string runRegistrationRequest(TransferInfo& info, int retry_count = 0)`: Sends a registration request to the server and handles the response.
    ```cpp
    std::string runRegistrationRequest(TransferInfo& info, int retry_count = 0) {
        const int max_length = 1024;
        try {
            // Set default values for RegistrationRequest
            RegistrationRequest req;
            req.client_id[0] = '\0';
            req.version = VERSION; // version 3
            req.code = 825;
            req.payload_size = NAME_SIZE; // Set payload size to the length of info.name
            req.name = info.name;

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
            } else if (error) {
                throw boost::system::system_error(error);
            }

            // Handle the server response
            return handleServerResponse(reply, reply_length, info, retry_count, s, decodeByteCodeToString(info.name));
        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << "\n";
            return "";
        }
    }
    ```

- **Purpose**:
  - This file is responsible for handling the client-side registration process. It sends a registration request to the server, processes the server's response, and retries the request if necessary. It ensures that the client is registered with a unique name and receives a client ID from the server.

This file is crucial for managing the client registration process and ensuring that the client can successfully register with the server.
### `publicKeySending.cpp`
This file handles the client-side process of sending a public RSA key to the server and receiving an encrypted AES key in response. It ensures secure communication by encrypting the AES key with the client's public key.

- **Includes**:
  - `#include "Base64Wrapper.h"`: Includes the header file for Base64 encoding and decoding.
  - `#include "RSAWrapper.h"`: Includes the header file for RSA encryption and decryption.
  - `#include "AESWrapper.h"`: Includes the header file for AES encryption and decryption.
  - `#include <iostream>`: Provides input and output stream objects.
  - `#include <fstream>`: Provides file stream classes.
  - `#include <boost/asio.hpp>`: Includes Boost.Asio for network programming.
  - `#include <iomanip>`: Provides I/O manipulators.
  - `#include "client.h"`: Includes the header file for client-related functions.
  - `#include "commonFunctions.h"`: Includes the header file for common utility functions.
  - `#include "bytesCodeCasting.h"`: Includes the header file for byte code casting functions.

- **Structures**:
  - `struct RSAGenerating`: Represents a message for generating an RSA key.
    ```cpp
    struct RSAGenerating : public Message {
        std::vector<uint8_t> name;
        std::string pubKey160Bytes;
        RSAGenerating() : name(NAME_SIZE, 0) {}
    };
    ```

- **Functions**:
  - `void EncryptedAESKeySendingResponse::hundlePrivateKey(RSAPrivateWrapper& rsapriv, static std::string base64key) const`: Handles the private key by encoding it as Base64 and saving it to a file.
    ```cpp
    void EncryptedAESKeySendingResponse::hundlePrivateKey(RSAPrivateWrapper& rsapriv, static std::string base64key) const {
        std::string filePath = "me.info"; // Replace with your file path
        std::ofstream meFile(filePath, std::ios::app); // Open in append mode

        if (meFile.is_open()) {
            meFile << base64key; // Write the new line
            meFile.close();
        } else {
            std::cerr << "Unable to open file for writing\n";
        }
        std::ofstream privfile("priv.key");
        if (privfile.is_open()) {
            privfile << base64key;
            privfile.close();
        } else {
            std::cout << "priv.key file created with name and UUID.\n";
        }
    }
    ```

  - `std::string EncryptedAESKeySendingResponse::decryptAesKeyCipher(EncryptedAESKeySendingResponse& res, const char* response, size_t length, boost::asio::ip::tcp::socket& s, RSAPrivateWrapper& rsapriv, std::string privateKey) const`: Decrypts the AES key cipher received from the server.
    ```cpp
    std::string EncryptedAESKeySendingResponse::decryptAesKeyCipher(EncryptedAESKeySendingResponse& res, const char* response, size_t length, boost::asio::ip::tcp::socket& s, RSAPrivateWrapper& rsapriv, std::string privateKey) const {
        res.client_id_hex = loadClientId(response);
        std::string cipher(response + HEADER_SIZE_SERVER + ID_SIZE, length - HEADER_SIZE_SERVER - ID_SIZE);

        std::string base64key = Base64Wrapper::encode(privateKey);
        hundlePrivateKey(rsapriv, base64key);

        RSAPrivateWrapper rsapriv_other(Base64Wrapper::decode(base64key));
        std::string decrypted = rsapriv_other.decrypt(cipher);

        s.close();
        return decrypted;
    }
    ```

  - `std::string handleServerResponse(const char* response, size_t length, boost::asio::ip::tcp::socket& s, RSAPrivateWrapper& rsapriv, std::string base64key)`: Handles the server's response to the public key sending request.
    ```cpp
    std::string handleServerResponse(const char* response, size_t length, boost::asio::ip::tcp::socket& s, RSAPrivateWrapper& rsapriv, std::string base64key) {
        EncryptedAESKeySendingResponse res;
        handlingHeaderResponse(response, res);

        if (res.code == 1602) {
            return res.decryptAesKeyCipher(res, response, length, s, rsapriv, base64key);
        } else {
            return "";
        }
    }
    ```

  - `std::string runPublicKeySending(TransferInfo& info)`: Sends the public key to the server and handles the response.
    ```cpp
    std::string runPublicKeySending(TransferInfo& info) {
