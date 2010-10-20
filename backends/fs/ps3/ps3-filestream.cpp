#include "ps3-filestream.h"
#include "common/config-manager.h"
#include "common/file.h"
#include "engines/engine.h"
#include <cell/cell_fs.h>


PS3FileStream *PS3FileStream::makeFromPath(const Common::String &path, bool writeMode) {
	//Ps2File *file = new Ps2File();

	int mode = writeMode ? (CELL_FS_O_WRONLY | CELL_FS_O_CREAT) : CELL_FS_O_RDONLY;
	int fd=0;

	CellFsErrno err=cellFsOpen(path.c_str(),mode,&fd,NULL,0);
	
	//printf("PS3FileStream::makeFromPath(%s) --> %d, %d, %d\n",path.c_str(),err,fd,mode);

	if(err==CELL_FS_SUCCEEDED)
		return new PS3FileStream(fd);


	return 0;
}

PS3FileStream::PS3FileStream(int fd)
{
	_fd=fd;
	_err=0;
	_eos=false;
}

PS3FileStream:: ~PS3FileStream()
{
	_err=cellFsClose(_fd);
}

bool PS3FileStream::err() const
{
//	if(_err)
//		printf("PS3FileStream::err() - %d\n",_err);
	return _err!=0;
}

void PS3FileStream::clearErr()
{
//	if(_err)
//		printf("PS3FileStream::clearErr() - %d\n",_err);
	_err=0;
}

bool PS3FileStream::eos() const
{
//	if(_eos)
//		printf("PS3FileStream::eos() - %d\n",_eos);
	return _eos;
}

uint32 PS3FileStream::write(const void *dataPtr, uint32 dataSize)
{
//	printf("PS3FileStream::write() - %X,%d  ",dataPtr,dataSize);

	uint64_t written=0;
	_err=cellFsWrite(_fd,dataPtr,dataSize,&written);

//	printf(" written: %d,%d\n",_err,written);

	return written;
}

bool PS3FileStream::flush()
{
//	printf("PS3FileStream::flush()\n");
	_err=cellFsFsync(_fd);
	
	return err();
}

int32 PS3FileStream::pos() const
{
//	printf("PS3FileStream::pos() - ");
	uint64_t pos=0;
	cellFsLseek(_fd,0,CELL_FS_SEEK_CUR,&pos);
//	printf("%d\n",pos);
	return pos;
}

int32 PS3FileStream::size() const
{
//	printf("PS3FileStream::size() - ");
	uint64_t old_pos=0;
	uint64_t size=0;
	int err;
	err=cellFsLseek(_fd,0,CELL_FS_SEEK_CUR,&old_pos);
//	printf("%d,%d -- ",old_pos,err);
	err=cellFsLseek(_fd,0,CELL_FS_SEEK_END,&size);
//	printf("%d,%d -- ",size,err);
	err=cellFsLseek(_fd,old_pos,CELL_FS_SEEK_SET,&old_pos);
//	printf("%d,%d\n",old_pos,err);
	return size;
}

bool PS3FileStream::seek(int32 offs, int whence)
{
	//printf("PS3FileStream::seek(%d,%d)\n",offs,whence);
	uint64_t pos=0;
	_err=cellFsLseek(_fd,offs,whence,&pos);
	return err();
}

uint32 PS3FileStream::read(void *dataPtr, uint32 dataSize)
{
	if(dataSize<1)
		return 0;

	//printf("PS3FileStream::read() - %X,%d  ",dataPtr,dataSize);
	uint64_t read=0;
	_err=cellFsRead(_fd,dataPtr,dataSize,&read);
	//printf(" read: %d,%d\n",_err,read);

	if(read==0)
		_eos=true;

	return read;
}
