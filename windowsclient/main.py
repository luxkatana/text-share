from socket import socket as ClientSocket
from threading import Thread
from wintoast import ToastNotifier
SERVER_TO_LISTEN_IP: str =  '127.0.0.1'
SERVER_TO_LISTEN_PORT: int = 9099


def yield_until_wifi() -> ClientSocket:
    buffer_socket: ClientSocket = ClientSocket()
    while True:
        try:
            buffer_socket.connect((SERVER_TO_LISTEN_IP, SERVER_TO_LISTEN_PORT))
        except: #noqa
            ...
        else:
            return buffer_socket

def listen(socket: ClientSocket, is_notifier: bool):
    while 1:
        content = socket.recv(257).decode()
        if is_notifier:
            toaster = ToastNotifier()
            toaster.show_toast("From python", msg=content, duration=10, icon_path='')
        print(content)
if __name__ == '__main__':
    socket = yield_until_wifi()
    print("Connection estabilished")
    socket.send("hello".encode())
    first = socket.recv(200).decode()
    print(first)
    if first[:-1] == 'notifier=1':
        Thread(target=listen, args=(socket, True), daemon=True).start()
        print("Found server")
        while True:
            match input('>>'):
                case 'exit':
                    break
    elif first == 'hello':
        listener = Thread(target=listen, args=(socket, False), daemon=True)
        listener.start()
        print("Success")
        while True:
            if (input_ := input(">>").strip()):
                continue
            elif input_ == 'exit':
                break
            socket.send(input_.encode())
    else:
        print("Server didn't respond back, closing")
        socket.close()
