from .endpoint import Endpoint
from .device_types import DevTypeId
import random
from socket import htons
import struct


class Thermometer(Endpoint):
    def __init__(self, key, name, pin):
        Endpoint.__init__(self, key, name)
        self.__pin = pin

    def getTemp(self):
        # TODO: real temp returning
        temp = random.random() - 0.2
        temp *= 40
        return temp

    def generateMsg(self, endian='little'):
        msg, length = super().generateMsg(endian)
        # print('light super msg: {m}'.format(m=msg))
        msg = struct.pack('!H', DevTypeId.thermometer.value) + msg + struct.pack('!H', self.__pin)
        return msg, length + 4

    def str(self):
        return 'Thermometer=[{} pin={}]'.format(super().str(), self.__pin)

