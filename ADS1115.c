#include "ADS1115.h"

static I2C_HandleTypeDef *hi2c;
static uint8_t addr;

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

static void handle_adc_ready(uint16_t gpio_pin, uint8_t state) {	
	xTaskNotifyFromISR(adcPollTaskHandle, 0x01, eSetBits, NULL);	
}
static void configure(I2C_HandleTypeDef *hi2c_in, uint8_t addr_in) {
	hi2c = hi2c_in;
	addr = addr_in;
	/*todo:
		- set samples per second
		- set gain
		- alert pin should be passed in, should be conditional
	*/
	IRQDispatcher.subscribe(GPIO_PIN_14, &handle_adc_ready);		
}

static uint16_t adc_channels[4] = { ADS1X15_REG_CONFIG_MUX_SINGLE_0, ADS1X15_REG_CONFIG_MUX_SINGLE_1, ADS1X15_REG_CONFIG_MUX_SINGLE_2, ADS1X15_REG_CONFIG_MUX_SINGLE_3 };

static float get_reading(int idx) {
	//adc
	uint16_t value = ADS1X15_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1X15_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1X15_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1115_REG_CONFIG_DR_128SPS   | // 128 samples per second (default)
                    ADS1X15_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
	// Set PGA/voltage range
	value |= GAIN_TWOTHIRDS; //todo: this can  probably be set to a gain of "1" - 4v max

	value |= adc_channels[idx];
	
	value |= ADS1X15_REG_CONFIG_OS_SINGLE;
	
	marshal(0, ADS1X15_REG_POINTER_CONFIG, value);
	HAL_Delay(8);
	uint16_t res = unmarshal(0, ADS1X15_REG_POINTER_CONVERT) >> 0;  
	
	return (res * 0.1875) / 1000; 
	
}

static float get_differential_reading(int idx) {
  // Start with default values
	uint16_t config = ADS1X15_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
	                  ADS1X15_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
	                  ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
	                  ADS1X15_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
	                  ADS1115_REG_CONFIG_DR_128SPS   | // 128 samples per second (default)
	                  ADS1X15_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

	                    // Set PGA/voltage range
	config |= GAIN_TWOTHIRDS;
                    
	                // Set channels
	if (idx == 0) {
		config |= ADS1X15_REG_CONFIG_MUX_DIFF_0_1;          // AIN0 = P, AIN1 = N	
	}
	else {
		config |= ADS1X15_REG_CONFIG_MUX_DIFF_2_3;          // AIN2 = P, AIN3 = N	
	}
	

	  // Set 'start single-conversion' bit
	config |= ADS1X15_REG_CONFIG_OS_SINGLE;

	  // Write config register to the ADC
	marshal(0, ADS1X15_REG_POINTER_CONFIG, config);

	  // Wait for the conversion to complete
	HAL_Delay(8);

	  // Read the conversion results
	uint16_t res = unmarshal(0, ADS1X15_REG_POINTER_CONVERT) >> 0;
	return (res * 0.1875) / 1000; 
}
uint16_t getSingleEndedConfigBitsForMUX(uint8_t channel) {
	uint16_t c = 0;
	switch (channel) {
	case (0):
		c = ADS1X15_REG_CONFIG_MUX_SINGLE_0;
		break;
	case (1):
		c = ADS1X15_REG_CONFIG_MUX_SINGLE_1;
		break;
	case (2):
		c = ADS1X15_REG_CONFIG_MUX_SINGLE_2;
		break;
	case (3):
		c = ADS1X15_REG_CONFIG_MUX_SINGLE_3;
		break;
	}
	return c;
}
static void start_continuous(int idx) {
	// Initial single ended non-contunuous read primes the conversion buffer with a valid reading
	// so that the initial interrupts produced a correct result instead of a left over 
	// conversion result from previous operations.
	int16_t primingRead = get_reading(idx);

	uint16_t config = ADS1X15_REG_CONFIG_CQUE_1CONV   | // Comparator enabled and asserts on 1 match
                      ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
					  GAIN_TWOTHIRDS|					// 2/3 gain
					  ADS1115_REG_CONFIG_DR_128SPS|     // 128 sps
                      ADS1X15_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode
	config |= getSingleEndedConfigBitsForMUX(idx);

	// Continuous mode is set by setting the most signigicant bit for the HIGH threshold to 1
	// and for the LOW threshold to 0.  This is accomlished by setting the HIGH threshold to the 
	// low default (a negative number) and the LOW threshold to the HIGH default (a positive number)
	marshal(0, ADS1X15_REG_POINTER_HITHRESH, ADS1X15_LOW_THRESHOLD_DEFAULT);
	marshal(0, ADS1X15_REG_POINTER_LOWTHRESH, ADS1X15_HIGH_THRESHOLD_DEFAULT);

	// Write config register to the ADC
	marshal(0, ADS1X15_REG_POINTER_CONFIG, config);
}

const struct ads1115 ADS1115 = { 
	.configure = configure,
	.get_reading = get_reading,
	.start_continuous = start_continuous
};
static int16_t getLastConversionResults() {
  // Read the conversion results
	return (int16_t)unmarshal(0, ADS1X15_REG_POINTER_CONVERT);
}
float floatval;
void StartAdcPollTask(void const * argument) {
	while (true)
	{
		T_SYSTEM_UPDATE *update;
	
		int i = 0;
		float reading;
		
		vTaskDelay(200);
		for (i = 0; i < 1; i++) {
		
			reading = get_reading(i);
			floatval = reading;
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
