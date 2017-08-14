#include "application.h"

View* views[5];

void StartSysUpdateTask(void const * argument) {
	osEvent event;	
	T_SYSTEM_UPDATE *update;

#ifdef INCLUDE_uxTaskGetStackHighWaterMark
	UBaseType_t uxHighWaterMark;
#endif
	
	MCP4725_CONFIG mcp4725_configs[1];
	mcp4725_configs[0].addr = 0xC4;
	mcp4725_configs[0].p_i2c = &hi2c1;

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
		
	MCP4725.configure(mcp4725_configs, 1);
	ADS1115.configure(ads1115_configs, 1);
	ROTARY_ENCODER.configure(encoder_configs, 1);

	
	while (1) {
		event = osMailGet(SYS_UPDATE_MAILBOX_ID, osWaitForever);
		if (event.status == osEventMail) {
			update = event.value.p;		
			views[0]->on_update(update);	
		}				
		
		osMailFree(SYS_UPDATE_MAILBOX_ID, update);		

#ifdef INCLUDE_uxTaskGetStackHighWaterMark
		uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
		LOG("App high water: %d\n", uxHighWaterMark);
#endif

		osThreadYield();	
	}
}
void StartGUIDrawTask(void const * argument) {

#ifdef INCLUDE_uxTaskGetStackHighWaterMark
	UBaseType_t uxHighWaterMark;
#endif
	while (1)
	{
	if (views[0]->dirty)
	{
		views[0]->render();	
	}	
#ifdef INCLUDE_uxTaskGetStackHighWaterMark
		HAL_Delay(500);
		uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
		LOG("GUI high water: %d\n", uxHighWaterMark);
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