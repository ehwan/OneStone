#include <OneStone.h>
#include <GameEngine/Rendering/THDrawing.h>
//#include <malloc.h>

extern float thofsX;
extern float thofsY;
extern THTexture* numberTextures[10];

static void drawColorClip(const THColorClip *gc)
{
	glDisableVertexAttribArray(textureHandler);
	glVertexAttribPointer(positionHandler,2,GL_FLOAT,GL_FALSE,0,gc->vertexBuffer);
	glVertexAttrib4f(rotposHandler,1.0f,0.0f,0.0f,0.0f);
	glVertexAttrib4f(colorHandler,0.0f,0.0f,0.0f,gc->a);
	glUniform1i(hasColorHandler,1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glEnableVertexAttribArray(textureHandler);
}
static void drawNumberClip(const THNumberClip* mc)
{
	const float x=mc->x+thofsX;
	const float y=mc->y+thofsY;
	const float width=mc->wid;
	const float height=mc->hei;
	const unsigned int firstNum=mc->firstNumber;
	const unsigned int secondNum=mc->secondNumber;
	const unsigned int thirdNum=mc->thirdNumber;

	THTexture* numTexture;

	const GLfloat vertexs[]={
			0.0f,height,
			width,height,
			0.0f,0.0f,
			width,0.0f
	};

	glUseProgram(program);
	
	unsigned int hasThird=thirdNum<10;
	unsigned int hasSecond=secondNum<10;
	if(hasThird)
	{
		numTexture=numberTextures[thirdNum];

		glBindTexture(GL_TEXTURE_2D,numTexture->textureID);
		
		glVertexAttribPointer(positionHandler,2,GL_FLOAT,GL_FALSE,0,vertexs);

		glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,numTexture->textureBuffer);

		glVertexAttrib4f(rotposHandler,1.0f,0.0f,x+(width*0.5f),y);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	if(hasSecond)
	{
		numTexture=numberTextures[secondNum];

		glBindTexture(GL_TEXTURE_2D,numTexture->textureID);

		glVertexAttribPointer(positionHandler,2,GL_FLOAT,GL_FALSE,0,vertexs);

		glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,numTexture->textureBuffer);

		glVertexAttrib4f(rotposHandler,1.0f,0.0f,x+(hasThird?-width*0.5f:0.0f),y);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	if(firstNum<10)
	{
		numTexture=numberTextures[firstNum];

		glBindTexture(GL_TEXTURE_2D,numTexture->textureID);

		glVertexAttribPointer(positionHandler,2,GL_FLOAT,GL_FALSE,0,vertexs);

		glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,numTexture->textureBuffer);

		glVertexAttrib4f(rotposHandler,1.0f,0.0f,x+(hasSecond?hasThird?-width*1.5f:-width:-width*0.5f),y);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}
THColorClip* newColorClip(float a,const GLfloat* ver)
{
	THColorClip* mc=(THColorClip*)malloc(sizeof(THColorClip));
	assert(mc);
	mc->displayObject.visible=1;
	mc->displayObject.drawFunction=(THDrawFunctions)drawColorClip;
	mc->displayObject.frame=0;

	mc->a=a;
	mc->vertexBuffer=ver;

	return mc;
}
void THNumberSet(THNumberClip* mc,unsigned int num)
{
	assert(num<1000);
	if(num>99)
	{
		unsigned int fn=num*0.01f;
		unsigned int sn=(num*0.1f)-(fn*10);
		mc->thirdNumber=num-(fn*100)-(sn*10);
		mc->secondNumber=sn;
		mc->firstNumber=fn;
	}else if(num>9)
	{
		unsigned int fn=num*0.1f;
		mc->secondNumber=num-(fn*10);
		mc->firstNumber=fn;
		mc->thirdNumber=10;
	}else
	{
		mc->firstNumber=num;
		mc->secondNumber=10;
		mc->thirdNumber=10;
	}
}
THNumberClip* newNumberClip(unsigned int num)
{
	THNumberClip* mc=(THNumberClip*)malloc(sizeof(THNumberClip));
	assert(mc);
	mc->displayObject.visible=1;
	mc->displayObject.frame=0;
	mc->displayObject.drawFunction=(THDrawFunctions)drawNumberClip;
	mc->x=0.0f;
	mc->y=0.0f;
	
	THNumberSet(mc,num);

	return mc;
}
