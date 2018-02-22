#pragma once

#pragma once
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "events.h"
#include <stdbool.h>
#include "DebouncingDispatcher.h"
#include "IRQDispatcher.h"

//adapted from https://github.com/brianlow/Rotary
// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Anti-clockwise step.
#define DIR_CCW 0x20

/*
 * The below state table has, for each state (row), the new state
 * to set based on the next encoder output. From left to right in,
 * the table, the encoder outputs are 00, 01, 10, 11, and the value
 * in that position is the new state to set.
 */

#define R_START 0x0
#define HALF_STEP 1

#ifdef HALF_STEP
// Use the half-step state table (emits a code at 00 and 11)
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
static const unsigned char ttable[6][4] = {
  // R_START (00)
	{ R_START_M, R_CW_BEGIN, R_CCW_BEGIN, R_START },
	// R_CCW_BEGIN
	{ R_START_M | DIR_CCW, R_START, R_CCW_BEGIN, R_START },
	// R_CW_BEGIN
	{ R_START_M | DIR_CW, R_CW_BEGIN, R_START, R_START },
	// R_START_M (11)
	{ R_START_M, R_CCW_BEGIN_M, R_CW_BEGIN_M, R_START },
	// R_CW_BEGIN_M
	{ R_START_M, R_START_M, R_CW_BEGIN_M, R_START | DIR_CW },
	// R_CCW_BEGIN_M
	{ R_START_M, R_CCW_BEGIN_M, R_START_M, R_START | DIR_CCW },
};
#else
// Use the full-step state table (emits a code at 00 only)
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

static const unsigned char ttable[7][4] = {
	// R_START
	{ R_START, R_CW_BEGIN, R_CCW_BEGIN, R_START },
	// R_CW_FINAL
	{ R_CW_NEXT, R_START, R_CW_FINAL, R_START | DIR_CW },
	// R_CW_BEGIN
	{ R_CW_NEXT, R_CW_BEGIN, R_START, R_START },
	// R_CW_NEXT
	{ R_CW_NEXT, R_CW_BEGIN, R_CW_FINAL, R_START },
	// R_CCW_BEGIN
	{ R_CCW_NEXT, R_START, R_CCW_BEGIN, R_START },
	// R_CCW_FINAL
	{ R_CCW_NEXT, R_CCW_FINAL, R_START, R_START | DIR_CCW },
	// R_CCW_NEXT
	{ R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START },
};
#endif


typedef enum {
	none,
	button,
	encoder		
} RotaryEncoderEventSource;

#define ENCODER_COUNT 1

typedef struct {
	GPIO_TypeDef* pin_a_bus;
	uint16_t pin_a_idx;
	GPIO_TypeDef* pin_b_bus;
	uint16_t pin_b_idx;
	GPIO_TypeDef* switch_bus;
	uint16_t switch_idx;

	unsigned short int direction;
	unsigned short int state;
	bool dirty;		
} RotaryEncoderConfig;

struct rotaryencoder {
	void(*configure)(RotaryEncoderConfig* config);			
};

static void handle_rotary_click(uint16_t gpio_pin, uint8_t state);
static void handle_rotary_spin(uint16_t gpio_pin, uint8_t state);

extern const struct rotaryencoder RotaryEncoder;

