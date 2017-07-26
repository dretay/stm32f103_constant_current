#include "mcp4725.h"

void set_dac(uint8_t value) {
	//dac
	I2C_HandleTypeDef* hi2c1 = (I2C_HandleTypeDef*)get_hi2c1();

	uint8_t output[] = {0x40, (value / 16), ((value % 16) << 4) };
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c1, 0xC4, output, 3, 4);
	if (status != HAL_OK) {
		LOG("OOPS");
	}
}

void StartDacTask(void const * argument) {
	osEvent event;	
	T_ENCODER_READING *reading;
	T_STATE_UPDATE *update;


	while (1) {
		event = osMailGet(ENDCODER_MAILBOX_ID, osWaitForever);
		if (event.status == osEventMail)
		{
			reading = event.value.p;
			set_dac(reading->val);	//maybe this should return voltage / current as float?
						
			update = osMailAlloc(SYSUPDATE_MAILBOX_ID, osWaitForever); 
			update->idx = reading->idx;
			update->measurement = CURRENT;
			update->type = SETTING;
			update->val = reading->val;
			osMailPut(SYSUPDATE_MAILBOX_ID, update);
			
					
		}				
		osMailFree(ENDCODER_MAILBOX_ID, reading);		
	}
}
