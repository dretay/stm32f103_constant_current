#pragma once

#include "stm32f1xx_hal.h"

#ifdef HAL_PCD_MODULE_ENABLED
#include <usbd_core.h>
#include <usbd_cdc.h>
#include "usbd_cdc_if.h"
#include <usbd_desc.h>
#include "SerialCommandAdapter.h"

extern PCD_HandleTypeDef hpcd_USB_FS;	                    

struct usbserialcommand {	
	SerialCommandAdapter*(*configure)(void);		
};

extern const struct usbserialcommand UsbSerialCommandAdapter;
#endif