#ifndef THKim_THDrawing
#define THKim_THDrawing

#include <GLES2/gl2.h>

extern GLuint program;
extern GLuint positionHandler;
extern GLuint rotposHandler;
extern GLuint textureHandler;
extern GLuint hasColorHandler,colorHandler;

void genTexture(GLuint* tex,void* data,GLsizei width,GLsizei height);//,const unsigned int isRepeat);
static inline void drawTexture(const GLfloat* vertexBuf,const float cos,const float sin,const float x,const float y,THTexture* texture,const GLenum mode,const unsigned int num)
{
	glBindTexture(GL_TEXTURE_2D,texture->textureID);
	glVertexAttribPointer(positionHandler,2,GL_FLOAT,GL_FALSE,0,vertexBuf);
	glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,texture->textureBuffer);
	glVertexAttrib4f(rotposHandler,cos,sin,x,y);
	glUniform1i(hasColorHandler,0);
	glDrawArrays(mode, 0, num);
}

#endif
