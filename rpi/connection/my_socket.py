import socket
from logger.logger import Log
import struct

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

    def bind(self, port):
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
        self.sock.bind(('', port))

    def listen(self, n):
        self.sock.listen(n)

    def accept(self):
        return self.sock.accept()

    def fileno(self):
        return self.sock.fileno()

    def get_sock(self):
        return self.sock

    def set_timeout(self, timeout):
        self.sock.settimeout(timeout)

    def get_msg(self):
        msg_len = self.myreceive(2)
        unpacked_len = struct.unpack('!H', msg_len)[0]
        Log.log(3, 'received msg len is {}'.format(unpacked_len))
        msg = self.myreceive(unpacked_len - 2)
        return msg_len + msg

