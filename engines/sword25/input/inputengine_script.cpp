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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/sword25/input/inputengine_script.cpp $
 * $Id: inputengine_script.cpp 53568 2010-10-18 17:12:00Z sev $
 *
 */

/*
 * This code is based on Broken Sword 2.5 engine
 *
 * Copyright (c) Malte Thiesen, Daniel Queteschiner and Michael Elsdoerfer
 *
 * Licensed under GNU GPL v2
 *
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "common/ptr.h"
#include "common/str.h"
#include "sword25/kernel/common.h"
#include "sword25/kernel/kernel.h"
#include "sword25/kernel/callbackregistry.h"
#include "sword25/script/script.h"
#include "sword25/script/luabindhelper.h"
#include "sword25/script/luacallback.h"

#include "sword25/input/inputengine.h"

#define BS_LOG_PREFIX "INPUTENGINE"

namespace Sword25 {

// -----------------------------------------------------------------------------
// Callback-Objekte
// -----------------------------------------------------------------------------

static void TheCharacterCallback(int Character);
static void TheCommandCallback(int Command);

namespace {
class CharacterCallbackClass : public LuaCallback {
public:
	CharacterCallbackClass(lua_State *L) : LuaCallback(L) {}

	Common::String Character;

protected:
	int PreFunctionInvokation(lua_State *L) {
		lua_pushstring(L, Character.c_str());
		return 1;
	}
};
Common::SharedPtr<CharacterCallbackClass> CharacterCallbackPtr;

// -----------------------------------------------------------------------------

class CommandCallbackClass : public LuaCallback {
public:
	CommandCallbackClass(lua_State *L) : LuaCallback(L) {
		Command = InputEngine::KEY_COMMAND_BACKSPACE;
	}

	InputEngine::KEY_COMMANDS Command;

protected:
	int PreFunctionInvokation(lua_State *L) {
		lua_pushnumber(L, Command);
		return 1;
	}
};
Common::SharedPtr<CommandCallbackClass> CommandCallbackPtr;

// -------------------------------------------------------------------------

struct CallbackfunctionRegisterer {
	CallbackfunctionRegisterer() {
		CallbackRegistry::instance().registerCallbackFunction("LuaCommandCB", TheCommandCallback);
		CallbackRegistry::instance().registerCallbackFunction("LuaCharacterCB", TheCharacterCallback);
	}
};
static CallbackfunctionRegisterer Instance;
}

// -----------------------------------------------------------------------------

static InputEngine *GetIE() {
	Kernel *pKernel = Kernel::GetInstance();
	BS_ASSERT(pKernel);
	InputEngine *pIE = pKernel->GetInput();
	BS_ASSERT(pIE);
	return pIE;
}

// -----------------------------------------------------------------------------

static int Init(lua_State *L) {
	InputEngine *pIE = GetIE();

	lua_pushbooleancpp(L, pIE->Init());
	return 1;
}

// -----------------------------------------------------------------------------

static int Update(lua_State *L) {
	InputEngine *pIE = GetIE();

	// Beim ersten Aufruf der Update()-Methode werden die beiden Callbacks am Input-Objekt registriert.
	// Dieses kann nicht in _RegisterScriptBindings() passieren, da diese Funktion vom Konstruktor der abstrakten Basisklasse aufgerufen wird und die
	// Register...()-Methoden abstrakt sind, im Konstruktor der Basisklasse also nicht aufgerufen werden k�nnen.
	static bool FirstCall = true;
	if (FirstCall) {
		FirstCall = false;
		pIE->RegisterCharacterCallback(TheCharacterCallback);
		pIE->RegisterCommandCallback(TheCommandCallback);
	}

	pIE->Update();
	return 0;
}

// -----------------------------------------------------------------------------

static int IsLeftMouseDown(lua_State *L) {
	InputEngine *pIE = GetIE();

	lua_pushbooleancpp(L, pIE->IsLeftMouseDown());
	return 1;
}

// -----------------------------------------------------------------------------

static int IsRightMouseDown(lua_State *L) {
	InputEngine *pIE = GetIE();

	lua_pushbooleancpp(L, pIE->IsRightMouseDown());
	return 1;
}

// -----------------------------------------------------------------------------

static int WasLeftMouseDown(lua_State *L) {
	InputEngine *pIE = GetIE();

	lua_pushbooleancpp(L, pIE->WasLeftMouseDown());
	return 1;
}

// -----------------------------------------------------------------------------

static int WasRightMouseDown(lua_State *L) {
	InputEngine *pIE = GetIE();

	lua_pushbooleancpp(L, pIE->WasRightMouseDown());
	return 1;
}

// -----------------------------------------------------------------------------

static int IsLeftDoubleClick(lua_State *L) {
	InputEngine *pIE = GetIE();

	lua_pushbooleancpp(L, pIE->IsLeftDoubleClick());
	return 1;
}

// -----------------------------------------------------------------------------

static int GetMouseX(lua_State *L) {
	InputEngine *pIE = GetIE();

	lua_pushnumber(L, pIE->GetMouseX());
	return 1;
}

// -----------------------------------------------------------------------------

static int GetMouseY(lua_State *L) {
	InputEngine *pIE = GetIE();

	lua_pushnumber(L, pIE->GetMouseY());
	return 1;
}

// -----------------------------------------------------------------------------

static int IsKeyDown(lua_State *L) {
	InputEngine *pIE = GetIE();

	lua_pushbooleancpp(L, pIE->IsKeyDown((uint) luaL_checknumber(L, 1)));
	return 1;
}

// -----------------------------------------------------------------------------

static int WasKeyDown(lua_State *L) {
	InputEngine *pIE = GetIE();

	lua_pushbooleancpp(L, pIE->WasKeyDown((uint) luaL_checknumber(L, 1)));
	return 1;
}

// -----------------------------------------------------------------------------

static int SetMouseX(lua_State *L) {
	InputEngine *pIE = GetIE();

	pIE->SetMouseX((int) luaL_checknumber(L, 1));
	return 0;
}

// -----------------------------------------------------------------------------

static int SetMouseY(lua_State *L) {
	InputEngine *pIE = GetIE();

	pIE->SetMouseY((int) luaL_checknumber(L, 1));
	return 0;
}

// -----------------------------------------------------------------------------

static void TheCharacterCallback(int Character) {
	CharacterCallbackPtr->Character = static_cast<byte>(Character);
	lua_State *L = static_cast<lua_State *>(Kernel::GetInstance()->GetScript()->getScriptObject());
	CharacterCallbackPtr->invokeCallbackFunctions(L, 1);
}

// -----------------------------------------------------------------------------

static int RegisterCharacterCallback(lua_State *L) {
	luaL_checktype(L, 1, LUA_TFUNCTION);
	CharacterCallbackPtr->registerCallbackFunction(L, 1);

	return 0;
}

// -----------------------------------------------------------------------------

static int UnregisterCharacterCallback(lua_State *L) {
	luaL_checktype(L, 1, LUA_TFUNCTION);
	CharacterCallbackPtr->unregisterCallbackFunction(L, 1);

	return 0;
}

// -----------------------------------------------------------------------------

static void TheCommandCallback(int Command) {
	CommandCallbackPtr->Command = static_cast<InputEngine::KEY_COMMANDS>(Command);
	lua_State *L = static_cast<lua_State *>(Kernel::GetInstance()->GetScript()->getScriptObject());
	CommandCallbackPtr->invokeCallbackFunctions(L, 1);
}

// -----------------------------------------------------------------------------

static int RegisterCommandCallback(lua_State *L) {
	luaL_checktype(L, 1, LUA_TFUNCTION);
	CommandCallbackPtr->registerCallbackFunction(L, 1);

	return 0;
}

// -----------------------------------------------------------------------------

static int UnregisterCommandCallback(lua_State *L) {
	luaL_checktype(L, 1, LUA_TFUNCTION);
	CommandCallbackPtr->unregisterCallbackFunction(L, 1);

	return 0;
}

// -----------------------------------------------------------------------------

static const char *PACKAGE_LIBRARY_NAME = "Input";

static const luaL_reg PACKAGE_FUNCTIONS[] = {
	{"Init", Init},
	{"Update", Update},
	{"IsLeftMouseDown", IsLeftMouseDown},
	{"IsRightMouseDown", IsRightMouseDown},
	{"WasLeftMouseDown", WasLeftMouseDown},
	{"WasRightMouseDown", WasRightMouseDown},
	{"IsLeftDoubleClick", IsLeftDoubleClick},
	{"GetMouseX", GetMouseX},
	{"GetMouseY", GetMouseY},
	{"SetMouseX", SetMouseX},
	{"SetMouseY", SetMouseY},
	{"IsKeyDown", IsKeyDown},
	{"WasKeyDown", WasKeyDown},
	{"RegisterCharacterCallback", RegisterCharacterCallback},
	{"UnregisterCharacterCallback", UnregisterCharacterCallback},
	{"RegisterCommandCallback", RegisterCommandCallback},
	{"UnregisterCommandCallback", UnregisterCommandCallback},
	{0, 0}
};

#define X(k) {"KEY_" #k, InputEngine::KEY_##k}
#define Y(k) {"KEY_COMMAND_" #k, InputEngine::KEY_COMMAND_##k}
static const lua_constant_reg PACKAGE_CONSTANTS[] = {
	X(BACKSPACE), X(TAB), X(CLEAR), X(RETURN), X(PAUSE), X(CAPSLOCK), X(ESCAPE), X(SPACE), X(PAGEUP), X(PAGEDOWN), X(END), X(HOME), X(LEFT),
	X(UP), X(RIGHT), X(DOWN), X(PRINTSCREEN), X(INSERT), X(DELETE), X(0), X(1), X(2), X(3), X(4), X(5), X(6), X(7), X(8), X(9), X(A), X(B),
	X(C), X(D), X(E), X(F), X(G), X(H), X(I), X(J), X(K), X(L), X(M), X(N), X(O), X(P), X(Q), X(R), X(S), X(T), X(U), X(V), X(W), X(X), X(Y),
	X(Z), X(NUMPAD0), X(NUMPAD1), X(NUMPAD2), X(NUMPAD3), X(NUMPAD4), X(NUMPAD5), X(NUMPAD6), X(NUMPAD7), X(NUMPAD8), X(NUMPAD9), X(MULTIPLY),
	X(ADD), X(SEPARATOR), X(SUBTRACT), X(DECIMAL), X(DIVIDE), X(F1), X(F2), X(F3), X(F4), X(F5), X(F6),  X(F7), X(F8), X(F9), X(F10), X(F11),
	X(F12), X(NUMLOCK), X(SCROLL), X(LSHIFT), X(RSHIFT), X(LCONTROL), X(RCONTROL),
	Y(ENTER), Y(LEFT), Y(RIGHT), Y(HOME), Y(END), Y(BACKSPACE), Y(TAB), Y(INSERT), Y(DELETE),
	{0, 0}
};
#undef X
#undef Y

// -----------------------------------------------------------------------------

bool InputEngine::registerScriptBindings() {
	Kernel *pKernel = Kernel::GetInstance();
	BS_ASSERT(pKernel);
	ScriptEngine *pScript = pKernel->GetScript();
	BS_ASSERT(pScript);
	lua_State *L = static_cast<lua_State *>(pScript->getScriptObject());
	BS_ASSERT(L);

	if (!LuaBindhelper::addFunctionsToLib(L, PACKAGE_LIBRARY_NAME, PACKAGE_FUNCTIONS)) return false;
	if (!LuaBindhelper::addConstantsToLib(L, PACKAGE_LIBRARY_NAME, PACKAGE_CONSTANTS)) return false;

	CharacterCallbackPtr = Common::SharedPtr<CharacterCallbackClass>(new CharacterCallbackClass(L));
	CommandCallbackPtr = Common::SharedPtr<CommandCallbackClass>(new CommandCallbackClass(L));

	return true;
}

} // End of namespace Sword25
