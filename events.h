#pragma once

#include <stdint.h>

typedef struct {
	enum { ENCODER, ADC } source;
	enum { VOLTAGE, CURRENT, OTHER } measurement;

	uint8_t idx;
	uint8_t val;

} T_SYSTEM_UPDATE;