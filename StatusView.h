#pragma once

#include "View.h"
#include <stdint.h>
#include <stm32f1xx_hal.h>
#include "gfx.h"
#include "MCP4725.h"
#include "numtostring.h"
#include "math.h"
#include <stdlib.h>
#include "CurrentSink.h"


uint8_t my_encoder_val;
struct statusView {	
	View*(*init)(void);
};

extern const struct statusView StatusView;

