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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/backends/mutex/null/null-mutex.h $
 * $Id: null-mutex.h 54584 2010-11-29 18:48:43Z lordhoto $
 *
 */

#ifndef BACKENDS_MUTEX_NULL_H
#define BACKENDS_MUTEX_NULL_H

#include "backends/mutex/mutex.h"

/**
 * Null mutex manager
 */
class NullMutexManager : MutexManager {
public:
	virtual OSystem::MutexRef createMutex() { return OSystem::MutexRef(); }
	virtual void lockMutex(OSystem::MutexRef mutex) {}
	virtual void unlockMutex(OSystem::MutexRef mutex) {}
	virtual void deleteMutex(OSystem::MutexRef mutex) {}
};

#endif