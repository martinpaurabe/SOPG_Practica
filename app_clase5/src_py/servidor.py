import socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((127.0.0.1:5001))
data, address = sock.recvfrom(1024)
print(data.decode())
sock.close()
