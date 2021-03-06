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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/backends/platform/ds/arm9/source/scummhelp.h $
 * $Id: scummhelp.h 50689 2010-07-05 16:11:11Z fingolfin $
 *
 */

#ifndef _SCUMMHELP_H_
#define _SCUMMHELP_H_

#include "common/str.h"
#include "common/util.h"

namespace DS {

void updateStrings(byte gameId, byte version, Common::Platform platform,
			int page, Common::String &title, Common::String *&key, Common::String *&dsc);

}	// End of namespace DS


#endif
