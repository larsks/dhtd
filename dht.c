#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef DEBUG
#include <bcm2835.h>
#endif

#ifndef BCM2708_PERI_BASE
#define BCM2708_PERI_BASE        0x20000000
#endif

#ifndef GPIO_BASE
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#endif

#define MAXTIMINGS 100

#include "dht.h"

float temperature  = 0;
float humidity     = 0;
time_t last_update = 0;

int bits[250], data[100];
int bitidx = 0;

int readDHT(int type, int pin) {
	int counter = 0;
	int laststate = HIGH;
	int i=0, j=0;

	// Set GPIO pin to output
	bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);

	bcm2835_gpio_write(pin, HIGH);
	usleep(500000);  // 500 ms
	bcm2835_gpio_write(pin, LOW);
	usleep(20000);

	bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	// wait for pin to drop.
	while (bcm2835_gpio_lev(pin) == 1) {
		usleep(1);
	}

	// read data.
	for (i=0; i< MAXTIMINGS; i++) {
		counter = 0;
		while ( bcm2835_gpio_lev(pin) == laststate) {
			counter++;
			//nanosleep(1);		// overclocking might change this?
			if (counter == 1000)
				break;
		}
		laststate = bcm2835_gpio_lev(pin);
		if (counter == 1000) break;
		bits[bitidx++] = counter;

		if ((i>3) && (i%2 == 0)) {
			// shove each bit into the storage bytes
			data[j/8] <<= 1;
			if (counter > 200)
				data[j/8] |= 1;
			j++;
		}
	}


//	printf("Data (%d): 0x%x 0x%x 0x%x 0x%x 0x%x\n", j, data[0], data[1], data[2], data[3], data[4]);

	if ((j >= 39) &&
			(data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
		// yay!

		last_update = time(NULL);

		if (type == DHT11) {
			temperature = data[2];
			humidity = data[0];
		} else if (type == DHT22) {
			float f, h;
			h = data[0] * 256 + data[1];
			h /= 10;

			f = (data[2] & 0x7F)* 256 + data[3];
			f /= 10.0;
			if (data[2] & 0x80)  f *= -1;
			temperature = f;
			humidity = h;
		}
		return 1;
	}

	return 0;
}

