#include "ps3-fs-node.h"
#include "backends/fs/stdiostream.h"
#include <psl1ght/lv2/filesystem.h>

Common::String _homedir_=Common::String("/dev_hdd0/scummvm/");

const char *_lastPathComponent(const Common::String &str)
{
	if (str.empty())
		return "";

	//return strrchr(str.c_str(),'/')+1;

	const char *start = str.c_str();
	const char *cur = start + str.size() - 2;

	while (cur >= start && *cur != '/' && *cur != ':')
	{
		--cur;
	}

	cur++;

	//printf("lastPathComponent path=%s token=%s\n", start, cur);

	return cur;
}

Ps3FilesystemNode::Ps3FilesystemNode()
{
	//printf("Ps3FilesystemNode::NEW FSNODE() - ");

	_isHere = true;
	_isDirectory = true;
	_verified = true;
	//_displayName = Common::String("PlayStation 3");
	_path = _homedir_;
	_displayName = _lastPathComponent(_path);

	//printf("path: %s\n",_path.c_str());
}

Ps3FilesystemNode::Ps3FilesystemNode(const Common::String &path)
{
	//printf("Ps3FilesystemNode::NEW FSNODE(%s)\n", path.c_str());


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
	//printf("Ps3FilesystemNode::NEW FSNODE(%s, %d)\n", path.c_str(), verify);

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

	//printf(" verify: %s -> ", _path.c_str());

	if (_path.empty() || _path[0] != '/')
	{
		//printf("relative path ! fixing... ");

		_path=_homedir_+_path;
	}


	_isHere = false;
	_isDirectory = false;

	Lv2FsFile fd=0;
	u32 err=0;
	err = lv2FsOpen(_path.c_str(), LV2_O_RDONLY, &fd,0, NULL, 0);

	//printf("_path = %s -- fio.open -> %d,%d\n", _path.c_str(), fd,err);

	if (err == 0 || err == 0x80010012)
	{
		lv2FsClose(fd);
		_isHere = true;
		if (err == 0x80010012)
		{
			_isDirectory = true;
			//printf("  yes (dir)\n");
		}
		else
		{
			_isDirectory = false;
			//printf("  yes (file)\n");
		}
		return;
	}
	if(fd>0)
		lv2FsClose(fd);

	_isHere = false;
	_isDirectory = false;

	//printf("  no\n");
	return;
}

AbstractFSNode *Ps3FilesystemNode::getChild(const Common::String &n) const
{
	//printf("Ps3FilesystemNode::getChild : %s\n", n.c_str());

	if (!_isDirectory)
		return NULL;

	return new Ps3FilesystemNode(_path+n, 1);
}

bool Ps3FilesystemNode::getChildren(AbstractFSList &list, ListMode mode, bool hidden) const {
	//TODO: honor the hidden flag

	//printf("Ps3FilesystemNode::getChildren(%s)\n",_path.c_str());

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
		s32 err;
		Lv2FsFile fd;

		err=lv2FsOpenDir(_path.c_str(),&fd);

		// printf("dopen = %d\n", fd);

		if (err == 0)
		{
			Lv2FsDirent dirent;
			uint64_t dreadRes;
			while (true)
			{
				//memset(&dirent,0,sizeof(dirent));
				err = lv2FsReadDir(fd, &dirent,&dreadRes);
				//printf("   err: %d,%d\n",err,(u32)dreadRes);
				if(dreadRes<1 || err!=0)
					break;

				if (dirent.d_name[0] == '.')
				{
					//printf("   ignoring: %s\n",dirent.d_name);
					continue; // ignore '.' and '..'
				}
				//if(strcasecmp("EBOOT.BIN",dirent.d_name)==0)
				//{
				//	//printf("   ignoring: %s\n",dirent.d_name);
				//	continue;
				//}

				if ( (mode == Common::FSNode::kListAll) ||

					((mode == Common::FSNode::kListDirectoriesOnly) &&
					(dirent.d_type & 1)) || //dir

					((mode == Common::FSNode::kListFilesOnly) &&
					!(dirent.d_type & 1)) )
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
					//printf("   ignoring: %s\n",dirent.d_name);
				}
			}
			lv2FsCloseDir(fd);
			//printf("=====Ps3FilesystemNode::getChildren()===success\n");
			return true;
		}
	}
	//printf("=====Ps3FilesystemNode::getChildren()===fail\n");
	return false;
}

AbstractFSNode *Ps3FilesystemNode::getParent() const {
	//printf("Ps3FilesystemNode::getParent : path = %s\n", _path.c_str());

	//if (_isRoot)
	//	return new Ps3FilesystemNode(this); // FIXME : 0 ???

	//if (_path.lastChar() == ':') // devs
	//	return new Ps3FilesystemNode(); // N: default is root

	const char *start = _path.c_str();
	const char *end = _lastPathComponent(_path);

	Common::String str(start, end - start);
//	//printf("  parent = %s\n", str.c_str());

	return new Ps3FilesystemNode(str, true);
}


Common::SeekableReadStream *Ps3FilesystemNode::createReadStream() {
	//printf("Ps3FilesystemNode::createReadStream(%s)\n", _path.c_str());
	Common::SeekableReadStream *ss = StdioStream::makeFromPath(getPath().c_str(), false);
	//printf("  0x%08X\n",(u32)(u64)ss);
	return ss;
}

Common::WriteStream *Ps3FilesystemNode::createWriteStream() {
	//printf("Ps3FilesystemNode::createWriteStream(%s)\n", _path.c_str());
	Common::WriteStream *ss =  StdioStream::makeFromPath(getPath().c_str(), true);
	//printf("  0x%08X\n",(u32)(u64)ss);
	return ss;
}
