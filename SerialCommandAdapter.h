#pragma once

typedef struct {
	void(*write)(const void* buffer, int size);	
	int(*read)(void *buffer, int size);	
} SerialCommandAdapter;