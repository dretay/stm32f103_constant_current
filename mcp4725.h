#pragma once

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"


typedef struct {
	I2C_HandleTypeDef* p_i2c;
	uint8_t addr;
} MCP4725_CONFIG;

struct mcp4725 {
	void(*configure)(MCP4725_CONFIG* config, uint8_t cnt);	
	void(*set_dac)(uint8_t idx, uint16_t value);
	uint8_t(*get_dac)(void);
};

extern const struct mcp4725 MCP4725;

