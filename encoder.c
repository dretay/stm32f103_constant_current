#include "encoder.h"

ROTARY_ENCODER_CONFIG* config;
uint8_t config_cnt = 0;

void configure(ROTARY_ENCODER_CONFIG* config_in, uint8_t cnt_in) {
	uint8_t i;
	config_cnt = cnt_in;
	config = config_in;

	for (i = 0; i < config_cnt; i++)
	{
		config[i].state = R_START;
		config[i].dirty = false;
		config[i].counter = 0;
	}
}

//todo: probably need the other external callback when using more perhiperals
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	uint8_t i = 0;
	unsigned char result;
	uint8_t pinstate;

	//todo: there's a more efficient way to do this but I'm lazy tonight
	for (i = 0; i < config_cnt; i++)
	{
		if (GPIO_Pin == config[i].pin_a_idx || GPIO_Pin == config[i].pin_b_idx) {
			break;
		}

	}

	// Grab state of input pins.
	pinstate = (HAL_GPIO_ReadPin(config[i].pin_a_bus, config[i].pin_a_idx) << 1) | (HAL_GPIO_ReadPin(config[i].pin_b_bus, config[i].pin_b_idx));	

	// Determine new state from the pins and state table.
	config[i].state = ttable[config[i].state & 0xf][pinstate];
	result = config[i].state & 0x30;


	if (result == DIR_CW) {		
		config[i].counter++;
		config[i].dirty = true;
	}
	else if (result == DIR_CCW) {
		config[i].counter--;	
		config[i].dirty = true;
	}  
}
void encoderCallback(void const * argument) {
	T_SYSTEM_UPDATE *update;
	uint8_t i = 0;
	for (i = 0; i < config_cnt; i++)
	{
		if (config[i].dirty)
		{
			config[i].dirty = false;

			update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, osWaitForever); /* Allocate memory */
			update->idx = i;
			update->val = config[i].counter;	
			update->source = ENCODER;
			update->parameter = VOLTAGE;
			osMailPut(SYS_UPDATE_MAILBOX_ID, update);
		}
	}	
}


const struct rotary_encoder ROTARY_ENCODER = { 
	.configure = configure,
};
