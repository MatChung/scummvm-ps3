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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/backends/mutex/sdl/sdl-mutex.cpp $
 * $Id: sdl-mutex.cpp 54584 2010-11-29 18:48:43Z lordhoto $
 *
 */

#include "common/scummsys.h"

#if defined(SDL_BACKEND)

#include "backends/mutex/sdl/sdl-mutex.h"
#include "backends/platform/sdl/sdl-sys.h"


OSystem::MutexRef SdlMutexManager::createMutex() {
	return (OSystem::MutexRef) SDL_CreateMutex();
}

void SdlMutexManager::lockMutex(OSystem::MutexRef mutex) {
	SDL_mutexP((SDL_mutex *) mutex);
}

void SdlMutexManager::unlockMutex(OSystem::MutexRef mutex) {
	SDL_mutexV((SDL_mutex *) mutex);
}

void SdlMutexManager::deleteMutex(OSystem::MutexRef mutex) {
	SDL_DestroyMutex((SDL_mutex *) mutex);
}

#endif
