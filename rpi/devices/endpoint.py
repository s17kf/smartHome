class Endpoint:    
    def __init__(self, key, name):
        self.__key = key
        self.__name = name

    def getKey(self):
        return self.__key

    def getName(self):
        return self.__name

    def setName(self, new_name):
        self.__name = new_name
    
    

