#include <stdio.h>
#include "bme280-i2c.h"
#include "si1132.h"
#include "si702x.h"
#include "bmp180.h"

const char version[] = "v1.5";

static int pressure;
static int temperature;
static int humidity;
static float altitude;

float SEALEVELPRESSURE_HPA = 1024.25;

int main(int argc, char **argv)
{
	int status = 0;
	int WBVersion = 2;
	char *device = "/dev/i2c-1";

	if (argc == 2) {
		device = argv[1];
	} else if (argc > 2) {
		printf("Usage :\n");
		printf("sudo ./weather_board [i2c node](default \"/dev/i2c-1\")\n");
		return -1;
	}

	si1132_begin(device);
	if (bme280_begin(device) < 0) {
		si702x_begin(device);
		bmp180_begin(device);
		WBVersion = 1;
	}

	printf("\e[2J");
	printf("\e[5;30HWEATHER-BOARD %s\n", version);

	while (1) {
		printf("\e[H======== si1132 ========\n");
		printf("UV_index : %.2f\e[K\n", Si1132_readUV()/100.0);
		printf("Visible : %.0f Lux\e[K\n", Si1132_readVisible());
		printf("IR : %.0f Lux\e[K\n", Si1132_readIR());
		if (WBVersion == 2) {
			bme280_read_pressure_temperature_humidity(
						&pressure, &temperature, &humidity);
			printf("======== bme280 ========\n");
			printf("temperature : %.2lf 'C\e[K\n", (double)temperature/100.0);
			printf("humidity : %.2lf %%\e[K\n", (double)humidity/1024.0);
			printf("pressure : %.2lf hPa\e[K\n", (double)pressure/100.0);
			printf("altitude : %f m\e[K\n", bme280_readAltitude(pressure,
								SEALEVELPRESSURE_HPA));
		} else {
			printf("======== bmp180 ========\n");
			printf("temperature : %.2f 'C\e[K\n", BMP180_readTemperature());
			printf("pressure : %.2f hPa\e[K\n", BMP180_readPressure()/100);
			printf("Altitude : %.2f meter\e[K\n",
					BMP180_readAltitude(SEALEVELPRESSURE_HPA));
			printf("======== si7020 ========\n");
			printf("temperature : %.2f 'C\e[K\n", Si702x_readTemperature());
			printf("humidity : %.2f %%\e[K\n", Si702x_readHumidity());
		}

		usleep(1000000);
	}
	return 0;
}
