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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/lastexpress/drawable.h $
 * $Id: drawable.h 53579 2010-10-18 19:17:38Z sev $
 *
 */

#ifndef LASTEXPRESS_DRAWABLE_H
#define LASTEXPRESS_DRAWABLE_H

#include "graphics/surface.h"

namespace LastExpress {

class Drawable {
public:
	virtual ~Drawable() {}

	virtual Common::Rect draw(Graphics::Surface *surface) = 0;
};

} // End of namespace LastExpress

#endif // LASTEXPRESS_DRAWABLE_H
