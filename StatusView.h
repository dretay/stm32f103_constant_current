#pragma once

#include "View.h"
#include <stdint.h>
#include <stm32f1xx_hal.h>
#include "log.h"
#include "gfx.h"
#include "MCP4725.h"
#include "numtostring.h"
#include "math.h"
#include <stdlib.h>
#include "polyfit.h"
#include "Flash.h"

uint8_t my_encoder_val;
struct statusView {	
	View*(*init)(void);
};

extern const struct statusView StatusView;

