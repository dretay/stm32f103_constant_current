#include "ToggleSwitch.h"

static TOGGLE_SWITCH_CONFIG config[TOGGLESWITCH_COUNT];
static uint8_t config_cnt = 0;

static void configure(TOGGLE_SWITCH_CONFIG* config_in, uint8_t cnt_in) {
	uint8_t i;
	config_cnt = cnt_in;

	for (i = 0; i < config_cnt; i++) {
		config[i].pin_bus = config_in[i].pin_bus;
		config[i].pin_idx = config_in[i].pin_idx;
		
		DebouncingDispatcher.subscribe(config[i].pin_idx, &handle);		
	}
}
static int8_t find_gpio_pin(uint16_t GPIO_Pin) {
	uint8_t i = 0;
	for (i = 0; i < config_cnt; i++) {		
		if (config[i].pin_idx == GPIO_Pin) {
			return i;
		}			
			
	} 
	return -1;
}
static bool idx_valid(int8_t idx) {
	return idx >= 0 && idx < config_cnt;
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

const struct toggle_switch TOGGLE_SWITCH = { 
	.configure = configure,
};