import socket


# Start scoket
s = socket.socket()
s.bind(('192.168.4.1', 8090)) 
s.listen(0) # increase number for more clients

while True:
    client, addr = s.accept() # receive a connection

    while True:
        content = client.recv(32) #receive 32 bytes of data

        if len(content) == 0: # 0 if client disconnects
            break
        else:
            print(content)
    
    print("Closing connection")
    client.close()
    