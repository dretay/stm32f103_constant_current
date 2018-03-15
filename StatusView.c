﻿#include "StatusView.h"

View statusView;
static font_t DejaVuSans10, DejaVuSans12, DejaVuSans12Bold, DejaVuSans20, DejaVuSans32, Fixed_5x8, Fixed_7x14;
static coord_t DisplayWidth, DisplayHeight;

static float voltage_setting = 0.0;
static float voltage_reading = 0.0;
static float current_setting = 0.0;
static float current_reading = 0.0;
static float wattage_reading = 0.0;
static char* status = "OFF";

static int voltage_scale = 0;
static int current_scale = 0;

//this are the raw "units" of the dac
const static float voltage_multiplier = (4096.0f / 30.0f);
const static float current_multiplier = (4095.0f / 2.341f);

static void on_update(T_SYSTEM_UPDATE* update) {
	static float multiplier;
	static float temp_setting;
	int calculated_dac;

	//temp
	const unsigned int order = 3;
	const unsigned int countOfElements = 8;
	const double acceptableError = 0.01;
	int result;
	double xData[8] = {
		0.21,
		0.64,
		1.07,
		1.49,
		1.92,
		2.33,
		2.71,
		3.21		 
	};
	double yData[8] = {
		500,
		1000,
		1500,
		2000,
		2500,
		3000,
		3500,
		4000 
	};
	double coefficients[order + 1];
	//end temp
	switch (update->source)
	{
	case ENCODER_BUTTON_EVENT:
		//todo: this is bad - needs to be fixed?
		if (update->idx  == 0) {
			current_scale = (current_scale + 1) % 4;
		}
		else {
			voltage_scale = (voltage_scale + 1) % 4;
		}
		break;
	case ENCODER_SPIN_EVENT:
		multiplier = 10 / pow(10, current_scale); 
		temp_setting += (update->int_val * multiplier);
		
		if (temp_setting >= 0.f)
		{
			current_setting = temp_setting;
			CurrentSink.set(current_setting);
		}
		break;
	case ADC_READING_EVENT:
		switch (update->idx)
		{
		case 0:
			voltage_reading = update->int_val/100.0f;
			wattage_reading = voltage_reading * current_reading;
			statusView.dirty = true;
			break;
		case 1:		
			current_reading = abs(update->int_val-2296)/150.0f;
			wattage_reading = voltage_reading * current_reading;
			statusView.dirty = true;
			break;
		}
		break;
	case TOGGLE_SWITCH_EVENT:
		if (update->bool_val)
		{
			status = "ON";
		}
		else
		{
			status = "OFF";			
		}
		break;

		
	}
	statusView.dirty = true;	
}


