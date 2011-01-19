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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/mohawk/resource_cache.h $
 * $Id: resource_cache.h 54165 2010-11-09 18:52:29Z mthreepwood $
 *
 */

#ifndef RESOURCE_CACHE_H
#define RESOURCE_CACHE_H

#include "common/array.h"
#include "common/stream.h"

namespace Mohawk {

class ResourceCache {
public:
	ResourceCache();
	~ResourceCache();

	bool enabled;

	void clear();
	void add(uint32 tag, uint16 id, Common::SeekableReadStream *data);

	// Returns NULL if not found
	Common::SeekableReadStream *search(uint32 tag, uint16 id);

private:
	struct DataObject {
		uint32 tag;
		uint16 id;
		Common::SeekableReadStream *data;
	};

	Common::Array<DataObject> _store;
};

} // End of namespace Mohawk

#endif
