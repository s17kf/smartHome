try:
    from .endpoint import Endpoint
    from .device_types import DevTypeCode
    from .device_types import DevTypeId
except SystemError:
    from endpoint import Endpoint
    from device_types import DevTypeCode
    from device_types import DevTypeId

class Light(Endpoint):
    def __init__(self, key, name, pin, zero_triggered = False):
        super().__init__(key, name)
        self.__pin = pin
        self.__zero_triggered = zero_triggered

    def turnOn(self):
        #TODO
        print('light', self.getKey(), self.getName(), 'isNowOn')

    def turnOff(self):
        #TODO
        print('turn off light is not implemented yet')

    def generateMsg(self, endian = 'little'):
        msg, length = super().generateMsg(endian)
        print('light super msg: {m}'.format(m=msg))
        msg = DevTypeId.light.value.to_bytes(2, endian) + msg
        msg += self.__pin.to_bytes(2, endian) + self.__zero_triggered.to_bytes(1, endian)
        return (msg, length + 5)
        # length = super.length + 2(dev id len) + 2(pin len) + 1(0 trig len) = length + 5


        
    
if __name__ == '__main__':
    light = Light(2, 'name', 12)
    light.turnOn()
