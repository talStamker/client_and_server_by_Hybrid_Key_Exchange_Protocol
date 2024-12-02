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
