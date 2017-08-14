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

	gfxInit();
	views[0] = StatusView.init();

	
	MCP4725.configure(mcp4725_configs, 1);
	ADS1115.configure(ads1115_configs, 1);

	
	while (1) {
		event = osMailGet(SYS_UPDATE_MAILBOX_ID, osWaitForever);
		if (event.status == osEventMail) {
			update = event.value.p;		
			views[0]->on_update(update);	
		}				
		views[0]->render();
		osMailFree(SYS_UPDATE_MAILBOX_ID, update);		

#ifdef INCLUDE_uxTaskGetStackHighWaterMark
		uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
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