#pragma once

#include "view.h"
#include <stdint.h>
#include "log.h"
#include "gfx.h"
#include <stm32f1xx_hal.h>
#include <string.h>

uint8_t my_encoder_val;
struct statusView {	
	View*(*init)(void);
};

extern const struct statusView StatusView;

