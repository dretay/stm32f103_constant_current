#include "CurrentSink.h"

static unsigned int DAC_IDX;
static float current_setting;
static float voltage_reading = 0.0;
static float current_reading = 0.0;
static float wattage_reading = 0.0;
bool status = false;
int VCP_write(const void *pBuffer, int size);



static void configure(uint8_t idx_in, I2C_HandleTypeDef* p_i2c_in, uint8_t addr_in) {
	DAC_IDX = idx_in;

	MCP4725.add_dac(DAC_IDX, p_i2c_in, addr_in);	
	MCP4725.set_dac(DAC_IDX, 0);

}

//this site is magic: http://www.xuru.org/rt/NLR.asp
//todo these should be stored as constants?
const static float current_multiplier = (4095.0f / (5.0f));
static void set(float setting) {
	double cal_const_0 = Flash.get(CURRENT_SINK_COEFFICIENT_0, type_double).double_val;
	double cal_const_1 = Flash.get(CURRENT_SINK_COEFFICIENT_1, type_double).double_val;
	double cal_const_2 = Flash.get(CURRENT_SINK_COEFFICIENT_2, type_double).double_val;
	
	current_setting = setting;
	unsigned int converted_setting = 
	(cal_const_0 * setting) + 
	(cal_const_1 * setting) /
	(setting + cal_const_2);

	MCP4725.set_dac(DAC_IDX, MIN(4095, floor(converted_setting)));

}


static void save_cal_const() {
	
	int idx = SerialCommand.next_int();
	double value = SerialCommand.next_double();
	
	if (idx >= 0 && value > 0.0)
	{
		Flash.set_double(idx, value);
	}	
}
static void print_cal_const() {
	
	int idx = SerialCommand.next_int();
	FLASH_RECORD record = Flash.get(idx, type_double);	
	char textbuf[32];	
	sprintf(textbuf, "%0.13f\n\r", record.double_val);
	SerialCommand.echo(textbuf, strlen(textbuf));
		
}
static void calibrate_dump() {
	char textbuf[128];
	unsigned int i=0;
	for(; i < 8; i++)
	{
//		sprintf(textbuf, "%d: %d %d\n\r",i, (int)(yData[i]*100), (int)(xData[i]*100));
//		VCP_write(textbuf, strlen(textbuf));
	}
	
}

static void print_setting(void) {
	char textbuf[32];	
	sprintf(textbuf, "%.2f\n\r", current_setting);
	SerialCommand.echo(textbuf, strlen(textbuf));
}
static void print_voltage_reading(void) {
	char textbuf[32];	
	sprintf(textbuf, "%.2f\n\r", voltage_reading);
	SerialCommand.echo(textbuf, strlen(textbuf));
}
static void print_current_reading(void) {
	char textbuf[32];	
	sprintf(textbuf, "%.2f\n\r", current_reading);
	SerialCommand.echo(textbuf, strlen(textbuf));
}
static void print_power_reading(void) {
	char textbuf[32];	
	sprintf(textbuf, "%.2f\n\r", wattage_reading);
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

	while (1) {
		event = osMailGet(SYS_UPDATE_MAILBOX_ID, osWaitForever);
		update = event.value.p;
		switch (update->source) {
		
		case ADC_READ_EVENT:
			switch (update->idx) {
			case 0:
				voltage_reading = update->int_val / 100.0f;
				wattage_reading = voltage_reading * current_reading;				
				break;
			case 1:		
				current_reading = abs(update->int_val - 2296) / 150.0f;
				wattage_reading = voltage_reading * current_reading;				
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
	.save_cal_const = save_cal_const,
	.print_setting = print_setting,
	.get_current_setting = get_current_setting,
	.get_enable_setting = get_enable_setting,
	.print_voltage_reading = print_voltage_reading,
	.print_current_reading = print_current_reading,
	.print_power_reading = print_power_reading,
	.get_voltage_reading = get_voltage_reading,
	.get_current_reading = get_current_reading,
	.get_power_reading = get_power_reading,
	.print_cal_const = print_cal_const
};
