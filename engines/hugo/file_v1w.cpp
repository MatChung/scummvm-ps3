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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/hugo/file_v1w.cpp $
 * $Id: file_v1w.cpp 54018 2010-11-01 20:20:21Z strangerke $
 *
 */

/*
 * This code is based on original Hugo Trilogy source code
 *
 * Copyright (c) 1989-1995 David P. Gray
 *
 */

#include "common/system.h"

#include "hugo/hugo.h"
#include "hugo/file.h"
#include "hugo/util.h"

namespace Hugo {
FileManager_v1w::FileManager_v1w(HugoEngine *vm) : FileManager_v2d(vm) {
}

FileManager_v1w::~FileManager_v1w() {
}

/**
* Open and read in an overlay file, close file
*/
void FileManager_v1w::readOverlay(int screenNum, image_pt image, ovl_t overlayType) {
	debugC(1, kDebugFile, "readOverlay(%d, ...)", screenNum);

	image_pt tmpImage = image;                      // temp ptr to overlay file
	_sceneryArchive1.seek((uint32)screenNum * sizeof(sceneBlock_t), SEEK_SET);

	sceneBlock_t sceneBlock;                        // Database header entry
	sceneBlock.scene_off = _sceneryArchive1.readUint32LE();
	sceneBlock.scene_len = _sceneryArchive1.readUint32LE();
	sceneBlock.b_off = _sceneryArchive1.readUint32LE();
	sceneBlock.b_len = _sceneryArchive1.readUint32LE();
	sceneBlock.o_off = _sceneryArchive1.readUint32LE();
	sceneBlock.o_len = _sceneryArchive1.readUint32LE();
	sceneBlock.ob_off = _sceneryArchive1.readUint32LE();
	sceneBlock.ob_len = _sceneryArchive1.readUint32LE();

	uint32 i = 0;
	switch (overlayType) {
	case BOUNDARY:
		_sceneryArchive1.seek(sceneBlock.b_off, SEEK_SET);
		i = sceneBlock.b_len;
		break;
	case OVERLAY:
		_sceneryArchive1.seek(sceneBlock.o_off, SEEK_SET);
		i = sceneBlock.o_len;
		break;
	case OVLBASE:
		_sceneryArchive1.seek(sceneBlock.ob_off, SEEK_SET);
		i = sceneBlock.ob_len;
		break;
	default:
		Utils::Error(FILE_ERR, "%s", "Bad ovl_type");
		break;
	}
	if (i == 0) {
		for (i = 0; i < OVL_SIZE; i++)
			image[i] = 0;
		return;
	}
	_sceneryArchive1.read(tmpImage, OVL_SIZE);
}

} // End of namespace Hugo

