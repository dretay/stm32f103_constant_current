#pragma once

#include "MCP4725.h"
#include "polyfit.h"
#include "Flash.h"
#include "math.h"
#include "serialcommand.h"
#include "minmax.h"

#define CURRENT_SINK_COEFFICIENT_0 0
#define CURRENT_SINK_COEFFICIENT_1 1
#define CURRENT_SINK_COEFFICIENT_2 2
#define CURRENT_SINK_COEFFICIENT_3 3

struct currentsink {
	void(*configure)(uint8_t idx, I2C_HandleTypeDef* p_i2c_in, uint8_t addr_in);			
	void(*set)(float setting);	
	void(*calibrate_save)();
	void(*calibrate_setup)();
	void(*calibrate_finish)();
	void(*calibrate_dump)();
	
	
};

extern const struct currentsink CurrentSink;
