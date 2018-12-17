import socket, time, threading
from select import select
from logger.logger import Log
from connection.msgtype import MsgType


class rpi_server(threading.Thread):
    def __init__(self, port, server_ip, timeout=10, endian = 'little'):
        threading.Thread.__init__(self);
        self.__port = port
        self.__server_ip = server_ip
        self.__timeout = timeout
        self.__endian = endian
        self.__socket = socket.socket()
        self.__socket.bind(('', port))
        self.__socket.listen(1)
        self.__is_stopped = threading.Event
        self.__inputs = []

    def run(self):
        while not self.__is_stopped.is_set():
            acceptable = select([self.__socket], None, None, self.__timeout)
            if self.__socket in acceptable:
                Log.log(2, 'connection from server')
                connection = self.__socket.accept()
                self.__inputs.append(connection)
            if len(self.__inputs) > 0:
                readable = select(self.__inputs, None, None, self.__timeout)
                for s in readable:
                    self.server_ask_service(s)
                # TODO: if there is no read enable in few tries close connection

    def server_ask_service(self, fd):
        msg = fd.recv(2)
        msg_len = int.from_bytes(msg, self.__endian)
        msg = fd.recv(msg_len - 2)
        # TODO: check (received) msg len
        received_packet = msg[0]
        try:
            received_packet = MsgType(received_packet.to_bytes(1, self.__endian))
        except ValueError:
            Log.log(0, 'rpi_server: unresolved packet received: {}'.format(received_packet))
            return None
            # TODO: some action with unresolved packet
        else:
            if received_packet == MsgType.set:
                Log.log(1, 'received set {} from server'.format(int.from_bytes(msg[1:3], self.__endian)))
                self.set_value_service(fd, msg)
            else:
                Log.log(0, 'received {} when expecting {}'.format(received_packet, MsgType.ack))
                # TODO: some action with wrong packet

    def set_value_service(self, fd, msg):
        dev_id = msg[1:3]
        val = msg[3:]
        Log.log(0, 'set {} to {}'.format(int.from_bytes(dev_id, self.__endian), int.from_bytes(val, self.__endian)))
        answer = (5).to_bytes(2, self.__endian) + MsgType.ack + dev_id
        fd.send(answer)

    def stop(self):
        self.__is_stopped.set()

