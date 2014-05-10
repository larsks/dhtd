#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <poll.h>
#include <getopt.h>
#include <arpa/inet.h>

#include "options.h"
#include "dht.h"

#ifdef DEBUG
time_t last_update = 0;
float temperature  = 0.0;
float humidity     = 0.0;
#endif

void start_server();

int main(int argc, char **argv) {
	int model = 0;

	process_args(argc, argv);

	if (strcmp(dht_model, "DHT11") == 0) model = DHT11;
	if (strcmp(dht_model, "DHT22") == 0) model = DHT22;
	if (strcmp(dht_model, "AM2302") == 0) model = DHT22;

	printf("Starting dhtd using pin %d, socket %s\n",
			dht_pin, socket_path);

	start_server(model, dht_pin);
}

int configure_socket() {
	int sock;
	struct sockaddr_un sockaddr;

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(1);
	}

	unlink(socket_path);
	memset(&sockaddr, 0, sizeof(struct sockaddr_un));
	sockaddr.sun_family = AF_UNIX;
	snprintf(sockaddr.sun_path, PATH_MAX, socket_path);

	if (bind(sock, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_un)) != 0) {
		perror("bind");
		exit(1);
	}

	if (chown(socket_path, name_to_uid(socket_user), name_to_gid(socket_group)) != 0) {
		perror("chown");
		exit(1);
	}

	if (chmod(socket_path, strtol(socket_mode, NULL, 8)) != 0) {
		perror("chmod");
		exit(1);
	}

	if (listen(sock, 5) != 0) {
		perror("listen");
		exit(1);
	}

	return sock;
}

struct _ctx {
	struct pollfd *fdlist;
	int nfds;
};

void append_socket(struct _ctx *ctx, int sock, int events) {
	ctx->nfds++;
	ctx->fdlist = realloc(ctx->fdlist, sizeof(struct pollfd) * ctx->nfds);
	ctx->fdlist[ctx->nfds-1].fd = sock;
	ctx->fdlist[ctx->nfds-1].events = events;
	ctx->fdlist[ctx->nfds-1].revents = 0;
}

void remove_socket(struct _ctx *ctx, int index) {
	int i, j;

	for (i=0, j=0; i < ctx->nfds; i++) {
		if (i != index) {
			ctx->fdlist[j++] = ctx->fdlist[i];
		}
	}

	ctx->nfds--;
	ctx->fdlist = realloc(ctx->fdlist, sizeof(struct pollfd) * ctx->nfds);
}

void dump_sockets(struct _ctx *ctx) {
	int i;

	for (i=0; i<ctx->nfds; i++) {
		printf("%d: %d %d %d\n", i,
				ctx->fdlist[i].fd,
				ctx->fdlist[i].events,
				ctx->fdlist[i].revents);
	}
}

void send_response(int sock) {
	char buffer[80];

	snprintf(buffer, 80, "%d %.4f %.4f\n",
			last_update,
			temperature,
			humidity);

	write(sock, buffer, strlen(buffer));
}

void start_server(int model, int pin) {
	int sock;
	struct _ctx ctx = {NULL, 0};

	if (!bcm2835_init()) {
		fprintf(stderr, "failed to initialize bcm2835\n");
		exit(1);
	}

	sock = configure_socket();
	append_socket(&ctx, sock, POLLIN);

	signal(SIGPIPE, SIG_IGN);

	while (1) {
		int res;
		int i;
		time_t now;

		res = poll(ctx.fdlist, ctx.nfds, 500);

		now = time(NULL);
		if (now - last_update >= read_interval) {
			if (readDHT(model, pin)) {
				if (verbose)
					fprintf(stderr, "successful sensor read at %d: %d %fC %f%%\n",
							now, last_update, temperature, humidity);
			} else {
				if (verbose)
					fprintf(stderr, "failed sensor read at %d\n",
							now);
			}
		}

		if (res == 0)
			continue;

		for (i=0; i<ctx.nfds; i++) {
			struct pollfd this = ctx.fdlist[i];

			if (i == 0 && this.revents & POLLIN) {
				int client;

				client = accept(sock, NULL, NULL);
				if (debug)
					fprintf(stderr, "accepted new client connection (fd=%d)\n", client);
				append_socket(&ctx, client, POLLOUT);
			} else if (this.revents & POLLOUT) {
				send_response(this.fd);
				if (debug)
					fprintf(stderr, "closing client connection (fd=%d)\n", this.fd);
				close(this.fd);
				remove_socket(&ctx, i);
			}
		}
	}
}

