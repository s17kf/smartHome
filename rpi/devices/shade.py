# try:
from .endpoint import Endpoint
from .device_types import DevTypeCode
from .device_types import DevTypeId
from socket import htons
import struct
# except SystemError:
#     from endpoint import Endpoint
#     from device_types import DevTypeCode
#     from device_types import DevTypeId

class Shade(Endpoint):
    def __init__(self, key, name, pinA, pinB):
        super().__init__(key, name)
        self.__pinA = pinA
        self.__pinB = pinB
        
    def moveDown(self):
        #TODO
        print('moving Down')

    def moveUp(self):
        #TODO
        print('moving Up')

    def stop(self):
        #TODO
        print('stopped')

    def generateMsg(self, endian = 'little'):
        msg, length = super().generateMsg(endian)
        msg = struct.pack('!H', DevTypeId.shade.value) + msg + struct.pack('!HH', self.__pinA, self.__pinB)
        return msg, length + 6
        # length = super.length + 2(devid len) + 2(pin a len) + 2(pin b len) = length + 6

    def str(self):
        return 'Shade=[' + super().str() + ' pinA={}, pinB={}]'.format(self.__pinA, self.__pinB)


if __name__ == '__main__':
    shade = Shade(1, 'shade name', 1, 2)
    shade.moveDown()
    shade.moveUp()
    shade.stop()
