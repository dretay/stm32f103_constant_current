#include "StatusView.h"

View statusView;

void reverse(s) char *s; {
	char *j;
	int c;
 
	j = s + strlen(s) - 1;
	while (s < j) {
		c = *s;
		*s++ = *j;
		*j-- = c;
	}
}
void itoa(int n, char s[]) {
	int i, sign;
 
	if ((sign = n) < 0)  /* record sign */
		n = -n;          /* make n positive */
	i = 0;
	do {       /* generate digits in reverse order */
		s[i++] = n % 10 + '0';   /* get next digit */
	} while ((n /= 10) > 0);     /* delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

static void on_update(T_SYSTEM_UPDATE* update) {
	char input[10];
	font_t font;
	coord_t height, width, rx, ry, rcx, rcy;
	itoa(update->val, input);


	width = gdispGetWidth();
	font = gdispOpenFont("DEJAVUSANS10");
	gdispClear(White);
	HAL_Delay(100);//this needs to be fixed...:(

	gdispDrawStringBox(0, 0, width, 20, input, font, Black, justifyCenter);

	set_dac(update->val);
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