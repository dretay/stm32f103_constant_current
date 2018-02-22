#pragma once

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define MCP4725_CONFIG_CNT 1

typedef struct {
	I2C_HandleTypeDef* p_i2c;
	uint8_t addr;
} MCP4725Config;

struct mcp4725 {
	void(*add_dac)(uint8_t idx, I2C_HandleTypeDef* p_i2c_in, uint8_t addr_in);	
	void(*set_dac)(uint8_t idx, uint16_t value);
	uint8_t(*get_dac)(void);
};

extern const struct mcp4725 MCP4725;

