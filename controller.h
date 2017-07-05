#pragma once
#include <stdint.h>
#include "cmsis_os.h"
#include "log.h"

#include "view.h"

typedef struct Controller*Controller;

struct icontroller
{
	void(*init)(void);
	Controller(*make)(uint8_t);
	void(*work)(Controller*);
};

extern const struct icontroller IController;

//void init(void);
//Controller make(uint8_t data);
//void work(Controller a);
