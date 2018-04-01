#include "application.h"



void set_dac() {
	char* arg = SerialCommand.next();
	unsigned int value = atoi(arg);
	MCP4725.set_dac(0, value);
}
void get_dac() {
	char* arg = SerialCommand.next();
	unsigned int idx = atoi(arg);
	int value = MCP4725.get_dac(idx);
	char textbuf[128];	
	sprintf(textbuf, "%d\n\r",value);
	SerialCommand.echo(textbuf, strlen(textbuf));
}
void set_contrast() {
	char* arg = SerialCommand.next();
	int value = atoi(arg);	
	gdispSetContrast(value);	
}
void get_contrast() {
	char textbuf[128];	
	int value = gdispGetContrast();	
	sprintf(textbuf, "%d\n\r", value);
	SerialCommand.echo(textbuf, strlen(textbuf));
}
void set_current() {
	char* arg = SerialCommand.next(); 	
	
	//todo this is bad bad bad
	arg[strcspn(arg, "\r\n")] = '\0';
	
	float val = atof(arg);
	CurrentSink.set(val);
}
static void configure_usb_serial_commands() {
	SerialCommand.register_command(0, "setdac", &set_dac);
	SerialCommand.register_command(1, "getdac", &get_dac);
	SerialCommand.register_command(2, "setcont", &set_contrast);
	SerialCommand.register_command(3, "getcont", &get_contrast);
	SerialCommand.register_command(4, "getcurr", CurrentSink.print_setting);
	SerialCommand.register_command(5, "setcurr", set_current);
	SerialCommand.register_command(6, "readcurr", CurrentSink.print_current_reading);
	SerialCommand.register_command(7, "calconst", CurrentSink.save_cal_const);
	SerialCommand.register_command(8, "getcal", CurrentSink.print_cal_const);


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
	GUI.configure();
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

static void configure() {
	configure_usb_serial_commands();
	configure_current_sink();
	configure_adc();
	configure_rotary_encoders();
	configure_toggle_switches();
	configure_graphics();	
	configure_flash();

}

const struct application Application = { 
	.configure = configure
};


