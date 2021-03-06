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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/graphics/video/codecs/qtrle.h $
 * $Id: qtrle.h 49165 2010-05-23 18:33:55Z mthreepwood $
 *
 */

#ifndef GRAPHICS_VIDEO_QTRLE_H
#define GRAPHICS_VIDEO_QTRLE_H

#include "graphics/pixelformat.h"
#include "graphics/video/codecs/codec.h"

namespace Graphics {

class QTRLEDecoder : public Codec {
public:
	QTRLEDecoder(uint16 width, uint16 height, byte bitsPerPixel);
	~QTRLEDecoder();

	Surface *decodeImage(Common::SeekableReadStream *stream);
	PixelFormat getPixelFormat() const { return _pixelFormat; }

private:
	byte _bitsPerPixel;

	Surface *_surface;
	PixelFormat _pixelFormat;

	void decode1(Common::SeekableReadStream *stream, uint32 rowPtr, uint32 linesToChange);
	void decode2_4(Common::SeekableReadStream *stream, uint32 rowPtr, uint32 linesToChange, byte bpp);
	void decode8(Common::SeekableReadStream *stream, uint32 rowPtr, uint32 linesToChange);
	void decode16(Common::SeekableReadStream *stream, uint32 rowPtr, uint32 linesToChange);
	void decode24(Common::SeekableReadStream *stream, uint32 rowPtr, uint32 linesToChange);
	void decode32(Common::SeekableReadStream *stream, uint32 rowPtr, uint32 linesToChange);
};

} // End of namespace Graphics

#endif
