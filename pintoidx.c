#include "pintoidx.h"

int8_t pintoidx(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
	case GPIO_PIN_0:
		return 0;
		break;
	case GPIO_PIN_1:
		return 1;
		break;
	case GPIO_PIN_2:
		return 2;
		break;
	case GPIO_PIN_3:
		return 3;
		break;
	case GPIO_PIN_4:
		return 4;
		break;
	case GPIO_PIN_5:
		return 5;
		break;
	case GPIO_PIN_6:
		return 6;
		break;
	case GPIO_PIN_7:
		return 7;
		break;
	case GPIO_PIN_8:
		return 8;
		break;
	case GPIO_PIN_9:
		return 9;
		break;
	case GPIO_PIN_10:
		return 10;
		break;
	case GPIO_PIN_11:
		return 11;
		break;
	case GPIO_PIN_12:
		return 12;
		break;
	case GPIO_PIN_13:
		return 13;
		break;
	case GPIO_PIN_14:
		return 14;
		break;
	case GPIO_PIN_15:
		return 15;
		break;
	default:
		return -1;
		break;
	}
}