#include "MCP4725.h"

static MCP4725Config config[MCP4725_CONFIG_CNT];

//do conversion of data to send to device
static void marshal(uint8_t idx, uint16_t value) {
	I2C_HandleTypeDef* hi2c1 = config[idx].p_i2c;
	uint8_t addr = config[idx].addr;
	uint8_t output[] = { 0x40, (value / 16), ((value % 16) << 4) };
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c1, addr, output, 3, 4);
	if (status != HAL_OK) {
		LOG("MCP4725 Marshal Failed\n");
	}
}

//do conversion from device response
static void unmarshal(void) {

}

static void set_dac(uint8_t idx, uint16_t value) {
	marshal(idx, value);	
}
static void add_dac(uint8_t idx, I2C_HandleTypeDef* p_i2c_in, uint8_t addr_in) {
	if (idx <= MCP4725_CONFIG_CNT) {
		config[idx].p_i2c = p_i2c_in;
		config[idx].addr = addr_in;		
	}
}
const struct mcp4725 MCP4725 = { 
	.add_dac = add_dac,
	.set_dac = set_dac
};
