#!/usr/bin/env python

# Echo client program
import socket, datetime, sys, random, string
from optparse import OptionParser

usage = "usage: EchoClient <datalength>"
parser = OptionParser(usage=usage)
parser.add_option("-l", "--length",
                  action="store",
                  dest="datalen",
                  default=1,
                  help="Length of data to send")
(options, args) = parser.parse_args()

payload = ''.join(random.choice(string.ascii_uppercase) \
                for x in range(int(options.datalen)))
receivepayload = ''

HOST = '192.168.4.74'    # The remote host
PORT = 50007              # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

start = datetime.datetime.now()
s.connect((HOST, PORT))
s.sendall(payload)
while 1:
    data = s.recv(1024)
    if not data: break
    receivepayload = receivepayload + data
s.close()
end = datetime.datetime.now()

if len(payload) != len(receivepayload):
    print "length mismatch"
    print "I sent %d bytes but got back %d bytes" % (len(payload), len(receivepayload))

if payload != receivepayload:
    print "payload content mismatch"

print 'start', start.microsecond
print 'end', end.microsecond
print 'duration', end.microsecond - start.microsecond
