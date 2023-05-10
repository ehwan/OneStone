#include <THPrivate.h>
#include <malloc.h>
#include <assert.h>

extern THFrame* currentFrame;
static void DefaultTouch(AInputEvent* event,int32_t action,float x,float y){}
static void DefaultEnterFrame(){}

THFrame* newFrame(const unsigned int objectListCount,const unsigned int buttonlistCount)
{
	THFrame* frame=(THFrame*)malloc(sizeof(THFrame));
	assert(frame);

	THFrameInit(frame,objectListCount,buttonlistCount);

	return frame;
}
void THFrameInit(THFrame* frame,const unsigned int objectListCount,const unsigned int buttonlistCount)
{
	THArrayInit(&frame->objectList,objectListCount);
	THArrayInit(&frame->buttonList,buttonlistCount);

	frame->onTouchEvent=DefaultTouch;
	frame->onBackReleased=0;
	frame->onEnterFrame=DefaultEnterFrame;
	frame->action=0;
	frame->onChangeFrame=0;

	frame->canTouch=1;
}
void THFrameFree(THFrame* frame)
{
	THArrayFree(&frame->objectList);
	THArrayFree(&frame->buttonList);
}
void _THFrameAddChild(THFrame* frame,THDisplayObject* displayObject)
{
	THError(displayObject->frame!=0?"AddChild To Another Frame!":"");
	assert(displayObject->frame==0);
	
	THArrayPush(&frame->objectList,displayObject);
	displayObject->frame=frame;
}
void THFrameAddButton(THFrame* frame,THButton* btn)
{
	THError(btn->frame!=0?"AddButton To Another Frame!":"");
	assert(btn->frame==0);

	THArrayPush(&frame->buttonList,btn);
	btn->frame=frame;
}
void _THFrameRemoveChild(THFrame* frame,THDisplayObject* object)
{
	THError(object->frame!=frame?"RemoveChild From Another frame!!":"");
	assert(object->frame==frame);

	object->frame=0;
	THArrayDeleteObj(&frame->objectList,object);
}
void THFrameRemoveButton(THFrame* f,THButton* btn)
{
	THError(btn->frame!=f?"RemoveButton From Another frame!!":"");
	assert(btn->frame==f);

	btn->frame=0;
	THArrayDeleteObj(&f->buttonList,btn);
}

void GoFrame(THFrame* f,void* data)
{
	if(currentFrame){if(currentFrame->onChangeFrame){currentFrame->onChangeFrame();}}
	if(f->action){f->action(currentFrame,data);}									 
	currentFrame = f;
}