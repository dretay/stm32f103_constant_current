#pragma once
#include <stdint.h>
#include "cmsis_os.h"
#include "log.h"
#include "StatusView.h"

struct application
{
	void(*init)(void);
	void(*showView)(uint8_t idx);
};

extern const struct application Application;
