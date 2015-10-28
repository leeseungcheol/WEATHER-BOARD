#include <stdio.h>
#include "bme280-i2c.h"
#include "si1132.h"

static int pressure;
static int temperature;
static int humidity;
static float altitude;

#define SEALEVELPRESSURE_HPA	1024.25F

int main(int argc, char **argv)
{
	int status = 0;
	char *device = "/dev/i2c-1";

	if (argc == 2) {
		device = argv[1];
	} else if (argc > 2) {
		printf("Usage :\n");
		printf("sudo ./weather_board [i2c node](default \"/dev/i2c-1\")\n");
		return -1;
	}

	si1132_begin(device);
	bme280_begin(device);

	while (1) {
		bme280_read_pressure_temperature_humidity(
					&pressure, &temperature, &humidity);
		altitude = readAltitude(pressure, SEALEVELPRESSURE_HPA);

		printf("======== bme280 ========\n");
		printf("temperature : %.2lf 'C\n", (double)temperature/100.0);
		printf("humidity : %.2lf %%\n", (double)humidity/1024.0);
		printf("pressure : %.2lf hPa\n", (double)pressure/100.0);
		printf("altitude : %f m\n", altitude);
		printf("======== si1132 ========\n");
		printf("UV_index : %.2f\n", readUV()/100.0);
		printf("Visible : %.0f Lux\n", readVisible());
		printf("IR : %.0f Lux\n", readIR());
		printf("========================\n");

		usleep(1000000);
	}
	return 0;
}
