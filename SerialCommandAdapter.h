#pragma once
#include <string.h>
#include <stdbool.h>
typedef struct {
	void(*write)(const void* buffer, int size);	
	int(*read)(void *buffer, int size);	
	bool echo;
} SerialCommandAdapter;