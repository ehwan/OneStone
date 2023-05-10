#include <GameFrame.h>
#include <assert.h>
#include <malloc.h>
#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <android/asset_manager.h>
#include <GameEngine/Util/THFile.h>
#include <OneStone.h>

const float xOrder[]={56.0f , 142.66667f , 229.33334f , 316.0f , 402.6667f , 490.0f , 576.6667f , 663.3334f};
const float yOrder[]={296.0f , 382.66667f , 469.33334f , 556.0f , 642.6667f , 730.0f , 816.6667f , 903.3334f};
MAKE_CENTER_VERTEX_CONST(smallStoneVertex,SMALLSTONER,SMALLSTONER);
MAKE_CENTER_VERTEX_CONST(bigStoneVertex,BIGSTONER,BIGSTONER);
MAKE_CENTER_VERTEX_CONST(gameBGVertex,360.0f,360.0f);
MAKE_VERTEX_CONST(btnVer,0.0f,300.0f,0.0f,120.0f);
MAKE_CENTER_VERTEX_CONST(levelBGVer,180.0f,83.33f);
MAKE_CENTER_VERTEX_CONST(targetVertex,65.0f,65.0f);
GLfloat tutoArrVer[8];
extern AAssetManager* assetManager;

extern const GLfloat fullVertex[8];

extern THFrame gameFrame;
extern THFrame levelSelectFrame;

extern THTexture* backGroundTexture;

//tutorial

THMovieClip* bgClip;
THColorClip* bgaClip;
THButton* resetButton;
THButton* backButton;
THMovieClip* resetClip;
THMovieClip* backClip;
THMovieClip* tutoArrClip=0;

THGroupClip* stoneGroup;
THMovieClip* gameBG;
THMovieClip* stones[64];
THMovieClip* levelBG;
THNumberClip* curLevelClip;


unsigned char* history;
unsigned int historyCount;
unsigned int maxHistory;

THMovieClip* stoneChosen;
int chosenNum;
int8_t gameSt[64];

unsigned int curLevelInd;

AAsset* levelDataAsset;

extern THTexture* stoneTexture;
extern THTexture* gameBGTexture;
extern THTexture* backTexture;
extern THTexture* resetTexture;
extern THTexture* backGlowTexture;
extern THTexture* resetGlowTexture;

extern THTexture* tutorialArrowTexture;

extern THTexture* currentLevelBGTexture;
extern THTexture* currentLevelBGClearTexture;
				

static void onEndTween(THTween* tween)
{
	switch(tween->data)
	{
	case 0:
		{
			gameFrame.canTouch=1;
		}
		break;
	case 1:
		{
			GoFrame(&levelSelectFrame,0);
		}
		break;
	}
}
void synchronizeStoneVisible()
{
	unsigned int i=64;
	while(i)
	{
		--i;
		stones[i]->displayObject.visible=gameSt[i];
	}
}
void pushHistory(unsigned int index,unsigned int dir)
{
	if(historyCount==maxHistory)
	{
		maxHistory+=10;
		history=(unsigned char*)realloc(history,sizeof(unsigned char)*maxHistory);
		assert(history);
	}
	unsigned char inf=(unsigned char)((dir<<6)|index);
	*(history+historyCount)=inf;
	++historyCount;
}
void resetHistory()
{
	historyCount=0;
	if(maxHistory!=10)
	{
		free(history);
		history=(unsigned char*)malloc(sizeof(unsigned char)*10);
		maxHistory=10;
		assert(history);
	}
}
void back()
{
	if(historyCount<1){return;}
	unsigned char inf=*(history+(--historyCount));

	unsigned int index=inf&63;
	unsigned int dir=inf>>6;

	switch(dir)
	{
	case 0:
		gameSt[index+2]=0;
		gameSt[index+1]=1;
		gameSt[index]=1;

		stones[index+2]->displayObject.visible=0;
		stones[index+1]->displayObject.visible=1;
		stones[index]->displayObject.visible=1;
		break;
	case 1:
		gameSt[index-2]=0;
		gameSt[index-1]=1;
		gameSt[index]=1;

		stones[index-2]->displayObject.visible=0;
		stones[index-1]->displayObject.visible=1;
		stones[index]->displayObject.visible=1;
		break;
	case 2:
		gameSt[index+16]=0;
		gameSt[index+8]=1;
		gameSt[index]=1;

		stones[index+16]->displayObject.visible=0;
		stones[index+8]->displayObject.visible=1;
		stones[index]->displayObject.visible=1;
		break;
	case 3:
		gameSt[index-16]=0;
		gameSt[index-8]=1;
		gameSt[index]=1;

		stones[index-16]->displayObject.visible=0;
		stones[index-8]->displayObject.visible=1;
		stones[index]->displayObject.visible=1;
		break;
	}
}
void reset()
{
	if(historyCount<1){return;}

	unsigned int i=historyCount;
	unsigned char inf;
	unsigned int index,dir;
	while(i)
	{
		inf=*(history+(--i));

		index=inf&63;
		dir=inf>>6;

		switch(dir)
		{
		case 0:
			gameSt[index+2]=0;
			gameSt[index+1]=1;
			gameSt[index]=1;
			break;
		case 1:
			gameSt[index-2]=0;
			gameSt[index-1]=1;
			gameSt[index]=1;
			break;
		case 2:
			gameSt[index+16]=0;
			gameSt[index+8]=1;
			gameSt[index]=1;
			break;
		case 3:
			gameSt[index-16]=0;
			gameSt[index-8]=1;
			gameSt[index]=1;
			break;
		}
	}
	resetHistory();
	synchronizeStoneVisible();
}
static int getStoneAtPoint(float x,float y)
{
	unsigned int i=8;
	unsigned int yOrd=8;
	while(i)
	{
		--i;
		if((yOrder[i]-SMALLSTONER-3.0f <= y) && (yOrder[i]+SMALLSTONER+3.0f >= y))
		{
			yOrd=i;
			break;
		}
	}
	if(8==yOrd)
	{
		return -1;
	}
	i=8;
	while(i)
	{
		--i;
		if((xOrder[i]-SMALLSTONER-3.0f <= x) && (xOrder[i]+SMALLSTONER+3.0f >= x))
		{
			const unsigned int ind=(yOrd<<3)|i;
			if(stones[ind]->displayObject.visible){return (int)ind;}
			return -1;
		}
	}
	return -1;
	
}
unsigned int StoneHitTest(THMovieClip* s1,THMovieClip* s2)
{
	float b1=s1->x;
	float b2=s2->x;
	const float rw=b1>b2?b1-b2:b2-b1;
	b1=s1->y;
	b2=s2->y;
	const float rh=b1>b2?b1-b2:b2-b1;

	if((rw<=SMALLPLUSBIGR)&&(rh<=SMALLPLUSBIGR))
	{
		return 1;
	}
	return 0;
}

