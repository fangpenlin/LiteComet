# A simple comet client for debugging
import sys
import socket

def write():
    HOST = '127.0.0.1'
    PORT = 9080
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))

    content = 'Hello baby!'
    s.send('GET /comet?channel_name=ab_benchmark&data=hahaha HTTP/1.1\r\nContent-Length: %d\r\n\r\n%s' % (
        len(content), content
    ))

    while True:
        data = s.recv(1024)
        if not data:
            break
        print repr(data)
    s.close()

    print 'Closed by peer'

while True:
    write()
