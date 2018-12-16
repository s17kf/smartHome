#!/usr/bin/python3
from devices import initialize
from connection.packet import Packet
from logger.logger import Log
import threading
from connection.my_socket import MySocket


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
        self.__main_thread = threading.Thread(target=self.main)

    def stop(self):
        self.__stopped.set()
        self.__main_thread.join()
        Log.log(0, 'client main thread joined')

    def getDevices(self):
        return self.__devices

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

        while not self.__stopped.is_set():
            Log.log(1, 'waiting for stop')
            self.__stopped.wait(self.__timeout)

    def register_on_server(self):
        socket = MySocket()
        socket.connect(self.__server_ip, self.__port)
        Log.log(1, "connected to server")
        msg = (3).to_bytes(2, self.endian)
        msg += Packet.reg.value
        try:
            socket.mysend(msg)
            answer = self.get_answer(socket)
            Log.log(2, "received msg {}, packet is {}".format(answer, answer[0]))
            packet = Packet.to_packet(answer[0].to_bytes(1, self.endian))
            if packet == Packet.ack:
                self.__id = int.from_bytes(answer[1:3], self.endian)
                Log.log(3, 'received id is {}'.format(self.__id))
            elif packet == Packet.nack:
                raise RuntimeError('nack received, reg refused by server')
            else:
                # Log.log(3, 'received packet {} when expecting ack'.format(packet))
                raise RuntimeError('unresolved packet received {}, expecting ack/nack'.format(packet))
            for device in self.__devices:
                msg, length = device.generateMsg()
                msg = (length + 5).to_bytes(2, self.endian) + Packet.dev.value + \
                      self.__id.to_bytes(2, self.endian) + msg
                socket.mysend(msg)
                answer = self.get_answer(socket)
                packet = Packet.to_packet(answer[0].to_bytes(1, self.endian))
                if packet == Packet.ack:
                    dev_id = int.from_bytes(answer[1:3], self.endian)
                    Log.log(3, 'device {} successful sent'.format(dev_id))
                elif packet == Packet.nack:
                    dev_id = int.from_bytes(answer[1:3], self.endian)
                    Log.log(3, 'failed to send device {}'.format(dev_id))
                else:
                    Log.log(3, 'receive   d packet {} when expecting ack'.format(packet))
                    raise RuntimeError('unresloved packet received')
            msg = (5).to_bytes(2, self.endian) + Packet.end.value + self.__id.to_bytes(2, self.endian)
            socket.mysend(msg)
            Log.log(2, 'successfully registered on server')
            socket.close()
            self.__registered.set()
        except RuntimeError as e:
            Log.log(0, 'registering failed: {} ... closing connection'.format(e))
            socket.close()

    def get_answer(self, socket):
        answer_len = socket.myreceive(2)
        answer_len = int.from_bytes(answer_len, self.endian)
        answer = socket.myreceive(answer_len - 2)
        return answer

    def start_work(self):
        self.__main_thread.start()
        # self.__answer_service_thread.start()


