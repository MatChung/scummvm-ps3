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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/saga/font_map.cpp $
 * $Id: font_map.cpp 33373 2008-07-28 12:46:30Z thebluegr $
 *
 */

// Font module character mapping table ( MS CP-850 to ISO 8859-1 )

// Translation table derived from http://www.kostis.net/charsets/

#include "saga/saga.h"
#include "saga/font.h"

namespace Saga {

const int Font::_charMap[128] = {
	// Characters 0 - 127 are mapped directly to ISO 8859-1
	199,		// 128 LATIN CAPITAL LETTER C WITH CEDILLA
	252,		// 129 LATIN SMALL LETTER U WITH DIAERESIS
	233,		// 130 LATIN SMALL LETTER E WITH ACUTE
	226,		// 131 LATIN SMALL LETTER A WITH CIRCUMFLEX
	228,		// 132 LATIN SMALL LETTER A WITH DIAERESIS
	224,		// 133 LATIN SMALL LETTER A WITH GRAVE
	229,		// 134 LATIN SMALL LETTER A WITH RING ABOVE
	231,		// 135 LATIN SMALL LETTER C WITH CEDILLA
	234,		// 136 LATIN SMALL LETTER E WITH CIRCUMFLEX
	235,		// 137 LATIN SMALL LETTER E WITH DIAERESIS
	232,		// 138 LATIN SMALL LETTER E WITH GRAVE
	239,		// 139 LATIN SMALL LETTER I WITH DIAERESIS
	238,		// 140 LATIN SMALL LETTER I WITH CIRCUMFLEX
	236,		// 141 LATIN SMALL LETTER I WITH GRAVE
	196,		// 142 LATIN CAPITAL LETTER A WITH DIAERESIS
	197,		// 143 LATIN CAPITAL LETTER A WITH RING ABOVE
	201,		// 144 LATIN CAPITAL LETTER E WITH ACUTE
	230,		// 145 LATIN SMALL LETTER AE
	198,		// 146 LATIN CAPITAL LETTER AE
	244,		// 147 LATIN SMALL LETTER O WITH CIRCUMFLEX
	246,		// 148 LATIN SMALL LETTER O WITH DIAERESIS
	242,		// 149 LATIN SMALL LETTER O WITH GRAVE
	251,		// 150 LATIN SMALL LETTER U WITH CIRCUMFLEX
	249,		// 151 LATIN SMALL LETTER U WITH GRAVE
	255,		// 152 LATIN SMALL LETTER Y WITH DIAERESIS
	214,		// 153 LATIN CAPITAL LETTER O WITH DIAERESIS
	220,		// 154 LATIN CAPITAL LETTER U WITH DIAERESIS
	248,		// 155 LATIN SMALL LETTER O WITH STROKE
	163,		// 156 POUND SIGN
	216,		// 157 LATIN CAPITAL LETTER O WITH STROKE
	215,		// 158 MULTIPLICATION SIGN
	0,			// 159 LATIN SMALL LETTER F WITH HOOK
	225,		// 160 LATIN SMALL LETTER A WITH ACUTE
	237,		// 161 LATIN SMALL LETTER I WITH ACUTE
	243,		// 162 LATIN SMALL LETTER O WITH ACUTE
	250,		// 163 LATIN SMALL LETTER U WITH ACUTE
	241,		// 164 LATIN SMALL LETTER N WITH TILDE
	209,		// 165 LATIN CAPITAL LETTER N WITH TILDE
	170,		// 166 FEMININE ORDINAL INDICATOR
	186,		// 167 MASCULINE ORDINAL INDICATOR
	191,		// 168 INVERTED QUESTION MARK
	174,		// 169 REGISTERED SIGN
	172,		// 170 NOT SIGN
	189,		// 171 VULGAR FRACTION ONE HALF
	188,		// 172 VULGAR FRACTION ONE QUARTER
	161,		// 173 INVERTED EXCLAMATION MARK
	171,		// 174 LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
	187,		// 175 RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
	0,			// 176 LIGHT SHADE
	0,			// 177 MEDIUM SHADE
	0,			// 178 DARK SHADE
	0,			// 179 BOX DRAWINGS LIGHT VERTICAL
	0,			// 180 BOX DRAWINGS LIGHT VERTICAL AND LEFT
	193,		// 181 LATIN CAPITAL LETTER A WITH ACUTE
	194,		// 182 LATIN CAPITAL LETTER A WITH CIRCUMFLEX
	192,		// 183 LATIN CAPITAL LETTER A WITH GRAVE
	169,		// 184 COPYRIGHT SIGN
	0,			// 185 BOX DRAWINGS DOUBLE VERTICAL AND LEFT
	0,			// 186 BOX DRAWINGS DOUBLE VERTICAL
	0,			// 187 BOX DRAWINGS DOUBLE DOWN AND LEFT
	0,			// 188 BOX DRAWINGS DOUBLE UP AND LEFT
	162,		// 189 CENT SIGN
	165,		// 190 YEN SIGN
	0,			// 191 BOX DRAWINGS LIGHT DOWN AND LEFT
	0,			// 192 BOX DRAWINGS LIGHT UP AND RIGHT
	0,			// 193 BOX DRAWINGS LIGHT UP AND HORIZONTAL
	0,			// 194 BOX DRAWINGS LIGHT DOWN AND HORIZONTAL
	0,			// 195 BOX DRAWINGS LIGHT VERTICAL AND RIGHT
	0,			// 196 BOX DRAWINGS LIGHT HORIZONTAL
	0,			// 197 BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL
	227,		// 198 LATIN SMALL LETTER A WITH TILDE
	195,		// 199 LATIN CAPITAL LETTER A WITH TILDE
	0,			// 200 BOX DRAWINGS DOUBLE UP AND RIGHT
	0,			// 201 BOX DRAWINGS DOUBLE DOWN AND RIGHT
	0,			// 202 BOX DRAWINGS DOUBLE UP AND HORIZONTAL
	0,			// 203 BOX DRAWINGS DOUBLE DOWN AND HORIZONTAL
	0,			// 204 BOX DRAWINGS DOUBLE VERTICAL AND RIGHT
	0,			// 205 BOX DRAWINGS DOUBLE HORIZONTAL
	0,			// 206 BOX DRAWINGS DOUBLE VERTICAL AND HORIZONTAL
	164,		// 207 CURRENCY SIGN
	240,		// 208 LATIN SMALL LETTER ETH
	208,		// 209 LATIN CAPITAL LETTER ETH
	202,		// 210 LATIN CAPITAL LETTER E WITH CIRCUMFLEX
	203,		// 211 LATIN CAPITAL LETTER E WITH DIAERESIS
	200,		// 212 LATIN CAPITAL LETTER E WITH GRAVE
	305,		// 213 LATIN SMALL LETTER DOTLESS I
	205,		// 214 LATIN CAPITAL LETTER I WITH ACUTE
	206,		// 215 LATIN CAPITAL LETTER I WITH CIRCUMFLEX
	207,		// 216 LATIN CAPITAL LETTER I WITH DIAERESIS
	0,			// 217 BOX DRAWINGS LIGHT UP AND LEFT
	0,			// 218 BOX DRAWINGS LIGHT DOWN AND RIGHT
	0,			// 219 FULL BLOCK
	0,			// 220 LOWER HALF BLOCK
	166,		// 221 BROKEN BAR
	204,		// 222 LATIN CAPITAL LETTER I WITH GRAVE
	0,			// 223 UPPER HALF BLOCK
	211,		// 224 LATIN CAPITAL LETTER O WITH ACUTE
	223,		// 225 LATIN SMALL LETTER SHARP S
	212,		// 226 LATIN CAPITAL LETTER O WITH CIRCUMFLEX
	210,		// 227 LATIN CAPITAL LETTER O WITH GRAVE
	245,		// 228 LATIN SMALL LETTER O WITH TILDE
	213,		// 229 LATIN CAPITAL LETTER O WITH TILDE
	181,		// 230 MICRO SIGN
	254,		// 231 LATIN SMALL LETTER THORN
	222,		// 232 LATIN CAPITAL LETTER THORN
	218,		// 233 LATIN CAPITAL LETTER U WITH ACUTE
	219,		// 234 LATIN CAPITAL LETTER U WITH CIRCUMFLEX
	217,		// 235 LATIN CAPITAL LETTER U WITH GRAVE
	253,		// 236 LATIN SMALL LETTER Y WITH ACUTE
	221,		// 237 LATIN CAPITAL LETTER Y WITH ACUTE
	175,		// 238 MACRON
	180,		// 239 ACUTE ACCENT
	173,		// 240 SOFT HYPHEN
	177,		// 241 PLUS-MINUS SIGN
	0,			// 242 DOUBLE LOW LINE
	190,		// 243 VULGAR FRACTION THREE QUARTERS
	182,		// 244 PILCROW SIGN
	167,		// 245 SECTION SIGN
	247,		// 246 DIVISION SIGN
	184,		// 247 CEDILLA
	176,		// 248 DEGREE SIGN
	168,		// 249 DIAERESIS
	183,		// 250 MIDDLE DOT
	185,		// 251 SUPERSCRIPT ONE
	179,		// 252 SUPERSCRIPT THREE
	178,		// 253 SUPERSCRIPT TWO
	0,			// 254 BLACK SQUARE
	160			// 255 NO-BREAK SPACE
};

} // End of namespace Saga
