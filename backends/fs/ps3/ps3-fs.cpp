/* ScummVM - Graphic Adventure Engine
*
* ScummVM is the legal property of its developers, whose names
* are too numerous to list here. Please refer to the COPYRIGHT
* file distributed with this source distribution.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
* $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/backends/fs/ps3/ps3-fs.cpp $
* $Id: ps3-fs.cpp 46111 2009-11-23 23:17:50Z fingolfin $
*/

#include "backends/fs/abstract-fs.h"
#include "backends/platform/ps3/ps3.h"
#include "ps3-fs.h"
#include "ps3-filestream.h"
#include <cell/cell_fs.h>
#include <sys/paths.h>

extern OSystem_PS3 *g_systemPs3;
Common::String _homedir_;

void setupHomeDir(const char *argv0)
{
	if(argv0==NULL)
		return;

	printf("setupHomeDir(%s)\n",argv0);

	int lpos=((int)strrchr(argv0,'/'))-((int)argv0);


	Common::String hd(argv0,lpos+1);

	printf("setupHomeDir() - found \"%s\"\n",hd.c_str());

	_homedir_=hd;
}

const char *_lastPathComponent(const Common::String &str)
{
	if (str.empty())
		return "";

	return strrchr(str.c_str(),'/')+1;
	/*
	const char *start = str.c_str();
	const char *cur = start + str.size() - 2;

	while (cur >= start && *cur != '/' && *cur != ':') {
	--cur;
	}

	cur++;

	// printf("lastPathComponent path=%s token=%s\n", start, cur);

	return cur;*/
}

Ps3FilesystemNode::Ps3FilesystemNode()
{
//	printf("Ps3FilesystemNode::NEW FSNODE() - ");

	_isHere = true;
	_isDirectory = true;
	_verified = true;
	//_displayName = Common::String("PlayStation 3");
	_path = _homedir_;
	_displayName = _lastPathComponent(_path);

	printf("path: %s\n",_path.c_str());
}

Ps3FilesystemNode::Ps3FilesystemNode(const Common::String &path)
{
//	printf("Ps3FilesystemNode::NEW FSNODE(%s)\n", path.c_str());


	if (path.empty() || path.equals("."))
	{
		_isHere = true;
		_isDirectory = true; /* root is always a dir */
		_displayName = Common::String("PlayStation 3");
		_verified = true;
		_path=_homedir_;
	}
	else
	{
		_path = path;
		_verified = false;
		doverify();
		if (!_isHere)
			return;

		//printf("  --  _lastPathComponent...");
		_displayName = Common::String(_lastPathComponent(_path));
		//printf(" \"%s\"\n",_displayName.c_str());

		if (_isDirectory && _path.lastChar() != '/')
		{
			//printf(" pathpadding...");
			_path+= '/';
			//printf(" done\n");
		}
		//printf("......\n");
	}
	//printf("::::::::\n");
}

Ps3FilesystemNode::Ps3FilesystemNode(const Common::String &path, bool verify)
{
//	printf("Ps3FilesystemNode::NEW FSNODE(%s, %d)\n", path.c_str(), verify);

	if (path.empty() || path.equals("."))
	{
		_isHere = true;
		_isDirectory = true; /* root is always a dir */
		_displayName = Common::String("PlayStation 3");
		_verified = true;
		_path=_homedir_;
	}
	else
	{
		_path = path;
		_verified = false;
		if (verify) {
			doverify();

			if (!_isHere)
				return;

		} else {
			_verified = false;
			_isDirectory = false;
			_isHere = false; // true
		}

		//printf("  --  _lastPathComponent");
		_displayName = Common::String(_lastPathComponent(_path));
		//printf(" %s\n",_displayName.c_str());

		if (_isDirectory && _path.lastChar() != '/')
		{
			//printf(" pathpadding...");
			_path+= '/';
			//printf(" done\n");
		}
		//printf("......\n");
	}
	//printf("::::::::\n");
}

Ps3FilesystemNode::Ps3FilesystemNode(const Ps3FilesystemNode *node)
{
	_displayName = node->_displayName;
	_isDirectory = node->_isDirectory;
	_path = node->_path;
	_isHere = node->_isHere;
	_verified = node->_verified;
}

