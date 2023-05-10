#include <StartFrame.h>
#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <OneStone.h>

extern const GLfloat fullVertex[8];
MAKE_CENTER_VERTEX_CONST(logoVertex,322.0f,142.0f);
MAKE_CENTER_VERTEX_CONST(touchVertex,203.0f,31.65f);

extern THFrame startFrame;
extern THFrame levelSelectFrame;

extern GLuint glGameTexture;
extern GLuint glGameTexture2;
extern THTexture* backGroundTexture;

extern THTexture* logoTexture;
extern THTexture* touchAnywhere;

THMovieClip* bgClip;
THColorClip* bgaClip;
THMovieClip* logoClip;
THMovieClip* touchClip;

static void onEndTween(THTween* tween)
{
	if(tween->data==0)
	{
		startFrame.canTouch=1;
		return;
	}
	extern unsigned int curPageInd;
	curPageInd=0;
	GoFrame(&levelSelectFrame,0);
}
void StartFrameAction(THFrame* frame,void* data)
{
	startFrame.canTouch=0;

	bgClip=newMovieClip(backGroundTexture,fullVertex);
	logoClip=newMovieClip(logoTexture,logoVertex);
	touchClip=newMovieClip(touchAnywhere,touchVertex);
	bgaClip=newColorClip(1.0f,fullVertex);

	logoClip->x=360.0f;
	logoClip->y=250.0f;
	touchClip->x=360.0f;
	touchClip->y=1050.0f;

	THFrameAddChild(&startFrame,bgClip);
	THFrameAddChild(&startFrame,logoClip);
	THFrameAddChild(&startFrame,touchClip);
	THFrameAddChild(&startFrame,bgaClip);

	THTween* tween=AddTween2(&(bgaClip->a),0.0f,BLACKIN_SEC,0.0f);
	tween->data=0;
	tween->onEndTween=onEndTween;
}
void StartFrameOut()
{
	THArrayRefresh(&startFrame.objectList,START_FRAME_OBJECT_COUNT);
	THArrayRefresh(&startFrame.buttonList,START_FRAME_BUTTON_COUNT);

	free(bgClip);
	free(bgaClip);
	free(logoClip);
	free(touchClip);
}
void StartFrameTouch(AInputEvent* in,int32_t action,float x,float y)
{
	if(action==AMOTION_EVENT_ACTION_UP)
	{
		startFrame.canTouch=0;
		THTween* tween=AddTween2(&(bgaClip->a),1.0f,BLACKOUT_SEC,0.0f);
		tween->data=1;
		tween->onEndTween=onEndTween;
	}
}