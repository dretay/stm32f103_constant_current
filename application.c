#include "application.h"

View* views[5];

static void init(void) {
	views[0] = StatusView.init();
}
static void showView(uint8_t idx) {
	views[idx]->render();
}

const struct application Application = { 
	.init = init,
	.showView = showView
};