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

	ADS1115_CONFIG ads1115_configs[1];
	ads1115_configs[0].addr = 0x90;
	ads1115_configs[0].p_i2c = &hi2c1;

	ROTARY_ENCODER_CONFIG encoder_configs[1];
	encoder_configs[0].pin_a_bus = GPIOB;
	encoder_configs[0].pin_a_idx = GPIO_PIN_4;
	encoder_configs[0].pin_b_bus = GPIOB;
	encoder_configs[0].pin_b_idx = GPIO_PIN_5;

	gfxInit();
	views[0] = StatusView.init();
		
	MCP4725.configure(mcp4725_configs, mcp4725_config_cnt);	
	ADS1115.configure(ads1115_configs, 1);
	ROTARY_ENCODER.configure(encoder_configs, 1);

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
	if (views[0]->dirty)
	{
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