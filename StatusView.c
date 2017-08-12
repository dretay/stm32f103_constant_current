#include "StatusView.h"

View statusView;


static float voltage_setting = 0.0;
static float voltage_reading = 0.0;
static float current_setting = 0.0;
static float current_reading = 0.0;
const static float voltage_multiplier = (5.0 / 4096.0);
static void on_update(T_SYSTEM_UPDATE* update) {
	if (update->source == ENCODER)
	{
		if (update->parameter == VOLTAGE)
		{			
			voltage_setting = voltage_multiplier * update->val;
			MCP4725.set_dac(0,update->val);
		}
		else
		{
			current_setting = voltage_multiplier * update->val;
		}
	}
	else
	{
		if (update->parameter == VOLTAGE) {			
			voltage_reading = update->val;
		}
	}
}

static void render(void) {
	char output1[10];
	char output2[10];
	font_t font;
	coord_t height, width, rx, ry, rcx, rcy;

	width = gdispGetWidth();
	font = gdispOpenFont("DEJAVUSANS10");	

//	ftos(output1, voltage_reading, 3);
//	output1[5] = ' ';
//	output1[6] = 'V';
//	output1[7] = '\0';
//
//	ftos(output2, voltage_setting, 3);
//	output2[5] = ' ';
//	output2[6] = 'V';
//	output2[7] = '\0';
	output1[0] = 'h';
//	output1[1] = 'e';
//	output1[2] = 'l';
//	output1[3] = 'l';
//	output1[4] = 'o';
	
	
	
	//todo: this should probably be its own fn
	gdispClear(Black);
	gdispDrawStringBox(0, 0, width, 20, output1, font, White, justifyCenter);	
	//gdispDrawStringBox(0, 13, width, 20, output2, font, White, justifyCenter);	
	gdispGFlush(gdispGetDisplay(0));


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