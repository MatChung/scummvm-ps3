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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/gob/console.h $
 * $Id: console.h 53544 2010-10-16 17:25:08Z drmccoy $
 *
 */

#ifndef GOB_CONSOLE_H
#define GOB_CONSOLE_H

#include "gui/debugger.h"

namespace Gob {

class GobEngine;

class GobConsole : public GUI::Debugger {
public:
	GobConsole(GobEngine *vm);
	virtual ~GobConsole(void);

protected:
	virtual void preEnter();
	virtual void postEnter();

private:
	GobEngine *_vm;

	bool cmd_varSize(int argc, const char **argv);
	bool cmd_var8(int argc, const char **argv);
	bool cmd_var16(int argc, const char **argv);
	bool cmd_var32(int argc, const char **argv);
	bool cmd_varString(int argc, const char **argv);
};

} // End of namespace Mohawk

#endif
