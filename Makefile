prefix = /usr/local
sbindir = $(prefix)/sbin
sysconfdir = /etc

SRCS = main.c options.c dht.c
OBJS = $(SRCS:.c=.o)

CFLAGS += -g
LIBS = -lbcm2835

INSTALL = install

all: dhtd
	
dhtd: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

install: all
	$(INSTALL) -d -m 755 $(DESTDIR)$(sbindir)
	$(INSTALL) -d -m 755 $(DESTDIR)$(sysconfdir)
	$(INSTALL) -m 755 dhtd $(sbindir)/dhtd
	$(INSTALL) -m 755 init.d/dhtd $(sysconfdir)/init.d/dhtd

clean:
	rm -f $(OBJS)

