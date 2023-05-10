#include <LevelFrame.h>
#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <malloc.h>
#include <assert.h>
#include <GameEngine/Util/THFile.h>
#include <OneStone.h>

extern const GLfloat fullVertex[8];

extern THFrame levelSelectFrame;
extern THFrame startFrame;
extern THFrame gameFrame;

extern GLuint glGameTexture;
extern GLuint glGameTexture2;
extern THTexture* backGroundTexture;

extern THTexture* levelSelectTitle;
extern THTexture* levelSelectPanel;
extern THTexture* levelSelectPanelDown;
extern THTexture* levelSelectPanelCleared;
extern THTexture* levelSelectPanelClearedDown;
extern THTexture* rightTexture;
extern THTexture* leftTexture;
extern THTexture* rightDownTexture;
extern THTexture* leftDownTexture;

extern THTexture* fastNextTexture;
extern THTexture* fastPrevTexture;
extern THTexture* fastNextDownTexture;
extern THTexture* fastPrevDownTexture;

MAKE_CENTER_VERTEX_CONST(levelTitleVer,360.0f,83.333f);
MAKE_CENTER_VERTEX_CONST(levelArrowVer,60.0f,48.0f);
MAKE_CENTER_VERTEX_CONST(levelFastArrowVer,45,36.0f);
MAKE_VERTEX_CONST(levelPanelVer,-80.0f,80.0f,0.0f,160.0f);
//MAKE_CENTER_VERTEX_CONST(levelPanelVertex,80.0f,80.0f);

THMovieClip* bgClip;
THColorClip* bgaClip;

THMovieClip* levelTitle;
THButton* levelSelectButtons[20];
THMovieClip* levelSelectPanels[20];
THNumberClip* levelSelectNumbers[20];
THButton* levelNextPage;
THMovieClip* nextPageClip;
THButton* levelPrevPage;
THMovieClip* prevPageClip;

THButton* levelNextFastPage;
THMovieClip* fastNextClip;
THButton* levelPrevFastPage;
THMovieClip* fastPrevClip;

unsigned int curPageInd;
FILE* levelClearedData=0;


