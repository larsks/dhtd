dhtd
====

This is a simple daemon that reads data from a DHT 11/22/2302
temperature sensor and exposes that information on a local Unix
socket.  Interacting directly with the Raspberry Pi's GPIO interface
using the [bcm2835][] library requires root access; with this daemon
running as root, the rest of your project can run as an unprivileged
user.

This code was originally based on Adafruit's [Sample DHT Code][ada].

Synopsis
========

    dhtd [ -s socket_path ] [ -u socket_user ] [ -g socket_group ]
         [ -m socket_mode ] [ -i read_interval ] [ -M dht_model ]
         [ -P dht_pin ] [ -vd ]

Examples
========

Running the daemon:

    # dhtd -P 18 -m 666
    Starting dhtd using pin 18, socket /var/run/dhtd.socket

The `-m 666` sets the socket mode to `rw-rw-rw-`, allowing all local
users to connect and read the temperature.  By default dhtd will read
the temperature every 10 seconds, but you can modify this with the
`-i` command line option.

Reading the temperature:

    $ socat - unix:/var/run/dhtd.socket
    1399718984 21.1000 43.7000

Reading the temperature in Python:

    #!/usr/bin/python

    import argparse
    import socket
    import time

    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect('/var/run/dhtd.socket')

    data = s.recv(80)
    when, temp, hum = data.split()
    when = int(when)
    temp = float(temp)
    hum = float(hum)

    print time.ctime(when), '%.2fC' % temp, '%.2f%%' % hum

Running this might output:

    $ python client.py
    Sat May 10 12:14:24 2014 22.20C 51.90%

[bcm2835]: http://www.airspayce.com/mikem/bcm2835/
[ada]: https://github.com/adafruit/Adafruit-Raspberry-Pi-Python-Code/tree/master/Adafruit_DHT_Driver

