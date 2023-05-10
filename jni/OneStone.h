#ifndef THKim_THOneStone
#define THKim_THOneStone

#include <GameEngine/DisplayObject/THDisplayObject.h>

struct _THColorClip
{
	THDisplayObject displayObject;
	float a;
	const GLfloat* vertexBuffer;
};
typedef struct _THColorClip THColorClip;
THColorClip* newColorClip(float a,const GLfloat* ver);

typedef struct
{
	THDisplayObject displayObject;
	unsigned char firstNumber;
	unsigned char secondNumber;
	unsigned char thirdNumber;
	float wid;
	float hei;
	float x;
	float y;
} THNumberClip;
static inline void THNumberSetScale(THNumberClip* mc,float scale)
{
	mc->wid=120.0f*scale;
	mc->hei=233.0f*scale;
}
THNumberClip* newNumberClip(unsigned int num);
void THNumberSet(THNumberClip* mc,unsigned int num);

#endif
