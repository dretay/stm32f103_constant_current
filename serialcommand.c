#include "serialcommand.h"

static UsbSerialCommand_Command commands[USB_SERIAL_COMMAND_CNT];

uint32_t BufPtrOut = 0;
char UartRxBuffer[RX_DATA_SIZE];
volatile uint32_t BufPtrIn = 0;                          
char* last, *token;
SerialCommandAdapter *serial_adapter;


static void clear_buffer(void) {
	uint8_t i;
	for (i = 0; i < RX_DATA_SIZE; i++) {
		UartRxBuffer[i] = ' ';
	}
	BufPtrIn = 0;
	BufPtrOut = 0;
}
static void process_command(void) {
	uint8_t i;

	token = strtok_r(UartRxBuffer, " ", &last);
	for (i = 0; i < USB_SERIAL_COMMAND_CNT; i++) {
		if (strncmp(token, commands[i].command, strcspn(token, "\r")) == 0) {
			commands[i].function();	
		}
	}
}
void StartSerialCmdTask(void const * argument) {	
	
	// Don't clear bits on entry., Clear all bits on exit., Stores the notified value.
	xTaskNotifyWait(pdFALSE, ULONG_MAX, NULL, osWaitForever);

	char byte;
	uint32_t buffptr;
	uint32_t buffsize;
	uint8_t i;
	const static char NEWLINE = '\n';

	while (1) {
		if (serial_adapter->read(&byte, 1) != 1)
			continue;
		serial_adapter->write(&byte, 1);

		//increment the "next" pointer
		UartRxBuffer[BufPtrIn++] = byte;

		//wrap the ring buffer if we've overflowed
		if (BufPtrIn == RX_DATA_SIZE) {
			BufPtrIn = 0;
		}


		if (BufPtrOut != BufPtrIn) {
			if (BufPtrOut > BufPtrIn) /* rollback */ {
				buffsize = RX_DATA_SIZE - BufPtrOut;
			}
			else {
				buffsize = BufPtrIn - BufPtrOut;
			}

			buffptr = BufPtrOut;

			BufPtrOut += buffsize;
			if (BufPtrOut == RX_DATA_SIZE) {
				BufPtrOut = 0;
			}
			for (i = 0; i < buffsize; i++) {
				//user has finished entering their command - let's show them what they've won
				if (UartRxBuffer[buffptr + i] == '\r') {
					serial_adapter->write(&NEWLINE, 1);						
					process_command();		
					clear_buffer();	
				}			
			}
		}
#ifdef INCLUDE_uxTaskGetStackHighWaterMark
		SerialCmdTask_Watermark = uxTaskGetStackHighWaterMark(NULL);
#endif
		osThreadYield();
	}	
}

static void configure(SerialCommandAdapter* serial_adapter_in) {
	serial_adapter = serial_adapter_in;
	xTaskNotify(serialCmdTaskHandle, 0x01, eSetBits);

}
static size_t my_strlcpy(char *dst, const char *src, size_t size) {
	size_t bytes = 0;
	char *q = dst;
	const char *p = src;
	char ch;

	while ((ch = *p++)) {
		if (bytes + 1 < size)
			*q++ = ch;

		bytes++;
	}

	/* If size == 0 there is no space for a final null... */
	if (size)
		*q = '\0';

	return bytes;
}
static void register_command(uint8_t idx, char* command_in, void* function_in) {
	if (idx <= USB_SERIAL_COMMAND_CNT)
	{	
		strncpy(commands[idx].command , command_in, 8);
		commands[idx].function = function_in;
		
	}
}
static char* next() {
	char *nextToken;
	nextToken = strtok_r(NULL, " ", &last); 
	return nextToken; 
}
const struct serialcommand SerialCommand = { 
	.register_command = register_command,
	.configure = configure,	
	.next = next	
};