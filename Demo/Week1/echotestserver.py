import socket
#from thread import *

# 1.Gets the local ip/ip over LAN.
HOST ='192.168.7.7'

print (HOST)

# 2.Use port no. above 1800 so it does not interfere with ports already in use.
PORT =int(input ("Enter the PORT number (1 - 10,000)")) 

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM )
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
print ("Socket Created on: ", HOST, ":", PORT)
s.bind((HOST, PORT))
print ("Socket Bind Complete")
s.listen(10)
print ("Socket now listening")
while True:
    connection, addr = s.accept()
    print ("Connection Established!")
    connection.send("Welcome to the server. Type something and hit enter\n")
    while True:
        data = connection.recv(1024)
        if not data:
            break
        connection.sendall(data)
        print (data)
        connection.close()
s.close()