#ifndef THKim_THStartFrame
#define THKim_THStartFrame

#include <THPrivate.h>

void StartFrameAction(THFrame* frame,void* data);
void StartFrameOut();
void StartFrameTouch(AInputEvent* in,int32_t action,float x,float y);



#endif