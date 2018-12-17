#!/usr/bin/python3
from rpi_client import Client
from logger.logger import Log
import signal
import sys
import time


def sigint_handler(signum, frame):
    Log.log(0, 'ending by SIGINT')
    client.stop()
    Log.log(0, 'ending')
    Log.close()


signal.signal(signal.SIGINT, sigint_handler)


port = 1234
server_ip = '192.168.1.19'
rpi_key = 12
# TODO: rpi_key port and ip from file or other better way
devices_path = 'devices.conf'
log_filepath = 'logs/file.log'
Log.init(log_filepath, 5)
Log.log(0, 'server parameters: port = {}, ip = {}'.format(port, server_ip))
# TODO: path from file or something

client = Client(devices_path, server_ip, port)

Log.log(0, 'configured {} devices'.format(len(client.getDevices())))
for dev in client.getDevices():
    # Log.log(1, dev.__dict__)
    Log.log(1, dev.str())

client.start_work()
