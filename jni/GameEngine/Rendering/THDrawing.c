#include <THPrivate.h>
#include <assert.h>
#include <malloc.h>

void genTexture(GLuint* tex,void* data,GLsizei width,GLsizei height)//,const unsigned int isRepeat)
{
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*
	if(isRepeat)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
	*/
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, data);
}
void THTextureSet(THTexture* texture,GLuint textureID,float x,float y,float w,float h,float imgW,float imgH)
{
	texture->textureBuffer=((float*)&(texture->textureBuffer))+1;
	texture->textureID=textureID;
	const float iW=1.0f/imgW;
	const float iH=1.0f/imgH;
	const float sx=x*iW;
	const float sy=y*iH;
	const float sx2=(x+w)*iW;
	const float sy2=(y+h)*iH;

	float* textureBuffer=texture->textureBuffer;
	textureBuffer[0]=sx;
	textureBuffer[1]=sy2;
	textureBuffer[2]=sx2;
	textureBuffer[3]=sy2;
	textureBuffer[4]=sx;
	textureBuffer[5]=sy;
	textureBuffer[6]=sx2;
	textureBuffer[7]=sy;
}
void THTextureSet2(THTexture* texture,GLuint textureID)
{
	texture->textureBuffer=((float*)&(texture->textureBuffer))+1;
	texture->textureID=textureID;

	float* const textureBuffer=texture->textureBuffer;
	textureBuffer[0]=0.0f;
	textureBuffer[1]=1.0f;
	textureBuffer[2]=1.0f;
	textureBuffer[3]=1.0f;
	textureBuffer[4]=0.0f;
	textureBuffer[5]=0.0f;
	textureBuffer[6]=1.0f;
	textureBuffer[7]=0.0f;
}
