#include "StatusView.h"

View statusView;
static font_t LargeNumbers, DejaVuSans10, DejaVuSansBold12, DejaVuSans20, Fixed_5x8, Fixed_7x14;
static coord_t DisplayWidth, DisplayHeight;

static float voltage_setting = 0.0;
static float voltage_reading = 0.0;
static float current_setting = 0.0;
static float current_reading = 0.0;
static float wattage_reading = 0.0;

//this are the raw "units" of the dac
const static float voltage_multiplier = (5.0 / 4096.0);

static void on_update(T_SYSTEM_UPDATE* update) {
	if (update->source == ENCODER)
	{
		if (update->parameter == VOLTAGE)
		{			
			voltage_setting = voltage_multiplier * update->val;
			MCP4725.set_dac(0,update->val);
			statusView.dirty = true;
		}
		else
		{
			current_setting = voltage_multiplier * update->val;
		}
	}
	else
	{
		if (update->parameter == VOLTAGE && update->val != voltage_reading) {			
			voltage_reading = update->val;
			wattage_reading = voltage_reading * current_reading;
			statusView.dirty = true;
		}
		else if (update->parameter == CURRENT && update->val != current_reading) {			
			current_reading = update->val;
			wattage_reading = voltage_reading * current_reading;
			statusView.dirty = true;
		}
	}
}


static void render_reading(uint8_t idx, float reading, char *units) {
	static char buffer[7];	
	ftos(buffer, reading, 2);	

	gdispDrawStringBox(-2,
		gdispGetFontMetric(LargeNumbers, fontHeight) * idx-2,
		gdispGetStringWidth(buffer, LargeNumbers),
		gdispGetFontMetric(LargeNumbers, fontHeight),
		buffer,
		LargeNumbers,
		White,
		justifyLeft);		

	gdispDrawStringBox(gdispGetStringWidth(buffer, LargeNumbers), 
		gdispGetFontMetric(LargeNumbers, fontHeight) * idx, 		
		gdispGetStringWidth(units, Fixed_7x14)+5, 
		gdispGetFontMetric(LargeNumbers, fontHeight) -5,		
		units,
		Fixed_7x14,
		White,
		justifyCenter);	
}
static void render_setting(uint8_t idx, float setting, char *units) {
	static char buffer[7];	
	ftos(buffer, setting, 2);	
	buffer[5] = ' ';
	buffer[6] = units[0];

	gdispDrawStringBox(DisplayWidth-gdispGetStringWidth(buffer, DejaVuSans10)-1,
		11 + (9*idx),
		gdispGetStringWidth(buffer, DejaVuSans10)+2,
		gdispGetFontMetric(DejaVuSans10, fontHeight),
		buffer,
		DejaVuSans10,
		White,
		justifyLeft);	
}
static void render_status(char *status) {
	gdispDrawStringBox(DisplayWidth-gdispGetStringWidth(status, DejaVuSans20)+1,
		44,
		gdispGetStringWidth(status, DejaVuSans20)+2,
		gdispGetFontMetric(DejaVuSans20, fontHeight),
		status,
		DejaVuSans20,
		White,
		justifyCenter);
}
static void render_settings_outline(void) {
	static const char setting_str[10] = "SETTING";
	static const char status_str[10] = "STATUS";
	
	//line that separates the readings from the settings
	gdispDrawLine(DisplayWidth - 4 - gdispGetStringWidth(setting_str, Fixed_5x8), 
		0, 
		DisplayWidth - 4 - gdispGetStringWidth(setting_str, Fixed_5x8), 
		DisplayHeight, 
		White);
	
	//setting label
	gdispFillStringBox(DisplayWidth-gdispGetStringWidth(setting_str, Fixed_5x8)-3,
		0,		
		gdispGetStringWidth(setting_str, Fixed_5x8)+3,
		gdispGetFontMetric(Fixed_5x8, fontHeight)+2,		
		setting_str,
		Fixed_5x8,
		Black,
		White,
		justifyCenter);
	
	//status label
	gdispFillStringBox(DisplayWidth-gdispGetStringWidth(status_str, Fixed_5x8)-8,
		33,		
		gdispGetStringWidth(status_str, Fixed_5x8)+8,
		gdispGetFontMetric(Fixed_5x8, fontHeight)+2,		
		status_str,
		Fixed_5x8,
		Black,
		White,
		justifyCenter);
}
 
static void render(void) {
	
	//without this you end up writing over previous vals	
	gdispClear(Black);

	//render out readings
	render_reading(0, voltage_reading, "V");
	render_reading(1, current_reading, "A");
	render_reading(2, wattage_reading, "W");

	//render out settings
	render_setting(0, voltage_setting, "V");
	render_setting(1, current_setting, "A");
	
	//render display category outline
	render_settings_outline();

	//render status
	render_status("OFF");
	
	//manually flush at then end - otherwise you end up redrawing each time you you call a g* command
	gdispGFlush(gdispGetDisplay(0));


}

//should this be implemented in view somehow?
static View* init(void) {
	statusView.render = render;
	statusView.on_update = on_update;
	statusView.dirty = false;

	LargeNumbers = gdispOpenFont("LargeNumbers");	
	DejaVuSans10 =  gdispOpenFont("DejaVuSans10");
	DejaVuSans20 = gdispOpenFont("DejaVuSans20");
	Fixed_5x8 = gdispOpenFont("fixed_5x8");
	Fixed_7x14 = gdispOpenFont("fixed_7x14");
	
	DisplayWidth = gdispGetWidth();
	DisplayHeight = gdispGetHeight();	

	return &statusView;	
}

const struct statusView StatusView = { 
	.init = init
};