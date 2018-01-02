#include "uartadc.h"

static const uint8_t RXBUFFERSIZE = 12;
static uint8_t rx_buffer[32];
 
static uint8_t poll_char = 'x';
__IO ITStatus UartReady = SET;

//do conversion of data to send to device
static void marshal(uint8_t idx, uint16_t value) {
	
} 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
  /* Set transmission flag: trasfer complete*/
	UartReady = SET;
}
//do conversion from device response
static void unmarshal(void) {
	AdcReadings message = AdcReadings_init_default; 
	pb_istream_t stream = pb_istream_from_buffer(rx_buffer, RXBUFFERSIZE);
	T_SYSTEM_UPDATE *update;
	int i;
	bool status = true;
	status = pb_decode(&stream, AdcReadings_fields, &message);
        
	/* Check for errors... */
	if (!status)
	{
		LOG("Decoding failed: %s\n", PB_GET_ERROR(&stream));
	}
	
	update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, osWaitForever); /* Allocate memory */
	update->idx = 0;
	update->float_val = 0.0 + message.reading1;
	update->val_type = type_float;
	update->source = ADC;
	update->parameter = VOLTAGE;
	osMailPut(SYS_UPDATE_MAILBOX_ID, update);		
	


	
		
//	for (i = 0; i < config->channel_cnt; i++) {
//		channel_config = config->channel_configs[i];
//		reading = channel_config.differential ? get_differential_reading(channel_config.idx) : get_reading(channel_config.idx);
//		update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, osWaitForever); /* Allocate memory */
//		update->idx = channel_config.idx;
//		update->float_val = reading * channel_config.ratio;	
//		update->val_type = type_float;
//		update->source = ADC;
//		update->parameter = channel_config.parameter;
//		osMailPut(SYS_UPDATE_MAILBOX_ID, update);	
		
//	}
}
void StartAdcPollTask(void const * argument) {		
	//suspend until config is passed in and thread is resumed
	vTaskSuspend(NULL);		
	
	while (true) {
//		if (UartReady == SET) {
//			UartReady = RESET;
//			HAL_StatusTypeDef status;
//			status = HAL_UART_Transmit(&huart1, &poll_char, 1, 1000);
//			if (status != HAL_OK) {
//				Error_Handler();
//			}		
//			if (HAL_UART_Receive_DMA(&huart1, (uint8_t *)rx_buffer, 32) != HAL_OK) {
//				Error_Handler();
//			}
//			todo: this needs to be reimplemented using a task notification so it doesn't eat everythign!!!!
//			while (UartReady == RESET)
//			{
//				
//			}
//			unmarshal();
//#ifdef INCLUDE_uxTaskGetStackHighWaterMark
//			AdcPollTask_Watermark = uxTaskGetStackHighWaterMark(NULL);
//#endif
//		}
		HAL_Delay(1000);
		osThreadYield();	
	}
	
}


