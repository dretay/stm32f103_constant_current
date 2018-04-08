#include "CurrentSink.h"

static unsigned int DAC_IDX;
static int raw_current_reading = 0;
static int raw_voltage_reading = 0;
static float current_setting = 0;
static float voltage_reading = 0.0;
static float current_reading = 0.0;
static float wattage_reading = 0.0;
bool status = false;


static void configure(uint8_t idx_in, I2C_HandleTypeDef* p_i2c_in, uint8_t addr_in) {
	DAC_IDX = idx_in;

	MCP4725.add_dac(DAC_IDX, p_i2c_in, addr_in);	
	MCP4725.set_dac(DAC_IDX, 0);

}

//this site is magic: http://www.xuru.org/rt/NLR.asp
const static float current_multiplier = (4095.0f / (5.0f));
static void set(float setting) {
	double cal_const_0 = Flash.get(CURRENT_SINK_COEFFICIENT_0, type_double).double_val;
	double cal_const_1 = Flash.get(CURRENT_SINK_COEFFICIENT_1, type_double).double_val;
	double cal_const_2 = Flash.get(CURRENT_SINK_COEFFICIENT_2, type_double).double_val;
	
	current_setting = setting;
	unsigned int converted_setting = 
	(593.5650713 * setting) + 
	(382.4129242*pow(setting,0.5)) + 
	( 67.33226485);

	MCP4725.set_dac(DAC_IDX, MIN(4095, floor(converted_setting)));

}

static void print_setting(void) {
	char textbuf[32];	
	sprintf(textbuf, "%.2f\n\r", current_setting);
	osDelay(100);
	SerialCommand.echo(textbuf, strlen(textbuf));
}
static void print_voltage_reading(void) {
	char textbuf[32];	
	sprintf(textbuf, "%.2f\n\r", voltage_reading);
	osDelay(100);
	SerialCommand.echo(textbuf, strlen(textbuf));
}
static void print_current_reading(void) {
	char textbuf[32];	
	sprintf(textbuf, "%.2f\n\r", current_reading);
	osDelay(100);
	SerialCommand.echo(textbuf, strlen(textbuf));
}
static void print_power_reading(void) {
	char textbuf[32];	
	sprintf(textbuf, "%.2f\n\r", wattage_reading);
	osDelay(100);
	SerialCommand.echo(textbuf, strlen(textbuf));
}
static void print_raw_current(void) {
	char textbuf[32];	
	sprintf(textbuf, "%d\n\r", raw_current_reading);
	osDelay(100);
	SerialCommand.echo(textbuf, strlen(textbuf));
}
static void print_raw_voltage(void) {
	char textbuf[32];	
	sprintf(textbuf, "%d\n\r", raw_voltage_reading);
	osDelay(100);
	SerialCommand.echo(textbuf, strlen(textbuf));
}
static float get_voltage_reading(void) {
	return voltage_reading;	
}
static float get_current_reading(void) {
	return current_reading;	
}
static float get_power_reading(void) {
	return wattage_reading;	
}
static float get_current_setting(void) {
	return current_setting;	
}
static bool get_enable_setting(void) {
	return status;	
}


void StartSysUpdateTask(void const * argument) {
	osEvent event;	
	T_SYSTEM_UPDATE *update;
	float temp_reading;
	while (1) {
		event = osMailGet(SYS_UPDATE_MAILBOX_ID, osWaitForever);
		update = event.value.p;
		switch (update->source) {
		
		//todo: make these constants in eeprom
		case ADC_READ_EVENT:
			switch (update->idx) {
			case 0:
				raw_voltage_reading = update->int_val;				
				temp_reading = 
					(0.009379665312 * raw_voltage_reading) +
					(0.02846160893 * raw_voltage_reading) /
					(raw_voltage_reading - 0.7092668529);
				if (temp_reading > 0)
				{
					voltage_reading = temp_reading;
					wattage_reading = voltage_reading * current_reading;				
					
				}
				break;
			case 1:		
				raw_current_reading = update->int_val;				
				temp_reading = 
					(-0.000001318965458 * pow(raw_current_reading, 2)) -
					(0.1934410638 * pow(raw_current_reading,0.5)) +
					(16.6824594);
				if (temp_reading > 0) 
				{
					current_reading = temp_reading;
					wattage_reading = voltage_reading * current_reading;				
				}				
				break;
			}
			break;
		case TOGGLE_SWITCH_EVENT:
			if (update->bool_val) {
				status = true;
			}
			else {
				status = false;
			}
			break;		
		}		
		osMailFree(SYS_UPDATE_MAILBOX_ID, update);		

#ifdef INCLUDE_uxTaskGetStackHighWaterMark
		SysUpdateTask_Watermark = uxTaskGetStackHighWaterMark(NULL);
#endif
		osThreadYield();	
	}
}

const struct currentsink CurrentSink= { 
	.configure = configure,
	.set = set,
	.print_setting = print_setting,
	.get_current_setting = get_current_setting,
	.get_enable_setting = get_enable_setting,
	.print_voltage_reading = print_voltage_reading,
	.print_current_reading = print_current_reading,
	.print_power_reading = print_power_reading,
	.get_voltage_reading = get_voltage_reading,
	.get_current_reading = get_current_reading,
	.get_power_reading = get_power_reading,
	.print_raw_current = print_raw_current,
	.print_raw_voltage = print_raw_voltage
};
