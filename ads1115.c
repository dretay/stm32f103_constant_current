#include "ads1115.h"

static void writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value) {
	
	uint8_t tx_buffer[3] = { (uint8_t)reg, (uint8_t)(value >> 8), (uint8_t)(value & 0xFF) };
	//HAL_I2C_Master_Transmit(hi2c1, i2cAddress, tx_buffer, 3, 4);	
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c1, i2cAddress, tx_buffer, 3, 4);
	if (status != HAL_OK) {
		LOG("OOPS");
	}
}
static uint16_t readRegister(uint8_t i2cAddress, uint8_t reg) {
	
	uint8_t tx_buffer[1] = { ADS1015_REG_POINTER_CONVERT };
	uint8_t rx_buffer[2] = { 0, 0 };

	//HAL_I2C_Master_Transmit(hi2c1, i2cAddress, tx_buffer, 1, 4);
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c1, i2cAddress, tx_buffer, 1, 4);
	if (status != HAL_OK) {
		LOG("OOPS");
	}
	HAL_I2C_Master_Receive(&hi2c1, i2cAddress + 1, rx_buffer, 2, 4);

	return ((rx_buffer[0] << 8) | rx_buffer[1]);  
}

float convert() {
//adc
	uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
	// Set PGA/voltage range
	config |= GAIN_TWOTHIRDS;

	config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
	
	config |= ADS1015_REG_CONFIG_OS_SINGLE;
	
	writeRegister(0x90, ADS1015_REG_POINTER_CONFIG, config);
	HAL_Delay(8);
	uint16_t res = readRegister(0x90, ADS1015_REG_POINTER_CONVERT) >> 0;  

	return (res * 0.1875) / 1000; 
	
}
void adcCallback(void const * argument) {
	T_SYSTEM_UPDATE *update;
		
	update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, osWaitForever); /* Allocate memory */
	update->idx = 0;
	update->val = convert();	
	update->source = ADC;
	update->parameter = VOLTAGE;
	osMailPut(SYS_UPDATE_MAILBOX_ID, update);
	
}

const struct ads1115 ADS1115 = { 
	.convert = convert
};