static void onEndTween(THTween* tween)
{
	if(tween->data==0){
		levelSelectFrame.canTouch=1;
		return;
	}else if(tween->data==1){
		GoFrame(&gameFrame,0);
	}else if(tween->data==2){
		GoFrame(&startFrame,0);
		return;
	}
}
static void LevelPanelOnDown(float x,float y,THButton* btn)
{
	THMovieClip* mc=(THMovieClip*)btn->data;
	if(mc->texture==levelSelectPanel)
	{
		mc->texture=levelSelectPanelDown;
		return;
	}
	mc->texture=levelSelectPanelClearedDown;
}
static void LevelPanelOnRelease(float x,float y,THButton* btn)
{
	unsigned int i=20;
	while(i)
	{
		--i;
		if(*(levelSelectButtons+i)==btn)
		{
			THLog("Level Clicked : %d",i+1);
			extern unsigned int curLevelInd;
			curLevelInd=(20*curPageInd)+i;
			levelSelectFrame.canTouch=0;
			THTween* tween=AddTween2(&(bgaClip->a),1.0f,BLACKOUT_SEC,0.0f);
			tween->data=1;
			tween->onEndTween=onEndTween;
			return;
		}
	}
}
static void ArrowDown(float x,float y,THButton* btn)
{
	THMovieClip* mc=(THMovieClip*)btn->data;
	if(mc==nextPageClip)
	{
		mc->texture=rightDownTexture;
		return;
	}
	mc->texture=leftDownTexture;
}
static void ArrowRelease(float x,float y,THButton* btn)
{
	if(btn==levelNextPage)
	{
		if(curPageInd==maxPage-1){return;}
		++curPageInd;
	}
	else
	{
		if(curPageInd==0){return;}
		--curPageInd;
	}

	const unsigned int lvoffset=20*curPageInd;
	char lvd[20];
	THFileMove(levelClearedData,2+lvoffset);
	THFileRead(levelClearedData,lvd,20);

	unsigned int i=20;
	while(i)
	{
		--i;
		THNumberSet(*(levelSelectNumbers+i),lvoffset+i+1);
		
		(*(levelSelectPanels+i))->texture=lvd[i]?levelSelectPanelCleared:levelSelectPanel;
	}
}
static void FastArrowDown(float x,float y,THButton* btn)
{
	THMovieClip* mc=(THMovieClip*)btn->data;
	if(mc==fastNextClip)
	{
		mc->texture=fastNextDownTexture;
		return;
	}
	mc->texture=fastPrevDownTexture;
}
static void FastArrowRelease(float x,float y,THButton* btn)
{
	if(btn==levelNextFastPage)
	{
		curPageInd=maxPage-1;
	}
	else
	{
		curPageInd=0;
	}

	const unsigned int lvoffset=20*curPageInd;
	char lvd[20];
	THFileMove(levelClearedData,2+lvoffset);
	THFileRead(levelClearedData,lvd,20);

	unsigned int i=20;
	while(i)
	{
		--i;
		THNumberSet(*(levelSelectNumbers+i),lvoffset+i+1);
		
		(*(levelSelectPanels+i))->texture=lvd[i]?levelSelectPanelCleared:levelSelectPanel;
	}
}
void LevelFrameAction(THFrame* frame,void* data)
{
	THLog("LevelAction");
	unsigned int i;
	levelSelectFrame.canTouch=0;

	bgClip=newMovieClip(backGroundTexture,fullVertex);
	bgaClip=newColorClip(1.0f,fullVertex);
	levelTitle=newMovieClip(levelSelectTitle,levelTitleVer);
	nextPageClip=newMovieClip(rightTexture,levelArrowVer);
	prevPageClip=newMovieClip(leftTexture,levelArrowVer);

	fastNextClip=newMovieClip(fastNextTexture,levelFastArrowVer);
	fastPrevClip=newMovieClip(fastPrevTexture,levelFastArrowVer);

	levelNextPage=newButton(120.0f,96.0f);
	levelPrevPage=newButton(120.0f,96.0f);
	THButtonSetPosition(levelNextPage,430.0f,1150.0f);
	THButtonSetPosition(levelPrevPage,170.0f,1150.0f);
	levelNextPage->data=nextPageClip;
	levelPrevPage->data=prevPageClip;
	levelNextPage->onDown=ArrowDown;
	levelNextPage->onRelease=ArrowRelease;
	levelPrevPage->onDown=ArrowDown;
	levelPrevPage->onRelease=ArrowRelease;

	levelNextFastPage=newButton(127.0f,96.0f);
	levelPrevFastPage=newButton(127.0f,96.0f);
	THButtonSetPosition(levelNextFastPage,570.0f,1150.0f);
	THButtonSetPosition(levelPrevFastPage,23.0f,1150.0f);
	levelNextFastPage->data=fastNextClip;
	levelPrevFastPage->data=fastPrevClip;
	levelNextFastPage->onDown=FastArrowDown;
	levelNextFastPage->onRelease=FastArrowRelease;
	levelPrevFastPage->onDown=FastArrowDown;
	levelPrevFastPage->onRelease=FastArrowRelease;


	levelTitle->x=360.0f;
	levelTitle->y=100.0f;
	
	nextPageClip->x=430.0f+60.0f;
	nextPageClip->y=1150.0f+48.0f;
	prevPageClip->x=170.0f+60.0f;
	prevPageClip->y=1150.0f+48.0f;

	fastNextClip->x=570.0f + 63.666f;
	fastNextClip->y=1150.0f+48.0f;
	fastPrevClip->x=23.0f + 63.666f;
	fastPrevClip->y=1150.0f+48.0f;



	THFrameAddChild(&levelSelectFrame,(THDisplayObject*)bgClip);
	THFrameAddChild(&levelSelectFrame,(THDisplayObject*)levelTitle);

	levelClearedData=newFile("lvc",3,0);
	
	const unsigned int lvoffset=20*curPageInd;
	char lvd[20];
	THFileMove(levelClearedData,2+lvoffset);
	THFileRead(levelClearedData,lvd,20);

	i=20;
	const float levelXOrder[]={92.0f,272.0f,452.0f,632.0f};
	const float levelYOrder[]={220.0f,405.0f,590.0f,775.0f,960.0f};
	while(i)
	{
		--i;
		const unsigned int yOrd=i>>2;
		const unsigned int xOrd=i&3;
		char lvcc=lvd[i];
		
		levelSelectPanels[i]=newMovieClip(lvcc?levelSelectPanelCleared:levelSelectPanel,levelPanelVer);
		levelSelectPanels[i]->x=levelXOrder[xOrd];
		levelSelectPanels[i]->y=levelYOrder[yOrd];

		levelSelectNumbers[i]=newNumberClip(lvoffset+i+1);
		levelSelectNumbers[i]->x=levelXOrder[xOrd];
		levelSelectNumbers[i]->y=levelYOrder[yOrd]+25.0f;
		THNumberSetScale(levelSelectNumbers[i],0.4f);

		levelSelectButtons[i]=newButton(160.0f,160.0f);
		THButtonSetPosition(levelSelectButtons[i],levelXOrder[xOrd]-80.0f,levelYOrder[yOrd]);
		levelSelectButtons[i]->onDown=LevelPanelOnDown;
		levelSelectButtons[i]->onRelease=LevelPanelOnRelease;
		levelSelectButtons[i]->data=*(levelSelectPanels+i);
		
		THFrameAddButton(&levelSelectFrame,levelSelectButtons[i]);
		THFrameAddChild(&levelSelectFrame,levelSelectPanels[i]);
		THFrameAddChild(&levelSelectFrame,levelSelectNumbers[i]);
	}

	THFrameAddChild(&levelSelectFrame,nextPageClip);
	THFrameAddChild(&levelSelectFrame,prevPageClip);
	THFrameAddButton(&levelSelectFrame,levelNextPage);
	THFrameAddButton(&levelSelectFrame,levelPrevPage);

	THFrameAddChild(&levelSelectFrame,fastNextClip);
	THFrameAddChild(&levelSelectFrame,fastPrevClip);
	THFrameAddButton(&levelSelectFrame,levelNextFastPage);
	THFrameAddButton(&levelSelectFrame,levelPrevFastPage);

	THFrameAddChild(&levelSelectFrame,(THDisplayObject*)bgaClip);

	THTween* tween=AddTween2(&(bgaClip->a),0.0f,BLACKIN_SEC,0.0f);
	tween->data=0;
	tween->onEndTween=onEndTween;
}
void LevelFrameOut()
{
	unsigned int i;
	THArrayRefresh(&levelSelectFrame.objectList,LEVEL_FRAME_OBJECT_COUNT);
	THArrayRefresh(&levelSelectFrame.buttonList,LEVEL_FRAME_BUTTON_COUNT);

	i=20;
	while(i)
	{
		--i;
		free(levelSelectButtons[i]);
		free(levelSelectPanels[i]);
		free(levelSelectNumbers[i]);
	}

	free(nextPageClip);
	free(levelNextPage);
	free(prevPageClip);
	free(levelPrevPage);
	free(levelTitle);
	free(fastNextClip);
	free(fastPrevClip);
	free(levelNextFastPage);
	free(levelPrevFastPage);

	free(bgClip);
	free(bgaClip);

	fclose(levelClearedData);
}
void LevelFrameTouch(AInputEvent* in,int32_t action,float x,float y)
{
	if(action==AMOTION_EVENT_ACTION_UP)
	{
		extern THButton* downedButton;
		if(downedButton==0){return;}
		
		if(downedButton && downedButton!=levelNextPage && downedButton!=levelPrevPage && downedButton != levelNextFastPage && downedButton!=levelPrevFastPage)
		{
			THMovieClip* mc=(THMovieClip*)downedButton->data;
			if(mc->texture==levelSelectPanelDown)
			{
				mc->texture=levelSelectPanel;
			}else if(mc->texture==levelSelectPanelClearedDown)
			{
				mc->texture=levelSelectPanelCleared;
			}
		}
		nextPageClip->texture=rightTexture;
		prevPageClip->texture=leftTexture;
		fastNextClip->texture=fastNextTexture;
		fastPrevClip->texture=fastPrevTexture;
	}
}
int LevelFrameBack()
{
	levelSelectFrame.canTouch=0;
	THTween* tween=AddTween2(&(bgaClip->a),1.0f,BLACKOUT_SEC,0.0f);
	tween->data=2;
	tween->onEndTween=onEndTween;
	return 0;
}