#include "application.h"

View* views[5];

void setdat() {
	LOG("POOP!");
}
static void configure_usb_serial_commands() {
	UsbSerialCommand_Command serialcommands[1];
	serialcommands[0].command = "dac";
	serialcommands[0].function = &setdat;
	
	UsbSerialCommand_Config serialcommand_config;
	serialcommand_config.command_cnt = 1;
	serialcommand_config.commands = serialcommands;

	UsbSerialCommand.configure(&serialcommand_config, 1);
}
static void configure_dac() {
	uint8_t mcp4725_config_cnt = 2;
	MCP4725_CONFIG mcp4725_configs[mcp4725_config_cnt];
	mcp4725_configs[1].addr = 0xC4;
	mcp4725_configs[1].p_i2c = &hi2c2;
	mcp4725_configs[0].addr = 0xC6;
	mcp4725_configs[0].p_i2c = &hi2c2;
	MCP4725.configure(mcp4725_configs, mcp4725_config_cnt);	

	//dac 0 = 0.64v
	//dac 1 = 10v
//	MCP4725.set_dac(0, 2000);
//	MCP4725.set_dac(1, 2000);
}
static void configure_rotary_encoders() {
	//todo: maybe use an add fn rather than hard-coding all this nonsense?
	ROTARY_ENCODER_CONFIG encoder_configs[2];
	encoder_configs[0].switch_bus = GPIOA;
	encoder_configs[0].switch_idx = GPIO_PIN_0;
	encoder_configs[0].pin_a_bus = GPIOA;
	encoder_configs[0].pin_a_idx = GPIO_PIN_2;
	encoder_configs[0].pin_b_bus = GPIOA;
	encoder_configs[0].pin_b_idx = GPIO_PIN_1;	
	encoder_configs[0].parameter = CURRENT;

	encoder_configs[1].switch_bus = GPIOC;
	encoder_configs[1].switch_idx = GPIO_PIN_13;
	encoder_configs[1].pin_a_bus = GPIOC;
	encoder_configs[1].pin_a_idx = GPIO_PIN_14;
	encoder_configs[1].pin_b_bus = GPIOC;
	encoder_configs[1].pin_b_idx = GPIO_PIN_15;
	encoder_configs[1].parameter = VOLTAGE;
	ROTARY_ENCODER.configure(encoder_configs, 2);
}
static void configure_graphics() {
	gfxInit();
	gdispSetContrast(90);
	views[0] = StatusView.init();
	
	//todo: maybe set this by default on the first view?
	views[0]->dirty = true;
	xTaskNotify(guiDrawTaskHandle, 0x01, eSetBits);
}


void StartSysUpdateTask(void const * argument) {
	osEvent event;	
	T_SYSTEM_UPDATE *update;
	
	configure_usb_serial_commands();
	configure_dac();
	configure_rotary_encoders();

	while (1) {
		event = osMailGet(SYS_UPDATE_MAILBOX_ID, osWaitForever);
		if (event.status == osEventMail) {
			update = event.value.p;		
			views[0]->on_update(update);	
		}	
		xTaskNotify(guiDrawTaskHandle, 0x01, eSetBits);
		
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
		// Don't clear bits on entry., Clear all bits on exit., Stores the notified value.
		//todo: maybe we should notify on the view we want to be rendered?
		xTaskNotifyWait(pdFALSE, ULONG_MAX, NULL, osWaitForever);
		
		if (views[0]->dirty)
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






