#pragma once

#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_dma.h"
#include "SerialCommandAdapter.h"
#include "cmsis_os.h"
#include "stdbool.h"
#include "events.h"

extern UART_HandleTypeDef huart1;
extern osMailQId SERCMD_UPDATE_MAILBOX_ID;

struct uartserialcommand {	
	SerialCommandAdapter*(*configure)(void);		
};

extern const struct uartserialcommand UartSerialCommandAdapter;