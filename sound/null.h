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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/sound/null.h $
 * $Id: null.h 50281 2010-06-25 18:47:52Z athrxx $
 */

#ifndef SOUND_NULL_H
#define SOUND_NULL_H

#include "sound/musicplugin.h"
#include "sound/mpu401.h"
#include "common/translation.h"

/* NULL driver */
class MidiDriver_NULL : public MidiDriver_MPU401 {
public:
	int open() { return 0; }
	void send(uint32 b) { }
};


// Plugin interface

class NullMusicPlugin : public MusicPluginObject {
public:
	virtual const char *getName() const {
		return _s("No music");
	}

	virtual const char *getId() const {
		return "null";
	}

	virtual MusicDevices getDevices() const;
	Common::Error createInstance(MidiDriver **mididriver, MidiDriver::DeviceHandle = 0) const;
};

#endif