FILE* levelClearedData;
static void onEndClearTween(THTween* tween)
{
	if(tween->data==0)
	{
		resetHistory();
		char cc=1;
		THFileMove(levelClearedData,2+curLevelInd);
		THFileWrite(levelClearedData,&cc,1);
		++curLevelInd;
		if(curLevelInd==maxLevel)
		{
			GameFrameBack();
			return;
		}
		THNumberSet(curLevelClip,curLevelInd+1);
		THFileMove(levelClearedData,2+curLevelInd);
		THFileRead(levelClearedData,&cc,1);
		if(cc)
		{
			levelBG->texture=currentLevelBGClearTexture;
		}else
		{
			levelBG->texture=currentLevelBGTexture;
		}
		AAsset_seek(levelDataAsset,curLevelInd<<6,SEEK_SET);
		AAsset_read(levelDataAsset,gameSt,64);
		synchronizeStoneVisible();

		stoneGroup->x=1200.0f;

		THTween* tween=AddTween2(&(stoneGroup->x),0.0f,STONES_MOTION_SEC2,-0.6f);
		tween->data=1;
		tween->onEndTween=onEndClearTween;
	}else if(tween->data==1)
	{
		gameFrame.canTouch=1;
	}
}

static void onClear()
{
	gameFrame.canTouch=0;
	if(curLevelInd==0)
	{
		tutoArrClip->displayObject.visible=0;
	}
	THTween* tween=AddTween2(&(stoneGroup->x),-720.0f,STONES_MOTION_SEC,-0.7f);
	tween->data=0;
	tween->onEndTween=onEndClearTween;
}

static void gameBtnDown(float x,float y,THButton* btn)
{
	if(btn==backButton)
	{
		THMovieClip* mc=(THMovieClip*)btn->data;
		mc->texture=backGlowTexture;
	}else if(btn==resetButton)
	{
	THMovieClip* mc=(THMovieClip*)btn->data;
	mc->texture=resetGlowTexture;
	}
}
static void gameBtnRelease(float x,float y,THButton* btn)
{
	if(btn==backButton)
	{
		back();
	}else if(btn==resetButton){
	reset();
	}
}

void GameFrameOut()
{
	THLog("GameOut");
	AAsset_close(levelDataAsset);
	THArrayRefresh(&gameFrame.objectList,GAME_FRAME_OBJECT_COUNT);
	THArrayRefresh(&gameFrame.buttonList,GAME_FRAME_BUTTON_COUNT);
	THArrayFree(&(stoneGroup->list));
	THLog("GameOut1");
	free(bgClip);
	free(bgaClip);
	free(resetButton);
	free(backButton);
	free(resetClip);
	free(backClip);
	free(stoneGroup);

	free(gameBG);
	free(levelBG);


	free(tutoArrClip);
	

	unsigned int i=64;
	while(i)
	{
		--i;
		free(stones[i]);
	}
	free(curLevelClip);
	free(history);
	
	fclose(levelClearedData);
}

