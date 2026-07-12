import socket
import sys
import time

HOST = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
PORT = int(sys.argv[2]) if len(sys.argv) > 2 else 11001
ACTION = sys.argv[3] if len(sys.argv) > 3 else "view"

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.settimeout(5)

try:
    client.connect((HOST, PORT))
    client.sendall(ACTION.encode())
    data = client.recv(1024).decode().strip()
    print(data)
finally:
    client.close()
