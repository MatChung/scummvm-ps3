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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/backends/platform/sdl/posix/posix-main.cpp $
 * $Id: posix-main.cpp 54584 2010-11-29 18:48:43Z lordhoto $
 *
 */

#include "common/scummsys.h"

#if defined(UNIX) && !defined(MACOSX) && !defined(SAMSUNGTV) && !defined(LINUXMOTO) && !defined(GP2XWIZ) && !defined(GP2X) && !defined(DINGUX)

#include "backends/platform/sdl/posix/posix.h"
#include "backends/plugins/sdl/sdl-provider.h"
#include "base/main.h"

int main(int argc, char *argv[]) {

	// Create our OSystem instance
	g_system = new OSystem_POSIX();
	assert(g_system);

	// Pre initialize the backend
	((OSystem_POSIX *)g_system)->init();

#ifdef DYNAMIC_MODULES
	PluginManager::instance().addPluginProvider(new SDLPluginProvider());
#endif

	// Invoke the actual ScummVM main entry point:
	int res = scummvm_main(argc, argv);

	// Free OSystem
	delete (OSystem_POSIX *)g_system;

	return res;
}

#endif