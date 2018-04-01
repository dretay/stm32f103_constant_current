#pragma once

#include "MCP4725.h"
#include "polyfit.h"
#include "Flash.h"
#include "math.h"
#include "serialcommand.h"
#include "minmax.h"
#include "cmsis_os.h"
#include "events.h"
#include <stdlib.h>

#define CURRENT_SINK_COEFFICIENT_0 0
#define CURRENT_SINK_COEFFICIENT_1 1
#define CURRENT_SINK_COEFFICIENT_2 2
#define CURRENT_SINK_COEFFICIENT_3 3

#ifdef INCLUDE_uxTaskGetStackHighWaterMark
UBaseType_t SysUpdateTask_Watermark;
#endif

struct currentsink {
	void(*configure)(uint8_t idx, I2C_HandleTypeDef* p_i2c_in, uint8_t addr_in);			
	void(*set)(float setting);	
	void(*print_setting)(void);
	void(*print_voltage_reading)(void);
	void(*print_current_reading)(void);
	void(*print_power_reading)(void);
	float(*get_voltage_reading)(void);
	float(*get_current_reading)(void);
	float(*get_power_reading)(void);
	float(*get_current_setting)(void);
	bool(*get_enable_setting)(void);
	void(*save_cal_const)();
	void(*print_cal_const)();
	
};

extern const struct currentsink CurrentSink;
