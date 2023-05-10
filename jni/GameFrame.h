#ifndef THKim_THGameFrame
#define THKim_THGameFrame

#include <THPrivate.h>

#define SMALLSTONER 40
#define BIGSTONER 60
#define SMALLPLUSBIGR 100

void GameFrameAction(THFrame* frame,void* data);
void GameFrameOut();
void GameFrameTouch(AInputEvent* in,int32_t action,float x,float y);
int GameFrameBack();

#endif