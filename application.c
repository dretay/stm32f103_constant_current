#include "application.h"

View* views[5];

void StartApplicationTask(void const * argument) {
	osEvent event;	
	T_STATE_UPDATE *update;

	gfxInit();
	views[0] = StatusView.init();
	while (1) {
		event = osMailGet(SYSUPDATE_MAILBOX_ID, osWaitForever);
		if (event.status == osEventMail) {
			update = event.value.p;		
			views[0]->on_update(update);	
		}				
		osMailFree(SYSUPDATE_MAILBOX_ID, update);		
	
	}
}
static void showView(uint8_t idx) {
	views[idx]->render();
}

const struct application Application = { 
	.showView = showView
};