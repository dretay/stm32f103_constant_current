#pragma once

#include "stm32f1xx_hal.h"
#include "stdbool.h"
#include "cmsis_os.h"
#include <limits.h>
#include "SerialCommandAdapter.h"
#include <string.h>
#include <stdlib.h>
#include "events.h"

#define RX_DATA_SIZE  80
#define USB_SERIAL_COMMAND_CNT 11
extern osMailQId SERCMD_UPDATE_MAILBOX_ID;

extern osThreadId serialCmdTaskHandle;						         
UBaseType_t SerialCmdTask_Watermark;

typedef struct {
	char command[8];
	void(*function)();
} UsbSerialCommand_Command;

typedef struct {
	int command_cnt;
	UsbSerialCommand_Command *commands;
} UsbSerialCommand_Config;



struct serialcommand {
	void(*configure)(SerialCommandAdapter*);	
	void(*register_command)(int idx, char* command_in, void* function_in);	
	char*(*next)(void);	
	int(*next_int)(void);	
	float(*next_float)(void);	
	double(*next_double)(void);	
	void(*echo)(char* output, int length);
};


extern const struct serialcommand SerialCommand;
