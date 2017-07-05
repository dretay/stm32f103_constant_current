#pragma once
#include <stdint.h>
#include "cmsis_os.h"
#include "log.h"
#include "view.h"

struct application
{
	void(*init)(void);
	void(*showView)(View);
};

extern const struct application Application;
//typedef struct Application*Application;
//void init(void);
//Application make(uint8_t data);
//void work(Application a);