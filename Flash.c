#include "Flash.h"



static HAL_StatusTypeDef configure(bool erase) {

	static FLASH_EraseInitTypeDef EraseInitStruct;
	unsigned long page_error;
	if (erase)
	{
		HAL_FLASH_Unlock();	
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
		EraseInitStruct.NbPages     = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

		HAL_FLASHEx_Erase(&EraseInitStruct, &page_error);

		HAL_FLASH_Lock();		
	}

	return page_error;


}
static HAL_StatusTypeDef marshal(unsigned int idx, unsigned int value) {
	unsigned long address;
	HAL_FLASH_Unlock();	

	address = FLASH_USER_START_ADDR + (idx * 4);
	if (address < FLASH_USER_END_ADDR) {
		return HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, value);		
		HAL_FLASH_Lock();
	}
	
	HAL_FLASH_Lock();
	return HAL_ERROR;
}

static __IO unsigned int unmarshal(unsigned int idx) {
	unsigned long address;

	address = FLASH_USER_START_ADDR + (idx * 4);
	
	if (address < FLASH_USER_END_ADDR) {
		return *(__IO unsigned int*)address;
	}	
	
	return 0;
}
static unsigned int record_size(FLASH_RECORD *record) {
	switch (record->val_type)
	{
	case type_float: 
		return sizeof(float);
	case type_int: 
		return sizeof(int);
	case type_uint:
		return sizeof(uint);
	case type_double:
		return sizeof(double);
	case type_bool:
		return sizeof(bool);
	}
}

static HAL_StatusTypeDef set(FLASH_RECORD record) {
	unsigned int i = 0;
	
	for (; i < record_size(&record)/4; i++)
	{
		if (marshal(record.idx++, record.uint_array[i]) != HAL_OK)
		{
			return HAL_ERROR;
		}							
	}
	return HAL_OK;
}
static HAL_StatusTypeDef set_double(unsigned int idx, double value) {
	FLASH_RECORD record;
	record.val_type = type_double;
	record.idx = idx*2;
	record.double_val = value;

	return set(record);
}
static FLASH_RECORD get(unsigned int idx, VALUE_TYPE val_type) {
	unsigned int i = 0;	 
	FLASH_RECORD record;
	record.idx = idx;
	if (val_type == type_double)
	{
		idx *= 2;
	}
	record.val_type = val_type;
	
	for (; i < record_size(&record) / 4; i++) {
		record.uint_array[i] = unmarshal(idx++);
	}

	return record;
}



const struct flash Flash= { 
	.configure = configure,
	.set = set,
	.set_double = set_double,
	.get = get
};
