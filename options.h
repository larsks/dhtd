#ifndef _OPTIONS_H
#define _OPTIONS_H

#ifndef DEFAULT_SOCKET_PATH
#define DEFAULT_SOCKET_PATH "/var/run/dhtd.socket"
#endif

#ifndef DEFAULT_SOCKET_MODE
#define DEFAULT_SOCKET_MODE "0666"
#endif

#ifndef DEFAULT_SOCKET_USER
#define DEFAULT_SOCKET_USER "root"
#endif

#ifndef DEFAULT_SOCKET_GROUP
#define DEFAULT_SOCKET_GROUP "root"
#endif

#ifndef DEFAULT_READ_INTERVAL
#define DEFAULT_READ_INTERVAL 10
#endif

#ifndef DEFAULT_MODEL
#define DEFAULT_MODEL "DHT22"
#endif

#ifndef DEFAULT_RETRY_INTERVAL
#define DEFAULT_RETRY_INTERVAL 5
#endif

extern char	*socket_path;
extern char	*socket_user;
extern char	*socket_group;
extern char	*socket_mode;
extern char	*dht_model;
extern int	dht_pin;
extern int	read_interval;
extern int	retry_interval;
extern int	debug;
extern int 	verbose;
#endif

