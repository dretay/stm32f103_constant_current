#pragma once

#include "gfx.h"
#include <stm32f1xx_hal.h>

static void start_transfer(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); 
}
static void stop_transfer(void) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); 
}
static void send_command(uint8_t cmd) {
	if (HAL_SPI_Transmit_DMA(&hspi1, 0x038, 1) != HAL_OK) {}

	//wait for the xfer to finish
	while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {}
}
static GFXINLINE void init_board(GDisplay* g) {
	(void) g; 
	
	//enable chip
	HAL_Delay(100);
	start_transfer();
	HAL_Delay(10);



	send_command(0x038);            			/* 8 Bit interface (DL=1), basic instruction set (RE=0) */
	send_command(0x008);		                /* display on, cursor & blink off; 0x08: all off */
	send_command(0x006);		                /* Entry mode: Cursor move to right ,DDRAM address counter (AC) plus 1, no shift  */  
	send_command(0x002);		                /* disable scroll, enable CGRAM adress  */
	send_command(0x001);		                /* clear RAM, needs 1.6 ms */
	HAL_Delay(4);								/* delay 4ms */
	stop_transfer();							/* disable chip */
	send_command(0xFF);							/* end of sequence */


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
}
void KS0108_delay(uint16_t microsec) {
	(void) microsec;
}
static GFXINLINE void KS0108_write(uint8_t value) {
	(void) value;
}
static GFXINLINE void KS0108_select(uint8_t chip) {
	(void) chip;
}
static GFXINLINE void KS0108_unselect(void) {
}
#if KS0108_NEED_READ  //Hardware Read ########### needs more testing but my display is broken
static GFXINLINE uint8_t read_KS0108(void) {
}
#endif
static GFXINLINE void write_data(GDisplay* g, uint16_t data) {
	(void) g;
	(void) data;
}
static GFXINLINE void write_cmd(GDisplay* g, uint16_t cmd) {
	(void) g;
	(void) cmd;
}
static GFXINLINE void setreadmode(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
}

static GFXINLINE uint16_t read_data(GDisplay *g) {
	(void) g;
	return 0;
}
