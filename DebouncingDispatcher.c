#include "DebouncingDispatcher.h"

#define COUNTER_TOP 10
#define NUM_BUTTONS 16
#define OLD_STATE 0
#define STATE 1

//todo: maybe rather than mapping 1:1 on GPIO... maybe just add GPIO pin to struct?
typedef struct {
	uint8_t bitmap;
	uint8_t counter;	
	void(*callback)(uint8_t state);
} Button;

static volatile Button buttons[NUM_BUTTONS];

void buttonDispatchCallback(void const * argument) {
	uint8_t i =0, old_state = 0, state=0;
	
	for (i = 0; i < NUM_BUTTONS; i++) {
		//Skip items that have been idle
		if (buttons[i].counter == 0)
		{
			continue;			
		}
		//If debounce period has elapsed
		if (--buttons[i].counter == 0)
		{
			state = bitRead(buttons[i].bitmap, STATE);
			old_state = bitRead(buttons[i].bitmap, OLD_STATE);

			if (state != old_state)
			{
				bitToggle(buttons[i].bitmap, OLD_STATE);				
				buttons[i].callback(state);				
			}
		}
	}
}

static bool subscribe(uint16_t GPIO_Pin, void* callback_in) {	
	uint8_t idx;
	if (IRQDispatcher.subscribe(GPIO_Pin, &signalStateChanged))
	{
		idx = pintoidx(GPIO_Pin);
		if (idx > 0 && idx < NUM_BUTTONS) {
			buttons[idx].callback = callback_in;
		}
		return true;
	}
	return false;
	
}
static bool unsubscribe(uint16_t GPIO_Pin) {
	uint8_t idx;
	if (IRQDispatcher.unsubscribe(GPIO_Pin)) {
		if (idx > 0 && idx < NUM_BUTTONS) {
			idx = pintoidx(GPIO_Pin);
			buttons[idx].callback = NULL;
			return true;
		}
	}
	return false;
}

static void signalStateChanged(uint16_t GPIO_Pin) {
	uint8_t idx = pintoidx(GPIO_Pin);
	if (idx > 0 && idx < NUM_BUTTONS) {
		buttons[idx].counter = COUNTER_TOP;
		bitToggle(buttons[idx].bitmap, STATE);
	}
}

const struct debounceddispatcher DebouncedDispatcher = { 
	.subscribe = subscribe,
	.unsubscribe = unsubscribe
};
