#include "CurrentSink.h"

static unsigned int DAC_IDX;

int VCP_write(const void *pBuffer, int size);

static double xData[8] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0	 
};
static double yData[8] = {
	0,
	1000,
	1500,
	2000,
	2500,
	3000,
	3500,
	4095 
};

static void configure(uint8_t idx_in, I2C_HandleTypeDef* p_i2c_in, uint8_t addr_in) {
	DAC_IDX = idx_in;

	MCP4725.add_dac(DAC_IDX, p_i2c_in, addr_in);	
	MCP4725.set_dac(DAC_IDX, 0);

}

const static float current_multiplier = (4095.0f / (5.0f));
static void set(float setting) {
//	unsigned int converted_setting;
//	double coefficient0 = Flash.get(CURRENT_SINK_COEFFICIENT_0, type_double).double_val;
//	double coefficient1 = Flash.get(CURRENT_SINK_COEFFICIENT_1, type_double).double_val;
//	double coefficient2 = Flash.get(CURRENT_SINK_COEFFICIENT_2, type_double).double_val;
//	double coefficient3 = Flash.get(CURRENT_SINK_COEFFICIENT_3, type_double).double_val;
//
//	converted_setting = floor((coefficient3 * pow(setting, 3)) + 
//	(coefficient2 * pow(setting, 2)) + 
//	(coefficient1 * setting) + 
//	coefficient0);

	MCP4725.set_dac(DAC_IDX, MIN(4095,floor(setting*current_multiplier)));

}

static void calibrate_setup() {
	char* arg = SerialCommand.next();
	uint16_t idx = atoi(arg);
	
	MCP4725.set_dac(DAC_IDX, yData[idx]);
}
static void calibrate_save() {
	char* arg = SerialCommand.next();
	uint16_t idx = atoi(arg);
	arg = SerialCommand.next();
	double val = atof(arg);
	xData[idx] = val;
}
static void calibrate_dump() {
	char textbuf[128];
	unsigned int i=0;
	for(; i < 8; i++)
	{
//		sprintf(textbuf, "%d: %d %d\n\r",i, (int)(yData[i]*100), (int)(xData[i]*100));
//		VCP_write(textbuf, strlen(textbuf));
	}
	
}
static void calibrate_finish() {
	const unsigned int order = 3;
	const unsigned int countOfElements = 8;
	const double acceptableError = 0.01;
	double coefficients[order + 1];
	int result = polyfit(xData, yData, countOfElements, order, coefficients);
			
	Flash.set_double(CURRENT_SINK_COEFFICIENT_0, coefficients[0]);
	Flash.set_double(CURRENT_SINK_COEFFICIENT_1, coefficients[1]);
	Flash.set_double(CURRENT_SINK_COEFFICIENT_2, coefficients[2]);
	Flash.set_double(CURRENT_SINK_COEFFICIENT_3, coefficients[3]);
}
const struct currentsink CurrentSink= { 
	.configure = configure,
	.set = set,
	.calibrate_setup = calibrate_setup,
	.calibrate_save = calibrate_save,
	.calibrate_dump = calibrate_dump,
	.calibrate_finish = calibrate_finish
};
