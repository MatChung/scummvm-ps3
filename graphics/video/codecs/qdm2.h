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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/graphics/video/codecs/qdm2.h $
 * $Id: qdm2.h 49843 2010-06-15 12:33:20Z fingolfin $
 *
 */

// Only compile if Mohawk is enabled or if we're building dynamic modules
#if defined(ENABLE_MOHAWK) || defined(DYNAMIC_MODULES)

#ifndef GRAPHICS_QDM2_H
#define GRAPHICS_QDM2_H

namespace Common {
	class SeekableReadStream;
}

namespace Audio {
	class AudioStream;
}

namespace Graphics {

/**
 * Create a new AudioStream from the QDM2 data in the given stream.
 *
 * @param stream       the SeekableReadStream from which to read the FLAC data
 * @param extraData    the QuickTime extra data stream
 * @return   a new AudioStream, or NULL, if an error occurred
 */
Audio::AudioStream *makeQDM2Stream(Common::SeekableReadStream *stream, Common::SeekableReadStream *extraData);

} // End of namespace Graphics

#endif // GRAPHICS_QDM2_H
#endif // Mohawk/Plugins guard
