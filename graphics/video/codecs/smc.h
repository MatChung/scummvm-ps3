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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/graphics/video/codecs/smc.h $
 * $Id: smc.h 49165 2010-05-23 18:33:55Z mthreepwood $
 *
 */

#ifndef GRAPHICS_VIDEO_SMC_H
#define GRAPHICS_VIDEO_SMC_H

#include "graphics/video/codecs/codec.h"

namespace Graphics {

enum {
	CPAIR = 2,
	CQUAD = 4,
	COCTET = 8,
	COLORS_PER_TABLE = 256
};

class SMCDecoder : public Codec {
public:
	SMCDecoder(uint16 width, uint16 height);
	~SMCDecoder() { delete _surface; }

	Surface *decodeImage(Common::SeekableReadStream *stream);
	PixelFormat getPixelFormat() const { return PixelFormat::createFormatCLUT8(); }

private:
	Surface *_surface;

	// SMC color tables
	byte _colorPairs[COLORS_PER_TABLE * CPAIR];
	byte _colorQuads[COLORS_PER_TABLE * CQUAD];
	byte _colorOctets[COLORS_PER_TABLE * COCTET];
};

} // End of namespace Graphics

#endif
