try:
    from .endpoint import Endpoint
except SystemError:
    from endpoint import Endpoint

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
        
    
if __name__ == '__main__':
    light = Light(2, 'name', 12)
    light.turnOn()
