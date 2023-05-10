#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <GameEngine/Rendering/THDrawing.h>

#include <THPrivate.h>

#include <libpng/png.h>
#include <libpng/pngstruct.h>

#include <android/asset_manager.h>

EGLDisplay eglDisplay;
EGLSurface eglSurface;
EGLContext eglContext;

pthread_t glThread;
pthread_mutex_t glPauseMut;
pthread_cond_t glPauseCond;
unsigned int hasSthDo;
//GLThread HasSthDo Bit
#define HAS_STH_NOTHING_TODO 0
#define HAS_STH_TERM_DISPLAY 1
#define HAS_STH_PAUSE_THREAD 2
#define HAS_STH_TEXTURE_CREATE 4
#define HAS_STH_REMOVE_TWEENS 8

void* textureColorBuffer;
GLuint* texturePointer;
unsigned int textureInfo;
pthread_mutex_t textureMutex;
pthread_cond_t textureCond;

unsigned int stageWidth;
unsigned int stageHeight;
unsigned int gameWidth;
unsigned int gameHeight;
float scaleX;
float scaleY;
float viewportX,viewportY;

GLuint program;
GLuint positionHandler;
GLuint rotposHandler;
GLuint textureHandler;
GLuint sxHandler,syHandler;
GLuint viewportXHandler,viewportYHandler;
GLuint hasColorHandler,colorHandler;

THFrame* currentFrame;
THButton* downedButton;
THTween* tweens;
THTween* tweensLast;

AAssetManager* assetManager;

