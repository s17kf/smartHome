#!/usr/bin/python3
from devices import initialize
from connection.msgtype import MsgType
from logger.logger import Log
import threading
from connection.my_socket import MySocket
from devices.thermometer import Thermometer
import struct
from connection.msgtype import ValMsg


class Client:
    def __init__(self, devs_file_path, server_ip, port = 1234, timeout = 2):
        self.endian = 'little'      # #TODO: endians correctly
        # self.__connected = False
        self.__devices = []
        # self.__con = Connection(port, server_ip)
        self.__server_ip = server_ip
        self.__port = port
        self.__createDevicesList(devs_file_path)
        self.__id = None
        self.__id_event = threading.Event()
        self.__registered = threading.Event()
        self.__timeout = timeout
        self.__stopped = threading.Event()
        self.__main_thread = threading.Thread(target=self.main, name='client_t')

    def stop(self):
        self.__stopped.set()
        self.__main_thread.join()
        Log.log(0, 'client main thread joined')

    def getDevices(self):
        return self.__devices

    def getDevice(self, dev_key):
        for device in self.__devices:
            if device.getKey() == dev_key:
                return device
        raise IndexError('bad device key {}'.format(dev_key))

    def getId(self):
        return self.__id

    def __createDevicesList(self, devs_file_path):
        with open(devs_file_path, 'r') as devices_file:
            lines = devices_file.readlines()
            dev_key = 0
            current_line = 0
            for line in lines:
                current_line += 1
                try:
                    self.__devices.append(initialize.addDevice(line, dev_key))
                    dev_key += 1
                except KeyError as error:
                    Log.log(1, 'unresolved device code {} at line {}'.format(error, current_line))
                except ValueError as error:
                    Log.log(1, '{} at line {}: {}'.format(devs_file_path, current_line, error))

    def main(self):
        while not self.__registered.isSet() and not self.__stopped.isSet():
            self.register_on_server()
            if self.__registered.isSet():
                break
            self.__stopped.wait(self.__timeout)

        self.__stopped.wait(self.__timeout)
        while not self.__stopped.is_set():
            for device in self.__devices:
                if type(device) is Thermometer:
                    temp = device.getTemp()
                    Log.log(4, 'temp is {}'.format(temp))
                    self.send_float_val(device.getKey(), temp)

            Log.log(1, 'waiting for stop')
            self.__stopped.wait(self.__timeout)

    def send_float_val(self, dev_key, val):
        socket = MySocket()
        socket.connect(self.__server_ip, self.__port)
        Log.log(1, 'connected to server')
        msg = ValMsg.generate_float_val_msg(val, self.__id, dev_key)
        try:
            socket.mysend(msg)
            answer = self.get_answer(socket)
            Log.log(2, 'reveived msg {}, msg type is {}'.format(answer, answer[0]))
            msg_type = MsgType.to_msg_type(struct.pack('B', answer[0]))
            if msg_type == MsgType.ack:
                Log.log(4, 'successfully sent value')
            elif msg_type == MsgType.nack:
                Log.log(4, 'Failed to send val')
            else:
                Log.log(3, 'received msg_type {} when expecting ack/nack'.format(msg_type))
                raise RuntimeError('unresolved msg_type received')
        except RuntimeError as e:
            Log.log(0, 'Failed to send value {}: {} ... closing connection'.format(val, e))
            socket.close()

    def register_on_server(self):
        socket = MySocket()
        socket.connect(self.__server_ip, self.__port)
        Log.log(1, "connected to server")
        msg = (3).to_bytes(2, self.endian)
        msg += MsgType.reg.value
        try:
            socket.mysend(msg)
            answer = self.get_answer(socket)
            Log.log(2, "received msg {}, msg_type is {}".format(answer, answer[0]))
            # msg_type = MsgType.to_msg_type(answer[0].to_bytes(1, self.endian))
            msg_type = MsgType.to_msg_type(struct.pack('B', answer[0]))
            if msg_type == MsgType.ack:
                self.__id = struct.unpack('!H', answer[1:3])[0]
                Log.log(3, 'received id is {}'.format(self.__id))
            elif msg_type == MsgType.nack:
                raise RuntimeError('nack received, reg refused by server')
            else:
                # Log.log(3, 'received msg_type {} when expecting ack'.format(msg_type))
                raise RuntimeError('unresolved msg_type received {}, expecting ack/nack'.format(msg_type))
            for device in self.__devices:
                msg, length = device.generateMsg()
                msg = struct.pack('!H', length + 5) + MsgType.dev.value + struct.pack('!H', self.__id) + msg
                socket.mysend(msg)
                answer = self.get_answer(socket)
                # msg_type = MsgType.to_msg_type(answer[0].to_bytes(1, self.endian))
                msg_type = MsgType.to_msg_type(struct.pack('B', answer[0]))
                if msg_type == MsgType.ack:
                    dev_id = struct.unpack('!H', answer[1:3])[0]
                    Log.log(3, 'device {} successful sent'.format(dev_id))
                elif msg_type == MsgType.nack:
                    dev_id = struct.unpack('!H', answer[1:3])[0]
                    Log.log(3, 'failed to send device {}'.format(dev_id))
                else:
                    Log.log(3, 'received msg_type {} when expecting ack'.format(msg_type))
                    raise RuntimeError('unresolved msg_type received')
            msg = struct.pack('!H', 5) + MsgType.end.value + struct.pack('!H', self.__id)
            socket.mysend(msg)
            Log.log(2, 'successfully registered on server')
            socket.close()
            self.__registered.set()
        except RuntimeError as e:
            Log.log(0, 'registering failed: {} ... closing connection'.format(e))
            socket.close()

    def get_answer(self, socket):
        answer_len = socket.myreceive(2)
        # answer_len = int.from_bytes(answer_len, self.endian)
        answer_len = struct.unpack('!H', answer_len)[0]
        Log.log(5, 'answer len is {}'.format(answer_len))
        answer = socket.myreceive(answer_len - 2)
        return answer

    def start_work(self):
        self.__main_thread.start()
        # self.__answer_service_thread.start()


