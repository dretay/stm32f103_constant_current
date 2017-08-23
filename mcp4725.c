#include "mcp4725.h"

static MCP4725_CONFIG* config;
static uint8_t config_cnt;

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

static void configure(MCP4725_CONFIG* config_in, uint8_t cnt_in) {
	int i = 0;
	config = config_in;
	config_cnt = cnt_in;
	for (i = 0; i < cnt_in; i++)
	{
		marshal(i, 0);
	}
}

static void set_dac(uint8_t idx, uint16_t value) {
	marshal(idx, value);	
}

const struct mcp4725 MCP4725 = { 
	.configure = configure,
	.set_dac = set_dac
};
