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
	
	net_send("PS3FileStream::makeFromPath(%s) --> %d, %d, %d\n",path.c_str(),err,fd,mode);

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
	flush();
	_err=cellFsClose(_fd);
}

bool PS3FileStream::err() const
{
	if(_err)
		net_send("PS3FileStream::err() - %d\n",_err);
	return _err!=0;
}

void PS3FileStream::clearErr()
{
	if(_err)
		net_send("PS3FileStream::clearErr() - %d\n",_err);
	_err=0;
}

bool PS3FileStream::eos() const
{
	if(_eos)
		net_send("PS3FileStream::eos() - %d\n",_eos);
	return _eos;
}

uint32 PS3FileStream::write(const void *dataPtr, uint32 dataSize)
{
	net_send("PS3FileStream::write() - %d  ",dataSize);

	uint64_t written=0;
	_err=cellFsWrite(_fd,dataPtr,dataSize,&written);

	net_send(" written: %d,%d\n",_err,(int)written);

	return written;
}

bool PS3FileStream::flush()
{
	net_send("PS3FileStream::flush()\n");
	_err=cellFsFsync(_fd);
	
	return err();
}

int32 PS3FileStream::pos() const
{
//	printf("PS3FileStream::pos() - ");
	uint64_t l_pos=0;
	cellFsLseek(_fd,0,CELL_FS_SEEK_CUR,&l_pos);
//	printf("%d\n",pos);
	return l_pos;
}

int32 PS3FileStream::size() const
{
//	printf("PS3FileStream::size() - ");
	uint64_t old_pos=0;
	uint64_t l_size=0;
	int l_err;
	l_err=cellFsLseek(_fd,0,CELL_FS_SEEK_CUR,&old_pos);
//	printf("%d,%d -- ",old_pos,err);
	l_err=cellFsLseek(_fd,0,CELL_FS_SEEK_END,&l_size);
//	printf("%d,%d -- ",size,err);
	l_err=cellFsLseek(_fd,old_pos,CELL_FS_SEEK_SET,&old_pos);
//	printf("%d,%d\n",old_pos,err);
	return l_size;
}

bool PS3FileStream::seek(int32 offs, int whence)
{
	//printf("PS3FileStream::seek(%d,%d)\n",offs,whence);
	uint64_t l_pos=0;
	_err=cellFsLseek(_fd,offs,whence,&l_pos);
	return err();
}

uint32 PS3FileStream::read(void *dataPtr, uint32 dataSize)
{
	if(dataSize<1)
		return 0;

	//net_send("PS3FileStream::read() - %X,%d  ",dataPtr,dataSize);
	uint64_t l_read=0;
	_err=cellFsRead(_fd,dataPtr,dataSize,&l_read);
	//net_send(" read: %d,%d\n",_err,l_read);

	if(l_read==0)
		_eos=true;

	return l_read;
}
