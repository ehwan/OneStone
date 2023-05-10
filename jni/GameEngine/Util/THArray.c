#include <THPrivate.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>

void THArrayInit(THArray* arr,const unsigned int num)
{
	arr->num = 0;
	arr->max = num;
	arr->arr = (void **)malloc(num*sizeof(void*));
	assert(arr->arr);
}
THArray* THArrayNew(const unsigned int num)
{
	THArray *arr = (THArray *)malloc(sizeof(THArray));
	assert(arr);
	
	THArrayInit(arr,num);
	
	return arr;
}
void THArrayPush(THArray *arr, void *object)
{
	if(arr->num==arr->max)
	{
		THError("THArray : ReAlloced()");
		arr->max<<=1;
		arr->arr = (void **)realloc(arr->arr, arr->max*sizeof(void*));
		assert(arr->arr);
	}
	arr->arr[arr->num] = object;
	++arr->num;
}
void THArrayDeleteObj(THArray *arr, void *obj)
{
	unsigned int i=arr->num;
	void** list=arr->arr;
	while(i)
	{
		if(list[--i]==obj)
		{
			--arr->num;
			memmove(list+i,list+(i+1),(arr->num-i)*sizeof(void*));
			return;
		}
	}
}
void THArrayRePush(THArray* arr,void* obj)
{
	const unsigned int oc=arr->num-1;
	unsigned int i=arr->num;
	void** list=arr->arr;
	while(i)
	{
		if(list[--i]==obj)
		{
			memmove(list+i,list+(i+1),(oc-i)*sizeof(void*));
			list[oc]=obj;
			return;
		}
	}
}
void THArrayRefresh(THArray* arr,const unsigned int num)
{
	arr->num=0;
	if(num!=arr->max)
	{
		THError("THArray : Refresh With Different Number");
		arr->max=num;
		free(arr->arr);
		arr->arr=(void**)malloc(num*sizeof(void*));
		assert(arr->arr);
	}
}
