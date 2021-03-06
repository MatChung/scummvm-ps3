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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/backends/platform/psp/trace.cpp $
 * $Id: trace.cpp 50113 2010-06-21 13:58:51Z Bluddy $
 *
 */


#include <pspkernel.h>
#include <pspdebug.h>
#include <stdarg.h>
#include <stdio.h>

int psp_debug_indent = 0;
bool firstWriteToFile = true;

void PspDebugTrace(bool alsoToScreen, const char *format, ...) {
	va_list	opt;
	char		buffer[2048];
	int			bufsz;
	FILE *fd = 0;

	va_start(opt, format);
	bufsz = vsnprintf(buffer, (size_t) sizeof(buffer), format, opt);
	va_end(opt);

	if (firstWriteToFile) {
		fd = fopen("SCUMMTRACE.TXT", "wb");		// erase the file the first time we write
		firstWriteToFile = false;
	} else {
		fd = fopen("SCUMMTRACE.TXT", "ab");
	}

	if (fd == 0)
		return;

	fwrite(buffer, 1, bufsz, fd);
	fclose(fd);

	if (alsoToScreen)
		fprintf(stderr, buffer);
}
