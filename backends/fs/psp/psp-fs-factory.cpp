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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/backends/fs/psp/psp-fs-factory.cpp $
 * $Id: psp-fs-factory.cpp 54258 2010-11-16 08:23:13Z fingolfin $
 */

#if defined(__PSP__)
#include "backends/fs/psp/psp-fs-factory.h"
#include "backends/fs/psp/psp-fs.cpp"

DECLARE_SINGLETON(PSPFilesystemFactory);

AbstractFSNode *PSPFilesystemFactory::makeRootFileNode() const {
	return new PSPFilesystemNode();
}

AbstractFSNode *PSPFilesystemFactory::makeCurrentDirectoryFileNode() const {
	char buf[MAXPATHLEN];
	char * ret = 0;

	PowerMan.beginCriticalSection();
	ret = getcwd(buf, MAXPATHLEN);
	PowerMan.endCriticalSection();

	return (ret ? new PSPFilesystemNode(buf) : NULL);
}

AbstractFSNode *PSPFilesystemFactory::makeFileNodePath(const Common::String &path) const {
	return new PSPFilesystemNode(path, true);
}
#endif
