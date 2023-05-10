#ifndef THKim_THDisplayObject
#define THKim_THDisplayObject

#include <THPrivate.h>
#include <assert.h>
#include <math.h>

struct _THMovieClip
{
	THDisplayObject displayObject;
	THTexture* texture;
	float x;
	float y;
	const GLfloat* vertexBuffer;
	float cos;
	float sin;
};
typedef struct _THMovieClip THMovieClip;

THMovieClip* newMovieClip(THTexture* texture,const GLfloat* vertexBuffer);
static inline void THMovieClipSetRotation(THMovieClip* mc,const float angle)
{
	const float a=angle*0.01745329251f;
	mc->cos=cosf(a);
	mc->sin=sinf(a);
}
static inline void THMovieClipSetRotationRadian(THMovieClip* mc,const float angle)
{
	mc->cos=cosf(angle);
	mc->sin=sinf(angle);
}
static inline void THMovieClipSetRotationCS(THMovieClip* mc,const float cos,const float sin)
{
	mc->cos=cos;
	mc->sin=sin;
}
static inline void THMovieClipRotate(THMovieClip* mc,const float angle)
{
	const float a=angle*0.01745329251f;
	const float cos=cosf(a);
	const float sin=sinf(a);

	const float mccos=mc->cos;

	mc->cos=mccos*cos-mc->sin*sin;
	mc->sin=mccos*sin+mc->sin*cos;
}
static inline void THMovieClipRotateRadian(THMovieClip* mc,const float angle)
{
	const float cos=cosf(angle);
	const float sin=sinf(angle);

	const float mccos=mc->cos;

	mc->cos=mccos*cos-mc->sin*sin;
	mc->sin=mccos*sin+mc->sin*cos;
}
static inline void THMovieClipRotateCS(THMovieClip* mc,const float cos,const float sin)
{
	const float mccos=mc->cos;
	mc->cos=mccos*cos-mc->sin*sin;
	mc->sin=mccos*sin+mc->sin*cos;
}

struct _THGroupClip
{
	THDisplayObject displayObject;
	THArray list;
	float x;
	float y;
};
typedef struct _THGroupClip THGroupClip;
THGroupClip* newGroupClip(const unsigned int groupNum);
void THGroupClipAddChild(THGroupClip* gc,THDisplayObject* displayObject);
static inline void THGroupReAddChild(THGroupClip* gc,THDisplayObject* displayObject)
{
	assert(displayObject->frame==gc);
	THArrayRePush(&gc->list,displayObject);
}

THButton* newButton(float wid,float hei);
static inline void THButtonSetX(THButton* btn,const float x)
{
	btn->xplusw=(btn->xplusw)-(btn->x)+x;
	btn->x=x;
}
static inline void THButtonSetY(THButton* btn,const float y)
{
	btn->yplush=(btn->yplush)-(btn->y)+y;
	btn->y=y;
}
static inline void THButtonSetPosition(THButton* btn,const float x,const float y)
{
	btn->xplusw=btn->xplusw-btn->x+x;
	btn->yplush=btn->yplush-btn->y+y;
	btn->x=x;
	btn->y=y;
}
static inline void THButtonSetWidth(THButton* btn,const float w)
{
	btn->xplusw=btn->x+w;
}
static inline void THButtonSetHeight(THButton* btn,const float h)
{
	btn->yplush=btn->y+h;
}
static inline void THButtonSetSize(THButton* btn,const float w,const float h)
{
	btn->xplusw=btn->x+w;
	btn->yplush=btn->y+h;
}
static inline float THButtonGetWidth(THButton* btn)
{
	return (btn->xplusw)-(btn->x);
}
static inline float THButtonGetHeight(THButton* btn)
{
	return (btn->yplush)-(btn->y);
}
static inline int THButtonHitTest(THButton* btn,const float x,const float y)
{
	return (btn->x<x)&&(btn->xplusw>x)&&(btn->y<y)&&(btn->yplush>y);
}

#endif
