#!/usr/bin/python3
from devices import initialize
from connection.connection import Connection

REG = b'\x02'
ACK = b'\x01'
port = 1234
server_ip = '192.168.1.19'
rpi_key = 12
#TODO: rpi_key port and ip from file or other better way
devices_path = 'devices.conf'
#TODO: path from file or something
devices = []
with open(devices_path, 'r') as devices_file:
    lines = devices_file.readlines()
    i = 0
    for line in lines:
        try:
            devices.append(initialize.addDevice(line, i))
            i += 1
        except KeyError as error:
            print('key error:', error)
        except ValueError as error:
            print('val error:', error)

print("devices:")
for dev in devices:
    print(dev.__dict__)

con = Connection(port, server_ip)
con.connect()
#con.send(rpi_key.to_bytes(2, 'little'))
con.sendFormattedMsg(REG)       #TODO: packets
answer_length = int.from_bytes(con.recv(2), 'little')
#TODO endians in some properly way
#answer = con.recv(2)
#answer_length = int.from_bytes(answer, 'little')
print('get answer length {l1} bytes'.format(l1 = answer_length))
answer = con.recv(answer_length - 2)
#TODO: catch rec errors e.g. negative length
#print(answer)
if answer[0] == ACK[0]:
    print('received id is {id}'.format(id = int.from_bytes(answer[1:2], 'little')))
else:
    print('wrong packet received {packet}'.format(packet = answer[0]))
con.disconnect()
