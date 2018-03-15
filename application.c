#include "application.h"

View* views[5];

void set_dac() {
	char* arg = SerialCommand.next();
	unsigned int value = atoi(arg);
	MCP4725.set_dac(0, value);
}
void set_contrast() {
	char* arg = SerialCommand.next();
	uint16_t value = atoi(arg);
	
	gdispSetContrast(value);
}
static void configure_usb_serial_commands() {
	SerialCommand.register_command(0, "dacset", &set_dac);
	SerialCommand.register_command(1, "contr", &set_contrast);
	SerialCommand.register_command(2, "dacsetup", CurrentSink.calibrate_setup);
	SerialCommand.register_command(3, "dacsave", CurrentSink.calibrate_save);
	SerialCommand.register_command(4, "dacfin", CurrentSink.calibrate_finish);
	SerialCommand.register_command(5, "dacdump", CurrentSink.calibrate_dump);
	SerialCommand.configure(UartSerialCommandAdapter.configure());
}
static void configure_current_sink() {
	CurrentSink.configure(0, &hi2c1, 0xC6);		
}
static void configure_rotary_encoders() {		
	RotaryEncoderConfig encoder_configs[1];
	encoder_configs[0].switch_bus = GPIOA;
	encoder_configs[0].switch_idx = GPIO_PIN_15;
	encoder_configs[0].pin_a_bus = GPIOB;
	encoder_configs[0].pin_a_idx = GPIO_PIN_3;
	encoder_configs[0].pin_b_bus = GPIOB;
	encoder_configs[0].pin_b_idx = GPIO_PIN_4;	
	

	RotaryEncoder.configure(encoder_configs);
}
static void configure_adc() {
	Adc.configure(&hadc1);
//	ADS1115_CHANNEL_CONFIG ads1115_channel_configs[1];
//	ads1115_channel_configs[0].idx = 0;
//	ads1115_channel_configs[0].ratio = 1;
//	ads1115_channel_configs[0].differential = false;	
//
//
//	ADS1115_CONFIG ads1115_configs[1];
//	ads1115_configs[0].addr = 0x90;
//	ads1115_configs[0].p_i2c = &hi2c1;
//	ads1115_configs[0].channel_cnt = 1;
//	ads1115_configs[0].channel_configs = ads1115_channel_configs;

	ADS1115.configure(&hi2c1, 0x90);
}
static void configure_graphics() {
	gfxInit();
	gdispSetContrast(40);
	views[0] = StatusView.init();
	
	//todo: maybe set this by default on the first view?
	views[0]->dirty = true;
	xTaskNotify(guiDrawTaskHandle, 0x01, eSetBits);
}
static void configure_toggle_switches() {
	ToggleSwitchConfig switch_configs[1];
	switch_configs[0].pin_bus = GPIOB;
	switch_configs[0].pin_idx = GPIO_PIN_9;

	ToggleSwitch.configure(switch_configs);
}
static void configure_flash() {
	Flash.configure(false);
}


void StartSysUpdateTask(void const * argument) {
	osEvent event;	
	T_SYSTEM_UPDATE *update;
	configure_usb_serial_commands();
	configure_current_sink();
	configure_adc();
	configure_rotary_encoders();
	configure_toggle_switches();
	configure_graphics();
	
	configure_flash();
	
	

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
			views[0]->dirty = false;
			views[0]->render();	
		}	
#ifdef INCLUDE_uxTaskGetStackHighWaterMark
		GUIDrawTask_Watermark = uxTaskGetStackHighWaterMark(NULL);
#endif
		vTaskDelay(200);
	}
}

static void showView(uint8_t idx) {
	views[idx]->render();
}

const struct application Application = { 
	.showView = showView
};


