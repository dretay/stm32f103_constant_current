#pragma once
#include <stdint.h>
#include "cmsis_os.h"
#include <stdbool.h>
#include "gfx.h"
#include "stm32f1xx_hal.h"
#include "RotaryEncoder.h"
#include "ToggleSwitch.h"
#include "MCP4725.h"
#include "strings.h"
#include "limits.h"
#include "Adc.h"
#include "ADS1115.h"
#include "serialcommand.h"
#include "UsbSerialCommandAdapter.h"
#include "UartSerialCommandAdapter.h"
#include "Flash.h"
#include "CurrentSink.h"
#include "gui.h"

static void showView(uint8_t);

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc1;

struct application
{
	void(*configure)(void);
};

extern const struct application Application;

