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

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
* $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/toon/resource.h $
* $Id: resource.h 53098 2010-10-09 11:11:26Z lordhoto $
*
*/

#ifndef TOON_RESOURCE_H
#define TOON_RESOURCE_H

#include "common/array.h"
#include "common/str.h"
#include "common/file.h"
#include "common/stream.h"

namespace Toon {

class PakFile {
public:
	PakFile();
	~PakFile();

	void open(Common::SeekableReadStream *rs, Common::String packName, bool preloadEntirePackage);
	uint8 *getFileData(Common::String fileName, uint32 *fileSize);
	Common::String getPackName() { return _packName; }
	Common::SeekableReadStream *createReadStream(Common::String fileName);
	void close();

protected:
	struct File {
		char _name[13];
		int32 _offset;
		int32 _size;
	};
	Common::String _packName;

	uint8 *_buffer;
	int32 _bufferSize;

	uint32 _numFiles;
	Common::Array<File> _files;
	Common::File *_fileHandle;


};

class ToonEngine;

class Resources {
public:
	Resources(ToonEngine *vm);
	void openPackage(Common::String file, bool preloadEntirePackage);
	void closePackage(Common::String fileName);
	Common::SeekableReadStream *openFile(Common::String file);
	uint8 *getFileData(Common::String fileName, uint32 *fileSize);

protected:
	ToonEngine *_vm;
	Common::Array<PakFile *> _pakFiles;

};

} // End of namespace Toon
#endif
