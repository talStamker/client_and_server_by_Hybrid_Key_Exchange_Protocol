import socket
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP
from Crypto.Random import get_random_bytes
import binascii
from database_manager import update_public_key, get_id_by_name, get_client_info, update_AESkey
from common_functions import *

class EncryptedAESKeyResponse(Header):
    def __init__(self, code: int, payload_size: int, client_id: bytes, cipher: bytes) -> None:
        super().__init__(code, payload_size)
        self.client_id: bytes = client_id
        self.cipher: bytes = cipher

def encrypt_aes_key_with_rsa(public_key_bytes: bytes, aes_key: bytes) -> bytes:
    # Load the public key from the provided bytes
    public_key = RSA.import_key(public_key_bytes)
    cipher_rsa = PKCS1_OAEP.new(public_key)

    # Encrypt the AES key
    encrypted_aes_key = cipher_rsa.encrypt(aes_key)
    return encrypted_aes_key

def create_response(request: EncryptedAESKeyResponse) -> bytes:
    header_response: bytes = create_header_response(request.version, request.code, request.payload_size)
    client_id: bytes = request.client_id
    print("cipher hex: ", binascii.hexlify(request.cipher))
    cipher: bytes = request.cipher  # Use the cipher directly without converting to bytes again
    response: bytes = header_response + client_id + cipher
    return response

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
