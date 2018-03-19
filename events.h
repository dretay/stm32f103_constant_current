#pragma once

#include <stdint.h>
#include "cmsis_os.h"
#include "stdbool.h"
#include "types.h"

#define SYS_UPDATE_MAIL_SIZE        (uint32_t) 10
extern osMailQId SYS_UPDATE_MAILBOX_ID;


typedef struct {
	enum { ENCODER_BUTTON_EVENT, ENCODER_SPIN_EVENT, ADC_READ_EVENT, TOGGLE_SWITCH_EVENT, ADS1115_READ_EVENT } source;

	uint8_t idx;
	union
	{
		float float_val;
		int int_val;	
		bool bool_val;
	};
	VALUE_TYPE val_type;

} T_SYSTEM_UPDATE;