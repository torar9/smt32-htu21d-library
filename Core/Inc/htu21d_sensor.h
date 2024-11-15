#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_conf.h"

/* +++ Configurable macros +++ */

/**
  * @brief Delay I2C timeout - how long to wait for response before connection is considered as a fail
  */
#define HTU21D_TIMEOUT (100)

/**
  * @brief Maximum possible humidity
  */
#define HTU21D_MAX_HUM (100)

/**
  * @brief Minimum possible humidity
  */
#define HTU21D_MIN_HUM (0)

/**
  * @brief Maximum possible temperature
  */
#define HTU21D_MAX_TEMP (125)

/**
  * @brief Minimum possible temperature
  */
#define HTU21D_MIN_TEMP (-40)

/* --- Configurable macros --- */

/* extern reference to I2C - CHANGE BEFORE USAGE */

/**
  * @brief Resolution option for HTU21D
  */
typedef enum
{
	RH_12_TEMP_14 = 0x00, /* resolution: 12 bits for humidity, 14 for temperature */
	RH_8_TEMP_12 = 0x01, /* resolution: 8 bits for humidity, 12 for temperature */
	RH_10_TEMP_13 = 0x80, /* resolution: 10 bits for humidity, 13 for temperature */
	RH_11_TEMP_11 = 0x81 /* resolution: 11 bits for humidity, 11 for temperature */
} HTU21D_RESOLUTION;

/**
  * @brief Types of error
  */
typedef enum
{
	HTU21D_OK = 0, /* No error detected */
	HTU21D_COM_FAIL = 1, /* Communication error */
	HTU21D_CRC_FAIL = 2, /* CRC check failed, data corrupted */
	HTU21D_SATURATED = 4 /* Sensor raw values are below or above thresholds - readings are saturated */
} HTU21D_ERR_TYPE;

/**
  * @brief Return type of HTU21D status
  */
typedef uint8_t HTU21D_STATUS;

/**
  * @brief List of masks for each register - see datasheet for more info
  */
typedef enum
{
	HTU21D_MASK_OTP = 0x02, /* Mask for OTP register */
	HTU21D_MASK_HEAT = 0x04, /* Mask for heat setting register */
	HTU21D_MASK_RESERVED = 0x38, /* Mask for reserved registers */
	HTU21D_MASK_BAT = 0x40, /* Mask for battery register */
	HTU21D_MASK_RESOLUTION = 0x81 /* Mask for humidity and temperature resolution register */
} HTU21D_REG_MASK;

/**
  * @brief  Resets HTU21D
  * @retval HTU21D_STATUS
  */
HTU21D_STATUS HTU21D_reset();

/**
  * @brief  Initialize HTU21D - reads register
  * @retval HTU21D_STATUS
  */
HTU21D_STATUS HTU21D_init(I2C_HandleTypeDef* i2c);

/**
  * @brief  Initialize HTU21D - sets resolution of HTU21D measurement
  * @param  resolution - resolution for humidity and temperature
  * @retval HTU21D_STATUS
  */
HTU21D_STATUS HTU21D_setResolution(HTU21D_RESOLUTION resolution);

/**
  * @brief  Initialize HTU21D - starts reading of humidity
  * @param  resolution - return parameter of measured humidity
  * @retval HTU21D_STATUS - return result of HTU21D reading
  */
HTU21D_STATUS HTU21D_readHumidity(double *hum);

/**
  * @brief  Initialize HTU21D - starts reading of temperature
  * @param  resolution - return parameter of measured temperature
  * @retval HTU21D_STATUS - return result of HTU21D reading
  */
HTU21D_STATUS HTU21D_readTemperature(double *temp);

/**
  * @brief  Initialize HTU21D - reads registers from HTU21D
  * @param  resolution - return parameter of register readings
  * @retval HTU21D_STATUS - return result of HTU21D reading
  */
HTU21D_STATUS HTU21D_getRegisters(uint8_t *regSatus);

#endif
