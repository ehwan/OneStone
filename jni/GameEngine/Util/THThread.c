#include <THPrivate.h>
#include <assert.h>

pthread_t newThread(void* (*function)(void*), void* data)
{
	THLog("New Thread Created");
#ifndef NDEBUG
	unsigned int st;
#endif

	pthread_attr_t threadAttr;
	pthread_t threadID;
#ifndef NDEBUG
	st=
#endif
	pthread_attr_init(&threadAttr);
	assert(st==0);
#ifndef NDEBUG
	st=
#endif
	pthread_attr_setdetachstate(&threadAttr,PTHREAD_CREATE_DETACHED);
	assert(st==0);

#ifndef NDEBUG
	st=
#endif
	pthread_create(&threadID,&threadAttr,function,data);
	assert(st==0);

#ifndef NDEBUG
	st=
#endif
	pthread_attr_destroy(&threadAttr);
	assert(st==0);

	return threadID;
}
