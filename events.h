#pragma once

#include <stdint.h>
#include "cmsis_os.h"

#define SYS_UPDATE_MAIL_SIZE        (uint32_t) 10
extern osMailQId SYS_UPDATE_MAILBOX_ID;


typedef struct {
	enum { ENCODER, ADC } source;
	enum { VOLTAGE, CURRENT, OTHER } parameter;

	uint8_t idx;
	float val;

} T_SYSTEM_UPDATE;