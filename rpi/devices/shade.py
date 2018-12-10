try:
    from .endpoint import Endpoint
    from .device_types import DevTypeCode
    from .device_types import DevTypeId
except SystemError:
    from endpoint import Endpoint
    from device_types import DevTypeCode
    from device_types import DevTypeId

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
        msg = DevTypeId.shade.value.to_bytes(2, endian) + msg
        msg += self.__pinA.to_bytes(2, endian) + self.__pinB.to_bytes(2, endian)
        return (msg, length + 6)
        # length = super.length + 2(devid len) + 2(pin a len) + 2(pin b len) = length + 6


if __name__ == '__main__':
    shade = Shade(1, 'shade name', 1, 2)
    shade.moveDown()
    shade.moveUp()
    shade.stop()
