#include <THPrivate.h>
#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <android/asset_manager.h>
#include <malloc.h>
#include <unistd.h>

#include <GameFrame.h>
#include <StartFrame.h>
#include <LevelFrame.h>

#include <GameEngine/Util/THFile.h>
#include <assert.h>
#include <string.h>

extern AAssetManager* assetManager;

MAKE_VERTEX_CONST(fullVertex,0,720.0f,1280.0f,0.0f);


THFrame loadingFrame;
THFrame startFrame;
THFrame levelSelectFrame;
THFrame gameFrame;
//Frames


GLuint glGameTexture;
GLuint glGameTexture2;
THTexture* backGroundTexture;
THTexture* numberTextures[10];



THTexture* logoTexture;
THTexture* touchAnywhere;


THTexture* levelSelectTitle;
THTexture* levelSelectPanel;
THTexture* levelSelectPanelDown;
THTexture* levelSelectPanelCleared;
THTexture* levelSelectPanelClearedDown;
THTexture* rightTexture;
THTexture* leftTexture;
THTexture* rightDownTexture;
THTexture* leftDownTexture;




THTexture* stoneTexture;
THTexture* gameBGTexture;
THTexture* backTexture;
THTexture* resetTexture;
THTexture* backGlowTexture;
THTexture* resetGlowTexture;


THTexture* currentLevelBGTexture;
THTexture* currentLevelBGClearTexture;

THTexture* tutorialArrowTexture;

THTexture* fastNextTexture;
THTexture* fastPrevTexture;
THTexture* fastNextDownTexture;
THTexture* fastPrevDownTexture;

#include <android/configuration.h>
void OnCreate(struct android_app* app)
{
	FILE* lvc=newFile("lvc",3,0);
	THFileMove(lvc,0);
	char llvc[2];
	THFileRead(lvc,llvc,2);
	const unsigned int ilvc=(unsigned int)(llvc[0]<<8)+(unsigned int)llvc[1];
	THLog("%d",ilvc);
	if(ilvc<maxLevel)
	{
		 char* ndata=(char*)malloc(maxLevel-ilvc);
		 assert(ndata);
		 memset(ndata,0,maxLevel-ilvc);
		 THFileMove(lvc,0);
		 llvc[0]=maxLevel>>8;
		 llvc[1]=maxLevel&255;
		 THFileWrite(lvc,llvc,2);
		 THFileMove(lvc,2+ilvc);
		 THFileWrite(lvc,ndata,maxLevel-ilvc);
	}
	fclose(lvc);

	THFrameInit(&startFrame,START_FRAME_OBJECT_COUNT,START_FRAME_BUTTON_COUNT);
	startFrame.action=StartFrameAction;
	startFrame.onChangeFrame=StartFrameOut;
	startFrame.onTouchEvent=StartFrameTouch;

	THFrameInit(&levelSelectFrame,LEVEL_FRAME_OBJECT_COUNT,LEVEL_FRAME_BUTTON_COUNT);
	levelSelectFrame.action=LevelFrameAction;
	levelSelectFrame.onChangeFrame=LevelFrameOut;
	levelSelectFrame.onTouchEvent=LevelFrameTouch;
	levelSelectFrame.onBackReleased=LevelFrameBack;

	THFrameInit(&gameFrame,GAME_FRAME_OBJECT_COUNT,GAME_FRAME_BUTTON_COUNT);
	gameFrame.action=GameFrameAction;
	gameFrame.onBackReleased=GameFrameBack;
	gameFrame.onChangeFrame=GameFrameOut;
	gameFrame.onTouchEvent=GameFrameTouch;




	backGroundTexture=MALLOC_TEXTURE(4);
	numberTextures[0]=MALLOC_TEXTURE(4);
	numberTextures[1]=MALLOC_TEXTURE(4);
	numberTextures[2]=MALLOC_TEXTURE(4);
	numberTextures[3]=MALLOC_TEXTURE(4);
	numberTextures[4]=MALLOC_TEXTURE(4);
	numberTextures[5]=MALLOC_TEXTURE(4);
	numberTextures[6]=MALLOC_TEXTURE(4);
	numberTextures[7]=MALLOC_TEXTURE(4);
	numberTextures[8]=MALLOC_TEXTURE(4);
	numberTextures[9]=MALLOC_TEXTURE(4);

	logoTexture=MALLOC_TEXTURE(4);
	touchAnywhere=MALLOC_TEXTURE(4);

	levelSelectTitle=MALLOC_TEXTURE(4);
	levelSelectPanel=MALLOC_TEXTURE(4);
	levelSelectPanelDown=MALLOC_TEXTURE(4);
	levelSelectPanelCleared=MALLOC_TEXTURE(4);
	levelSelectPanelClearedDown=MALLOC_TEXTURE(4);
	rightTexture=MALLOC_TEXTURE(4);
	leftTexture=MALLOC_TEXTURE(4);
	rightDownTexture=MALLOC_TEXTURE(4);
	leftDownTexture=MALLOC_TEXTURE(4);

	stoneTexture=MALLOC_TEXTURE(4);
	gameBGTexture=MALLOC_TEXTURE(4);
	backTexture=MALLOC_TEXTURE(4);
	resetTexture=MALLOC_TEXTURE(4);
	backGlowTexture=MALLOC_TEXTURE(4);
	resetGlowTexture=MALLOC_TEXTURE(4);

	currentLevelBGTexture=MALLOC_TEXTURE(4);
	currentLevelBGClearTexture=MALLOC_TEXTURE(4);
	tutorialArrowTexture=MALLOC_TEXTURE(4);

	fastNextTexture=MALLOC_TEXTURE(4);
	fastPrevTexture=MALLOC_TEXTURE(4);
	fastNextDownTexture=MALLOC_TEXTURE(4);
	fastPrevDownTexture=MALLOC_TEXTURE(4);




	GoFrame(&startFrame,0);
}

