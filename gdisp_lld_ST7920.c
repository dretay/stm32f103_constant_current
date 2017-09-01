#include "gfx.h"

#if GFX_USE_GDISP



#define GDISP_DRIVER_VMT			GDISPVMT_ST7920
#include "gdisp_lld_config.h"
#include "../ugfx/src/gdisp/gdisp_driver.h"
#include "board_ST7920.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/


#ifndef GDISP_SCREEN_HEIGHT
#define GDISP_SCREEN_HEIGHT		64
#endif
#ifndef GDISP_SCREEN_WIDTH
#define GDISP_SCREEN_WIDTH		128
#endif

#ifndef GDISP_INITIAL_CONTRAST
#define GDISP_INITIAL_CONTRAST	95
#endif
#ifndef GDISP_INITIAL_BACKLIGHT
#define GDISP_INITIAL_BACKLIGHT	100
#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {

	// Initialise the board interface
	init_board(g);

	// Finish Init
	post_init_board(g);

	// Initialise the GDISP structure
	g->g.Width = GDISP_SCREEN_WIDTH;
	g->g.Height = GDISP_SCREEN_HEIGHT;
	g->g.Orientation = GDISP_ROTATE_0;
	g->g.Powermode = powerOn;
	g->g.Backlight = GDISP_INITIAL_BACKLIGHT;
	g->g.Contrast = GDISP_INITIAL_CONTRAST;

	return TRUE;
}

LLDSPEC void gdisp_lld_draw_pixel(GDisplay *g) {
	coord_t x, y;
	#if GDISP_NEED_CONTROL
		switch (g->g.Orientation) {
		default:
		case GDISP_ROTATE_0:
			x = g->p.x;
			y = g->p.y;
			break;
		case GDISP_ROTATE_90:
			x = g->p.y;
			y = g->g.Width - g->p.x - 1;
			break;
		case GDISP_ROTATE_180:
			x = g->g.Width  - g->p.x - 1;
			y = g->g.Height - g->p.y - 1;
			break;
		case GDISP_ROTATE_270:
			x = g->g.Height - g->p.y - 1;
			y = g->p.x;
			break;
		}
	#else
		x = g->p.x;
		y = g->p.y;
	#endif
	setPixel(x, y, g->p.color == White ? PixelSet : PixelClear);
}

#if GDISP_NEED_CONTROL
LLDSPEC void gdisp_lld_control(GDisplay *g) {
	switch (g->p.x) {
	case GDISP_CONTROL_POWER:
		if (g->g.Powermode == (powermode_t)g->p.ptr)
			return;
		switch ((powermode_t)g->p.ptr) {
		case powerOff:			
			sendLcdCommand(LcdDisplayStandby);			
		case powerSleep:
			break;
		case powerDeepSleep:			
			break;
		case powerOn:			
			break;
		default:
			return;
		}
		g->g.Powermode = (powermode_t)g->p.ptr;
		return;

	case GDISP_CONTROL_ORIENTATION:
		if (g->g.Orientation == (orientation_t)g->p.ptr)
			return;
		switch ((orientation_t)g->p.ptr) {
		case GDISP_ROTATE_0:
		case GDISP_ROTATE_180:
			if (g->g.Orientation == GDISP_ROTATE_90 || g->g.Orientation == GDISP_ROTATE_270) {
				coord_t		tmp;

				tmp = g->g.Width;
				g->g.Width = g->g.Height;
				g->g.Height = tmp;
			}
			break;
		case GDISP_ROTATE_90:
		case GDISP_ROTATE_270:
			if (g->g.Orientation == GDISP_ROTATE_0 || g->g.Orientation == GDISP_ROTATE_180) {
				coord_t		tmp;

				tmp = g->g.Width;
				g->g.Width = g->g.Height;
				g->g.Height = tmp;
			}
			break;
		default:
			return;
		}
		g->g.Orientation = (orientation_t)g->p.ptr;
		return;

	case GDISP_CONTROL_BACKLIGHT:
		return;

	case GDISP_CONTROL_CONTRAST:
		if ((unsigned)g->p.ptr > 100) g->p.ptr = (void *)100;
		board_contrast(g, (unsigned)g->p.ptr);
		g->g.Contrast = (unsigned)g->p.ptr;
		return;
	}
}
#endif /* GDISP_NEED_CONTROL */

#endif /* GFX_USE_GDISP */
