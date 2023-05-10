LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE:=THGLInstant
LOCAL_SRC_FILES:=GameEngine/Rendering/THDrawing.c\
GameEngine/Rendering/THRenderer.c\
GameEngine/Util/THArray.c\
GameEngine/Util/THThread.c\
GameEngine/Util/THFile.c\
GameEngine/DisplayObject/THFrame.c\
GameEngine/DisplayObject/THDisplayObject.c\
OneStone.c\
GameFrame.c\
LevelFrame.c\
StartFrame.c\
Callback.c
LOCAL_LDLIBS:=-lGLESv2 -lEGL -llog -landroid
LOCAL_STATIC_LIBRARIES:=android_native_app_glue THLibPNG

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:=THLibPNG
LOCAL_EXPORT_LDLIBS:=-lz
LOCAL_SRC_FILES:=libpng/png.c\
libpng/pngerror.c\
libpng/pngget.c\
libpng/pngmem.c\
libpng/pngpread.c\
libpng/pngread.c\
libpng/pngrio.c\
libpng/pngrtran.c\
libpng/pngrutil.c\
libpng/pngset.c\
libpng/pngtest.c\
libpng/pngtrans.c\
libpng/pngwio.c\
libpng/pngwrite.c\
libpng/pngwtran.c\
libpng/pngwutil.c
include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/native_app_glue)