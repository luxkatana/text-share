from socket import socket as ClientSocket
from threading import Thread
SERVER_TO_LISTEN_IP: str =  '127.0.0.1'
SERVER_TO_LISTEN_PORT: int = 9099


def yield_until_wifi() -> ClientSocket:
    buffer_socket: ClientSocket = ClientSocket()
    while True:
        try:
            buffer_socket.connect((SERVER_TO_LISTEN_IP, SERVER_TO_LISTEN_PORT))
        except: #noqa
            print("ok got it")
        else:
            return buffer_socket

def listen(socket: ClientSocket):
    while 1:
        print('\n', socket.recv(270).decode())
if __name__ == '__main__':
    socket = yield_until_wifi()
    print("Connection estabilished")
    while True:
        if (input_ := input(">>").strip()):
            socket.send(input_.encode())
            print(f"Sent {input_}")

    
