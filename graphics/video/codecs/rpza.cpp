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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/graphics/video/codecs/rpza.cpp $
 * $Id: rpza.cpp 49165 2010-05-23 18:33:55Z mthreepwood $
 *
 */

 // Based off ffmpeg's RPZA decoder

#include "graphics/video/codecs/rpza.h"

#include "common/system.h"
#include "graphics/colormasks.h"

namespace Graphics {

RPZADecoder::RPZADecoder(uint16 width, uint16 height) : Codec() {
	_pixelFormat = g_system->getScreenFormat();

	// We need to increase the surface size to a multiple of 4
	uint16 wMod = width % 4;
	if(wMod != 0)
		width += 4 - wMod;

	debug(2, "RPZA corrected width: %d", width);

	_surface = new Surface();
	_surface->create(width, height, _pixelFormat.bytesPerPixel);
}

#define ADVANCE_BLOCK() \
	pixelPtr += 4; \
	if (pixelPtr >= _surface->w) { \
		pixelPtr = 0; \
		rowPtr += _surface->w * 4; \
	} \
	totalBlocks--; \
	if (totalBlocks < 0) \
		error("block counter just went negative (this should not happen)") \

// Convert from RGB555 to the format specified by the screen
#define PUT_PIXEL(color) \
	if ((int32)blockPtr < _surface->w * _surface->h) { \
		byte r = 0, g = 0, b = 0; \
		colorToRGB<ColorMasks<555> >(color, r, g, b); \
		if (_pixelFormat.bytesPerPixel == 2) \
			*((uint16 *)_surface->pixels + blockPtr) = _pixelFormat.RGBToColor(r, g, b); \
		else \
			*((uint32 *)_surface->pixels + blockPtr) = _pixelFormat.RGBToColor(r, g, b); \
	} \
	blockPtr++

Surface *RPZADecoder::decodeImage(Common::SeekableReadStream *stream) {
	uint16 colorA = 0, colorB = 0;
	uint16 color4[4];

	uint32 rowPtr = 0;
	uint32 pixelPtr = 0;
	uint32 blockPtr = 0;
	uint32 rowInc = _surface->w - 4;
	uint16 ta;
	uint16 tb;

	// First byte is always 0xe1. Warn if it's different
	byte firstByte = stream->readByte();
	if (firstByte != 0xe1)
		warning("First RPZA chunk byte is 0x%02x instead of 0xe1", firstByte);

	// Get chunk size, ingnoring first byte
	uint32 chunkSize = stream->readUint16BE() << 8;
	chunkSize += stream->readByte();

	// If length mismatch use size from MOV file and try to decode anyway
	if (chunkSize != (uint32)stream->size()) {
		warning("MOV chunk size != encoded chunk size; using MOV chunk size");
		chunkSize = stream->size();
	}

	// Number of 4x4 blocks in frame
	int32 totalBlocks = ((_surface->w + 3) / 4) * ((_surface->h + 3) / 4);

	// Process chunk data
	while ((uint32)stream->pos() < chunkSize) {
		byte opcode = stream->readByte(); // Get opcode
		byte numBlocks = (opcode & 0x1f) + 1; // Extract block counter from opcode

		// If opcode MSbit is 0, we need more data to decide what to do
		if ((opcode & 0x80) == 0) {
			colorA = (opcode << 8) | stream->readByte();
			opcode = 0;
			if (stream->readByte() & 0x80) {
				// Must behave as opcode 110xxxxx, using colorA computed
				// above. Use fake opcode 0x20 to enter switch block at
				// the right place
				opcode = 0x20;
				numBlocks = 1;
			}
			stream->seek(-1, SEEK_CUR);
		}

		switch (opcode & 0xe0) {
		case 0x80: // Skip blocks
			while (numBlocks--) {
				ADVANCE_BLOCK();
			}
			break;
		case 0xa0: // Fill blocks with one color
			colorA = stream->readUint16BE();
			while (numBlocks--) {
				blockPtr = rowPtr + pixelPtr;
				for (byte pixel_y = 0; pixel_y < 4; pixel_y++) {
					for (byte pixel_x = 0; pixel_x < 4; pixel_x++) {
						PUT_PIXEL(colorA);
					}
					blockPtr += rowInc;
				}
				ADVANCE_BLOCK();
			}
			break;

		// Fill blocks with 4 colors
		case 0xc0:
			colorA = stream->readUint16BE();
		case 0x20:
			colorB = stream->readUint16BE();

			// Sort out the colors
			color4[0] = colorB;
			color4[1] = 0;
			color4[2] = 0;
			color4[3] = colorA;

			// Red components
			ta = (colorA >> 10) & 0x1F;
			tb = (colorB >> 10) & 0x1F;
			color4[1] |= ((11 * ta + 21 * tb) >> 5) << 10;
			color4[2] |= ((21 * ta + 11 * tb) >> 5) << 10;

			// Green components
			ta = (colorA >> 5) & 0x1F;
			tb = (colorB >> 5) & 0x1F;
			color4[1] |= ((11 * ta + 21 * tb) >> 5) << 5;
			color4[2] |= ((21 * ta + 11 * tb) >> 5) << 5;

			// Blue components
			ta = colorA & 0x1F;
			tb = colorB & 0x1F;
			color4[1] |= ((11 * ta + 21 * tb) >> 5);
			color4[2] |= ((21 * ta + 11 * tb) >> 5);

			while (numBlocks--) {
				blockPtr = rowPtr + pixelPtr;
				for (byte pixel_y = 0; pixel_y < 4; pixel_y++) {
					byte index = stream->readByte();
					for (byte pixel_x = 0; pixel_x < 4; pixel_x++){
						byte idx = (index >> (2 * (3 - pixel_x))) & 0x03;
						PUT_PIXEL(color4[idx]);
					}
					blockPtr += rowInc;
				}
				ADVANCE_BLOCK();
			}
			break;

		// Fill block with 16 colors
		case 0x00:
			blockPtr = rowPtr + pixelPtr;
			for (byte pixel_y = 0; pixel_y < 4; pixel_y++) {
				for (byte pixel_x = 0; pixel_x < 4; pixel_x++){
					// We already have color of upper left pixel
					if (pixel_y != 0 || pixel_x != 0)
						colorA = stream->readUint16BE();

					PUT_PIXEL(colorA);
				}
				blockPtr += rowInc;
			}
			ADVANCE_BLOCK();
			break;

		// Unknown opcode
		default:
			error("Unknown opcode %02x in rpza chunk", opcode);
		}
	}

	return _surface;
}

} // End of namespace Graphics
