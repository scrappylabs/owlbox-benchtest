import socket 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print "Socket Created"
host = raw_input("HOST >>>   ")

# 3. Use the same port no. entered on server.py as the client binds to the 
#    same port
port = input ("Enter the PORT number (1 - 10,000)") 
s.connect((host, port))

while True:   
    message = raw_input(">>>  ")    
    s.sendall(message)
    reply = s.recv(1024)
    print reply