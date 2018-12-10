from enum import Enum


class DevTypeCode(Enum):
    light   =   'lig'
    shade   =   'shd'

class DevTypeId(Enum):
    light   =   1
    shade   =   2


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

