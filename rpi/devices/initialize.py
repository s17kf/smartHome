try:
    from .light import Light
    from .shade import Shade
    from .device_types import DevTypeCode
except SystemError:
    from light import Light
    from shade import Shade
    from device_codes import DevTypeCode

def getNextArg(line, start = 0):
    i = start
    arg = ''
    if line[i] != '\'':
        while line[i] != ' ' and line[i] != '\n' and line[i] != '\0':
            arg += line[i]
            i += 1
    else:
        i += 1
        while line[i] != '\'':
            arg += line[i]
            i += 1
    return (arg, i) 


def getDevCode(line):
    return line[0:3]


def addLight(arg_line, key):
    pin, i = getNextArg(arg_line)
    flag, i = getNextArg(arg_line, i+1)
    if flag == '-0':
        zero_triggered_arg, i = getNextArg(arg_line, i+1)
        if zero_triggered_arg in ['0', 'n', 'N']:
            zero_triggered = False
        elif zero_triggered_arg in ['1', 't', 'T', 'y', 'Y']:
            zero_triggered = True
        else:
            raise ValueError ('bad 0 triggered value')
        name, i = getNextArg(arg_line, i+1)
    else:
        zero_triggered = False
        name = flag
    return Light(key, name, int(pin), zero_triggered)
#TODO some values are int


def addShade(arg_line, key):
    pinA, i = getNextArg(arg_line)
    pinB, i = getNextArg(arg_line, i+1)
    name, i = getNextArg(arg_line, i+1)
    return Shade(key, name, int(pinA), int(pinB))
#TODO: some values are ints


def addDevice(line, key):
    devCode = line[0:3]
    try:
        devCode = DevTypeCode(devCode)
    except ValueError:
        raise KeyError(devCode)
    if devCode == DevTypeCode.light:
        return addLight(line[4:], key)
    elif devCode == DevTypeCode.shade:
        return addShade(line[4:], key)


if __name__ == '__main__':
    rpi_key = 12
    #TODO: rpi_key from file or other better way
    devices_path = '../devices.conf'
    #TODO: path from file or something
    devices = []
    with open(devices_path, 'r') as devices_file:
        lines = devices_file.readlines()
        i = 0
        for line in lines:
            try:
                devices.append(addDevice(line, i))
                i += 1
            except KeyError as error:
                print('key error:', error)
            except ValueError as error:
                print('val error:', error)

    print("devices:")
    for dev in devices:
        print(dev.__dict__)



    
