#!/usr/bin/env python

# Echo client program
import socket, datetime, sys, random, string
from optparse import OptionParser

usage = "usage: EchoClient <datalength>"
parser = OptionParser(usage=usage)
parser.add_option("-l", "--length",
                  action="store",
                  dest="datalen",
                  type=int,
                  default=1,
                  help="Length of data to send")
parser.add_option("-t", "--trials",
                  action="store",
                  dest="numtrials",
                  type=int,
                  default=5,
                  help="Number of requests to send to the server")
(options, args) = parser.parse_args()

payload = ''.join(random.choice(string.ascii_uppercase) \
                for x in range(int(options.datalen)))
receivepayload = ''

HOST = '192.168.4.74'    # The remote host
PORT = 50007              # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

s.connect((HOST, PORT))
start = datetime.datetime.now()
for trials in range(options.numtrials):
    receivepayload = ''
    s.sendall(payload)
    while 1:
        data = s.recv(1024)
        if not data: break
        receivepayload = receivepayload + data
        if len(receivepayload) == options.datalen: break
end = datetime.datetime.now()
s.close()

if len(payload) != len(receivepayload):
    print "length mismatch"
    print "I sent %d bytes but got back %d bytes" % (len(payload), len(receivepayload))

if payload != receivepayload:
    print "payload content mismatch"

#print 'start', start.microsecond
#rint 'end', end.microsecond
print str(end.microsecond - start.microsecond)
