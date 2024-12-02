import socket
from database_manager import register_client
from common_functions import *
import binascii

class RegistrationResponse(Header):
    def __init__(self, code: int, payload_size: int, client_id: bytes) -> None:
        super().__init__(code, payload_size)
        self.client_id: bytes = client_id

def create_response(request: RegistrationResponse) -> bytes:
    # Create the header response
    header_response: bytes = create_header_response(request.version, request.code, request.payload_size)
    # Combine all parts into the response
    client_id = request.client_id
    response: bytes = header_response + client_id
    return response

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
