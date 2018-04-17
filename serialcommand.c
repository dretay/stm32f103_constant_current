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
			break;
		}
	}
}
void StartSerialCmdTask(void const * argument) {	
	
	// Don't clear bits on entry., Clear all bits on exit., Stores the notified value.
	xTaskNotifyWait(pdFALSE, ULONG_MAX, NULL, osWaitForever);

	osEvent event;
	T_SERCMD_UPDATE *update;
	char byte;
	char *bytes;
	uint32_t buffptr;
	uint32_t buffsize;
	uint8_t i;
	const static char NEWLINE = '\n';


	while (1) {
#ifdef INCLUDE_uxTaskGetStackHighWaterMark
		SerialCmdTask_Watermark = uxTaskGetStackHighWaterMark(NULL);
#endif

		event = osMailGet(SERCMD_UPDATE_MAILBOX_ID, osWaitForever);
		if (event.status != osEventMail)
		{
			continue;
		}
		update = event.value.p;
		bytes = update->string;
		//serial_adapter->write(update->string, update->size);
		for (i = 0; i < update->size; i++) {
			byte = bytes[i];
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
			
			}
		}
		for (i = 0; i < buffsize; i++) {
			//user has finished entering their command - let's show them what they've won
			if (UartRxBuffer[buffptr + i] == '\r' || UartRxBuffer[buffptr + i] == '\n') {								
				process_command();		
				clear_buffer();	
			}			
		}
		osMailFree(SERCMD_UPDATE_MAILBOX_ID, update);
		

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
static void register_command(int idx, char* command_in, void* function_in) {
	if (idx <= USB_SERIAL_COMMAND_CNT)
	{	
		strncpy(commands[idx].command , command_in, 8);
		commands[idx].function = function_in;
		
	}
}
//spit out space separated tokens that were arguments to functions
//once end is reached continually output the last token
//strip out line feed information from tokens that are returned
static char* next() {
	char *nextToken;
	int eol_idx;
	nextToken = strtok_r(NULL, " ", &last); 
	if (nextToken == NULL)
	{
		return last;
	}
	
	eol_idx = strcspn(nextToken, "\r\n");
	if (eol_idx != strlen(nextToken))
	{
		nextToken[eol_idx] = '\0';
	}
	return nextToken; 
}
static int next_int() {
	return atoi(SerialCommand.next());	
}
static float next_float() {
	return strtof(SerialCommand.next(), NULL);	
}
static double next_double() {
	return atof(SerialCommand.next());	
}
static void echo(char* output, int length) {
	serial_adapter->write(output, length);
}
const struct serialcommand SerialCommand = { 
	.register_command = register_command,
	.configure = configure,	
	.next = next,	
	.next_int = next_int,	
	.next_float = next_float,	
	.next_double = next_double,	
	.echo = echo
};