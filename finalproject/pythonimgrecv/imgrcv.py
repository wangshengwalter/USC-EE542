import socket
import struct
import os

def receive_image(host, port):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(1)
    print(f"Server listening on {host}:{port}")

    conn, addr = server_socket.accept()
    print(f"Connection established with {addr}")

    try:
        # Function to receive exact number of bytes
        def recv_exact(sock, size):
            data = bytearray()
            while len(data) < size:
                packet = sock.recv(size - len(data))
                if not packet:
                    return None
                data.extend(packet)
            return data

        # Receive two double numbers (16 bytes)
        double_data = recv_exact(conn, 16)
        if not double_data:
            print("Failed to receive two double numbers")
            return

        x, y = struct.unpack('>dd', double_data)  # Big-endian 8-byte double
        print(f"Received x={x}, y={y}")
        
        # Receive the size of the image (4 bytes)
        size_data = recv_exact(conn, 4)
        if not size_data:
            print("Failed to receive size header")
            return

        image_size = struct.unpack('>I', size_data)[0]  # Big-endian 4-byte integer
        print(f"Expected image size: {image_size} bytes")

        # Receive the image data
        image_data = recv_exact(conn, image_size)
        if not image_data:
            print("Incomplete image received.")
            return

        # Save the image to the specified directory
        save_path = os.path.join('.', 'finalproject', 'pythonimgrecv', 'uploads', 'received_image.png')
        os.makedirs(os.path.dirname(save_path), exist_ok=True)  # Create the directory if it doesn't exist

        with open(save_path, "wb") as f:
            f.write(image_data)
        print(f"Image received and saved to '{save_path}'")

    finally:
        conn.close()
        server_socket.close()

if __name__ == "__main__":
    receive_image('0.0.0.0', 5000)
