#include "application.h"

View* views[5];

void setdac() {
	char* arg = UsbSerialCommand.next();
	uint16_t value = atoi(arg);
	MCP4725.set_dac(0, value);
}
static void configure_usb_serial_commands() {
	UsbSerialCommand.register_command(0, "dac", &setdac);
	UsbSerialCommand.configure();
}
static void configure_dac() {
	MCP4725.add_dac(0, &hi2c1, 0xC6);	
}
static void configure_rotary_encoders() {		
	ROTARY_ENCODER_CONFIG encoder_configs[1];
	encoder_configs[0].switch_bus = GPIOA;
	encoder_configs[0].switch_idx = GPIO_PIN_15;
	encoder_configs[0].pin_a_bus = GPIOB;
	encoder_configs[0].pin_a_idx = GPIO_PIN_3;
	encoder_configs[0].pin_b_bus = GPIOB;
	encoder_configs[0].pin_b_idx = GPIO_PIN_4;	
	

	ROTARY_ENCODER.configure(encoder_configs, 1);
}
static void configure_adc() {
	ADC.configure(&hadc1);
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
	configure_adc();
	configure_rotary_encoders();
	configure_graphics();

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






