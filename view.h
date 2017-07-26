#pragma once

#include <stdint.h>
#include "events.h"

typedef struct {
	void(*render)(void);
	void(*on_update)(T_STATE_UPDATE*);
} View;
