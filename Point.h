#pragma once
#include "Object.h"

extern osPoolId  pointClassPool;

extern const void * Point;			/* new(Point, x, y); */

void draw(const void * self);
void move(void * point, int dx, int dy);

extern const void * PointClass;		/* adds draw */

void initPoint(void);
