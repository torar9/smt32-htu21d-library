#include <htu21d_sensor.h>

#define HTU21D_ADDRESS (0x40 << 1)
#define HTU21D_COMMAND_TRIG_TEMP_MEAS (0xe3)
#define HTU21D_COMMAND_TRIG_HUM_MEAS (0xe5)
#define HTU21D_COMMAND_WRITE_REG (0xE6)
#define HTU21D_COMMAND_READ_REG (0xE7)
#define HTU21D_COMMAND_RESET (0xFE)
#define HTU21D_DELAY (50)
#define HTU21D_MEAS_DELAY (50)
#define HTU21D_TIMEOUT (250)
#define HTU21D_CRC8_POLY      0x13100

typedef enum
{
	HTU21D_MASK_OTP = 0x02,
	HTU21D_MASK_HEAT = 0x04,
	HTU21D_MASK_RESERVED = 0x38,
	HTU21D_MASK_BAT = 0x40,
	HTU21D_MASK_RESOLUTION = 0x81
} HTU21D_REG_MASK;

uint8_t HT21UD_registers = 0;

static uint8_t checkCRC8(uint16_t data)
{
  for (uint8_t bit = 0; bit < 16; bit++)
  {
    if   (data & 0x8000) data = (data << 1) ^ HTU21D_CRC8_POLY;
    else data <<= 1;
  }
  return data >>= 8;
}

uint8_t HTU21D_read_registers()
{
	HAL_StatusTypeDef status = 0;
	uint8_t buffer[1] = {HTU21D_COMMAND_READ_REG};

	status |= HAL_I2C_Master_Transmit(&hi2c1, (HTU21D_ADDRESS), buffer, 1, HTU21D_TIMEOUT);
	status |= HAL_I2C_Master_Receive(&hi2c1, (HTU21D_ADDRESS + 1), buffer, 1, HTU21D_TIMEOUT);

	HT21UD_registers = buffer[0];

	return (status == 0);
}

uint8_t HTU21D_init()
{
	uint8_t result = 0;

	result = HTU21D_read_registers();

	return result;
}

uint8_t HTU21D_setResolution(HTU21D_RESOLUTION resolution)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint8_t buffer[2] = {HTU21D_COMMAND_WRITE_REG, 0};

	buffer[1] = resolution;
	status = HAL_I2C_Master_Transmit(&hi2c1, (HTU21D_ADDRESS), buffer, 2, HTU21D_TIMEOUT);

	return (HAL_OK == status);
}

uint8_t HTU21D_readHumidity(double *hum)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint16_t tmp = 0;
	uint8_t buffer[3] = {HTU21D_COMMAND_TRIG_HUM_MEAS, 0, 0};

	status |= HAL_I2C_Master_Transmit(&hi2c1, (HTU21D_ADDRESS), buffer, 1, HTU21D_TIMEOUT);
	status |= HAL_I2C_Master_Receive(&hi2c1, (HTU21D_ADDRESS + 1), buffer, 4, HTU21D_TIMEOUT);

	tmp = (((uint16_t)buffer[0] << 8) | ((uint16_t)buffer[1]));

	/* Check CRC */
	if(buffer[2] == checkCRC8(tmp))
	{
		*hum = ((-6.0) + (125 * (tmp/(double)(65536)))); /* Convert unsigned value to double according to datasheet */
	}
	else
	{
		status = 1;
	}

	return (HAL_OK == status);
}

uint8_t HTU21D_readTemperature(double *temp)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint16_t tmp = 0;
	uint8_t buffer[3] = {HTU21D_COMMAND_TRIG_TEMP_MEAS, 0, 0};

	status |= HAL_I2C_Master_Transmit(&hi2c1, (HTU21D_ADDRESS), buffer, 1, HTU21D_TIMEOUT);
	status |= HAL_I2C_Master_Receive(&hi2c1, (HTU21D_ADDRESS + 1), buffer, 3, HTU21D_TIMEOUT);

	tmp = (((uint16_t)buffer[0] << 8) | ((uint16_t)buffer[1]));

	/* Check CRC */
	if(buffer[2] == checkCRC8(tmp))
	{
		*temp = ((-46.85) + (175.72 * (tmp/(double)(65536)))); /* Convert unsigned value to double according to datasheet */
	}
	else
	{
		status = 1;
	}

	return (HAL_OK == status);
}
