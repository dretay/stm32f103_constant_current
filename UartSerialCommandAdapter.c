#include "UartSerialCommandAdapter.h"

static SerialCommandAdapter adapter;

static int read(void *buffer, int size) {
	if (HAL_UART_Receive(&huart1, (uint8_t *)buffer, size, 1) != HAL_OK)
	{
		return 0;
	}
	return 1;
}
static void write(const void* buffer, int size) {
	if (HAL_UART_Transmit(&huart1, (uint8_t *)buffer, size, 1) != HAL_OK)
	{		
		Error_Handler();
	}
}

static SerialCommandAdapter* configure(void) {
	adapter.read = read;
	adapter.write = write;
	return &adapter;

}



const struct uartserialcommand UartSerialCommandAdapter = { 
	.configure = configure,	
};