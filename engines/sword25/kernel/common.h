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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/sword25/kernel/common.h $
 * $Id: common.h 53378 2010-10-13 00:01:40Z sev $
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

/*
 * common.h
 * -----------
 * This file contains functions or macros that are used across the entire project.
 * It is therefore extremely important that this header file be referenced in all
 * the other header files in the project.
 *
 * Autor: Malte Thiesen
 */

#ifndef SWORD25_COMMON_H
#define SWORD25_COMMON_H

// Global constants
#define DEBUG

#define BS_ACTIVATE_LOGGING // When defined, logging is activated

// Engine Includes
#include "sword25/kernel/log.h"

#include "common/debug.h"

#define BS_ASSERT(EXP) assert(EXP)

#endif
