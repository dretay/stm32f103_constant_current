#include "encoder.h"

/*
 * The below state table has, for each state (row), the new state
 * to set based on the next encoder output. From left to right in,
 * the table, the encoder outputs are 00, 01, 10, 11, and the value
 * in that position is the new state to set.
 */

#define R_START 0x0

#ifdef HALF_STEP
// Use the half-step state table (emits a code at 00 and 11)
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char ttable[6][4] = {
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

const unsigned char ttable[7][4] = {
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

uint8_t encoder_counter = 0;
uint8_t old_encoder_counter = 0;

void initialize(void) {
	state = R_START;
}
uint8_t process(void) {
  // Grab state of input pins.
	uint8_t pinstate = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) << 1) | (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5));	
	// Determine new state from the pins and state table.
	state = ttable[state & 0xf][pinstate];
	// Return emit bits, ie the generated event.
	return state & 0x30;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	unsigned char result = process();
	if (result == DIR_CW) {		
		encoder_counter++;
	}
	else if (result == DIR_CCW) {
		encoder_counter--;		
	}  
}
void encoderCallback(void const * argument) {
	T_SYSTEM_UPDATE *update;
	if (old_encoder_counter != encoder_counter)
	{
		old_encoder_counter = encoder_counter;
		update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, osWaitForever); /* Allocate memory */
		update->idx = 0;
		update->val = encoder_counter;	
		update->source = ENCODER;
		update->parameter = VOLTAGE;
		osMailPut(SYS_UPDATE_MAILBOX_ID, update);
	}
}