from database_manager import get_client_info, delete_client_by_name, update_AESkey
from common_functions import *
import binascii
import socket
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.backends import default_backend
from Crypto.Random import get_random_bytes
from encrypted_AES_key_response import EncryptedAESKeyResponse, create_response, encrypt_aes_key_with_rsa

class RequestDeniedResponse(Header):
    def __init__(self, code: int, payload_size: int, client_id: bytes) -> None:
        super().__init__(code, payload_size)
        self.client_id: bytes = client_id

def request_denied(request: RequestDeniedResponse) -> bytes:
    # Create the header response
    header_response: bytes = create_header_response(request.version, request.code, request.payload_size)
    # Combine the header response with the client ID
    response: bytes = header_response + request.client_id
    return response

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
