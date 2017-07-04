#pragma once

#include "SEGGER_RTT.h"


/* USER CODE BEGIN Private defines */
#if DEBUG == 1
#define LOG(...) SEGGER_RTT_printf(0, __VA_ARGS__)
#else
#define LOG(...)
#endif