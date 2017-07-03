#pragma once

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"


// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Anti-clockwise step.
#define DIR_CCW 0x20


typedef struct
{
	uint8_t idx;
	uint8_t val;

} T_ENCODER_READING;

extern uint8_t encoder_counter;


void initialize(void);
uint8_t process(void);
uint8_t state;

