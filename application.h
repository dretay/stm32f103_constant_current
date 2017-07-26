#pragma once
#include <stdint.h>
#include "cmsis_os.h"
#include "log.h"
#include "StatusView.h"
#include "gfx.h"
#include "main.h"

struct application
{
	void(*showView)(uint8_t idx);
};

extern const struct application Application;
