#include <htu21d_sensor.h>

#define HTU21D_ADDRESS (0x40)
#define HTU21D_ADDRESS_READ ((HTU21D_ADDRESS << 1) + 1U)
#define HTU21D_ADDRESS_WRITE (HTU21D_ADDRESS << 1)
#define HTU21D_COMMAND_TRIG_TEMP_MEAS (0xe3)
#define HTU21D_COMMAND_TRIG_HUM_MEAS (0xe5)
#define HTU21D_COMMAND_WRITE_REG (0xE6)
#define HTU21D_COMMAND_READ_REG (0xE7)
#define HTU21D_COMMAND_RESET (0xFE)
#define HTU21D_CRC8_POLY (0x13100)

static uint8_t HT21UD_registers = 0U;

static uint8_t checkCRC8(uint16_t data)
{
  for (uint8_t bit = 0; bit < 16U; bit++)
  {
    if   (data & 0x8000) data = (data << 1U) ^ HTU21D_CRC8_POLY;
    else data <<= 1U;
  }
  return data >>= 8U;
}

/**
  * @brief  Resets HTU21D
  * @retval HTU21D_STATUS
  */
HTU21D_STATUS HTU21D_reset()
{
	HAL_StatusTypeDef comStatus = HAL_OK;
	HTU21D_STATUS result = HTU21D_OK;
	uint8_t buffer[1] = {HTU21D_COMMAND_RESET};

	comStatus |= HAL_I2C_Master_Transmit(&hi2c1, HTU21D_ADDRESS_WRITE, buffer, 1U, HTU21D_TIMEOUT);

	return result;
}

HTU21D_STATUS HTU21D_readRegisters()
{
	HAL_StatusTypeDef comStatus = HAL_OK;
	HTU21D_STATUS result = HTU21D_OK;
	uint8_t buffer[1] = {HTU21D_COMMAND_READ_REG};

	comStatus |= HAL_I2C_Master_Transmit(&hi2c1, HTU21D_ADDRESS_WRITE, buffer, 1U, HTU21D_TIMEOUT);
	comStatus |= HAL_I2C_Master_Receive(&hi2c1, HTU21D_ADDRESS_READ, buffer, 1U, HTU21D_TIMEOUT);

	HT21UD_registers = buffer[0];

	if(HAL_OK != comStatus)
	{
		result |= HTU21D_COM_FAIL;
	}

	return result;
}

/**
  * @brief  Initialize HTU21D - reads register
  * @retval HTU21D_STATUS
  */
HTU21D_STATUS HTU21D_init()
{
	HTU21D_STATUS result = HTU21D_OK;

	result |= HTU21D_readRegisters();

	return result;
}

/**
  * @brief  Initialize HTU21D - sets resolution of HTU21D measurement
  * @param  resolution - resolution for humidity and temperature
  * @retval HTU21D_STATUS
  */
HTU21D_STATUS HTU21D_setResolution(HTU21D_RESOLUTION resolution)
{
	HAL_StatusTypeDef comStatus = HAL_ERROR;
	HTU21D_STATUS result = HTU21D_OK;
	uint8_t buffer[2] = {HTU21D_COMMAND_WRITE_REG, 0U};

	buffer[1] = resolution;
	comStatus = HAL_I2C_Master_Transmit(&hi2c1, HTU21D_ADDRESS_WRITE, buffer, 2U, HTU21D_TIMEOUT);

	if(HAL_OK != comStatus)
	{
		result = HTU21D_COM_FAIL;
	}

	return result;
}

/**
  * @brief  Initialize HTU21D - starts reading of humidity
  * @param  resolution - return parameter of measured humidity
  * @retval HTU21D_STATUS - return result of HTU21D reading
  */
HTU21D_STATUS HTU21D_readHumidity(double *hum)
{
	HAL_StatusTypeDef comStatus = HAL_OK;
	HTU21D_STATUS result = HTU21D_OK;
	uint16_t tmp = 0U;
	uint8_t buffer[3] = {HTU21D_COMMAND_TRIG_HUM_MEAS, 0U, 0U};

	comStatus |= HAL_I2C_Master_Transmit(&hi2c1, HTU21D_ADDRESS_WRITE, buffer, 1U, HTU21D_TIMEOUT);
	comStatus |= HAL_I2C_Master_Receive(&hi2c1, HTU21D_ADDRESS_READ, buffer, 3U, HTU21D_TIMEOUT);

	if(HAL_OK != comStatus)
	{
		result |= HTU21D_COM_FAIL;
	}

	/* Transmit OK? */
	if(HTU21D_OK == result)
	{
		tmp = (((uint16_t)buffer[0] << 8U) | ((uint16_t)buffer[1]));
		/* Check CRC */
		if(buffer[2] == checkCRC8(tmp))
		{
			*hum = ((-6.0) + (125 * (tmp/(double)(65536)))); /* Convert unsigned value to double according to datasheet */
		}
		else
		{
			result |= HTU21D_CRC_FAIL;
		}
	}

	return result;
}

/**
  * @brief  Initialize HTU21D - starts reading of temperature
  * @param  resolution - return parameter of measured temperature
  * @retval HTU21D_STATUS - return result of HTU21D reading
  */
HTU21D_STATUS HTU21D_readTemperature(double *temp)
{
	HAL_StatusTypeDef comStatus = HAL_OK;
	HTU21D_STATUS result = HTU21D_OK;
	uint16_t tmp = 0U;
	uint8_t buffer[3] = {HTU21D_COMMAND_TRIG_TEMP_MEAS, 0, 0};

	comStatus |= HAL_I2C_Master_Transmit(&hi2c1, HTU21D_ADDRESS_WRITE, buffer, 1, HTU21D_TIMEOUT);
	comStatus |= HAL_I2C_Master_Receive(&hi2c1, HTU21D_ADDRESS_READ, buffer, 3, HTU21D_TIMEOUT);

	if(HAL_OK != comStatus)
	{
		result |= HTU21D_COM_FAIL;
	}

	/* Transmit OK? */
	if(HTU21D_OK == result)
	{
		tmp = (((uint16_t)buffer[0] << 8U) | ((uint16_t)buffer[1]));
		/* Check CRC */
		if(buffer[2] == checkCRC8(tmp))
		{
			*temp = ((-46.85) + (175.72 * (tmp/(double)(65536)))); /* Convert unsigned value to double according to datasheet */
		}
		else
		{
			result |= HTU21D_CRC_FAIL;
		}
	}

	return result;
}

/**
  * @brief  Initialize HTU21D - reads registers from HTU21D
  * @param  resolution - return parameter of register readings
  * @retval HTU21D_STATUS - return result of HTU21D reading
  */
HTU21D_STATUS HTU21D_getRegisters(uint8_t *regSatus)
{
	HTU21D_STATUS result = HTU21D_OK;

	*regSatus = HT21UD_registers;

	result |= HTU21D_readRegisters();

	return result;
}
