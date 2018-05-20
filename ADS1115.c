#include "ADS1115.h"

/*Private variables*/
static I2C_HandleTypeDef *hi2c;
static uint8_t addr;
static int  ADS1115_SSPS;
static int  ADS1115_GAIN;
static uint16_t adc_channels[4] = { ADS1X15_REG_CONFIG_MUX_SINGLE_0, ADS1X15_REG_CONFIG_MUX_SINGLE_1, ADS1X15_REG_CONFIG_MUX_SINGLE_2, ADS1X15_REG_CONFIG_MUX_SINGLE_3 };
static int num_channels;

/* Private methods*/

//do conversion of data to send to device
static void marshal(int idx, int reg, uint16_t value) {
	
	uint8_t tx_buffer[3] = { (uint8_t)reg, (uint8_t)(value >> 8), (uint8_t)(value & 0xFF) };
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c, addr, tx_buffer, 3, 4);
	
	if (status != HAL_OK) {
		
	}
}

//do conversion from device response
static uint16_t unmarshal(int idx, int reg) {
	
	uint8_t tx_buffer[1] = { ADS1X15_REG_POINTER_CONVERT };
	uint8_t rx_buffer[2] = { 0, 0 };

	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c, addr, tx_buffer, 1, 4);
	if (status != HAL_OK) {
		
	}
	HAL_I2C_Master_Receive(hi2c, addr + 1, rx_buffer, 2, 4);

	return ((rx_buffer[0] << 8) | rx_buffer[1]);  

}
//what to multiply reading by to get the volts
static float volts_per_bit() {
	float v = 0;	
	switch (ADS1115_GAIN) {
	case (GAIN_TWOTHIRDS):
		v = ADS1115_VOLTS_PER_BIT_GAIN_TWOTHIRDS;
		break;
	case (GAIN_ONE):
		v = ADS1115_VOLTS_PER_BIT_GAIN_ONE;
		break;
	case (GAIN_TWO):
		v = ADS1115_VOLTS_PER_BIT_GAIN_TWO;
		break;
	case (GAIN_FOUR):
		v = ADS1115_VOLTS_PER_BIT_GAIN_FOUR;
		break;
	case (GAIN_EIGHT):
		v = ADS1115_VOLTS_PER_BIT_GAIN_EIGHT;
		break;
	case (GAIN_SIXTEEN):
		v = ADS1115_VOLTS_PER_BIT_GAIN_SIXTEEN;
		break;
	}	
	return v;
}

// irq handler to wake up poll thread
static void handle_adc_ready(uint16_t gpio_pin, uint8_t state) {	
	xTaskNotifyFromISR(adcPollTaskHandle, 0x01, eSetBits, NULL);	
}

// write to the adc and trigger a single-shot conversion
static void trigger_conversion(int idx) {
	//adc
	uint16_t value = ADS1X15_REG_CONFIG_CQUE_1CONV   | // Comparator enabled and asserts on 1 match
                     ADS1X15_REG_CONFIG_CLAT_LATCH   | // Latching mode
                     ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                     ADS1X15_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                     ADS1115_SSPS                    | //set the samples per second
                     ADS1X15_REG_CONFIG_MODE_SINGLE  | // Single-shot mode (default)
					 ADS1115_GAIN                    | // Set PGA/voltage range
					 adc_channels[idx]               | // set the channel to trigger 
					 ADS1X15_REG_CONFIG_OS_SINGLE;     // start a single conversion
	
 	marshal(0, ADS1X15_REG_POINTER_CONFIG, value);

	// to have the ready pin raise on the conversion being ready you have to set the most 
	// signigicant bit for the HIGH threshold to 1 and for the LOW threshold to 0. 
	marshal(0, ADS1X15_REG_POINTER_HITHRESH, ADS1X15_LOW_THRESHOLD_DEFAULT);
	marshal(0, ADS1X15_REG_POINTER_LOWTHRESH, ADS1X15_HIGH_THRESHOLD_DEFAULT);
	
	// pause the thread until the irq fires for the conversion being ready
	xTaskNotifyWait(pdFALSE, ULONG_MAX, NULL, osWaitForever);
}
// reads the last conversion result from the adc
static short int get_last_conversion() {
	return (short int)unmarshal(0, ADS1X15_REG_POINTER_CONVERT);	
}

/* Public methods */
static void configure(int num_channels_in, I2C_HandleTypeDef *hi2c_in, uint8_t addr_in, short int irq_pin) {
	hi2c = hi2c_in;
	addr = addr_in;
	num_channels = num_channels_in;
	ADS1115_SSPS = ADS1115_REG_CONFIG_DR_128SPS;
	ADS1115_GAIN = GAIN_TWOTHIRDS;
	IRQDispatcher.subscribe(irq_pin, &handle_adc_ready);		
}
// set the samples per second
static void set_ssps(int ssps) {
	ADS1115_SSPS = ssps;
}
// set the adc gain
static void set_gain(int gain) {
	ADS1115_GAIN = gain;
}

const struct ads1115 ADS1115 = { 
	.configure = configure,	
	.set_ssps = set_ssps,
	.set_gain = set_gain
};

void StartAdcPollTask(void const * argument) {
	while (true)
	{
		T_SYSTEM_UPDATE *update;
	
		int i = 0;
		float reading;
		
		vTaskDelay(200);
		for (i = 0; i < num_channels; i++) {
		
			trigger_conversion(i);
			reading = (get_last_conversion() * volts_per_bit()); 

			update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, osWaitForever); /* Allocate memory */
			update->idx = i;
			update->float_val = reading * 1;//channel_config.ratio;	
			update->val_type = type_float;		
			update->source = ADC_READ_EVENT;		
			osMailPut(SYS_UPDATE_MAILBOX_ID, update);
		}
#ifdef INCLUDE_uxTaskGetStackHighWaterMark
		AdcPollTaskTask_Watermark = uxTaskGetStackHighWaterMark(NULL);
#endif	
	}
}
