#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_conf.h"

extern I2C_HandleTypeDef hi2c1;

typedef enum
{
	RH_12_TEMP_14 = 0x00,
	RH_8_TEMP_12 = 0x01,
	RH_10_TEMP_13 = 0x80,
	RH_11_TEMP_11 = 0x81
} HTU21D_RESOLUTION;

uint8_t HTU21D_init();

uint8_t HTU21D_setResolution(HTU21D_RESOLUTION resolution);

uint8_t HTU21D_readHumidity(double *hum);

uint8_t HTU21D_readTemperature(double *temp);

#endif
