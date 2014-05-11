#!/usr/bin/python

# This is a very simple Python script showing how to read
# temperature information from the dhtd socket.

import argparse
import socket
import time


def parse_args():
    p = argparse.ArgumentParser()
    p.add_argument('--socket-path',
                   default='/var/run/dhtd.socket')
    return p.parse_args()


def main():
    args = parse_args()
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect(args.socket_path)

    data = s.recv(80)
    when, temp, hum = data.split()
    when = int(when)
    temp = float(temp)
    hum = float(hum)

    print time.ctime(when), '%.2fC' % temp, '%.2f%%' % hum


if __name__ == '__main__':
    main()


