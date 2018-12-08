#!/usr/bin/python3
from devices import initialize
from connection.connection import Connection
from connection.packet import Packet
import time

class Client:
    def __init__(self, devs_file_path, server_ip, port = 1234):
        self.endian = 'little'      #TODO: endians correctly
        self.__connected = False
        self.__devices = []
        self.__con = Connection(port, server_ip)
#        self.__server_ip = server_ip
#        self.__port = port
        self.__createDevicesList(devs_file_path)
        self.__id = None

#    def connectToServer(self):
#        self.__connection.connect()

    def getDevices(self):
        return self.__devices
    def getId(self):
        return self.__id

#    def setPort(self, port):
#        self.__port = port
#    def setServerIp(self, server_ip):
#        self.__server_ip = server_ip
# TODO: set port and ip check Connection class too

    def __createDevicesList(self, devs_file_path):
        with open(devs_file_path, 'r') as devices_file:
            lines = devices_file.readlines()
            dev_key = 0
            for line in lines:
                try:
                    self.__devices.append(initialize.addDevice(line, dev_key))
                    dev_key += 1
                except KeyError as error:
                    print('key error:', error)
                except ValueError as error:
                    print('value error:', error)

    def registerOnServer(self):
        self.__con.connect()
        #TODO: cath exception if cannot connect
        self.__con.sendFormattedMsg(Packet.reg.value)
        answer_length = int.from_bytes(self.__con.recv(2), self.endian)
        print('get ansver length {l} bytes'.format(l = answer_length))
        answer = self.__con.recv(answer_length - 2)
        received_packet = Packet(answer[0].to_bytes(1, self.endian))
        if received_packet == Packet.ack:
            print('received id is {id}'.format(id = int.from_bytes(answer[1:3], self.endian)))
        else:
            print('wrong packet received {packet}'.format(packet = answer[0]))
        self.__id = int.from_bytes(answer[1:3], self.endian)
        for device in self.__devices:
            msg, length = device.generateMsg()
            msg = self.__id.to_bytes(2, self.endian) + Packet.dev.value + msg
            print(msg)
            print('msg len {l}'.format(l = length))
            self.__con.sendFormattedMsg(msg)
            answer_length = int.from_bytes(self.__con.recv(2), self.endian)
            answer = self.__con.recv(answer_length - 2)
            received_packet = Packet(answer[0].to_bytes(1, self.endian))
            if received_packet == Packet.ack:
                print('succesful sent device {key}'.format(key = device.getKey()))
            elif received_packet == Packet.nack:
                print('device {key} not sent!'.format(key = device.getKey()))
            else:
                print('unresolved answer received')
            #TODO: repeat if nack or delete endpoint device
        self.__con.sendFormattedMsg(self.__id.to_bytes(2, self.endian) + Packet.end.value)
        self.__con.disconnect()
        


if __name__ == '__main__': 
    #REG = b'\x02'
    #ACK = b'\x01'
    port = 1234
    server_ip = '192.168.1.19'
    rpi_key = 12
    #TODO: rpi_key port and ip from file or other better way
    devices_path = 'devices.conf'
    #TODO: path from file or something

    client = Client(devices_path, server_ip, port)

    print("devices:")
    for dev in client.getDevices():
        print(dev.__dict__)

    client.registerOnServer()
    print('clienr id is {id}'.format(id = client.getId()))

