#pragma once

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "events.h"


// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Anti-clockwise step.
#define DIR_CCW 0x20




extern uint8_t encoder_counter;


void initialize(void);
uint8_t process(void);
void broadcast(void);
uint8_t state;

//void encoderCallback(void const * argument);


