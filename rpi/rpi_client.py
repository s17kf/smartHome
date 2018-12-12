#!/usr/bin/python3
from devices import initialize
from connection.connection import Connection
from connection.packet import Packet
import time
from logger.logger import Log


class Client:
    def __init__(self, devs_file_path, server_ip, port = 1234):
        self.endian = 'little'      # #TODO: endians correctly
        self.__connected = False
        self.__devices = []
        self.__con = Connection(port, server_ip)
#        self.__server_ip = server_ip
#        self.__port = port
        self.__createDevicesList(devs_file_path)
        self.__id = None

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

    def registerOnServer(self):
        self.__con.connect()
        Log.log(2, 'connected to server');
        #TODO: cath exception if cannot connect
        self.__con.sendFormattedMsg(Packet.reg.value)
        time.sleep(4)
        answer_length = int.from_bytes(self.__con.recv(2), self.endian)
        Log.log(5, 'get answer length {} bytes'.format(answer_length))
        answer = self.__con.recv(answer_length - 2)
        received_packet = answer[0]
        try:
            received_packet = Packet(received_packet.to_bytes(1, self.endian))
        except ValueError:
            Log.log(0, 'unresolved packet received: {}'.format(received_packet))
            return None
            # TODO: some action with unresolved packet
        else:
            if received_packet == Packet.ack:
                Log.log(1, 'received id is {}'.format(int.from_bytes(answer[1:3], self.endian)))
            elif received_packet == Packet.nack:
                raise IndexError('nack received - max rpis registered')
            else:
                Log.log(0, 'received {} when expecting {}'.format(received_packet, Packet.ack))
                # TODO: some action with wrong packet
        self.__id = int.from_bytes(answer[1:3], self.endian)
        for device in self.__devices:
            msg, length = device.generateMsg()
            msg = Packet.dev.value + self.__id.to_bytes(2, self.endian) + msg
            sent = self.__con.sendFormattedMsg(msg)
            Log.log(3, 'sent {} bytes: {}'.format(sent, device.str()))
            answer_length = int.from_bytes(self.__con.recv(2), self.endian)
            answer = self.__con.recv(answer_length - 2)
            received_packet = answer[0]
            try:
                received_packet = Packet(received_packet.to_bytes(1, self.endian))
            except ValueError:
                Log.log(0, 'unresolved packet received: {}'.format(received_packet))
                # TODO: some action for that
                # TODO: test this case
            else:
                if received_packet == Packet.ack:
                    Log.log(2, 'successful sent device {} to server'.format(device.getKey()))
                elif received_packet == Packet.nack:
                    Log.log(1, 'Failed to send device {} to server'.format(device.getKey()))
                else:
                    Log.log(0, 'unresolved answer received: {}'.format(received_packet))
                    # TODO: do some action if unresolved
                    # TODO: test all cases
            #TODO: repeat if nack or delete endpoint device
        self.__con.sendFormattedMsg(Packet.end.value + self.__id.to_bytes(2, self.endian))
        self.__con.disconnect()
        

if __name__ == '__main__':
    #REG = b'\x02'
    #ACK = b'\x01'
    port = 1234
    server_ip = '192.168.1.19'
    rpi_key = 12
    #TODO: rpi_key port and ip from file or other better way
    devices_path = 'devices.conf'
    log_filepath = 'logs/file.log'
    Log.init(log_filepath, 5)
    Log.log(0, 'server parameters: port = {}, ip = {}'.format(port, server_ip))
    #TODO: path from file or something

    client = Client(devices_path, server_ip, port)

    Log.log(0, 'configured {} devices'.format(len(client.getDevices())))
    for dev in client.getDevices():
        Log.log(1, dev.str())

    try:
        client.registerOnServer()
    except IndexError as error:
        Log.log(0, "Failed to register on server: {}".format(error))
        # TODO: do something more
    # print('clienr id is {id}'.format(id = client.getId()))
    Log.log(0, 'my id on server is {}'.format(client.getId()))
    Log.close()

