#pragma once

#include "view.h"
#include <stdint.h>
#include "log.h"
#include "gfx.h"
#include <stm32f1xx_hal.h>



struct statusView {	
	View*(*init)(void);
};

extern const struct statusView StatusView;