void OnSurfaceCreated()
{
	AConfiguration* config=AConfiguration_new();
	AConfiguration_fromAssetManager(config,assetManager);
	int32_t d=AConfiguration_getDensity(config);
	AConfiguration_delete(config);

	SetGameSize(720.0f,1280.0f);
	if(d<240)
	{
		LoadTexture(&glGameTexture,"t1hdpi.png");
		LoadTexture(&glGameTexture2,"t2hdpi.png");
	}
	else
	{
		LoadTexture(&glGameTexture,"t1xxhdpi.png");
		LoadTexture(&glGameTexture2,"t2xxhdpi.png");
	}
	
	THTextureSet(backGroundTexture,glGameTexture,0.0f,0.0f,1080.0f,1920.0f,2048.0f,2048.0f);
	THTextureSet(logoTexture,glGameTexture,1080.0f,0.0f,968.0f,426.0f,2048.0f,2048.0f);
	THTextureSet(touchAnywhere,glGameTexture2,0.0f,1330.0f,610.0f,95.0f,2048.0f,2048.0f);

	THTextureSet(numberTextures[0], glGameTexture,1080,861,120,233,2048,2048);
	THTextureSet(numberTextures[1], glGameTexture,1200,861,120,233,2048,2048);
	THTextureSet(numberTextures[2], glGameTexture,1320,861,120,233,2048,2048);
	THTextureSet(numberTextures[3], glGameTexture,1440,861,120,233,2048,2048);
	THTextureSet(numberTextures[4], glGameTexture,1560,861,120,233,2048,2048);
	THTextureSet(numberTextures[5], glGameTexture,1680,861,120,233,2048,2048);
	THTextureSet(numberTextures[6], glGameTexture,1800,861,120,233,2048,2048);
	THTextureSet(numberTextures[7], glGameTexture,1920,861,120,233,2048,2048);
	THTextureSet(numberTextures[8], glGameTexture,1080,1094,120,233,2048,2048);
	THTextureSet(numberTextures[9], glGameTexture,1200,1094,120,233,2048,2048);

	THTextureSet(levelSelectPanel,glGameTexture,1080,561,270,270,2048,2048);
	THTextureSet(levelSelectPanelDown,glGameTexture,1350,561,270,270,2048,2048);
	THTextureSet(levelSelectPanelCleared,glGameTexture,1278,1508,270,270,2048,2048);
	THTextureSet(levelSelectPanelClearedDown,glGameTexture,1278,1778,270,270,2048,2048);
	THTextureSet(levelSelectTitle,glGameTexture2,0,1080,1080,250,2048,2048);
	THTextureSet(rightTexture,glGameTexture,1128,1928,150,120,2048,2048);
	THTextureSet(leftTexture,glGameTexture,1128,1808,150,120,2048,2048);
	THTextureSet(rightDownTexture,glGameTexture,1128,1568,150,120,2048,2048);
	THTextureSet(leftDownTexture,glGameTexture,1128,1688,150,120,2048,2048);

	THTextureSet(stoneTexture,glGameTexture,1080,426,135,135,2048,2048);
	THTextureSet(gameBGTexture,glGameTexture2,0,0,1080,1080,2048,2048);
	
	THTextureSet(backTexture,glGameTexture,1548,1848,500,200,2048,2048);
	THTextureSet(resetTexture,glGameTexture,1548,1648,500,200,2048,2048);
	THTextureSet(backGlowTexture,glGameTexture,1548,1448,500,200,2048,2048);
	THTextureSet(resetGlowTexture,glGameTexture,1548,1248,500,200,2048,2048);
	THTextureSet(currentLevelBGTexture,glGameTexture2,1080,0,540,250,2048,2048);
	THTextureSet(currentLevelBGClearTexture,glGameTexture2,1080,250,540,250,2048,2048);

	THTextureSet(tutorialArrowTexture,glGameTexture2,0,1445,414,248,2048,2048);

	THTextureSet(fastNextTexture,glGameTexture2,1080,850,191,120,2048,2048);
	THTextureSet(fastNextDownTexture,glGameTexture2,1080,970,191,120,2048,2048);
	THTextureSet(fastPrevTexture,glGameTexture2,1080,1090,191,120,2048,2048);
	THTextureSet(fastPrevDownTexture,glGameTexture2,1080,1210,191,120,2048,2048);

	//TextureLoading?
}

void OnResume()
{
}

void OnPause()
{
}

void OnDestroy()
{		
	//Memory Free
}