#include <string.h>
void GameFrameAction(THFrame* frame,void* data)
{
	unsigned int i;

	gameFrame.canTouch=0;

	historyCount=0;
	history=(unsigned char*)malloc(sizeof(unsigned char)*10);
	maxHistory=10;
	assert(history);

	bgClip=newMovieClip(backGroundTexture,fullVertex);
	bgaClip=newColorClip(1.0f,fullVertex);
	resetClip=newMovieClip(resetTexture,btnVer);
	backClip=newMovieClip(backTexture,btnVer);
	stoneGroup=newGroupClip(GAME_FRAME_STONE_GROUP_COUNT);
	gameBG=newMovieClip(gameBGTexture,gameBGVertex);
	

	i=64;
	while(i)
	{
		--i;
		stones[i]=newMovieClip(stoneTexture,smallStoneVertex);
		stones[i]->x=xOrder[i&7];
		stones[i]->y=yOrder[i>>3];
	}

	char cc;
	levelClearedData=newFile("lvc",3,0);
	THFileMove(levelClearedData,2+curLevelInd);
	THFileRead(levelClearedData,&cc,1);
	levelBG=newMovieClip(cc?currentLevelBGClearTexture:currentLevelBGTexture,levelBGVer);
	curLevelClip=newNumberClip(curLevelInd+1);
	THNumberSetScale(curLevelClip,0.5f);

	backButton=newButton(300.0f,120.0f);
	resetButton=newButton(300.0f,120.0f);
	backButton->data=backClip;
	resetButton->data=resetClip;
	backButton->onDown=gameBtnDown;
	backButton->onRelease=gameBtnRelease;
	resetButton->onDown=gameBtnDown;
	resetButton->onRelease=gameBtnRelease;

	gameBG->x=360.0f;
	gameBG->y=600.0f;
	THButtonSetPosition(backButton,0.0f,1040.0f);
	THButtonSetPosition(resetButton,420.0f,1040.0f);
	backClip->x=0.0f;
	backClip->y=1040.0f;
	resetClip->x=420.0f;
	resetClip->y=1040.0f;
	
	curLevelClip->x=360.0f;
	curLevelClip->y=60.0f;
	levelBG->x=360.0f;
	levelBG->y=120.0f;

	levelDataAsset=AAssetManager_open(assetManager,"levelData.png",AASSET_MODE_RANDOM);
	AAsset_seek(levelDataAsset,curLevelInd<<6,SEEK_SET);
	AAsset_read(levelDataAsset,gameSt,64);
	
	synchronizeStoneVisible();

	THFrameAddChild(&gameFrame,bgClip);
	THFrameAddChild(&gameFrame,gameBG);
	THFrameAddChild(&gameFrame,stoneGroup);

	
	THFrameAddChild(&gameFrame,resetClip);
	THFrameAddChild(&gameFrame,backClip);
	THFrameAddChild(&gameFrame,bgaClip);

	THGroupClipAddChild(stoneGroup,(THDisplayObject*)levelBG);
	THGroupClipAddChild(stoneGroup,(THDisplayObject*)curLevelClip);
	i=64;
	while(i)
	{
		--i;
		THGroupClipAddChild(stoneGroup,(THDisplayObject*)stones[i]);
	}

		tutoArrClip=newMovieClip(tutorialArrowTexture,tutoArrVer);

		//const float xOrder[]={56.0f , 142.66667f , 229.33334f , 316.0f , 402.6667f , 490.0f , 576.6667f , 663.3334f};
		//const float yOrder[]={296.0f , 382.66667f , 469.33334f , 556.0f , 642.6667f , 730.0f , 816.6667f , 903.3334f};

		MAKE_CENTER_VERTEX_CONST(ttutoArrVer,138.0f,82.666f);
		memcpy(tutoArrVer,ttutoArrVer,sizeof(float)<<3);

		tutoArrClip->x=314.0f;
		tutoArrClip->y=610.0f;
		if(curLevelInd==0){tutoArrClip->displayObject.visible=1;}else{tutoArrClip->displayObject.visible=0;}
		THGroupClipAddChild(stoneGroup,(THDisplayObject*)tutoArrClip);
	


	THFrameAddButton(&gameFrame,resetButton);
	THFrameAddButton(&gameFrame,backButton);

	THTween* tween=AddTween2(&(bgaClip->a),0.0f,BLACKIN_SEC,0.0f);
	tween->data=0;
	tween->onEndTween=onEndTween;
}
static void onGameFrameTouchDown(float x,float y)
{
	chosenNum=getStoneAtPoint(x,y);
	if(chosenNum!=-1)
	{
		stoneChosen=stones[chosenNum];
		THGroupReAddChild(stoneGroup,(THDisplayObject*)stoneChosen);
		stoneChosen->vertexBuffer=bigStoneVertex;
		stoneChosen->x=x;
		stoneChosen->y=y;
	}
}
static void onGameFrameTouchMove(float x,float y)
{
	if(chosenNum!=-1)
	{
		stoneChosen->x=x;
		stoneChosen->y=y;
	}
}
static void onGameFrameTouchUp(float x,float y)
{
	backClip->texture=backTexture;
	resetClip->texture=resetTexture;
	if(chosenNum!=-1)
	{
		const unsigned int yNum=chosenNum>>3;
		const unsigned int xNum=chosenNum&7;

		if((xNum<6)&&(*(gameSt+(chosenNum+2))==0)&&(*(gameSt+chosenNum+1)==1)&&StoneHitTest(stoneChosen,stones[chosenNum+2]))
		{
			pushHistory(chosenNum,0);

			*(gameSt+chosenNum) = 0;
			stoneChosen->displayObject.visible=0;

			*(gameSt+chosenNum+1) = 0;
			stones[chosenNum+1]->displayObject.visible=0;
			*(gameSt+chosenNum+2) = 1;
			stones[chosenNum+2]->displayObject.visible=1;

			if(curLevelInd==0)
			{
				tutoArrVer[0]*=-1.0f;
				tutoArrVer[2]*=-1.0f;
				tutoArrVer[4]*=-1.0f;
				tutoArrVer[6]*=-1.0f;
				tutoArrClip->x=404.0f;
			}
		}
		else if((xNum>1)&&(gameSt[chosenNum-2]==0)&&(gameSt[chosenNum-1]==1)&&StoneHitTest(stoneChosen,stones[chosenNum-2]))
		{
			pushHistory(chosenNum,1);

			gameSt[chosenNum] = 0;
			stoneChosen->displayObject.visible=0;

			gameSt[chosenNum-1] = 0;
			stones[chosenNum-1]->displayObject.visible=0;
			gameSt[chosenNum-2] = 1;
			stones[chosenNum-2]->displayObject.visible=1;
		}
		else if((yNum<6)&&(gameSt[chosenNum+16]==0)&&(gameSt[chosenNum+8]==1)&&StoneHitTest(stoneChosen,stones[chosenNum+16]))
		{
			pushHistory(chosenNum,2);

			gameSt[chosenNum] = 0;
			stoneChosen->displayObject.visible=0;

			gameSt[chosenNum+8] = 0;
			stones[chosenNum+8]->displayObject.visible=0;
			gameSt[chosenNum+16] = 1;
			stones[chosenNum+16]->displayObject.visible=1;
		}
		else if((yNum>1)&&(gameSt[chosenNum-16]==0)&&(gameSt[chosenNum-8]==1)&&StoneHitTest(stoneChosen,stones[chosenNum-16]))
		{
			pushHistory(chosenNum,3);

			gameSt[chosenNum] = 0;
			stoneChosen->displayObject.visible=0;

			gameSt[chosenNum-8] = 0;
			stones[chosenNum-8]->displayObject.visible=0;
			gameSt[chosenNum-16] = 1;
			stones[chosenNum-16]->displayObject.visible=1;
		}
		stoneChosen->vertexBuffer=smallStoneVertex;
		stoneChosen->x=xOrder[xNum];
		stoneChosen->y=yOrder[yNum];
		chosenNum=-1;
		stoneChosen=0;

		unsigned int i=64;
		unsigned int count=0;
		while(i)
		{
			--i;
			if(gameSt[i])
			{
				if(count)
				{
					return;
				}
				++count;
			}
		}
		onClear();
	}
#ifndef NDEBUG
	else
	{
		onClear();
	}
#endif
}
void GameFrameTouch(AInputEvent* in,int32_t action,float x,float y)
{
	switch(action)
	{
	case AMOTION_EVENT_ACTION_DOWN:
		onGameFrameTouchDown(x,y);
		break;
	case AMOTION_EVENT_ACTION_MOVE:
		onGameFrameTouchMove(x,y);
		break;
	case AMOTION_EVENT_ACTION_UP:
		onGameFrameTouchUp(x,y);
		break;
	}
}
int GameFrameBack()
{
	gameFrame.canTouch=0;
	THTween* tween=AddTween2(&(bgaClip->a),1.0f,BLACKOUT_SEC,0.0f);
	tween->data=1;
	tween->onEndTween=onEndTween;
	return 0;
}