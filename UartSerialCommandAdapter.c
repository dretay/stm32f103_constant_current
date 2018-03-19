#include "UartSerialCommandAdapter.h"

static SerialCommandAdapter adapter;

static void write(const void* buffer, int size) {
	if (HAL_UART_Transmit(&huart1, (uint8_t *)buffer, size, 1) != HAL_OK) {		
		Error_Handler();
	}
}

static int read(void *buffer, int size) {
	osDelay(100);
	return 0;
}


#define DMA_RX_BUFFER_SIZE          32
uint8_t DMA_RX_Buffer[DMA_RX_BUFFER_SIZE];
void USART1_IRQHandler(void) {
	if (LL_USART_IsActiveFlag_IDLE(USART1)) {
		LL_USART_ClearFlag_IDLE(USART1);

		//this is terrible - i really hate cubemx is this really the best I can do? 
		HAL_UART_AbortReceive_IT(&huart1);
		if (HAL_UART_Receive_DMA(&huart1, (uint8_t*)DMA_RX_Buffer, DMA_RX_BUFFER_SIZE) != HAL_OK) {        
			Error_Handler();
		}
	}
}
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart) {
	LOG("FOO");
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {	
	Error_Handler();
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	Error_Handler();
}

static SerialCommandAdapter* configure(void) {
	//enable idle line detection
	LL_USART_EnableIT_IDLE(USART1);
	 
	//enable USART global interrupt
	NVIC_SetPriority(USART1_IRQn, 5);
	NVIC_EnableIRQ(USART1_IRQn);
	
	//start uart dma
	if (HAL_UART_Receive_DMA(&huart1, (uint8_t*)DMA_RX_Buffer, DMA_RX_BUFFER_SIZE) != HAL_OK) {        
		Error_Handler();
	}
    
	/* Disable Half Transfer Interrupt */
	__HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
	
	adapter.read = read;
	adapter.write = write;
	adapter.echo = true;
	return &adapter;

}



const struct uartserialcommand UartSerialCommandAdapter = { 
	.configure = configure,		
};