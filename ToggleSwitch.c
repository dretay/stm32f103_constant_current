#include "ToggleSwitch.h"

static ToggleSwitchConfig config[TOGGLESWITCH_COUNT];

static void configure(ToggleSwitchConfig* config_in) {
	uint8_t i;

	for (i = 0; i < TOGGLESWITCH_COUNT; i++) {
		config[i].pin_bus = config_in[i].pin_bus;
		config[i].pin_idx = config_in[i].pin_idx;
		
		DebouncingDispatcher.subscribe(config_in[i].pin_bus, config[i].pin_idx, &handle);	
		handle(config[i].pin_idx, HAL_GPIO_ReadPin(config_in[i].pin_bus, config[i].pin_idx));	
	}
}
static int8_t find_gpio_pin(uint16_t GPIO_Pin) {
	uint8_t i = 0;
	for (i = 0; i < TOGGLESWITCH_COUNT; i++) {		
		if (config[i].pin_idx == GPIO_Pin) {
			return i;
		}			
			
	} 
	return -1;
}
static bool idx_valid(int8_t idx) {
	return idx >= 0 && idx < TOGGLESWITCH_COUNT;
}
static void handle(uint16_t gpio_pin, uint8_t state) {
	uint8_t idx;
	T_SYSTEM_UPDATE *update;

	idx = find_gpio_pin(gpio_pin);
	if (idx_valid(idx)) {
		update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, 0); 
		if (update != NULL) {				
			update->idx = idx;
			update->bool_val = state ? true : false;
			update->val_type = type_bool;
			update->source = TOGGLE_SWITCH_EVENT;
			osMailPut(SYS_UPDATE_MAILBOX_ID, update);
		}	
	}	
}

const struct toggleswitch ToggleSwitch = { 
	.configure = configure,
};