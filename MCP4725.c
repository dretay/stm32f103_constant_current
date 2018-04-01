#include "MCP4725.h"

static MCP4725Config config[MCP4725_CONFIG_CNT];

//do conversion of data to send to device
static void marshal(uint8_t idx, uint16_t value) {
	I2C_HandleTypeDef* hi2c1 = config[idx].p_i2c;
	uint8_t addr = config[idx].addr;
	uint8_t output[] = { MCP4726_CMD_WRITEDAC, (value / 16), ((value % 16) << 4) };
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c1, addr, output, 3, 4);
	if (status != HAL_OK) {
		LOG("MCP4725 Marshal Failed\n");
	}
}

//do conversion from device response
static void unmarshal(void) {

}

static void set_dac(int idx, int value) {
	marshal(idx, value);	
	config[idx].setting = value;
}
static int get_dac(int idx) {
	return config[idx].setting;
}
static void add_dac(int idx, I2C_HandleTypeDef* p_i2c_in, int addr_in) {
	if (idx <= MCP4725_CONFIG_CNT) {
		config[idx].p_i2c = p_i2c_in;
		config[idx].addr = addr_in;		
	}
}
const struct mcp4725 MCP4725 = { 
	.add_dac = add_dac,
	.set_dac = set_dac,
	.get_dac = get_dac
};
