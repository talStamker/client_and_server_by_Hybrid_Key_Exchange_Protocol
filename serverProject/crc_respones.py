import socket
from database_manager import register_client,verify_file_by_client_id
from common_functions import *
import binascii


class CRCResponse(Header):
    def __init__(self, code: int, payload_size: int, client_id: bytes) -> None:
        super().__init__(code, payload_size)
        self.client_id: bytes = client_id
#create header for request
def create_response(request: CRCResponse) -> bytes:
    header_response: bytes = create_header_response(request.version, request.code, request.payload_size)
    # Combine all parts into the response
    client_id=request.client_id
    response: bytes = header_response + client_id
    return response
#get the elemnt from client in request
def run_crc_response(client_socket: socket.socket,code,client_id) -> None:
    data: bytes = client_socket.recv(NAME_SIZE)
    if len(data) < NAME_SIZE:
        print("Incomplete name data received")
        return
    name_data: bytes = data[:NAME_SIZE]
    name: str = name_data.decode('utf-8').rstrip('\x00')
    if code==CORRECT_CRC_CODE:
        print(f"Server accept the file {name}")
        request = CRCResponse(ACCEPT_CODE, ID_SIZE, client_id)
        verify_file_by_client_id(client_id)
        response: bytes = create_response(request)
        client_socket.sendall(response)
        client_socket.shutdown(socket.SHUT_WR)
    elif code==FATA_ERROR_CODE:
        print(f"Fatal error client try sent 3 times to server but there are miss knowledge")
        request = CRCResponse(ACCEPT_CODE, ID_SIZE, client_id)
        response: bytes = create_response(request)
        client_socket.sendall(response)
        client_socket.shutdown(socket.SHUT_WR)

