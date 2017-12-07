#pragma once

#include "gfx.h"
#include "main.h"
#include "stdbool.h"
#include <stm32f1xx_hal.h>
#include "string.h"

// LCD basic instructions. These all take 72us to execute except LcdDisplayClear, which takes 1.6ms
const uint8_t LcdDisplayClear = 0x01;
const uint8_t LcdHome = 0x02;
const uint8_t LcdEntryModeSet = 0x06;       // move cursor right and indcement address when writing data
const uint8_t LcdDisplayOff = 0x08;
const uint8_t LcdDisplayOn = 0x0C;          // add 0x02 for cursor on and/or 0x01 for cursor blink on
const uint8_t LcdFunctionSetBasicAlpha = 0x20;
const uint8_t LcdFunctionSetBasicGraphic = 0x22;
const uint8_t LcdFunctionSetExtendedAlpha = 0x24;
const uint8_t LcdFunctionSetExtendedGraphic = 0x26;
const uint8_t LcdSetDdramAddress = 0x80;    // add the address we want to set


// LCD extended instructions
const uint8_t LcdDisplayStandby = 0x01;
const uint8_t LcdSetGdramAddress = 0x80;

const unsigned int numRows = 64;
const unsigned int numCols = 128;

enum PixelMode {
	PixelClear = 0,    // clear the pixel(s)
	PixelSet   = 1,      // set the pixel(s)
	PixelFlip  = 2      // invert the pixel(s)
};


uint8_t row, column;
uint8_t startRow, startCol, endRow, endCol; // coordinates of the dirty rectangle
uint8_t image[(128 * 64) / 8];       

extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim3;

//this is super ghetto and basically the count has to be tweaked 
//per spi prescaler / fsb combo 
void delay_2us() {	
	uint8_t i = 0;
	for (; i < 120; i++)
	{
		asm("nop");		
	}
}

static void enable(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); 
}
static void disable(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); 
}

static void reset_display(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
	HAL_Delay(5);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); 
	HAL_Delay(50);
}
void sendLcd(uint8_t data1, uint8_t data2) {
	uint8_t data[] = { data1, (data2 & 0xF0), (data2 << 4 ) };
	HAL_SPI_Transmit(&hspi1, data, 3, 1);
	while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY) {
		LOG("BUSY!");
	};
	
}
void sendLcdCommand(uint8_t command) {
	sendLcd(0xF8, command);
}
void sendLcdData(uint8_t command) {
	sendLcd(0xFA, command);
}

void setGraphicsAddress(unsigned int r, unsigned int c) {
	sendLcdCommand(LcdSetGdramAddress | (r & 31));
	//commandDelay();  // don't seem to need this one
	sendLcdCommand(LcdSetGdramAddress | c | ((r & 32) >> 2));
	delay_2us();    // we definitely need this one
}

void flush() {
	uint8_t startColNum;
	uint8_t endColNum;
	uint8_t row;
	uint8_t col;
	uint8_t *ptr;
	enable();
	if (endCol > startCol && endRow > startRow) {
		startColNum = startCol / 16;
		endColNum = (endCol + 15) / 16;
		row = startRow;		
		for (; row < endRow; ++row) {
			__disable_irq();
			setGraphicsAddress(row, startColNum);
			ptr = image + ((16 * row) + (2 * startColNum));
			for (col = startColNum; col < endColNum; ++col) {      				
				sendLcdData(*ptr++);
				sendLcdData(*ptr++);
				delay_2us();				
			}
			__enable_irq();
		}
		startRow = numRows;
		startCol = numCols;
		endCol = endRow = 0;
	}
	disable();
}
void clear() {
	memset(image, 0, sizeof(image));
	// flag whole image as dirty and update
	startRow = 0;
	endRow = numRows;
	startCol = 0;
	endCol = numCols;
	flush();
}

void setPixel(uint8_t x, uint8_t y, enum PixelMode mode) {
	if (y < numRows && x < numCols) {
		uint8_t *p = image + ((y * (numCols / 8)) + (x / 8));
		uint8_t mask = 0x80u >> (x % 8);
		switch (mode) {
		case PixelClear:
			*p &= ~mask;
			break;
		case PixelSet:
			*p |= mask;
			break;
		case PixelFlip:
			*p ^= mask;
			break;
		}
    
		// Change the dirty rectangle to account for a pixel being dirty (we assume it was changed)
		if (startRow > y) { startRow = y; }
		if (endRow <= y)  { endRow = y + 1; }
		if (startCol > x) { startCol = x; }
		if (endCol <= x)  { endCol = x + 1; }
	}
}
static GFXINLINE void init_board(GDisplay* g) {
	(void) g; 
	
	reset_display();
	enable();
		
	sendLcdCommand(LcdFunctionSetBasicAlpha);
	delay_2us();
	sendLcdCommand(LcdFunctionSetBasicAlpha);
	delay_2us();
	sendLcdCommand(LcdEntryModeSet);
	delay_2us();
	
	sendLcdCommand(LcdFunctionSetExtendedGraphic);
	clear();
	
	sendLcdCommand(LcdDisplayOn);
	delay_2us();

	disable();
	

}
static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

#if GDISP_NEED_CONTROL
static void board_backlight(GDisplay *g, uint8_t percent) {
	// TODO: Can be an empty function if your hardware doesn't support this
	(void) g;
	(void) percent;
}

static void board_contrast(GDisplay *g, uint8_t percent) {
	// TODO: Can be an empty function if your hardware doesn't support this
	(void) g;
	(void) percent;
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, (htim3.Init.Period / 100) * percent);
}

static void board_power(GDisplay *g, powermode_t pwr) {
	// TODO: Can be an empty function if your hardware doesn't support this
	(void) g;
	(void) pwr;
}
#endif /* GDISP_NEED_CONTROL */

void gdisp_lld_flush(GDisplay *g){
	(void) g;	
	flush();	
}