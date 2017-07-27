#include "application.h"

View* views[5];

void StartSysUpdateTask(void const * argument) {
	osEvent event;	
	T_SYSTEM_UPDATE *update;

	gfxInit();
	views[0] = StatusView.init();
	while (1) {
		event = osMailGet(SYS_UPDATE_MAILBOX_ID, osWaitForever);
		if (event.status == osEventMail) {
			update = event.value.p;		
			views[0]->on_update(update);	
		}				
		osMailFree(SYS_UPDATE_MAILBOX_ID, update);		
	
	}
}
void StartGuiUpdateTask(void const * argument) {
	osEvent event;
	while (1){
		event = osSignalWait(0, osWaitForever);
		views[0]->render();
	}
}
static void showView(uint8_t idx) {
	views[idx]->render();
}

const struct application Application = { 
	.showView = showView
};