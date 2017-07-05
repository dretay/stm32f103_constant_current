#include "view.h"

//osPoolDef(mpool, 3, View);
//osPoolId  mpool;
//
//typedef struct View {
//	int data1;	
//}*View;
//
//void render(void) {
//}
//
//View make(uint8_t data) {
//	int size = sizeof(View);
//	View view = osPoolAlloc(mpool);
//	view->data1 = data;
//	return view;	
//}
//
//const struct iview IView = { 
//	.render = render,
//	.make = make
//};