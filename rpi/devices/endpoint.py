from threading import Event
from socket import htons
import struct


class Endpoint:
    def __init__(self, key, name):
        self.__key = key
        self.__name = name
        self.__registered = Event()

    def getKey(self):
        return self.__key

    def getName(self):
        return self.__name

    def setName(self, new_name):
        self.__name = new_name
    
    def generateMsg(self, endian='little'):
        msg = struct.pack('!HH', self.__key, len(self.__name))
        msg += self.__name.encode()
        return msg, 4 + len(self.__name)

    def str(self):
        return 'key={}, name=\'{}\''.format(self.__key, self.__name)

    def setRegistered(self, val):
        if val:
            self.__registered.set()
        else:
            self.__registered.clear()

    def isRegistered(self):
        return self.__registered.isSet()

