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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/backends/platform/wince/CEgui/Toolbar.h $
 * $Id: Toolbar.h 53979 2010-10-31 17:11:43Z fingolfin $
 *
 */

#ifndef CEGUI_TOOLBAR_H
#define CEGUI_TOOLBAR_H

#include "common/scummsys.h"

#include "GUIElement.h"

namespace CEGUI {

class Toolbar : public GUIElement {
public:
	virtual ~Toolbar();
	virtual bool action(int x, int y, bool pushed) = 0;
protected:
	Toolbar();

};

} // End of namespace CEGUI

#endif
