from enum import Enum
from logger.logger import Log
import struct


class MsgType(Enum):
    nack    =   b'\x00'
    ack     =   b'\x01'
    reg     =   b'\x02'
    dev     =   b'\x03'
    # set     =   b'\x04'
    val     =   b'\x06'
    setState = b'\x07'
    all     =   b'\xf0'
    end     =   b'\x0f'
    exit    =   b'\xff'

    @staticmethod
    def to_msg_type(value):
        try:
            packet = MsgType(value)
        except ValueError:
            Log.log(0, 'unresolved packet received: {}'.format(value))
            return None
            # TODO: some action with unresolved packet
        else:
            return packet


class ValMsg:
    class ValType(Enum):
        int = b'\x01'
        float = b'\x02'

    @staticmethod
    def generate_float_val_msg(value, rpi_id, dev_id):
        # return struct.pack('!BHHBd', MsgType.val.value, rpi_id, dev_id, ValMsg.ValType.float.value, value)
        msg = MsgType.val.value
        msg += struct.pack('!HH', rpi_id, dev_id)
        msg += ValMsg.ValType.float.value
        msg += struct.pack('!d', value)
        msg = struct.pack('!H', len(msg) + 2) + msg
        return msg
