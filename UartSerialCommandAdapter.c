#include "UartSerialCommandAdapter.h"

static SerialCommandAdapter adapter;

static void write(const void* buffer, int size) {
	
	if (HAL_UART_Transmit(&huart1, (uint8_t *)buffer, size, 1) != HAL_OK) {		
		Error_Handler();
	}
}

#define DMA_RX_BUFFER_SIZE          32
char DMA_RX_Buffer[DMA_RX_BUFFER_SIZE];
bool pending = false;

//todo: this is probably not great since a fast enough xmit will just stomp the reads 
//but it works for tonight...
static int read(char *buffer, int size) {
	osDelay(50);
	int i = 0;
	int length = 0;
	if (pending)
	{
		length = strlen(DMA_RX_Buffer);
		for (i =0; i < length; i++)
		{
			buffer[i] = DMA_RX_Buffer[i];	
		}
		pending = false;		
	}
	return length;	
}



//idle interrupt handler
void USART1_IRQHandler(void) {
	T_SERCMD_UPDATE *update;
	if (LL_USART_IsActiveFlag_IDLE(USART1)) {
		LL_USART_ClearFlag_IDLE(USART1);

		//this is terrible - i really hate cubemx is this really the best I can do? 
		HAL_UART_AbortReceive_IT(&huart1);
		if (HAL_UART_Receive_DMA(&huart1, (uint8_t*)DMA_RX_Buffer, DMA_RX_BUFFER_SIZE) != HAL_OK) {        
			Error_Handler();
		}
	
		update = osMailAlloc(SERCMD_UPDATE_MAILBOX_ID, 0); /* Allocate memory */
		update->size = strlen(DMA_RX_Buffer);
		strncpy(update->string, DMA_RX_Buffer, update->size);		
		osMailPut(SERCMD_UPDATE_MAILBOX_ID, update);	
	}
}

//uart abort complete callback
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart) {
	pending = true;
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