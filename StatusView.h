#pragma once

#include "view.h"
#include <stdint.h>
#include "log.h"

struct statusView {	
	View*(*init)(void);
};

extern const struct statusView StatusView;

