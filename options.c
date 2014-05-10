#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include "options.h"

#define OPT_READ_INTERVAL	'i'
#define OPT_SOCKET_PATH		's'
#define OPT_SOCKET_GROUP	'g'
#define OPT_SOCKET_MODE		'm'
#define OPT_SOCKET_USER		'u'
#define OPT_MODEL		'M'
#define OPT_PIN			'P'
#define OPT_DEBUG		'd'
#define OPT_VERBOSE		'v'
#define OPT_HELP		'h'

#define OPTSTRING		"i:s:g:m:u:dvM:P:h"

char	*socket_path  = DEFAULT_SOCKET_PATH;
char	*socket_user  = DEFAULT_SOCKET_USER;
char	*socket_group = DEFAULT_SOCKET_GROUP;
char	*socket_mode  = DEFAULT_SOCKET_MODE;
char	*dht_model    = DEFAULT_MODEL;
int	dht_pin       = -1;
int	read_interval = DEFAULT_READ_INTERVAL;
int	debug         = 0;
int	verbose       = 0;

uid_t name_to_uid(const char *name) {
	struct passwd *pw;
	pw = getpwnam(name);
	return pw->pw_uid;
}

uid_t name_to_gid(const char *name) {
	struct group *gr;
	gr = getgrnam(name);
	return gr->gr_gid;
}

int check_model(const char *model) {
	if (strcmp(model, "DHT11") == 0)
		return 1;
	if (strcmp(model, "DHT22") == 0)
		return 1;
	if (strcmp(model, "AM2302") == 0)
		return 1;

	return 0;
}

void usage(FILE *out) {
	fprintf(out, "dhtd: usage: dhtd [ -s socket_path ] [ -u socket_user ] [ -g socket_group ]\n");
	fprintf(out, "                  [ -m socket_mode ] [ -i read_interval ] [ -M dht_model ]\n");
	fprintf(out, "                  [ -P dht_pin ] [ -vd ]\n");
}

void process_args(int argc, char **argv) {
	int c;

	while ((c = getopt(argc, argv, OPTSTRING)) != EOF) {
		switch (c) {
			case OPT_READ_INTERVAL:
				read_interval = atoi(optarg);
				break;
			case OPT_SOCKET_PATH:
				socket_path = strdup(optarg);
				break;
			case OPT_SOCKET_GROUP:
				socket_group = strdup(optarg);
				break;
			case OPT_SOCKET_USER:
				socket_user = strdup(optarg);
				break;
			case OPT_SOCKET_MODE:
				socket_mode = strdup(optarg);
				break;
			case OPT_MODEL:
				if (! check_model(optarg)) {
					fprintf(stderr, "unknown model name: %s\n", optarg);
					exit(1);
				}

				dht_model = strdup(optarg);
				break;
			case OPT_PIN:
				dht_pin = atoi(optarg);
				break;
			case OPT_VERBOSE:
				verbose=1;
				break;
			case OPT_DEBUG:
				debug=1;
				break;
			case OPT_HELP:
				usage(stdout);
				exit(0);

			case '?':
				usage(stderr);
				exit(2);
		}
	}

	if (debug) verbose=1;

	if (dht_pin < 0) {
		fprintf(stderr, "please provide a valid GPIO pin\n");
		exit(1);
	}
}

