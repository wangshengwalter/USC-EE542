import socket
import struct

def receive_image(host, port):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(1)
    print(f"Server listening on {host}:{port}")

    conn, addr = server_socket.accept()
    print(f"Connection established with {addr}")

    try:
        # Receive the size of the image
        size_data = conn.recv(4)
        if len(size_data) < 4:
            print("Failed to receive size header")
            return

        image_size = struct.unpack('>I', size_data)[0]  # Big-endian 4-byte integer

        print(f"Expected image size: {image_size} bytes")

        # Receive the image data
        image_data = bytearray()
        while len(image_data) < image_size:
            packet = conn.recv(image_size - len(image_data))
            if not packet:
                break
            image_data.extend(packet)

        if len(image_data) == image_size:
            # Save the image to a file
            with open("received_image.png", "wb") as f:
                f.write(image_data)
            print("Image received and saved as 'received_image.png'")
        else:
            print("Incomplete image received.")

    finally:
        conn.close()
        server_socket.close()

if __name__ == "__main__":
    receive_image('0.0.0.0', 5000)
