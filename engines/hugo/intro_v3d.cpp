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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/hugo/intro_v3d.cpp $
 * $Id: intro_v3d.cpp 54018 2010-11-01 20:20:21Z strangerke $
 *
 */

/*
 * This code is based on original Hugo Trilogy source code
 *
 * Copyright (c) 1989-1995 David P. Gray
 *
 */

#include "common/system.h"

#include "hugo/hugo.h"
#include "hugo/intro.h"
#include "hugo/file.h"
#include "hugo/display.h"
#include "hugo/util.h"


namespace Hugo {
intro_v3d::intro_v3d(HugoEngine *vm) : IntroHandler(vm) {
}

intro_v3d::~intro_v3d() {
}

void intro_v3d::preNewGame() {
}

void intro_v3d::introInit() {
	_vm->_screen->loadFont(0);
	_vm->_file->readBackground(_vm->_numScreens - 1); // display splash screen

	char buffer[128];
	if (_boot.registered)
		sprintf(buffer, "%s  Registered Version", COPYRIGHT);
	else
		sprintf(buffer,"%s  Shareware Version", COPYRIGHT);

	_vm->_screen->writeStr(CENTER, 190, buffer, _TBROWN);

	if (scumm_stricmp(_boot.distrib, "David P. Gray")) {
		sprintf(buffer, "Distributed by %s.", _boot.distrib);
		_vm->_screen->writeStr(CENTER, 0, buffer, _TBROWN);
	}

	_vm->_screen->displayBackground();
	g_system->updateScreen();
	g_system->delayMillis(5000);

	_vm->_file->readBackground(22); // display screen MAP_3d
	_vm->_screen->displayBackground();
	introTicks = 0;
}

/**
* Hugo 3 - Preamble screen before going into game.  Draws path of Hugo's plane.
* Called every tick.  Returns TRUE when complete
*/
bool intro_v3d::introPlay() {
	byte introSize = _vm->getIntroSize();

//TODO : Add proper check of story mode
//#if STORY
	if (introTicks < introSize) {
		_vm->_screen->writeStr(_vm->_introX[introTicks], _vm->_introY[introTicks] - DIBOFF_Y, "x", _TBRIGHTWHITE);
		_vm->_screen->displayBackground();

		// Text boxes at various times
		switch (introTicks) {
		case 4:
			Utils::Box(BOX_OK, "%s", _vm->_textIntro[kIntro1]);
			break;
		case 9:
			Utils::Box(BOX_OK, "%s", _vm->_textIntro[kIntro2]);
			break;
		case 35:
			Utils::Box(BOX_OK, "%s", _vm->_textIntro[kIntro3]);
			break;
		}
	}

	return (++introTicks >= introSize);
//#else //STORY
//	return true;
//#endif //STORY
}

} // End of namespace Hugo
