#!/usr/bin/env python

# Echo server program
import socket
from optparse import OptionParser

usage = "usage: EchoServer <datalength>"
parser = OptionParser(usage=usage)
parser.add_option("-l", "--length",
                  action="store",
                  dest="datalen",
                  type=int,
                  default=1,
                  help="Length of data to receive")
(options, args) = parser.parse_args()

HOST = ''                 # Symbolic name meaning the local host
PORT = 50007              # Arbitrary non-privileged port

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((HOST, PORT))
s.listen(1)
conn, addr = s.accept()

payload = ''

while 1:
    data = conn.recv(1024)
    if not data: break
    payload = payload + data
    if len(payload) == options.datalen: break

conn.sendall(payload)
conn.close()