void SetGameSize(const unsigned int w,const unsigned int h)
{
	gameWidth=w;
	gameHeight=h;
	scaleX=(float)w/(float)stageWidth;
	scaleY=(float)h/(float)stageHeight;
	THLog("Set GameSize : %d / %d",w,h);
	glUniform1f(sxHandler,2.0f/(float)w);
	glUniform1f(syHandler,2.0f/(float)h);
}
void SetViewport(const float x,const float y)
{
	THLog("SetViewport : %.2f / %.2f",x,y);
	viewportX=x;
	viewportY=y;
	glUniform1f(viewportXHandler,x);
	glUniform1f(viewportYHandler,y);
}
THTween* AddTween(float* src,const float dest,const unsigned int frcount,float contrast)
{
	THLog("New Tween Added : %d Frames , %.2f Seconds",frcount,(float)frcount/(float)GLTHREAD_FRAMERATE);
	assert(frcount>0);
	THTween* tween=(THTween*)malloc(sizeof(THTween));
	assert(tween);
	memset(tween,0,sizeof(THTween));

	const float invF=1.0f/(float)frcount;
	const float vel=(dest-*src)*invF;
	const float vc=vel*contrast;
	tween->src=src;
	tween->dest=dest;
	tween->vel=vel-vc;
	tween->velIncr=2*vc*invF;
	tween->cf=frcount;

	if(tweensLast)
	{
		tweensLast->next=tween;
		tweensLast=tween;
	}
	else
	{
		tweens=tween;
		tweensLast=tween;
	}
	
	return tween;
}
void RemoveTweens()
{
	hasSthDo|=HAS_STH_REMOVE_TWEENS;
}
static void term_display() {
    if (eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(eglDisplay, eglContext);
        }
        if (eglSurface != EGL_NO_SURFACE) {
            eglDestroySurface(eglDisplay, eglSurface);
        }
        eglTerminate(eglDisplay);
    }
    eglDisplay = EGL_NO_DISPLAY;
    eglContext = EGL_NO_CONTEXT;
    eglSurface = EGL_NO_SURFACE;
}
static int32_t handle_input(struct android_app* app, AInputEvent* event) {
	if(currentFrame->canTouch==0){return 1;}
	if(AINPUT_EVENT_TYPE_MOTION==AInputEvent_getType(event))
	{
		const int32_t action=AMotionEvent_getAction(event)&AMOTION_EVENT_ACTION_MASK;
		unsigned int i=currentFrame->buttonList.num;
		const float x=AMotionEvent_getX(event,0)*scaleX+viewportX;
		const float y=AMotionEvent_getY(event,0)*scaleY+viewportY;
		currentFrame->onTouchEvent(event,action,x,y);
		THButton** const list=(THButton**)currentFrame->buttonList.arr;
		THButton* btn;
		switch(action)
		{
			case AMOTION_EVENT_ACTION_DOWN:
				{
					 while(i)
					 {
						 btn=list[--i];
						 if(btn->enable && THButtonHitTest(btn,x,y))
						 {
							 downedButton=btn;
							 if(btn->onDown){btn->onDown(x,y,btn);}
						  	 return 1;
						 }
					 }
				}
				break;
			case AMOTION_EVENT_ACTION_UP:
				{
					while(i)
					 {
						 btn=list[--i];
						 if(btn->enable && THButtonHitTest(btn,x,y))
						 {
							 if(downedButton==btn && btn->onRelease)
							 {
								btn->onRelease(x,y,btn);
							 }
							 downedButton=NULL;
							 return 1;
						 }
					 }
				}
				break;
		}
		return 1;
	}else
	{
		if(AKEYCODE_BACK==AKeyEvent_getKeyCode(event))
		{
			if(AKEY_EVENT_ACTION_UP==AKeyEvent_getAction(event) && currentFrame->onBackReleased)
			{
				if(currentFrame->onBackReleased())
				{
					return 0;
				}else{return 1;}
			}
		}
		
	}
	return 0;
}
void THEGLInit(struct android_app* app)
{
	const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint format;
    EGLint numConfigs;
    EGLConfig config;

    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(eglDisplay, 0, 0);
    eglChooseConfig(eglDisplay, attribs, &config, 1, &numConfigs);
    eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
	

    ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

    eglSurface = eglCreateWindowSurface(eglDisplay, config, app->window, NULL);
	const EGLint attrib_list[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    eglContext = eglCreateContext(eglDisplay, config, NULL, attrib_list);

	if (eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext) == EGL_FALSE) {
		THError("Unable to eglMakeCurrent");
		assert(0);
        pthread_exit(0);
    }

	EGLint wh;
    eglQuerySurface(eglDisplay, eglSurface, EGL_WIDTH, &wh);
	stageWidth = wh;
    eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &wh);
	stageHeight = wh;
	gameWidth=wh;
	gameHeight=wh;
	scaleX=1.0f;
	scaleY=1.0f;
}
void THGLInit()
{
	GLuint vertex=glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vs=
			"precision mediump float;"
			"uniform highp float sx;"
			"uniform highp float sy;"
			"uniform float viewportX;"
			"uniform float viewportY;"
			"attribute vec2 pos;"
			"attribute vec4 rp;"
			"attribute vec2 aTex;"
			"varying vec2 vTex;"
			"attribute mediump vec4 aColor;"
			"varying mediump vec4 vColor;"
			"void main(){"
			"gl_Position=vec4(((rp[0]*pos[0])-(rp[1]*pos[1])+rp[2]-viewportX)*sx-1.0,-((rp[1]*pos[0])+(rp[0]*pos[1])+rp[3]+viewportY)*sy+1.0,0.0,1.0);"
			"vTex=aTex;"
			"vColor=aColor;"
			"}";
	glShaderSource(vertex,1,&vs,NULL);
	glCompileShader(vertex);

	GLuint fragment=glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fs=
			"precision mediump float;"
			"varying vec2 vTex;"
			"varying mediump vec4 vColor;"
			"uniform sampler2D sTexture;"
			"uniform lowp int hasColor;"
			"void main(){"
			"if(hasColor==1){"
			"gl_FragColor=vColor;"
			"}else{"
			"gl_FragColor=texture2D(sTexture,vTex);"
			"}"
			"}";
	glShaderSource(fragment,1,&fs,NULL);
	glCompileShader(fragment);

	program=glCreateProgram();
	glAttachShader(program,vertex);
	glAttachShader(program,fragment);
	glLinkProgram(program);
	glUseProgram(program);

	positionHandler=glGetAttribLocation(program,"pos");
	rotposHandler=glGetAttribLocation(program,"rp");
	textureHandler=glGetAttribLocation(program,"aTex");
	colorHandler=glGetAttribLocation(program,"aColor");
	sxHandler=glGetUniformLocation(program,"sx");
	syHandler=glGetUniformLocation(program,"sy");
	viewportXHandler=glGetUniformLocation(program,"viewportX");
	viewportYHandler=glGetUniformLocation(program,"viewportY");
	hasColorHandler=glGetUniformLocation(program,"hasColor");
	glUniform1f(viewportXHandler,0.0f);
	glUniform1f(viewportYHandler,0.0f);

	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, stageWidth, stageHeight);

	glEnableVertexAttribArray(positionHandler);
	glEnableVertexAttribArray(textureHandler);
}
static void* glThreadFunc(void* data)
{
	//Initialize EGL
	THEGLInit((struct android_app*)data);

    // Initialize GL state.
	THGLInit();

	OnSurfaceCreated();

	unsigned int i;
	unsigned int num;
	THDisplayObject** list;
	const THDisplayObject* object;
	THTween *lastTween,*curTween;
	while(1)
	{
		if(hasSthDo)
		{
			if(hasSthDo&HAS_STH_TERM_DISPLAY)
			{
				THLog("GLThread - StopThread");
				if(hasSthDo&HAS_STH_TEXTURE_CREATE)
				{
					hasSthDo-=HAS_STH_TEXTURE_CREATE;
					free(textureColorBuffer);
					pthread_cond_signal(&textureCond);
				}
				
				pthread_mutex_destroy(&glPauseMut);
				pthread_cond_destroy(&glPauseCond);
				pthread_exit(0);
			}
			if(hasSthDo&HAS_STH_PAUSE_THREAD)
			{
				THLog("GLThread - PauseThread");
				pthread_cond_wait(&glPauseCond,&glPauseMut);
				continue;
			}
			if(hasSthDo&HAS_STH_TEXTURE_CREATE)
			{
				hasSthDo-=HAS_STH_TEXTURE_CREATE;
				genTexture(texturePointer,textureColorBuffer,textureInfo>>16,textureInfo&65535);
				free(textureColorBuffer);
				pthread_cond_signal(&textureCond);
			}
			if(hasSthDo&HAS_STH_REMOVE_TWEENS)
			{
				curTween=tweens;
				tweens=0;
				tweensLast=0;
				while(curTween)
				{
					lastTween=curTween;
					curTween=curTween->next;
					free(lastTween);
				}
			}
		}

		//Tweening
		lastTween=0;
		curTween=tweens;
		while(curTween)
		{
			if(--(curTween->cf))
			{
				*(curTween->src)+=curTween->vel;
				curTween->vel+=curTween->velIncr;
				lastTween=curTween;
				curTween=curTween->next;
			}
			else
			{
				*(curTween->src)=curTween->dest;
				THTween* nt=curTween->next;
				if(lastTween)
				{
					lastTween->next=nt;
					if(nt==0){tweensLast=lastTween;}
				}
				else
				{
					tweens=nt;
					if(nt==0){tweensLast=0;}
				}

				if(curTween->onEndTween){curTween->onEndTween(curTween);free(curTween);}
				curTween=nt;
			}
		}

		//Draw Start
		glUseProgram(program);
		glClear(GL_COLOR_BUFFER_BIT);

		list=(THDisplayObject**)currentFrame->objectList.arr;
		num=currentFrame->objectList.num;
		for(i=0;i<num;++i)
		{
			object=list[i];
			assert(object->drawFunction);
			if(object->visible)
			{
				object->drawFunction(object);
			}
		}
		eglSwapBuffers(eglDisplay, eglSurface);
		//Draw End

		currentFrame->onEnterFrame();
		
		usleep(GLThreadSleepTime);
	}

	pthread_mutex_destroy(&glPauseMut);
	pthread_cond_destroy(&glPauseCond);
	pthread_exit(0);
}

