#pragma once

#include "stm32f1xx_hal.h"
//#include "string.h"
//#include "strlcpy.h"
#include "stdbool.h"
#include "cmsis_os.h"
#include <limits.h>

#include <usbd_core.h>
#include <usbd_cdc.h>
#include "usbd_cdc_if.h"
#include <usbd_desc.h>


#define RX_DATA_SIZE  32
#define USB_SERIAL_COMMAND_CNT 1
    
extern PCD_HandleTypeDef hpcd_USB_FS;	                    
extern osThreadId serialCmdTaskHandle;						         
UBaseType_t SerialCmdTask_Watermark;

typedef struct {
	char command[8];
	void(*function)();
} UsbSerialCommand_Command;

typedef struct {
	uint8_t command_cnt;
	UsbSerialCommand_Command *commands;
} UsbSerialCommand_Config;

struct usbserialcommand {
	void(*configure)(void);	
	void(*register_command)(uint8_t idx, char* command_in, void* function_in);	
	char*(*next)(void);	
};

extern const struct usbserialcommand UsbSerialCommand;
