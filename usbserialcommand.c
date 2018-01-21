#include "usbserialcommand.h"

static UsbSerialCommand_Command commands[USB_SERIAL_COMMAND_CNT];

USBD_HandleTypeDef USBD_Device;
void SysTick_Handler(void);
void OTG_FS_IRQHandler(void);
void OTG_HS_IRQHandler(void);
extern PCD_HandleTypeDef hpcd;
int VCP_read(void *pBuffer, int size);
int VCP_write(const void *pBuffer, int size);
extern char g_VCPInitialized;
uint32_t BufPtrOut = 0;
char UartRxBuffer[RX_DATA_SIZE];
volatile uint32_t BufPtrIn = 0;                          
char* last, *token;


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
		if (strncmp(token, commands[i].command, strlen(commands[i].command)) == 0) {
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

	while (1) {
		if (VCP_read(&byte, 1) != 1)
			continue;
		VCP_write(&byte, 1);
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
					process_command();		
					clear_buffer();							
				}			
			}
		}
		osThreadYield();
	}	
}

static void configure(void) {
	
	USBD_Device.pDesc = &VCP_Desc;
	USBD_Device.dev_state  = USBD_STATE_DEFAULT;
	USBD_Device.id = 0;
	hpcd_USB_FS.pData = &USBD_Device;
	USBD_Device.pData = &hpcd_USB_FS;

	HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x00, PCD_SNG_BUF, 0x18);
	HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x80, PCD_SNG_BUF, 0x58);
	HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x81, PCD_SNG_BUF, 0x100);

	USBD_RegisterClass(&USBD_Device, &USBD_CDC);
	USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_stm32usbdemo_fops);
	USBD_Start(&USBD_Device);

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
const struct usbserialcommand UsbSerialCommand = { 
	.register_command = register_command,
	.configure = configure,	
	.next = next	
};