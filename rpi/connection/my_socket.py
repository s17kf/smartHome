import socket
from logger.logger import Log


class MySocket:

    def __init__(self, sock=None):
        if sock is None:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        else:
            self.sock = sock

    def connect(self, host, port):
        self.sock.connect((host, port))

    def mysend(self, msg):
        totalsent = 0
        while totalsent < len(msg):
            sent = self.sock.send(msg[totalsent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totalsent = totalsent + sent
        Log.log(5, 'sent msg: {}'.format(msg))
        return totalsent

    def myreceive(self, to_read):
        chunks = []
        bytes_recd = 0
        while bytes_recd < to_read:
            chunk = self.sock.recv(to_read - bytes_recd)
            if chunk == b'':
                raise RuntimeError("socket connection broken")
            chunks.append(chunk)
            bytes_recd += len(chunk)
        # Log.log(5, 'received msg is {}'.format(chunks))
        return b''.join(chunks)

    def close(self):
        self.sock.close()

