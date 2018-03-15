#include "UsbSerialCommandAdapter.h"

#ifdef HAL_PCD_MODULE_ENABLED

USBD_HandleTypeDef USBD_Device;
void SysTick_Handler(void);
void OTG_FS_IRQHandler(void);
void OTG_HS_IRQHandler(void);
extern PCD_HandleTypeDef hpcd;
int VCP_read(void *pBuffer, int size);
int VCP_write(const void *pBuffer, int size);
extern char g_VCPInitialized;

static SerialCommandAdapter adapter;

static int read(void *buffer, int size) {
	return VCP_read(buffer, size);
}
static void write(const void* buffer, int size) {
	VCP_write(buffer, size);
}

static SerialCommandAdapter* configure(void) {
	adapter.read = read;
	adapter.write = write;
	
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

	return &adapter;

}



const struct usbserialcommand UsbSerialCommandAdapter= { 
	.configure = configure,	
};
#endif