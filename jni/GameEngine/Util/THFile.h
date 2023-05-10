#ifndef THKim_THFile
#define THKim_THFile

#include <stdio.h>

FILE* newFile(const char* filename,const unsigned int nameLen,const char* mode);

static inline long THFileGetLength(FILE* file)
{
	fseek(file,0,SEEK_END);
	return ftell(file);
}
static inline long THFileGetCurrentPos(FILE* file)
{
	return ftell(file);
}
static inline void THFileMove(FILE* file,const long pos)
{
	fseek(file,pos,SEEK_SET);
}
static inline void THFileWrite(FILE* file,void* data,const long len)
{
	fwrite(data,len,1,file);
}
static inline void THFileRead(FILE* file,void* buf,const long len)
{
	fread(buf,len,1,file);
}

#endif
