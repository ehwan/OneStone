#include <GameEngine/Util/THFile.h>
#include <THPrivate.h>
#include <assert.h>
#include <string.h>

char* filesPath;
unsigned int filesPathLen;

FILE* newFile(const char* filename,const unsigned int nameLen,const char* mode)
{
	THLog("File Open : %s , %s",filename,mode?mode:"r+");
#ifndef NDEBUG
	const unsigned int cnl=strlen(filename);
	if(nameLen!=cnl)
	{
		THError("Param NameLen is different %s",filename);
		assert(0);
	}
	if(nameLen>MAX_FILE_NAME)
	{
		THError("filename %s Name is Too long to open",filename);
		assert(0);
	}
#endif
	memcpy(filesPath+filesPathLen,filename,nameLen);
	filesPath[filesPathLen+nameLen]='\0';

	FILE* file=fopen(filesPath,mode?mode:"r+");
	return file;
}