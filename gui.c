#include "gui.h"

static View* views[5];

void StartGUIDrawTask(void const * argument) {	
	xTaskNotifyWait(pdFALSE, ULONG_MAX, NULL, osWaitForever);
	while (1) {	
		views[0]->render();	
			
#ifdef INCLUDE_uxTaskGetStackHighWaterMark
		GUIDrawTask_Watermark = uxTaskGetStackHighWaterMark(NULL);
#endif
		
		vTaskDelay(200);
	}
}
void StartUIUpdateTask(void const * argument) {
	osEvent event;	
	T_UI_UPDATE *update;

	while (1) {
#ifdef INCLUDE_uxTaskGetStackHighWaterMark
		UiUpdateTask_Watermark = uxTaskGetStackHighWaterMark(NULL);
#endif
		event = osMailGet(UI_UPDATE_MAILBOX_ID, osWaitForever);
		if (event.status == osEventMail) {
			update = event.value.p;		
			views[0]->on_update(update);			
		}
		
		osMailFree(UI_UPDATE_MAILBOX_ID, update);		


		osThreadYield();	
	}
}
void configure() {
	gfxInit();
	gdispSetContrast(40);
	views[0] = StatusView.init();
	
	//todo: maybe set this by default on the first view?
	views[0]->dirty = true;
	xTaskNotify(guiDrawTaskHandle, 0x01, eSetBits);
}
const struct gui GUI = { 
	.configure = configure,		
};