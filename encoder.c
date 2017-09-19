#include "encoder.h"

static ROTARY_ENCODER_CONFIG* config;
static uint8_t config_cnt = 0;

void configure(ROTARY_ENCODER_CONFIG* config_in, uint8_t cnt_in) {
	uint8_t i;
	config_cnt = cnt_in;
	config = config_in;

	for (i = 0; i < config_cnt; i++)
	{
		config[i].state = R_START;
		config[i].button_dirty = false;
		config[i].encoder_dirty = false;
		config[i].counter = 0;
	}
}

void handle_rotary_spin(uint8_t idx) {
	static unsigned char result;
	static uint8_t pinstate;

	// Grab state of input pins.
	pinstate = (HAL_GPIO_ReadPin(config[idx].pin_a_bus, config[idx].pin_a_idx) << 1) | (HAL_GPIO_ReadPin(config[idx].pin_b_bus, config[idx].pin_b_idx));	

	// Determine new state from the pins and state table.
	config[idx].state = ttable[config[idx].state & 0xf][pinstate];
	result = config[idx].state & 0x30;


	if (result == DIR_CW) {		
		config[idx].counter++;
		config[idx].encoder_dirty = true;
	}
	else if (result == DIR_CCW && config[idx].counter > 0) {
		config[idx].counter--;	
		config[idx].encoder_dirty = true;
	} 
}
void handle_rotary_click(uint8_t idx) {
	static uint8_t pinstate;
	pinstate = HAL_GPIO_ReadPin(config[idx].switch_bus, config[idx].switch_idx);
	if (pinstate == GPIO_PIN_SET) {
		config[idx].button_state = true;	
		config[idx].button_dirty = true;
	}
	else {
		config[idx].button_state = false;	
		config[idx].button_dirty = true;
	}
}

//todo: probably need something managed in the application that can dispatch to this since I eat all irqs...
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	uint8_t i = 0;
	
	//todo: there's a more efficient way to do this but I'm lazy tonight
	for (i = 0; i < config_cnt; i++)
	{
		if (GPIO_Pin == config[i].pin_a_idx || GPIO_Pin == config[i].pin_b_idx)
		{
			handle_rotary_spin(i);
		}
		else if (GPIO_Pin == config[i].switch_idx)
		{
			handle_rotary_click(i);
		}
	} 
}

//debounce encoder switch and send updates when appropriate
void encoderCallback(void const * argument) {
	T_SYSTEM_UPDATE *update;
	uint8_t i = 0;
	for (i = 0; i < config_cnt; i++)
	{
		if (config[i].encoder_dirty)
		{
			config[i].encoder_dirty = false;

			update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, osWaitForever); /* Allocate memory */
			update->idx = i;
			update->int_val = config[i].counter;	
			update->val_type = type_int;
			update->source = ENCODER;
			update->parameter = config[i].parameter;
			osMailPut(SYS_UPDATE_MAILBOX_ID, update);
		}
		if (config[i].button_dirty) {
			config[i].button_dirty = false;

			update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, osWaitForever); /* Allocate memory */
			update->idx = i;
			update->bool_val = config[i].button_state;	
			update->val_type = type_bool;
			update->source = ENCODER;
			update->parameter = OTHER;
			osMailPut(SYS_UPDATE_MAILBOX_ID, update);
		}
	}	
}


const struct rotary_encoder ROTARY_ENCODER = { 
	.configure = configure,
};
