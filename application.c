#include "application.h"

typedef struct Application{
	int data1;	
}*Application;

osPoolDef(mpool, 3, Application);
osPoolId  mpool;

void init(void) {
	mpool = osPoolCreate(osPool(mpool));

}
Application make(uint8_t data) {
	int size = sizeof(Application);
	Application application = osPoolAlloc(mpool);
	application->data1 = data;
	return application;	
}

void work(Application a) {
	LOG("%d\n", a->data1);
}