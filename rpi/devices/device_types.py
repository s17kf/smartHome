from enum import Enum


class DevTypeCode(Enum):
    light   =   'lig'
    shade   =   'shd'
    thermometer = 'thm'

class DevTypeId(Enum):
    light   =   1
    shade   =   2
    thermometer = 3


if __name__ == '__main__':
    devType = DevTypeCode('lig')
    print(devType)

    devType = 'new'
    try:
        devType = DevTypeCode(devType)
    except ValueError:
        print('not good value: {}'.format(devType))
    else:
        print(devType)

    print('can')

