#include "usbserialcommand.h"

static UsbSerialCommand_Config* config;
static uint8_t command_cnt;

USBD_HandleTypeDef USBD_Device;
void SysTick_Handler(void);
void OTG_FS_IRQHandler(void);
void OTG_HS_IRQHandler(void);
extern PCD_HandleTypeDef hpcd;
int VCP_read(void *pBuffer, int size);
int VCP_write(const void *pBuffer, int size);
extern char g_VCPInitialized;

static void configure(UsbSerialCommand_Config* config_in, uint8_t cnt_in) {
	config = config_in;
	command_cnt = cnt_in;

	

    /* USER CODE BEGIN SysInit */

	USBD_Device.pDesc = &VCP_Desc;
	USBD_Device.dev_state  = USBD_STATE_DEFAULT;
	USBD_Device.id = 0;
	hpcd_USB_FS.pData = &USBD_Device;
	USBD_Device.pData = &hpcd_USB_FS;

	/* USER CODE BEGIN 2 */
	HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x00, PCD_SNG_BUF, 0x18);
	HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x80, PCD_SNG_BUF, 0x58);
	HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x81, PCD_SNG_BUF, 0x100);

	USBD_RegisterClass(&USBD_Device, &USBD_CDC);
	USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_stm32usbdemo_fops);
	USBD_Start(&USBD_Device);

	xTaskNotify(serialCmdTaskHandle, 0x01, eSetBits);

}

void StartSerialCmdTask(void const * argument) {	
	
	// Don't clear bits on entry., Clear all bits on exit., Stores the notified value.
	xTaskNotifyWait(pdFALSE, ULONG_MAX, NULL, osWaitForever);

	/* USER CODE BEGIN 5 */
	char byte;

	while (1) {
		if (VCP_read(&byte, 1) != 1)
			continue;
		VCP_write("\r\nYou typed ", 12);
		VCP_write(&byte, 1);
		VCP_write("\r\n", 2);
		osThreadYield();
	}	
}

const struct usbserialcommand UsbSerialCommand = { 
	.configure = configure	
};