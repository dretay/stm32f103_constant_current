#include "UsbSerialCommandAdapter.h"

#ifdef HAL_PCD_MODULE_ENABLED

USBD_HandleTypeDef USBD_Device;
void SysTick_Handler(void);
void OTG_FS_IRQHandler(void);
void OTG_HS_IRQHandler(void);
extern PCD_HandleTypeDef hpcd;
extern char g_VCPInitialized;

static SerialCommandAdapter adapter;


int VCP_read(void *pBuffer, int size) {
//	if (!s_RxBuffer.ReadDone)
//		return 0;
//
//	int remaining = s_RxBuffer.Size - s_RxBuffer.Position;
//	int todo = MIN(remaining, size);
//	if (todo <= 0)
//		return 0;
//
//	memcpy(pBuffer, s_RxBuffer.Buffer + s_RxBuffer.Position, todo);
//	s_RxBuffer.Position += todo;
//	if (s_RxBuffer.Position >= s_RxBuffer.Size) {
//		s_RxBuffer.ReadDone = 0;
//		USBD_CDC_ReceivePacket(&USBD_Device);
//	}
//
//	return todo;
}

#ifdef USE_USB_HS
enum { kMaxOutPacketSize = CDC_DATA_HS_OUT_PACKET_SIZE };
#else
enum { kMaxOutPacketSize = CDC_DATA_FS_OUT_PACKET_SIZE };
#endif

int VCP_write(const void *pBuffer, int size) {
	if (size > kMaxOutPacketSize) {
		int offset;
		int done = 0;
		for (offset = 0; offset < size; offset += done) {
			int todo = MIN(kMaxOutPacketSize, size - offset);
			done = VCP_write(((char *)pBuffer) + offset, todo);
			if (done != todo)
				return offset + done;
		}

		return size;
	}

	USBD_CDC_HandleTypeDef *pCDC =
	        (USBD_CDC_HandleTypeDef *)USBD_Device.pClassData;
	while (pCDC->TxState) {} //Wait for previous transfer

	USBD_CDC_SetTxBuffer(&USBD_Device, (uint8_t *)pBuffer, size);
	if (USBD_CDC_TransmitPacket(&USBD_Device) != USBD_OK)
		return 0;

	while (pCDC->TxState) {} //Wait until transfer is done
	return size;
}


static int read(char *buffer, int size) {
	return VCP_read(buffer, size);
}
static void write(const void* buffer, int size) {
	VCP_write(buffer, size);
}

static SerialCommandAdapter* configure(void) {
	adapter.read = read;
	adapter.write = write;
	
	USBD_Device.pDesc = &FS_Desc;
	USBD_Device.dev_state  = USBD_STATE_DEFAULT;
	USBD_Device.id = 0;
	hpcd_USB_FS.pData = &USBD_Device;
	USBD_Device.pData = &hpcd_USB_FS;

	HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x00, PCD_SNG_BUF, 0x18);
	HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x80, PCD_SNG_BUF, 0x58);
	HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x81, PCD_SNG_BUF, 0x100);

	USBD_RegisterClass(&USBD_Device, &USBD_CDC);
	USBD_CDC_RegisterInterface(&USBD_Device, &USBD_Interface_fops_FS);
	USBD_Start(&USBD_Device);

	return &adapter;

}



const struct usbserialcommand UsbSerialCommandAdapter= { 
	.configure = configure,	
};
#endif