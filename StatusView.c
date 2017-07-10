#include "StatusView.h"

View statusView;

static void render(void) {
	LOG("RENDER!!!");
}

static View* init(void) {
	statusView.render = render;
	return &statusView;	
}

const struct statusView StatusView = { 
	.init = init
};