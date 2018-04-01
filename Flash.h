#pragma once

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "flashpages.h"
#include "types.h"
#include "stdbool.h"

#define FLASH_USER_START_ADDR   ADDR_FLASH_PAGE_126 
#define FLASH_USER_END_ADDR     ADDR_FLASH_PAGE_127 + FLASH_PAGE_SIZE   /* End @ of user Flash area */

typedef struct {
	unsigned int idx;
	union {
		float float_val;
		unsigned int uint_val;	
		int int_val;	
		double double_val;
		unsigned int uint_array[2];
	};
	VALUE_TYPE val_type;

} FLASH_RECORD;

struct flash {
	HAL_StatusTypeDef(*configure)(bool erase);			
	HAL_StatusTypeDef(*set)(FLASH_RECORD record);
	HAL_StatusTypeDef(*set_double)(unsigned int idx, double value);
	HAL_StatusTypeDef(*set_float)(unsigned int idx, float value);
	FLASH_RECORD(*get)(unsigned int idx, VALUE_TYPE val_type);		
};


extern const struct flash Flash;

