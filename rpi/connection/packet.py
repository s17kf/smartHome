from enum import Enum
from logger.logger import Log


class Packet(Enum):
    nack    =   b'\x00'
    ack     =   b'\x01'
    reg     =   b'\x02'
    dev     =   b'\x03'
    set     =   b'\x04'
    val     =   b'\x06'
    all     =   b'\xf0'
    end     =   b'\x0f'
    exit    =   b'\xff'

    @staticmethod
    def to_packet(value):
        try:
            packet = Packet(value)
        except ValueError:
            Log.log(0, 'unresolved packet received: {}'.format(value))
            return None
            # TODO: some action with unresolved packet
        else:
            return packet

