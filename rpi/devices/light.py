# try:
from .endpoint import Endpoint
from .device_types import DevTypeCode
from .device_types import DevTypeId
from socket import htons
import struct
from logger.logger import Log
GpioEnabled = True
try:
    import RPi.GPIO as gpio
except ModuleNotFoundError as e:
    Log.log(0, 'Failed to import RPi.GPIO: {}'.format(e))
    GpioEnabled = False

# except SystemError:
#     from endpoint import Endpoint
#     from device_types import DevTypeCode
#     from device_types import DevTypeId

class Light(Endpoint):
    def __init__(self, key, name, pin, zero_triggered = False):
        super().__init__(key, name)
        self.__pin = pin
        self.__zero_triggered = zero_triggered
        if GpioEnabled:
            gpio.setmode(gpio.BCM)
            gpio.setup(pin, gpio.OUT)

    def turnOn(self):
        #TODO
        print('light', self.getKey(), self.getName(), 'isNowOn')
        if GpioEnabled:
            gpio.output(self.__pin, not self.__zero_triggered)

    def turnOff(self):
        #TODO
        print('turn off light is not implemented yet')
        if GpioEnabled:
            gpio.output(self.__pin, self.__zero_triggered)

    def generateMsg(self, endian='little'):
        msg, length = super().generateMsg(endian)
        # print('light super msg: {m}'.format(m=msg))
        msg = struct.pack('!H', DevTypeId.light.value) + msg + struct.pack('!HB', self.__pin, self.__zero_triggered)
        return msg, length + 5
        # length = super.length + 2(dev id len) + 2(pin len) + 1(0 trig len) = length + 5

    def str(self):
        return 'Light=[' + super().str() + ' pin={}, 0 triggered={}]'.format(self.__pin, self.__zero_triggered)

    def setState(self, state):
        if state == 0:
            self.turnOff()
        elif state == 1:
            self.turnOn()
        else:
            raise KeyError('bad state value')


if __name__ == '__main__':
    light = Light(2, 'name', 12)
    light.turnOn()
