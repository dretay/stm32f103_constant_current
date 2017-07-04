#pragma once
#include <stdint.h>
#include "cmsis_os.h"
#include "log.h"

//based on https://stackoverflow.com/questions/26205329/interface-implementation-in-ansi-c
typedef struct Application*Application;
void init(void);
Application make(uint8_t data);
void work(Application a);