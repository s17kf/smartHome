from threading import Event


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
    
    def generateMsg(self, endian = 'little'):
        msg = self.__key.to_bytes(2, endian) + len(self.__name).to_bytes(2, endian)
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

