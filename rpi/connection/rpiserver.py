import time, threading
from connection.my_socket import MySocket
from select import select
from logger.logger import Log
from connection.msgtype import MsgType
import struct


class RpiServer:
    def __init__(self, port, server_ip, client, timeout=3):
        self.__port = port
        Log.log(1, 'pi server thread, port is {}'.format(port))
        self.__server_ip = server_ip
        self.__client = client
        self.__timeout = timeout
        self.__socket = MySocket()
        self.__socket.bind(port)
        Log.log(0, 'binded on port {}'.format(port))
        self.__socket.listen(2)
        self.__stopped = threading.Event()
        self.__server_thread = threading.Thread(target=self.main, name='pi_server_t')

    def main(self):
        Log.log(0, 'pi server is working')
        while not self.__stopped.isSet():
            Log.log(3, 'waiting for connection')
            self.__socket.set_timeout(1)
            try:
                connection, address = self.__socket.accept()
            except Exception as e:
                Log.log(0, 'exception when accepting connection {}'.format(e))
                continue

            Log.log(1, 'something connected')
            client_socket = MySocket(connection)
            msg = client_socket.get_msg()
            # Log.log(1, 'msg from server: {}'.format(msg))
            msg_type = MsgType.to_msg_type(struct.pack('B', msg[2]))
            if msg_type == MsgType.setState:
                dev_id = struct.unpack('!H', msg[3:5])[0]
                state = struct.unpack('!H', msg[5:7])[0]
                Log.log(2, 'received set state dev={}, state={}'.format(dev_id, state))
                device = self.__client.getDevice(dev_id)
                device.setState(state)
            else:
                Log.log(1, 'received bad msg type={} (expected setState)'.format(struct.pack('B', msg[0])))

    def set_value_service(self, fd, msg):
        pass

    def start(self):
        self.__server_thread.start()

    def stop(self):
        self.__stopped.set()
        self.__server_thread.join()
        Log.log(1, 'server pi thread joined')

