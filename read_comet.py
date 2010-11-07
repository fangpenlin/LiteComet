# A simple comet client for debugging
import sys
import socket

HOST = '127.0.0.1'
PORT = 8080
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
s.send('GET /comet?channel=test_channel&timeout=5000 HTTP/1.1\r\n\r\n')

while True:
    data = s.recv(1024)
    if not data:
        break
    print repr(data)
s.close()

print 'Closed by peer'
