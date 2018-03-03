#pragma once
#include <stdint.h>
#include "cmsis_os.h"
#include "log.h"
#include "StatusView.h"
#include "gfx.h"
#include "stm32f1xx_hal.h"
#include "RotaryEncoder.h"
#include "ToggleSwitch.h"
#include "MCP4725.h"
#include "strings.h"
#include "limits.h"
#include "Adc.h"
#include "usbserialcommand.h"
#include "Flash.h"

static void showView(uint8_t);

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc1;
extern osThreadId guiDrawTaskHandle;

#ifdef INCLUDE_uxTaskGetStackHighWaterMark
UBaseType_t SysUpdateTask_Watermark;
UBaseType_t GUIDrawTask_Watermark;
#endif


struct application
{
	void(*showView)(uint8_t idx);
};

extern const struct application Application;