static void init_display(struct android_app* app) {
	hasSthDo=HAS_STH_NOTHING_TODO;
	glThread=newThread(glThreadFunc,app);
}
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
			THLog("Init Window()");
            if (app->window) {
                init_display(app);
            }
            break;
		case APP_CMD_TERM_WINDOW:
			THLog("Term Window()");
			//쓰레드 완전 정지
			hasSthDo|=HAS_STH_TERM_DISPLAY;
			term_display();
			//term_display();
			if(hasSthDo&HAS_STH_PAUSE_THREAD)
			{
				hasSthDo-=HAS_STH_PAUSE_THREAD;
				pthread_cond_signal(&glPauseCond);
			}
			break;
		case APP_CMD_GAINED_FOCUS:
			THLog("Gained Focus()");
			//쓰레드 정지 해제
			OnResume();
			if(hasSthDo&HAS_STH_PAUSE_THREAD)
			{
				hasSthDo-=HAS_STH_PAUSE_THREAD;
				pthread_cond_signal(&glPauseCond);
			}
			break;
		case APP_CMD_LOST_FOCUS:
			THLog("Lost Focus()");
			//쓰레드 정지
			OnPause();
			hasSthDo|=HAS_STH_PAUSE_THREAD;
			break;

			/*
		case APP_CMD_SAVE_STATE:
		{
			
			THLog("Save States()");
			saved_state* savesth=(saved_state*)malloc(sizeof(saved_state));
			OnSaveInstanceState(savesth);
			app->savedState=savesth;
			app->savedStateSize=sizeof(saved_state);
			
		}
            break;
			*/
		case APP_CMD_DESTROY:
			THLog("OnDestroy()");
			pthread_mutex_destroy(&textureMutex);
			pthread_cond_destroy(&textureCond);
			OnDestroy();
			break;
    }
}
#include <jni.h>
void android_main(struct android_app* state)
{
	JavaVM* vm=state->activity->vm;
	JNIEnv* env;
	(*vm)->GetEnv(vm,(void**)&env,JNI_VERSION_1_6);
	(*vm)->AttachCurrentThread(vm, &env, NULL);
	jclass activityClass=(*env)->GetObjectClass(env,state->activity->clazz);
	jmethodID createFileMethod=(*env)->GetMethodID(env,activityClass,"CreateFile","()V");
	(*env)->CallVoidMethod(env,state->activity->clazz,createFileMethod);
	(*vm)->DetachCurrentThread(vm);
	THLog("OnCreate()");
	pthread_mutex_init(&textureMutex,NULL);
	pthread_mutex_init(&glPauseMut,NULL);

	pthread_cond_init(&textureCond,NULL);
	pthread_cond_init(&glPauseCond,NULL);
    app_dummy();

	assetManager=state->activity->assetManager;

	extern char* filesPath;
	extern unsigned int filesPathLen;
	filesPathLen=strlen(state->activity->internalDataPath)+1;
	filesPath=(char*)malloc(sizeof(char)*(filesPathLen+1+MAX_FILE_NAME));
	filesPath[filesPathLen-1]='/';
	memcpy(filesPath,state->activity->internalDataPath,filesPathLen-1);

	OnCreate(state);

	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent=handle_input;
	/*
	if (state->savedState) {
		THLog("Load From Saved State");
		OnLoadInstanceState((saved_state*)state->savedState);
    }
	*/
	struct android_poll_source* source;
    int events;

	while (1) {
        while (ALooper_pollAll(-1, NULL, &events,
                (void**)&source) >= 0) {
            if (source) {
                source->process(state, source);
            }

			if(state->destroyRequested)
			{
				THLog("Destroy Requested");
				hasSthDo|=HAS_STH_TERM_DISPLAY;
				term_display();
				if(hasSthDo&HAS_STH_PAUSE_THREAD)
				{
					hasSthDo-=HAS_STH_PAUSE_THREAD;
					pthread_cond_signal(&glPauseCond);
				}
				return;
			}
        }
    }
}

