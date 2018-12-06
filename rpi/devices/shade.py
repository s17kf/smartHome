try:
    from .endpoint import Endpoint
except SystemError:
    from endpoint import Endpoint

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

if __name__ == '__main__':
    shade = Shade(1, 'shade name', 1, 2)
    shade.moveDown()
    shade.moveUp()
    shade.stop()
