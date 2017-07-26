#include "StatusView.h"

View statusView;

static void on_update(T_STATE_UPDATE* update) {
	const char* input = "hello!";
	font_t font;
	coord_t height, width, rx, ry, rcx, rcy;
	width = gdispGetWidth();
	font = gdispOpenFont("DEJAVUSANS10");
	gdispClear(White);
	HAL_Delay(100);//this needs to be fixed...:(

	gdispDrawStringBox(0, 0, width, 20, input, font, Black, justifyCenter);
}

static void render(void) {
	LOG("RENDER!!!");
}

//should this be implemented in view somehow?
static View* init(void) {
	statusView.render = render;
	statusView.on_update = on_update;
	return &statusView;	
}

const struct statusView StatusView = { 
	.init = init
};