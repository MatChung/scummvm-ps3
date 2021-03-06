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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/tools/create_toon/create_toon.h $
 * $Id: create_toon.h 53087 2010-10-08 22:30:39Z sev $
 *
 */

#ifndef CREATE_TOON_H
#define CREATE_TOON_H

#define ARRAYSIZE(x) ((int)(sizeof(x) / sizeof(x[0])))

#define DATAALIGNMENT 4

#define TOON_DAT_VER_MAJ 0  // 1 byte
#define TOON_DAT_VER_MIN 3  // 1 byte

// Number of Variante of the game. For the moment, it's the same
// than the number of languages
#define NUM_VARIANTE   5

typedef unsigned char  uint8;
typedef unsigned char  byte;
typedef unsigned short uint16;
typedef signed   short int16;

void writeTextArray(FILE *outFile, const char *textData[], int nbrText);

#endif // CREATE_TOON_H
