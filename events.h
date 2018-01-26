#pragma once

#include <stdint.h>
#include "cmsis_os.h"
#include "stdbool.h"

#define SYS_UPDATE_MAIL_SIZE        (uint32_t) 10
extern osMailQId SYS_UPDATE_MAILBOX_ID;


typedef struct {
	enum { ENCODER_BUTTON, ENCODER_SPIN, ADC_READING } source;

	uint8_t idx;
	union
	{
		float float_val;
		int32_t int_val;	
		bool bool_val;
	};
	enum
	{
		type_float,
		type_int,
		type_bool
	} val_type;

} T_SYSTEM_UPDATE;