static void ReadDataFromAsset(struct png_struct_def* png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
{
	assert(png_ptr->io_ptr);

	AAsset_read((AAsset*)(png_ptr->io_ptr),outBytes,byteCountToRead);
}
void LoadTexture(GLuint* texture,const char* name)
{
	THLog("Texture Generation : %s",name);
#ifndef NDEBUG
	unsigned int st;
#endif
	AAsset* aasset=AAssetManager_open(assetManager, name, AASSET_MODE_STREAMING);
	{
	char pngSig[8];
	AAsset_read(aasset,pngSig,8);
#ifndef NDEBUG
	st=
#endif
	png_sig_cmp((png_const_bytep)pngSig,0, 8);
	}
	assert(st==0);
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	assert(png_ptr);
	png_infop info_ptr = NULL;
	info_ptr = png_create_info_struct(png_ptr);
	assert(info_ptr);
	png_set_read_fn(png_ptr, aasset, ReadDataFromAsset);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	png_uint_32 width,height;
	int bitDepth = 0;
	int colorType = -1;
	png_get_IHDR(png_ptr, info_ptr,
	   &width,
	   &height,
	   &bitDepth,
	   &colorType,
	   NULL, NULL, NULL);
	THLog("LibPNG // Width : %d , Height : %d",width,height);
#ifndef NDEBUG
		st=width&(width-1);
		assert(st==0);
		st=height&(height-1);
		assert(st==0);
#endif
	const png_uint_32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
	png_bytep colorBuf=(png_bytep)malloc(bytesPerRow*height);
	png_bytep* rowBytes=(png_bytep*)malloc(height*sizeof(png_bytep));
	
	unsigned int i=height;
	while(i)
	{
		--i;
		rowBytes[i]=colorBuf+(bytesPerRow*i);
		//png_read_row(png_ptr, (png_bytep)colorBuf+(i*bytesPerRow), NULL);
	}
	png_read_image(png_ptr,rowBytes);
	free(rowBytes);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	AAsset_close(aasset);

	if(pthread_equal(pthread_self(),glThread))
	{
		THLog("From GLThread");
		genTexture(texture,colorBuf,width,height);
		free(colorBuf);
		return;
	}else{
		THLog("Not From GLThread");
		texturePointer=texture;
		textureColorBuffer=colorBuf;
		textureInfo=(width<<16)|height;

		hasSthDo|=HAS_STH_TEXTURE_CREATE;

		pthread_cond_wait(&textureCond,&textureMutex);
	}
}