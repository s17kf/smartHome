from enum import Enum

class Packet(Enum):
    nack    =   b'\x00'
    ack     =   b'\x01'
    reg     =   b'\x02'
    dev     =   b'\x03'
    all     =   b'\xf0'
    end     =   b'\x0f'
    exit    =   b'\xff'
