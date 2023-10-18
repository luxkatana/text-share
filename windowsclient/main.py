from socket import socket as ClientSocket
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

if __name__ == '__main__':
    socket = yield_until_wifi()
    print("Connection estabilished")
    while True:
        if (input_ := input(">>").strip()):
            socket.send(input_.encode())
            print(f"Sent {input_}")

    
