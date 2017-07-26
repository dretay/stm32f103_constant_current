#pragma once

#include <stdint.h>


typedef struct {
	enum { SETTING, READING } type;
	enum { VOLTAGE, CURRENT } measurement;
	uint8_t idx;
	uint8_t val;
} T_STATE_UPDATE;

typedef struct {
	uint8_t idx;
	uint8_t val;

} T_ENCODER_READING;