import struct
import binascii

# Decode functions

def bytes_4_little_endian_to_big_endian(data):
    # Unpack the binary data to get the integer (4 bytes, big-endian)
    data_size = struct.unpack('!I', data)[0]
    return data_size

def bytes_2_little_endian_to_big_endian(data):
    # Unpack the binary data to get the integer (2 bytes, big-endian)
    data_size = struct.unpack('!H', data)[0]
    return data_size

def decode_string_using_UTF_8(data):
    # Decode the binary data to a UTF-8 string
    data_str = data.decode('utf-8')
    return data_str

def ecode_string_using_UTF_8_And_remove_null_terminator(data):
    # Decode the binary data to a UTF-8 string and remove null terminators
    data_str_with_out_nulls = decode_string_using_UTF_8(data).rstrip('\x00')
    return data_str_with_out_nulls

def get_hex(data):
    # Convert binary data to a hexadecimal string
    data_hex = decode_string_using_UTF_8(binascii.hexlify(data))
    return data_hex

def int_to_1_byte(data):
    # Convert an integer to a 1-byte binary representation
    data_byte: bytes = struct.pack('B', data)
    return data_byte

def encode_to_unsigned_little_endian_2_bytes(data):
    # Convert an integer to a 2-byte little-endian binary representation
    data_bytes: bytes = struct.pack('<H', data)
    return data_bytes

def encode_to_unsigned_little_endian_4_bytes(data):
    # Convert an integer to a 4-byte little-endian binary representation
    data_bytes: bytes = struct.pack('<I', data)  # '<I' for little-endian unsigned int
    return data_bytes

def convert_int_to_4_bytes(data):
    # Convert an integer to a 4-byte big-endian binary representation
    return data.to_bytes(4, byteorder='big')
