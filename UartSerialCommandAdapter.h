#pragma once

#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_dma.h"
#include "SerialCommandAdapter.h"
#include "cmsis_os.h"



extern UART_HandleTypeDef huart1;

struct uartserialcommand {	
	SerialCommandAdapter*(*configure)(void);		
};

extern const struct uartserialcommand UartSerialCommandAdapter;