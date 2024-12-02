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
