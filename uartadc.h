#pragma once

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include <pb_decode.h>
#include "adc_reading.pb.h"
#include <stdint.h>
#include "events.h"

#ifdef INCLUDE_uxTaskGetStackHighWaterMark
UBaseType_t AdcPollTask_Watermark;
#endif

extern UART_HandleTypeDef huart1;

