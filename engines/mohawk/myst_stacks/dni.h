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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/mohawk/myst_stacks/dni.h $
 * $Id: dni.h 55245 2011-01-14 21:42:33Z bgk $
 *
 */

#ifndef MYST_SCRIPTS_DNI_H
#define MYST_SCRIPTS_DNI_H

#include "common/scummsys.h"
#include "common/util.h"
#include "mohawk/myst_scripts.h"

namespace Mohawk {

#define DECLARE_OPCODE(x) void x(uint16 op, uint16 var, uint16 argc, uint16 *argv)

class MohawkEngine_Myst;
struct MystScriptEntry;

class MystScriptParser_Dni : public MystScriptParser {
public:
	MystScriptParser_Dni(MohawkEngine_Myst *vm);
	~MystScriptParser_Dni();

	void disablePersistentScripts();
	void runPersistentScripts();

private:
	void setupOpcodes();
	uint16 getVar(uint16 var);

	void atrus_run();
	void loopVideo_run();
	void atrusLeft_run();

	DECLARE_OPCODE(o_handPage);

	DECLARE_OPCODE(o_atrus_init);

	bool _atrusRunning;
	bool _notSeenAtrus; // 56
	uint32 _atrusLeftTime; // 60
	Common::String _video; // 64
	bool _waitForLoop;
	uint32 _loopStart; // 72
	uint32 _loopEnd; // 76
	bool _atrusLeft; // 80
};

} // End of namespace Mohawk

#undef DECLARE_OPCODE

#endif