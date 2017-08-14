#pragma once

#include <stdint.h>
#include "events.h"
#include <stdbool.h>

typedef struct {
	void(*render)(void);
	void(*on_update)(T_SYSTEM_UPDATE*);
	bool dirty;
} View;
