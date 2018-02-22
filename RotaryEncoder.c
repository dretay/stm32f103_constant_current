#include "RotaryEncoder.h"


static RotaryEncoderConfig config[ENCODER_COUNT];

static void configure(RotaryEncoderConfig* config_in) {
	uint8_t i;

	for (i = 0; i < ENCODER_COUNT; i++) {
		config[i].switch_bus = config_in[i].switch_bus;
		config[i].switch_idx = config_in[i].switch_idx;
		config[i].pin_a_bus = config_in[i].pin_a_bus;
		config[i].pin_a_idx = config_in[i].pin_a_idx;
		config[i].pin_b_bus = config_in[i].pin_b_bus;
		config[i].pin_b_idx = config_in[i].pin_b_idx;	
		
		config[i].state = R_START;	

		DebouncingDispatcher.subscribe(config_in[i].switch_bus, config[i].switch_idx, &handle_rotary_click);
		IRQDispatcher.subscribe(config[i].pin_a_idx, &handle_rotary_spin);		
		IRQDispatcher.subscribe(config[i].pin_b_idx, &handle_rotary_spin);		

		config[i].dirty = false;
	}
}

static void publish_encoder_change(uint8_t idx, bool val, RotaryEncoderEventSource source) {
	
	T_SYSTEM_UPDATE *update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, 0); 
	if (update != NULL) {				
		update->idx = idx;
		if (source == encoder) {
			update->int_val = val ? -1 : 1;	
			update->val_type = type_int;
			update->source = ENCODER_SPIN_EVENT;
			osMailPut(SYS_UPDATE_MAILBOX_ID, update);
		}
		else {
			update->idx = idx;
			update->bool_val = val;	
			update->val_type = type_bool;
			update->source = ENCODER_BUTTON_EVENT;
			osMailPut(SYS_UPDATE_MAILBOX_ID, update);			
		}
	}			
}
static int8_t find_gpio_pin(uint16_t GPIO_Pin) {
	uint8_t i = 0;
	for (i = 0; i < ENCODER_COUNT; i++) {		
		if (config[i].pin_a_idx == GPIO_Pin || config[i].pin_b_idx == GPIO_Pin || config[i].switch_idx == GPIO_Pin) {
			return i;
		}						
	} 
	return -1;
}
static bool idx_valid(int8_t idx) {
	return idx >= 0 && idx < ENCODER_COUNT;
}
static void handle_rotary_spin(uint16_t gpio_pin, uint8_t state) {
	uint8_t idx;
	GPIO_PinState pinstate, pinstate_a, pinstate_b;
	unsigned char result;

	idx = find_gpio_pin(gpio_pin);
	if (idx_valid(idx)) {
		pinstate_a = HAL_GPIO_ReadPin(config[idx].pin_a_bus, config[idx].pin_a_idx);		
		pinstate_b = HAL_GPIO_ReadPin(config[idx].pin_b_bus, config[idx].pin_b_idx);		
		pinstate = (pinstate_b << 1) |  (pinstate_a);
		config[idx].state = ttable[config[idx].state & 0xf][pinstate];
		result = config[idx].state & 0x30;
		if (result != DIR_NONE) {
			config[idx].dirty = true;
			config[idx].direction = result;			
		}
	}
}

static void handle_rotary_click(uint16_t gpio_pin, uint8_t state) {
	uint8_t idx;
	GPIO_PinState pinstate;

	idx = find_gpio_pin(gpio_pin);
	if (idx_valid(idx)) {
		publish_encoder_change(idx, state, button);	
	}	
}

//debounce encoder switch and send updates when appropriate
void encoder_timer_callback(void const * argument) {		
	uint8_t i = 0;

	for (i = 0; i < ENCODER_COUNT; i++) {
		if (config[i].dirty) {
			config[i].dirty = false;
			if (config[i].direction == DIR_CW) {
				publish_encoder_change(i, true, encoder);
			}
			else if (config[i].direction == DIR_CCW) {
				publish_encoder_change(i, false, encoder);
			}			
		}		
	}	
}

const struct rotaryencoder RotaryEncoder = { 
	.configure = configure,
};