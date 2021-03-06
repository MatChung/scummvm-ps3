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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/common/unarj.h $
 * $Id: unarj.h 48040 2010-02-13 11:56:30Z fingolfin $
 *
 */

#ifndef COMMON_UNARJ_H
#define COMMON_UNARJ_H

namespace Common {

class Archive;
class String;

/**
 * This factory method creates an Archive instance corresponding to the content
 * of the ARJ compressed file with the given name.
 *
 * May return 0 in case of a failure.
 */
Archive *makeArjArchive(const String &name);

} // End of namespace Common

#endif
