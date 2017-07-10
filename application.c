#include "application.h"

View* views[5];

static void init(void) {
	views[0] = StatusView.init();
	while (1)
	{
		osEvent evt = osMessageGet(*(osMessageQId*)get_encoderQueueHandle(), osWaitForever) ;
		if (evt.status == osEventMessage) {
			T_ENCODER_READING *message = (T_ENCODER_READING*)evt.value.p;
			
            
			osPoolFree(*(osPoolId*)get_encoderMessagePool(), message);
		}
	}
}
static void showView(uint8_t idx) {
	views[idx]->render();
}

const struct application Application = { 
	.init = init,
	.showView = showView
};