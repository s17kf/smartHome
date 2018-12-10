import socket, time
from logger.logger import Log

class Connection:
    def __init__(self, port, serverIP):
        self.__port = port
        self.__serverIP = serverIP
        self.__socket = socket.socket()
        self.__connected = False
        #TODO: connected status is here and in client class - not good

    def connect(self):
        self.__socket.connect((self.__serverIP, self.__port))
        self.__connected = True
        self.__connect_time = time.time()

    def disconnect(self):
        self.__socket.close()

    def send(self, msg):
        return self.__socket.send(msg)

    def sendFormattedMsg(self, msg):
        msg_len = len(msg) + 2
        Log.log(5, 'sending {} bytes: {}'.format(msg_len, (msg_len).to_bytes(2, 'little') + msg))
        return self.send(msg_len.to_bytes(2, 'little') + msg)

    # def sendFormattedMsg(self, msg, length):
    #     print('sending {l1} bytes: {b1}'.format(l1 = length, b1 = (length + 2).to_bytes(2, 'little') + msg))
    #     return self.send((length + 2).to_bytes(2, 'little') + msg)

    def recv(self, lenght):
        return self.__socket.recv(lenght)

    def getConnectTime(self):
        #TODO error if no connected
        return self.__connect_time


if __name__ == '__main__':
    con = Connection(1234, '192.168.1.19')
    con.connect()
#    time.sleep(4.1)
    con.send(b'\x64\x67\x00\x6a\x6b\x00')
    print('3 bytes sent')
    answer = con.recv(16)
    print(answer)
    answer = con.recv(16)
    print(answer)
#    answer = con.recv(16)
#    print(answer)
#    answer = con.recv(16)
#    print(answer)
#    answer = con.recv(16)
#    print(answer)
    con.disconnect()
