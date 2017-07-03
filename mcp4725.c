#include "mcp4725.h"

void set_dac(uint8_t value) {
	//dac
	I2C_HandleTypeDef* hi2c1 = (I2C_HandleTypeDef*)get_hi2c1();

	int val = 0;
	uint8_t output[] = {0x40, (val/16), ((val%16)<<4) };
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c1, 0xC4, output, 3, 4);
	if (status != HAL_OK) {
		LOG("OOPS");
	}
}