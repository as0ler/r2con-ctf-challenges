#!/usr/bin/env python3

import os
import sys
import time
import argparse
import paramiko

KEY_UP = b'\x1b[A'
KEY_DOWN = b'\x1b[B'
KEY_RIGHT = b'\x1b[C'
KEY_LEFT = b'\x1b[D'

solution = """
dsdwdsssdsdwdssassdd
ddsdwdsdwdsdssassasdds
ssssssdsdddddd
dsasddddwwdsdwdssssawwassawassawaasssdddwdddsd
ddssaasssssddddddd
dddsdwdsssssdwdssasd
ssdsdsaasssdwwddssdddd
sssssssddddwddwdss
ddddsdsdwwdsssssasds
dddssdsdwwawddsdsssasssd
dsasssddssddsddwwdss
dsdwdddddssssssasd
sssddwdsddssssdwwwwdssss
dddddsaaasassdwddsdwdssdss
dddssddwwddsasdsssss
ssssdsasdsdddddd
ssssssdddsdddwawwdsdss
dsasdssasdddsddwwawawdwddwdsssssasds
ddsasdssdwwdwawddsddsssasdsasd
dddsdwdddsasdssassds
ssddssssasdddddd
sssssddwdsdwdssdwdss
sdwdsdsasawassssdddwdsdwwaawdwdwdssdssss
ssssdsassdwdsddwdsdwds
ddsssawasssdwdsdssddwdsd
ddsddsdsaawaasssasdddddwdssd
dsasssdsssddwwdsdsdd
dsasdddsasdssasddwdsdd
sdwdsdwdssddwawddsssssawasssdwds
dsddwddsddsasssdsaaaasdddd
sssssdsddwwdwwdsssasdsdd
sdwddsasasssssdwwddssdwdsd
sssssssdddwwwdsdsdwwawwwawdddssasdssss
sssddwdssdsassdddd
ssssssdwwdsdsasddwwdwawdwddsassdsaasdd
sssddwwawddddsaasdsdsdwwwwdsssssss
sddwddsasssdsssddd
ssdssassddsasddwdwawdwdwwdwdssasdsasassdwds
sssdwwdssdwdwdddssssss
ddsaasdddwwddddssssssasd
sdsdssaassddsddddwds
sssssdddssdddd
"""

solution = solution.replace(" ", "")
solution = solution.replace("\r", "")
solution = solution.replace("\n", "")

keycodes = []
for char in solution:
    if char == "w":
        code = KEY_UP
    elif char == "a":
        code = KEY_LEFT
    elif char == "s":
        code = KEY_DOWN
    elif char == "d":
        code = KEY_RIGHT
    else:
        raise Exception("oops")
    keycodes.append(code)
keycodes = b''.join(keycodes)

parser = argparse.ArgumentParser()
parser.add_argument("-H", "--hostname", help="host name")
parser.add_argument("-P", "--port", type=int, default=22, help="port number")
parser.add_argument("-u", "--username", default="root", help="user name")
parser.add_argument("-p", "--password", help="password")
parser.add_argument("-k", "--keyfile", dest="key_filename", metavar="KEYFILE", help="private key file")
args = parser.parse_args()

if not args.password and not args.key_filename:
    parser.error("must specify either -p or -k")

client = paramiko.SSHClient()
client.load_system_host_keys()

params = {
    "timeout": 2.0,
    "allow_agent": False,
    "look_for_keys": False
}
params.update(vars(args))

client.connect(**params)

channel = client.invoke_shell(term="xterm-256color")

channel.sendall("./game\n")
channel.sendall(keycodes)
channel.settimeout(10.0)

fd = os.open(os.ttyname(sys.stdout.fileno()), os.O_RDWR)
while 1:
    try:
        buffer = channel.recv(1024)
    except Exception:
        break
    os.write(fd, buffer)
