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
const uint8_t LcdSetGdramAddress = 0x80;

//const unsigned int LcdCommandDelayMicros = 72 - 24; // 72us required, less 24us time to send the command @ 1MHz
const unsigned int LcdCommandDelayMicros = 2;
const unsigned int LcdDataDelayMicros = 2;// 10;         // Delay between sending data bytes
const unsigned int LcdDisplayClearDelayMillis = 2;  // 1.6ms should be enough

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


uint32_t getUs(void) {
	uint32_t usTicks = HAL_RCC_GetSysClockFreq() / 1000000;
	register uint32_t ms, cycle_cnt;
	do {
		ms = HAL_GetTick();
		cycle_cnt = SysTick->VAL;
	} while (ms != HAL_GetTick());
	return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}
 
void delay_us(uint16_t micros) {
	uint32_t start = getUs();
	while (getUs() - start < (uint32_t) micros) {
		asm("nop");
	}
}

static void enable(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); 
	HAL_Delay(5);
}
static void disable(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); 
	HAL_Delay(5);

}

static void reset_display(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
	HAL_Delay(5);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); 
	HAL_Delay(50);
}
void sendLcd(uint8_t data1, uint8_t data2) {
	//SPI_HandleTypeDef* hspi1 = (SPI_HandleTypeDef*)get_hspi1();
	uint8_t data[] = { data1, (data2 & 0xF0), (data2 << 4 ) };
	HAL_SPI_Transmit(&hspi1, data, 3, 1);
	while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY);
	
}
void sendLcdCommand(uint8_t command) {
	sendLcd(0xF8, command);
}
void sendLcdData(uint8_t command) {
	sendLcd(0xFA, command);
}
void commandDelay() {
	delay_us(LcdCommandDelayMicros);
}

void setGraphicsAddress(unsigned int r, unsigned int c) {
	sendLcdCommand(LcdSetGdramAddress | (r & 31));
	//commandDelay();  // don't seem to need this one
	sendLcdCommand(LcdSetGdramAddress | c | ((r & 32) >> 2));
	commandDelay();    // we definitely need this one
}

void flush() {
	if (endCol > startCol && endRow > startRow) {
		uint8_t startColNum = startCol / 16;
		uint8_t endColNum = (endCol + 15) / 16;
		for (uint8_t r = startRow; r < endRow; ++r) {
			setGraphicsAddress(r, startColNum);
			uint8_t *ptr = image + ((16 * r) + (2 * startColNum));
			for (uint8_t i = startColNum; i < endColNum; ++i) {      
				sendLcdData(*ptr++);
				//commandDelay();    // don't seem to need a delay here
				sendLcdData(*ptr++);
				//commandDelay();    // don't seem to need as long a delay as this
				delay_us(LcdDataDelayMicros);
			}
		}
		startRow = numRows;
		startCol = numCols;
		endCol = endRow = 0;
	}
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
	delay_us(1);
	sendLcdCommand(LcdFunctionSetBasicAlpha);
	commandDelay();
	sendLcdCommand(LcdEntryModeSet);
	commandDelay();
	
	sendLcdCommand(LcdFunctionSetExtendedGraphic);
	clear();
	
	sendLcdCommand(LcdDisplayOn);
	commandDelay();

	disable();
	

}
static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}
static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	(void) state;
}
static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
	flush();
}
void KS0108_delay(uint16_t microsec) {
	(void) microsec;
}

void gdisp_lld_flush(GDisplay *g){
	(void) g;
	enable();
	flush();
	disable();
}
static GFXINLINE void write_data(GDisplay* g, uint16_t data) {
	(void) g;
	(void) data;
}
static GFXINLINE void write_cmd(GDisplay* g, uint16_t cmd) {
	(void) g;
	(void) cmd;
}
