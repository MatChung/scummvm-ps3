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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/common/macresman.h $
 * $Id: macresman.h 54122 2010-11-07 17:17:21Z fingolfin $
 *
 */

#include "common/array.h"
#include "common/file.h"

#ifndef COMMON_MACRESMAN_H
#define COMMON_MACRESMAN_H

namespace Common {

class FSNode;

typedef Array<uint16> MacResIDArray;
typedef Array<uint32> MacResTagArray;

/**
 * Class for reading Mac Binary files.
 * Is able to read dumped resource forks too.
 */
class MacResManager {

public:
	MacResManager();
	~MacResManager();

	bool open(String filename);
	bool open(FSNode path, String filename);
	void close();

	bool hasDataFork() const;
	bool hasResFork() const;

	static bool isMacBinary(SeekableReadStream &stream);

	/**
	 * Read resource from the Mac Binary file
	 * @param typeID FourCC of the type
	 * @param resID Resource ID to fetch
	 * @return Pointer to a SeekableReadStream with loaded resource
	 */
	SeekableReadStream *getResource(uint32 typeID, uint16 resID);

	/**
	 * Read resource from the Mac Binary file
	 * @note This will take the first resource that matches this name, regardless of type
	 * @param filename filename of the resource
	 * @return Pointer to a SeekableReadStream with loaded resource
	 */
	SeekableReadStream *getResource(const String &filename);

	/**
	 * Read resource from the Mac Binary file
	 * @param typeID FourCC of the type
	 * @param filename filename of the resource
	 * @return Pointer to a SeekableReadStream with loaded resource
	 */
	SeekableReadStream *getResource(uint32 typeID, const String &filename);

	SeekableReadStream *getDataFork();
	String getResName(uint32 typeID, uint16 resID) const;
	uint32 getResForkSize() const;
	String computeResForkMD5AsString(uint32 length = 0) const;

	String getBaseFileName() const { return _baseFileName; }

	/**
	 * Convert cursor from crsr format to format suitable for feeding to CursorMan
	 * @param data Pointer to the cursor data
	 * @param datasize Size of the cursor data
	 * @param cursor Pointer to memory where result cursor will be stored. The memory
	 *               block will be malloc()'ed
	 * @param w Pointer to int where the cursor width will be stored
	 * @param h Pointer to int where the cursor height will be stored
	 * @param hotspot_x Storage for cursor hotspot X coordinate
	 * @param hotspot_Y Storage for cursor hotspot Y coordinate
	 * @param keycolor Pointer to int where the transpared color value will be stored
	 * @param colored If set to true then colored cursor will be returned (if any).
	 *                b/w version will be used otherwise
	 * @param palette Pointer to memory where the cursor palette will be stored.
	 *                The memory will be malloc()'ed
	 * @param palSize Pointer to integer where the palette size will be stored.
	 */
	static void convertCrsrCursor(byte *data, int datasize, byte **cursor, int *w, int *h,
					  int *hotspot_x, int *hotspot_y, int *keycolor, bool colored, byte **palette, int *palSize);

	/**
	 * Return list of resource IDs with specified type ID
	 */
	MacResIDArray getResIDArray(uint32 typeID);

	/**
	 * Return list of resource tags
	 */
	MacResTagArray getResTagArray();

private:
	SeekableReadStream *_stream;
	String _baseFileName;

	bool load(SeekableReadStream &stream);

	bool loadFromRawFork(SeekableReadStream &stream);
	bool loadFromMacBinary(SeekableReadStream &stream);
	bool loadFromAppleDouble(SeekableReadStream &stream);

	enum {
		kResForkNone = 0,
		kResForkRaw,
		kResForkMacBinary,
		kResForkAppleDouble
	} _mode;

	void readMap();

	struct ResMap {
		uint16 resAttr;
		uint16 typeOffset;
		uint16 nameOffset;
		uint16 numTypes;
	};

	struct ResType {
		uint32 id;
		uint16 items;
		uint16 offset;
	};

	struct Resource {
		uint16 id;
		int16 nameOffset;
		byte attr;
		uint32 dataOffset;
		char *name;
	};

	typedef Resource *ResPtr;

	int32 _resForkOffset;
	uint32 _resForkSize;

	uint32 _dataOffset;
	uint32 _dataLength;
	uint32 _mapOffset;
	uint32 _mapLength;
	ResMap _resMap;
	ResType *_resTypes;
	ResPtr  *_resLists;
};

} // End of namespace Common

#endif
