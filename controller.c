#include "controller.h"

//osPoolDef(mpool, 3, Controller);
//static osPoolId  mpool;
//
//typedef struct Controller {
//	int data1;	
//}*Controller;
//
//Controller make(uint8_t data) {
//	int size = sizeof(Controller);
//	Controller controller = osPoolAlloc(mpool);
//	controller->data1 = data;
//	return controller;	
//}
//
//const struct icontroller IController = { 
//	.make = make	
//};