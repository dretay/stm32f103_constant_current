#pragma once
#include "cmsis_os.h"
#include "StatusView.h"

#ifdef INCLUDE_uxTaskGetStackHighWaterMark
UBaseType_t GUIDrawTask_Watermark;
UBaseType_t UiUpdateTask_Watermark;
#endif

extern osThreadId guiDrawTaskHandle;

struct gui {	
	void(*configure)(void);		
};

extern const struct gui GUI;