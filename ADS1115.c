#include "ADS1115.h"

static I2C_HandleTypeDef *hi2c;
static uint8_t addr;

//do conversion of data to send to device
static void marshal(uint8_t idx, uint8_t reg, uint16_t value) {
	
	uint8_t tx_buffer[3] = { (uint8_t)reg, (uint8_t)(value >> 8), (uint8_t)(value & 0xFF) };
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c, addr, tx_buffer, 3, 4);
	
	if (status != HAL_OK) {
		LOG("ADS1115 Marshal Failed\n");
	}
}

//do conversion from device response
static uint16_t unmarshal(uint8_t idx, uint8_t reg) {
	
	uint8_t tx_buffer[1] = { ADS1015_REG_POINTER_CONVERT };
	uint8_t rx_buffer[2] = { 0, 0 };

	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c, addr, tx_buffer, 1, 4);
	if (status != HAL_OK) {
		LOG("ADS1115 Unmarshal Failed\n");
	}
	HAL_I2C_Master_Receive(hi2c, addr + 1, rx_buffer, 2, 4);

	return ((rx_buffer[0] << 8) | rx_buffer[1]);  

}

static void configure(I2C_HandleTypeDef *hi2c_in, uint8_t addr_in) {
	hi2c = hi2c_in;
	addr = addr_in;
}

static uint16_t adc_channels[4] = { ADS1015_REG_CONFIG_MUX_SINGLE_0, ADS1015_REG_CONFIG_MUX_SINGLE_1, ADS1015_REG_CONFIG_MUX_SINGLE_2, ADS1015_REG_CONFIG_MUX_SINGLE_3 };

static float get_reading(uint8_t idx) {
	//adc
	uint16_t value = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
	// Set PGA/voltage range
	value |= GAIN_TWOTHIRDS; //todo: this can  probably be set to a gain of "1" - 4v max

	value |= adc_channels[idx];
	
	value |= ADS1015_REG_CONFIG_OS_SINGLE;
	
	marshal(0, ADS1015_REG_POINTER_CONFIG, value);
	HAL_Delay(8);
	uint16_t res = unmarshal(0, ADS1015_REG_POINTER_CONVERT) >> 0;  
	
	return (res * 0.1875) / 1000; 
	
}

static float get_differential_reading(uint8_t idx) {
  // Start with default values
	uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
	                  ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
	                  ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
	                  ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
	                  ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
	                  ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

	                    // Set PGA/voltage range
	config |= GAIN_TWOTHIRDS;
                    
	                // Set channels
	if (idx == 0) {
		config |= ADS1015_REG_CONFIG_MUX_DIFF_0_1;          // AIN0 = P, AIN1 = N	
	}
	else {
		config |= ADS1015_REG_CONFIG_MUX_DIFF_2_3;          // AIN2 = P, AIN3 = N	
	}
	

	  // Set 'start single-conversion' bit
	config |= ADS1015_REG_CONFIG_OS_SINGLE;

	  // Write config register to the ADC
	marshal(0, ADS1015_REG_POINTER_CONFIG, config);

	  // Wait for the conversion to complete
	HAL_Delay(8);

	  // Read the conversion results
	uint16_t res = unmarshal(0, ADS1015_REG_POINTER_CONVERT) >> 0;
	return (res * 0.1875) / 1000; 
}
const struct ads1115 ADS1115 = { 
	.configure = configure,
	.get_reading = get_reading
};

void adcCallback(void const * argument) {
	T_SYSTEM_UPDATE *update;
	
	int i = 0;
	float reading;
		
	for (i = 0; i < 1; i++) {
		
		reading = get_reading(i);
		update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, osWaitForever); /* Allocate memory */
		update->idx = i;
		update->float_val = reading * 1;//channel_config.ratio;	
		update->val_type = type_float;		
		update->source = ADS1115_READ_EVENT;		
		osMailPut(SYS_UPDATE_MAILBOX_ID, update);	
		
	}	
	
	
}
