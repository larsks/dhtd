SRCS = main.c options.c dht.c
OBJS = $(SRCS:.c=.o)

CFLAGS += -g
LIBS = -lbcm2835

all: dhtd
	
dhtd: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)

