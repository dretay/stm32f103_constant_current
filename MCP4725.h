#pragma once

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define MCP4725_CONFIG_CNT 1

#define MCP4726_CMD_WRITEDAC            (0x40)  // Writes data to the DAC
#define MCP4726_CMD_WRITEDACEEPROM      (0x60)  // Writes data to the DAC and the EEPROM (persisting the assigned value after reset)

typedef struct {
	I2C_HandleTypeDef* p_i2c;
	int addr;
	int setting;
} MCP4725Config;

struct mcp4725 {
	void(*add_dac)(int idx, I2C_HandleTypeDef* p_i2c_in, int addr_in);	
	void(*set_dac)(int idx, int value);
	int(*get_dac)(int idx);
};

extern const struct mcp4725 MCP4725;

