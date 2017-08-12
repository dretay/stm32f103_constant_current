#pragma once
#include <stdint.h>
#include "cmsis_os.h"
#include "log.h"
#include "StatusView.h"
#include "gfx.h"
#include "stm32f1xx_hal.h"
#include "encoder.h"
#include "ads1115.h"
#include "mcp4725.h"

static void showView(uint8_t);

extern I2C_HandleTypeDef hi2c1;

struct application
{
	void(*showView)(uint8_t idx);
};

extern const struct application Application;
