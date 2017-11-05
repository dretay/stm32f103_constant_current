#include "application.h"

View* views[5];

void StartSysUpdateTask(void const * argument) {
	osEvent event;	
	T_SYSTEM_UPDATE *update;
	
	uint8_t mcp4725_config_cnt = 2;
	MCP4725_CONFIG mcp4725_configs[mcp4725_config_cnt];
	mcp4725_configs[0].addr = 0xC4;
	mcp4725_configs[0].p_i2c = &hi2c1;
	mcp4725_configs[1].addr = 0xC6;
	mcp4725_configs[1].p_i2c = &hi2c1;
		
	ADS1115_CHANNEL_CONFIG ads1115_channel_configs[2];
	ads1115_channel_configs[0].idx = 0;
	ads1115_channel_configs[0].ratio = 3.125;
	ads1115_channel_configs[0].parameter = CURRENT; //todo: is this really appropriate to encode here? even if it is probably should promote from internal enum
	ads1115_channel_configs[1].idx = 1;
	ads1115_channel_configs[1].ratio = 8.0645;
	ads1115_channel_configs[1].parameter = VOLTAGE;


	ADS1115_CONFIG ads1115_configs[1];
	ads1115_configs[0].addr = 0x90;
	ads1115_configs[0].p_i2c = &hi2c1;
	ads1115_configs[0].channel_cnt = 2;
	ads1115_configs[0].channel_configs = ads1115_channel_configs;
	

	//todo: maybe use an add fn rather than hard-coding all this nonsense?
	ROTARY_ENCODER_CONFIG encoder_configs[2];
	encoder_configs[0].switch_bus = GPIOB;
	encoder_configs[0].switch_idx = GPIO_PIN_3;
	encoder_configs[0].pin_a_bus = GPIOB;
	encoder_configs[0].pin_a_idx = GPIO_PIN_4;
	encoder_configs[0].pin_b_bus = GPIOB;
	encoder_configs[0].pin_b_idx = GPIO_PIN_5;	
	encoder_configs[0].parameter = CURRENT;

	encoder_configs[1].switch_bus = GPIOB;
	encoder_configs[1].switch_idx = GPIO_PIN_14;
	encoder_configs[1].pin_a_bus = GPIOB;
	encoder_configs[1].pin_a_idx = GPIO_PIN_12;
	encoder_configs[1].pin_b_bus = GPIOB;
	encoder_configs[1].pin_b_idx = GPIO_PIN_13;
	encoder_configs[1].parameter = VOLTAGE;

	gfxInit();
	gdispSetContrast(95);

	views[0] = StatusView.init();
		
	MCP4725.configure(mcp4725_configs, mcp4725_config_cnt);	
	ADS1115.configure(ads1115_configs, 1);
	ROTARY_ENCODER.configure(encoder_configs, 2);

	//dac 0 = 0.64v
	//dac 1 = 10v
	MCP4725.set_dac(0, 2000);
	MCP4725.set_dac(1, 2000);

	//todo maybe set this by default on the first view?
	views[0]->dirty = true;
	
	while (1) {
		event = osMailGet(SYS_UPDATE_MAILBOX_ID, osWaitForever);
		if (event.status == osEventMail) {
			update = event.value.p;		
			views[0]->on_update(update);	
		}				
		
		osMailFree(SYS_UPDATE_MAILBOX_ID, update);		

#ifdef INCLUDE_uxTaskGetStackHighWaterMark
		SysUpdateTask_Watermark = uxTaskGetStackHighWaterMark(NULL);

#endif

		osThreadYield();	
	}
}
void StartGUIDrawTask(void const * argument) {


	while (1)
	{
	//if (views[0]->dirty)
	if (true)
	{
		views[0]->dirty = false;
		views[0]->render();	
	}	
#ifdef INCLUDE_uxTaskGetStackHighWaterMark
		GUIDrawTask_Watermark = uxTaskGetStackHighWaterMark(NULL);
#endif
	osThreadYield();	
	
	}
}

static void showView(uint8_t idx) {
	views[idx]->render();
}

const struct application Application = { 
	.showView = showView
};