import sqlite3  # Import the sqlite3 module to interact with the SQLite database
import uuid  # Import the uuid module to generate unique identifiers
from typing import List, Tuple, Union  # Import typing for type hints
import binascii  # Import binascii for binary-to-ASCII conversions

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
        print(f"Error: {e}")
        return False
    finally:
        conn.close()  # Close the connection

def get_client_info(name: str) -> Union[Tuple[bytes, bytes], None]:
    """Retrieve the ID and public key of a client by name."""
    conn = sqlite3.connect('defensive.db')  # Connect to the SQLite database
    cursor = conn.cursor()  # Create a cursor object to execute SQL commands
    try:
        cursor.execute('''
            SELECT ID, PublicKey
            FROM clients
            WHERE Name = ?
        ''', (name,))

        result = cursor.fetchone()  # Fetch one result
        if result:
            return result  # Returns a tuple (ID, PublicKey)
        else:
            return None
    except sqlite3.Error as e:
        print(f"Error: {e}")
        return None
    finally:
        conn.close()  # Close the connection

def get_id_by_name(name: str) -> bytes:
    """Retrieve the ID of a client by name."""
    conn = sqlite3.connect('defensive.db')  # Connect to the SQLite database
    cursor = conn.cursor()  # Create a cursor object to execute SQL commands
    try:
        cursor.execute('''
            SELECT ID
            FROM clients
            WHERE Name = ?
        ''', (name,))

        result = cursor.fetchone()  # Fetch one result
        if result:
            return result[0]  # Return the ID
        else:
            return b""  # Return an empty byte string if the name is not found
    except sqlite3.Error as e:
        print(f"Error: {e}")
        return b""  # Return an empty byte string on error
    finally:
        conn.close()  # Close the connection

def delete_client_by_name(name: str) -> bool:
    """Delete a client from the database by name."""
    conn = sqlite3.connect('defensive.db')  # Connect to the SQLite database
    cursor = conn.cursor()  # Create a cursor object to execute SQL commands
    try:
        cursor.execute('''
            DELETE FROM clients
            WHERE Name = ?
        ''', (name,))

        conn.commit()  # Commit the transaction
        return cursor.rowcount > 0  # Return True if a row was deleted, False otherwise
    except sqlite3.Error as e:
        print(f"Error: {e}")
        return False
    finally:
        conn.close()  # Close the connection

def update_AESkey(client_id: bytes, new_aes_key: bytes) -> bool:
    """Update the AES key of an existing client."""
    conn = sqlite3.connect('defensive.db')  # Connect to the SQLite database
    cursor = conn.cursor()  # Create a cursor object to execute SQL commands
    try:
        cursor.execute('''
            UPDATE clients
            SET AESKey = ?
            WHERE ID = ?
        ''', (new_aes_key, client_id))

        conn.commit()  # Commit the transaction
        return cursor.rowcount > 0  # Return True if a row was updated, False otherwise
    except sqlite3.Error as e:
        print(f"Error: {e}")
        return False
    finally:
        conn.close()  # Close the connection

def get_AesKey_by_id(client_id: bytes) -> bytes:
    """Retrieve the AES key of a client by ID."""
    conn = sqlite3.connect('defensive.db')  # Connect to the SQLite database
    cursor = conn.cursor()  # Create a cursor object to execute SQL commands
    try:
        cursor.execute('''
            SELECT AESKey
            FROM clients
            WHERE ID = ?
        ''', (client_id,))

        result = cursor.fetchone()  # Fetch one result
        if result:
            return result[0]  # Return the AES key
        else:
            return b""  # Return an empty byte string if the client ID is not found
    except sqlite3.Error as e:
        print(f"Error: {e}")
        return b""  # Return an empty byte string on error
    finally:
        conn.close()  # Close the connection

# Users functions:
def insert_file_record(id: bytes, fileName: str, pathName: str) -> None:
    """Insert a new file record into the database."""
    conn = sqlite3.connect('defensive.db')  # Connect to the SQLite database
    cursor = conn.cursor()  # Create a cursor object to execute SQL commands

    # Insert a new row into the files table
    cursor.execute('''
        INSERT INTO files (ID, FileName, PathName, Verified)
        VALUES (?, ?, ?, ?)
    ''', (id, fileName, pathName, False))

    conn.commit()  # Commit the transaction
    conn.close()  # Close the connection

def verify_file_by_client_id(client_id: bytes) -> bool:
    """Verify a file by client ID."""
    conn = sqlite3.connect('defensive.db')  # Connect to the SQLite database
    cursor = conn.cursor()  # Create a cursor object to execute SQL commands
    try:
        cursor.execute('''
            UPDATE files
            SET Verified = 1
            WHERE ID = ?
        ''', (client_id,))

        conn.commit()  # Commit the transaction
        return cursor.rowcount > 0  # Return True if a row was updated, False otherwise
    except sqlite3.Error as e:
        print(f"Error: {e}")
        return False
    finally:
        conn.close()  # Close the connection