void Ps3FilesystemNode::doverify(void)
{
	//PS3Device medium;

	if (_verified)
		return;

	_verified = true;

//	printf(" verify: %s -> ", _path.c_str());

	if (_path.empty() || _path[0] != '/')
	{
//		printf("relative path ! fixing... ");

		_path=_homedir_+_path;
	}

	int fd=0;
	CellFsErrno err=0;
	err = cellFsOpen(_path.c_str(), CELL_FS_O_RDONLY, &fd, NULL, 0);

//	printf("_path = %s -- fio.open -> %d,%d\n", _path.c_str(), fd,err);

	if (err == CELL_FS_SUCCEEDED || err == CELL_FS_EISDIR)
	{
		cellFsClose(fd);
		_isHere = true;
		if (err == CELL_FS_EISDIR)
		{
			_isDirectory = true;
//			printf("  yes (dir)\n");
		}
		else
		{
			_isDirectory = false;
//			printf("  yes (file)\n");
		}
		return;
	}
	if(fd>0)
		cellFsClose(fd);

	_isHere = false;
	_isDirectory = false;

//	printf("  no\n");
	return;
}

AbstractFSNode *Ps3FilesystemNode::getChild(const Common::String &n) const
{
//	printf("Ps3FilesystemNode::getChild : %s\n", n.c_str());

	if (!_isDirectory)
		return NULL;

	return new Ps3FilesystemNode(_path+n, 1);
}

bool Ps3FilesystemNode::getChildren(AbstractFSList &list, ListMode mode, bool hidden) const {
	//TODO: honor the hidden flag

//	printf("Ps3FilesystemNode::getChildren()\n");

	if (!_isDirectory)
		return false;

	/*if (_isRoot) {
	list.push_back(new Ps3FilesystemNode(SYS_APP_HOME));
	list.push_back(new Ps3FilesystemNode(SYS_DEV_HDD0));
	list.push_back(new Ps3FilesystemNode(SYS_DEV_BDVD));
	return true;
	}
	else
	*/
	{
		CellFsErrno err;
		int fd;

		err=cellFsOpendir(_path.c_str(),&fd);

		// printf("dopen = %d\n", fd);

		if (err == CELL_FS_SUCCEEDED)
		{
			CellFsDirent dirent;
			uint64_t dreadRes;
			while (true)
			{
				//memset(&dirent,0,sizeof(dirent));
				err = cellFsReaddir(fd, &dirent,&dreadRes);
//				printf("   err: %d,%d\n",err,dreadRes);
				if(dreadRes<1 || err!=CELL_FS_SUCCEEDED)
					break;

				if (dirent.d_name[0] == '.')
				{
//					printf("   ignoring: %s\n",dirent.d_name);
					continue; // ignore '.' and '..'
				}
				//if(strcasecmp("EBOOT.BIN",dirent.d_name)==0)
				//{
				//	printf("   ignoring: %s\n",dirent.d_name);
				//	continue;
				//}

				if ( (mode == Common::FSNode::kListAll) ||

					((mode == Common::FSNode::kListDirectoriesOnly) &&
					(dirent.d_type & CELL_FS_TYPE_DIRECTORY)) ||

					((mode == Common::FSNode::kListFilesOnly) &&
					!(dirent.d_type & CELL_FS_TYPE_DIRECTORY)) )
				{

					/*
					dirEntry._isHere = true;
					dirEntry._isDirectory = (bool)(dirent.d_type & CELL_FS_TYPE_DIRECTORY);
					dirEntry._isRoot = false;

					dirEntry._path = _path;
					dirEntry._path += dirent.d_name;
					if (dirEntry._isDirectory && dirEntry._path.lastChar() != '/')
					dirEntry._path += '/';
					dirEntry._displayName = dirent.d_name;

					dirEntry._verified = true;
					*/
					Common::String newPath=_path+dirent.d_name;
					Ps3FilesystemNode *dirEntry=new Ps3FilesystemNode(newPath);
					//printf("   child: %s\n",newPath.c_str());
					list.push_back(dirEntry);
				}
				else
				{
//					printf("   ignoring: %s\n",dirent.d_name);
				}
			}
			cellFsClosedir(fd);
//			printf("=====Ps3FilesystemNode::getChildren()===success\n");
			return true;
		}
	}
//	printf("=====Ps3FilesystemNode::getChildren()===fail\n");
	return false;
}

AbstractFSNode *Ps3FilesystemNode::getParent() const {
//	printf("Ps3FilesystemNode::getParent : path = %s\n", _path.c_str());

	//if (_isRoot)
	//	return new Ps3FilesystemNode(this); // FIXME : 0 ???

	//if (_path.lastChar() == ':') // devs
	//	return new Ps3FilesystemNode(); // N: default is root

	const char *start = _path.c_str();
	const char *end = _lastPathComponent(_path);

	Common::String str(start, end - start);
//	printf("  parent = %s\n", str.c_str());

	return new Ps3FilesystemNode(str, true);
}


Common::SeekableReadStream *Ps3FilesystemNode::createReadStream() {
	Common::SeekableReadStream *ss = PS3FileStream::makeFromPath(getPath().c_str(), false);
	return ss;
}

Common::WriteStream *Ps3FilesystemNode::createWriteStream() {
	return PS3FileStream::makeFromPath(getPath().c_str(), true);
}
