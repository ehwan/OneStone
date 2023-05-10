#ifndef THKim_THLevelFrame
#define THKim_THLevelFrame

#include <THPrivate.h>

void LevelFrameAction(THFrame* frame,void* data);
void LevelFrameOut();
void LevelFrameTouch(AInputEvent* in,int32_t action,float x,float y);
int LevelFrameBack();

#endif