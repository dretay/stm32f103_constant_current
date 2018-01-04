#pragma once

#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdbool.h"
#include "cmsis_os.h"
#include <limits.h>

#include <usbd_core.h>
#include <usbd_cdc.h>
#include "usbd_cdc_if.h"
#include <usbd_desc.h>


#define RX_DATA_SIZE  32
    
extern PCD_HandleTypeDef hpcd_USB_FS;	                    
extern osThreadId serialCmdTaskHandle;						         

typedef struct {
	const char* command;
	void(*function)();
} UsbSerialCommand_Command;

typedef struct {
	uint8_t command_cnt;
	UsbSerialCommand_Command *commands;
} UsbSerialCommand_Config;

struct usbserialcommand {
	void(*configure)(UsbSerialCommand_Config* config_in, uint8_t cnt_in);	
	char*(*next)(void);	
};

extern const struct usbserialcommand UsbSerialCommand;