static void render_readings_label() {
	static const char readings_str[9] = "READINGS";
	
	//x1/y1 x2/y2
	gdispFillStringBox(0,
		0,
		DisplayWidth,
		gdispGetFontMetric(Fixed_5x8, fontHeight)+4,
		readings_str,
		Fixed_5x8,
		Black,
		White,
		justifyCenter);	
	gdispDrawLine(0, 6, 7, 6, Black);

	gdispDrawLine(7, 6, 11, 1, Black);
	gdispDrawLine(11, 1, 52, 1, Black);
	gdispDrawLine(52, 1, 56, 6, Black);
	
	gdispDrawLine(7, 6, 11, 10, Black);
	gdispDrawLine(11, 10, 52, 10, Black);
	gdispDrawLine(52, 10, 56, 6, Black);


	gdispDrawLine(DisplayWidth - 7, 6, DisplayWidth, 6, Black);

}
static void render_reading(uint8_t idx, float reading, char *units) {
	static char buffer[7];	
	ftos(buffer, reading, 2);	

	gdispDrawStringBox(-2,
		gdispGetFontMetric(DejaVuSans20, fontHeight)+20 * idx -(idx == 0 ? 10 : 11),
		gdispGetStringWidth(buffer, DejaVuSans20),
		gdispGetFontMetric(DejaVuSans20, fontHeight),
		buffer,
		DejaVuSans20,
		White,
		justifyLeft);		

	gdispDrawStringBox(gdispGetStringWidth(buffer, DejaVuSans20)-2, 
		gdispGetFontMetric(DejaVuSans20, fontHeight) * idx+(idx==0?14:10), 		
		gdispGetStringWidth(units, Fixed_7x14), 
		gdispGetFontMetric(DejaVuSans20, fontHeight) -5,		
		units,
		Fixed_5x8,
		White,
		justifyCenter);	
}
static void render_settings_label() {
	static const char readings_str[9] = "SETTINGS";
	uint8_t offset = 70;
	//x1/y1 x2/y2
	gdispFillStringBox(0,
		offset,
		DisplayWidth,
		gdispGetFontMetric(Fixed_5x8, fontHeight)+4,
		readings_str,
		Fixed_5x8,
		Black,
		White,
		justifyCenter);	
	gdispDrawLine(0, 6 + offset, 7, 6 + offset, Black);

	gdispDrawLine(7, 6 + offset, 11, 1 + offset, Black);
	gdispDrawLine(11, 1 + offset, 52, 1 + offset, Black);
	gdispDrawLine(52, 1 + offset, 56, 6 + offset, Black);
	
	gdispDrawLine(7, 6 + offset, 11, 10 + offset, Black);
	gdispDrawLine(11, 10 + offset, 52, 10 + offset, Black);
	gdispDrawLine(52, 10 + offset, 56, 6 + offset, Black);


	gdispDrawLine(DisplayWidth - 7, 6 + offset, DisplayWidth, 6 + offset, Black);

	gdispDrawLine(0, 99, DisplayWidth, 99, White);

}
static void render_setting(uint8_t idx, float setting, char *units, uint8_t scale) {
	static char buffer[7];	
	ftos(buffer, setting, 2);	
	buffer[5] = ' ';
	buffer[6] = units[0];

	gdispDrawStringBox(0,
		83,
		DisplayWidth,
		gdispGetFontMetric(DejaVuSans12Bold, fontHeight),
		buffer,
		DejaVuSans12Bold,
		White,
		justifyCenter);	

	//digit being manipulated
	//uint8_t scale = 0;
	static uint8_t scale_offsets[4] = {8, 16, 29, 37 };
	gdispDrawLine(scale_offsets[scale], 
		75 + (10*idx)+gdispGetFontMetric(DejaVuSans10, fontHeight), 
		scale_offsets[scale]+6, 
		75 + (10*idx)+gdispGetFontMetric(DejaVuSans10, fontHeight), 
		White);
}
static void render_status(char *status) {
	gdispDrawStringBox(0,
		DisplayHeight - gdispGetFontMetric(DejaVuSans32, fontHeight)+6,
		DisplayWidth,
		gdispGetFontMetric(DejaVuSans32, fontHeight)+4,
		status,
		DejaVuSans32,
		White,
		justifyCenter);
}

static void render(void) {
	
	//without this you end up writing over previous vals	
	gdispClear(Black);

	//render out readings
	render_readings_label();
	render_reading(0, voltage_reading, "V");
	render_reading(1, current_reading, "A");
	render_reading(2, wattage_reading, "W");

	//render out settings
	render_settings_label();
	render_setting(1, current_setting, "A", current_scale);

	//render status
	render_status(status);
	
	//manually flush at then end - otherwise you end up redrawing each time you you call a gdisp* command
	gdispGFlush(gdispGetDisplay(0));


}

//should the association w/ implementing method this be implemented in view somehow?
static View* init(void) {
	statusView.render = render;
	statusView.on_update = on_update;
	statusView.dirty = false;
		
	DejaVuSans10 =  gdispOpenFont("DejaVuSans10");
	DejaVuSans12 =  gdispOpenFont("DejaVuSans12");
	DejaVuSans12Bold =  gdispOpenFont("DejaVuSansBold12");
	
	DejaVuSans20 = gdispOpenFont("DejaVuSans20");
	
	DejaVuSans32 = gdispOpenFont("DejaVuSans32");
	Fixed_5x8 = gdispOpenFont("fixed_5x8");
	Fixed_7x14 = gdispOpenFont("fixed_7x14");
	
	DisplayWidth = gdispGetWidth();
	DisplayHeight = gdispGetHeight();	

	return &statusView;	
}

const struct statusView StatusView = { 
	.init = init
};