import struct
from typing import Union
import socket


# Codes of requests
REGISTRATION_REQUEST_CODE: int = 825
PUBLIC_KEY_SENDING_REQUEST_CODE: int = 826
RECONNECT_REQUEST_CODE=827
FILE_SENDING_REQUEST_CODE=828
CORRECT_CRC_CODE = 900
INCORRECT_CRC_CODE_REPEAT = 901
FATA_ERROR_CODE = 902

# Codes of responses
REGISTRATION_WAS_SUCCESS_CODE: int = 1600
REGISTRATION_WAS_FAILED_CODE: int = 1601
PRIVATE_KEY_ENCRYPTION_CODE: int = 1602
CORRECT_CRC_CODE_RESPONSE=1603
GENERAL_ERROR_CODE=1607
ACCEPT_CODE = 1604

# Header sizes
ID_SIZE: int = 16
VERSION_SIZE: int = 1
CODE_SIZE: int = 2
PAYLOAD_SIZE: int = 4
HEADER_SIZE: int = ID_SIZE + VERSION_SIZE + CODE_SIZE + PAYLOAD_SIZE

# Payload sizes
NAME_SIZE: int = 255
PUBLIC_KEY_SIZE: int = 160
CONTENT_SIZE_SIZE:int = PAYLOAD_SIZE
ORIG_FILE_SIZE_SIZE:int = PAYLOAD_SIZE
PACKET_NUMBER_SIZE:int = CODE_SIZE
TOTAL_PACKET_SIZE:int = CODE_SIZE
FILE_NAME_SIZE:int = NAME_SIZE
MESSAGE_CONTENT_SIZE = 730
CHECKSUM_SIZE=PAYLOAD_SIZE
DEFAULT_PORT: int = 1256

FAILED_PAYLOAD_SIZE=0
VERSION=3
#This header for respose
class Header:
    def __init__(self, code: int, payload_size: int) -> None:
        self.version: int = VERSION
        self.code: int = code
        self.payload_size: int = payload_size
#for response it convert the elements to baynary code
def create_header_response(version: int, code: int, payload_size: int) -> bytes:
    version_bytes: bytes = struct.pack('B', version)
    code_bytes: bytes = struct.pack('<H', code)  # '<H' for little-endian unsigned short
    payload_size_bytes: bytes = struct.pack('<I', payload_size)  # '<I' for little-endian unsigned int
    return version_bytes + code_bytes + payload_size_bytes
#for 1607 response
def create_general_failed_response():
    return create_header_response(VERSION,GENERAL_ERROR_CODE,FAILED_PAYLOAD_SIZE)
# send to client and close the socket
def send_and_close(client_socket:socket.socket,response:bytes):
    client_socket.send(response)
    client_socket.close()

