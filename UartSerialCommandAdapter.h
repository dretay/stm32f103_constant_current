#pragma once

#include "stm32f1xx_hal.h"
#include "SerialCommandAdapter.h"

extern UART_HandleTypeDef huart1;

struct uartserialcommand {	
	SerialCommandAdapter*(*configure)(void);		
};

extern const struct uartserialcommand UartSerialCommandAdapter;