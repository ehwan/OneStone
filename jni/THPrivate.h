#ifndef THKim_THPrivate
#define THKim_THPrivate

#include <android_native_app_glue.h>
#include <android/log.h>
#include <android/input.h>

#include <GLES2/gl2.h>

#include <THSetting.h>
#include <pthread.h>
#include <assert.h>

#define GLThreadSleepTime 1000000/GLTHREAD_FRAMERATE

#ifdef NDEBUG
	#define THLog(...)
	#define THError(...)
#else
	#define THLog(...) __android_log_print(ANDROID_LOG_DEBUG,"TH-DEBUG",__VA_ARGS__)
	#define THError(...) __android_log_print(ANDROID_LOG_ERROR,"TH-ERROR",__VA_ARGS__)
#endif

#define MAKE_CENTER_VERTEX(name,hw,hh) \
	GLfloat name[]={-hw, hh,hw, hh,-hw,-hh,hw,-hh}
#define MAKE_VERTEX(name,left,right,top,bottom) \
	GLfloat name[]={left, bottom,right, bottom,left,top,right,top}
#define MAKE_CENTER_VERTEX_CONST(name,hw,hh) \
	const GLfloat name[]={-hw, hh,hw, hh,-hw,-hh,hw,-hh}
#define MAKE_VERTEX_CONST(name,left,right,top,bottom) \
	const GLfloat name[]={left, bottom,right, bottom,left,top,right,top}
#define MAKE_ARRAY(name,type,num) type* name=(type*)malloc(sizoef(type)*num)
#define MALLOC_TEXTURE(vertexcount) (THTexture*)malloc(sizeof(THTexture)+(sizeof(float)*((vertexcount)<<1)))

#define AddTween2(src,dest,seconds,contrast) AddTween(src,dest,(seconds)*GLTHREAD_FRAMERATE,contrast)

//THTween--------------------------------------------
struct _THTween
{
	float* src;
	//Source
	float dest;
	//Destination
	float vel;
	float velIncr;
	//Velocity Per Frame
	unsigned int cf;
	//FrameCount
	struct _THTween* next;
	void (*onEndTween)(struct _THTween* tween);
	//Callback when tween ends;

	unsigned int data;
};
typedef struct _THTween	THTween;
//THTween--------------------------------------------


//THDisplayObject-----------------------------------------
struct _THDisplayObject
{
	unsigned int visible;
	void* frame;
	void (*drawFunction)(const struct _THDisplayObject* displayobject);
};
typedef void (*THDrawFunctions)(const struct _THDisplayObject* displayobject);
typedef struct _THDisplayObject THDisplayObject;
struct _THButton
{
	void* data;
	void* frame;
	unsigned int enable;
	float x;
	float y;
	float xplusw;
	float yplush;

	void (*onDown)(float,float,struct _THButton*);
	void (*onRelease)(float,float,struct _THButton*);
};
typedef struct _THButton THButton;
//THDisplayObject-----------------------------------------


//THArray---------------------------------------------
struct _THArray{
	unsigned int num;
	unsigned int max;
	void **arr;
};
typedef struct _THArray THArray;

THArray* THArrayNew(const unsigned int num);
void THArrayInit(THArray* arr,const unsigned int num);
void THArrayPush(THArray *arr, void *object);
void THArrayDeleteObj(THArray *arr, void *obj);
void THArrayRePush(THArray* arr,void* obj);
void THArrayRefresh(THArray* arr,const unsigned int num);
static inline void THArrayFree(THArray *arr)
{
	free(arr->arr);
}
//THArray----------------------------------------------


//THTexture---------------------------------------------
typedef struct
{
	GLuint textureID;
	float* textureBuffer;
} THTexture;

void THTextureSet(THTexture* texture,GLuint textureID,float x,float y,float w,float h,float imgW,float imgH);
void THTextureSet2(THTexture* texture,GLuint textureID);
//THTexture---------------------------------------------

//THFrame--------------------------------------------------------------
struct _THFrame
{
	THArray objectList;
	THArray buttonList;

	void (*onTouchEvent)(AInputEvent*,int32_t,float,float);
	int (*onBackReleased)();
	void (*onEnterFrame)();

	void (*action)(struct _THFrame* callFrame,void* data);
	void (*onChangeFrame)();

	unsigned int canTouch;
};
typedef struct _THFrame THFrame;

THFrame* newFrame(const unsigned int objectListCount,const unsigned int buttonlistCount);
void THFrameInit(THFrame* frame,const unsigned int objectListCount,const unsigned int buttonlistCount);
void THFrameFree(THFrame* frame);
void _THFrameAddChild(THFrame* frame,THDisplayObject* object);
void _THFrameRemoveChild(THFrame* frame,THDisplayObject* object);
void THFrameAddButton(THFrame* frame,THButton* btn);
void THFrameRemoveButton(THFrame* f,THButton* btn);
void GoFrame(THFrame* f,void* data);

#define THFrameAddChild(frame,object) _THFrameAddChild(frame,(THDisplayObject*)(object))
#define	THFrameRemoveChild(frame,object) _THFrameRemoveChild(frame,(THDisplayObject*)(object))

static inline void _THFrameReAddChild(THFrame* frame,THDisplayObject* displayObject)
{
	THError(displayObject->frame!=frame?"ReAddChild To Another Frame!":"");
	assert(displayObject->frame==frame);
	THArrayRePush(&frame->objectList,displayObject);
}
#define THFrameReAddChild(frame,object)	_THFrameReAddChild(frame,(THDisplayObject*)(object))
static inline void THFrameReAddButton(THFrame* frame,THButton* btn)
{
	THError(btn->frame!=frame?"ReAddButton To Another Frame!":"");
	assert(btn->frame==frame);
	THArrayRePush(&frame->buttonList,btn);
}
//THFrame--------------------------------------------------------------

pthread_t newThread(void* (*function)(void*), void* data);

void SetGameSize(const unsigned int w,const unsigned int h);
void OnCreate(struct android_app* state);
void LoadTexture(GLuint* texture,const char* name);
void OnSurfaceCreated();
//void OnSaveInstanceState(saved_state* saved);
//void OnLoadInstanceState(saved_state* loaded);
void OnResume();
void OnPause();
void OnDestroy();
void SetViewport(const float x,const float y);
THTween* AddTween(float* src,const float dest,const unsigned int frcount,float contrast);
void RemoveTweens();

#endif
