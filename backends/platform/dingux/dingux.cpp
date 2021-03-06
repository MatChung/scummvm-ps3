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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/backends/platform/dingux/dingux.cpp $
 * $Id: dingux.cpp 52309 2010-08-23 19:49:51Z djwillis $
 *
 */

#include "backends/platform/dingux/dingux.h"

#if defined(DINGUX)

bool OSystem_SDL_Dingux::hasFeature(Feature f) {
	return
	    (f == kFeatureAspectRatioCorrection) ||
	    (f == kFeatureCursorHasPalette);
}

void OSystem_SDL_Dingux::setFeatureState(Feature f, bool enable) {
	switch (f) {
	case kFeatureAspectRatioCorrection:
		setAspectRatioCorrection(enable);
		break;
	default:
		break;
	}
}

bool OSystem_SDL_Dingux::getFeatureState(Feature f) {
	assert(_transactionMode == kTransactionNone);

	switch (f) {
	case kFeatureAspectRatioCorrection:
		return _videoMode.aspectRatioCorrection;
	default:
		return false;
	}
}

#endif

