#ifndef _DHT_H
#define _DHT_H

#define DHT11	11
#define DHT22	22
#define AM2302	22

extern float temperature;
extern float humidity;
extern time_t last_update;

int readDHT(int type, int pin);
#endif
