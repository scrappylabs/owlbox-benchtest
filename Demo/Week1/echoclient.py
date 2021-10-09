#!/usr/bin/env python3

import socket

HOST = socket.gethostbyname(socket.gethostname())  # The server's hostname or IP address
PORT = 65432        # The port used by the server

print(HOST)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b'Hello, world')
    data = s.recv(1024)

print('Received', repr(data))