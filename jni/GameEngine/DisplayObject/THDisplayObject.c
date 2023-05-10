#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <GameEngine/Rendering/THDrawing.h>
#include <malloc.h>
#include <assert.h>

float thofsX=0.0f;
float thofsY=0.0f;
//Offset Coordinations;

static void drawMovieClip(const THMovieClip* mc)
{
	drawTexture(mc->vertexBuffer,mc->cos,mc->sin,mc->x+thofsX,mc->y+thofsY,mc->texture,GL_TRIANGLE_STRIP,4);
}
static void drawGroupClip(const THGroupClip *gc)
{
	THDisplayObject** list=(THDisplayObject**)gc->list.arr;
	THDisplayObject* object;
	unsigned int i;
	const unsigned int num=gc->list.num;
	thofsX+=gc->x;
	thofsY+=gc->y;
	for(i=0;i<num;++i)
	{
		object=list[i];
		assert(object->drawFunction);
		if(object->visible)
		{object->drawFunction(object);}
	}
	thofsX-=gc->x;
	thofsY-=gc->y;
}			  
THMovieClip* newMovieClip(THTexture* texture,const GLfloat* vertexBuffer)
{
	THMovieClip* mc=(THMovieClip*)malloc(sizeof(THMovieClip));
	assert(mc);
	mc->displayObject.visible=1;
	mc->displayObject.drawFunction=(THDrawFunctions)drawMovieClip;
	mc->displayObject.frame=0;
	mc->x=0.0f;
	mc->y=0.0f;
	mc->texture=texture;
	mc->sin=0.0f;
	mc->cos=1.0f;
	mc->vertexBuffer=vertexBuffer;

	return mc;
}
THGroupClip* newGroupClip(const unsigned int groupNum)
{
	THGroupClip* mc=(THGroupClip*)malloc(sizeof(THGroupClip));
	assert(mc);
	mc->displayObject.visible=1;
	mc->displayObject.drawFunction=(THDrawFunctions)drawGroupClip;
	mc->displayObject.frame=0;
	mc->x=0.0f;
	mc->y=0.0f;

	THArrayInit(&mc->list,groupNum);

	return mc;
}
void THGroupClipAddChild(THGroupClip* gc,THDisplayObject* displayObject)
{
	assert(displayObject->frame==0);
	THArrayPush(&gc->list,displayObject);
	displayObject->frame=(THFrame*)gc;
}

THButton* newButton(float wid,float hei)
{
	THButton* btn=(THButton*)malloc(sizeof(THButton));
	assert(btn);
	btn->data=0;
	btn->frame=0;
	btn->enable=1;
	btn->x=0.0f;
	btn->y=0.0f;
	btn->xplusw=wid;
	btn->yplush=hei;
	btn->onDown=0;
	btn->onRelease=0;

	return btn;
}
