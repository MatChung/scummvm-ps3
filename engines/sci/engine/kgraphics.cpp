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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/sci/engine/kgraphics.cpp $
 * $Id: kgraphics.cpp 53498 2010-10-15 14:33:57Z thebluegr $
 *
 */

#include "common/system.h"

#include "engines/util.h"
#include "graphics/cursorman.h"
#include "graphics/surface.h"

#include "gui/message.h"

#include "sci/sci.h"
#include "sci/debug.h"	// for g_debug_sleeptime_factor
#include "sci/resource.h"
#include "sci/engine/features.h"
#include "sci/engine/state.h"
#include "sci/engine/selector.h"
#include "sci/engine/kernel.h"
#include "sci/graphics/animate.h"
#include "sci/graphics/cache.h"
#include "sci/graphics/compare.h"
#include "sci/graphics/controls.h"
#include "sci/graphics/cursor.h"
#include "sci/graphics/palette.h"
#include "sci/graphics/paint16.h"
#include "sci/graphics/picture.h"
#include "sci/graphics/ports.h"
#include "sci/graphics/robot.h"
#include "sci/graphics/screen.h"
#include "sci/graphics/text16.h"
#include "sci/graphics/view.h"
#ifdef ENABLE_SCI32
#include "sci/graphics/frameout.h"
#endif

namespace Sci {

void showScummVMDialog(const Common::String &message) {
	GUI::MessageDialog dialog(message, "OK");
	dialog.runModal();
}

void kDirLoopWorker(reg_t object, uint16 angle, EngineState *s, int argc, reg_t *argv) {
	GuiResourceId viewId = readSelectorValue(s->_segMan, object, SELECTOR(view));
	uint16 signal = readSelectorValue(s->_segMan, object, SELECTOR(signal));

	if (signal & kSignalDoesntTurn)
		return;

	int16 useLoop = -1;
	if (getSciVersion() > SCI_VERSION_0_EARLY) {
		if ((angle > 315) || (angle < 45)) {
			useLoop = 3;
		} else if ((angle > 135) && (angle < 225)) {
			useLoop = 2;
		}
	} else {
		// SCI0EARLY
		if ((angle > 330) || (angle < 30)) {
			useLoop = 3;
		} else if ((angle > 150) && (angle < 210)) {
			useLoop = 2;
		}
	}
	if (useLoop == -1) {
		if (angle >= 180) {
			useLoop = 1;
		} else {
			useLoop = 0;
		}
	} else {
		int16 loopCount = g_sci->_gfxCache->kernelViewGetLoopCount(viewId);
		if (loopCount < 4)
			return;
	}

	writeSelectorValue(s->_segMan, object, SELECTOR(loop), useLoop);
}

static reg_t kSetCursorSci0(EngineState *s, int argc, reg_t *argv) {
	Common::Point pos;
	GuiResourceId cursorId = argv[0].toSint16();

	// Set pointer position, if requested
	if (argc >= 4) {
		pos.y = argv[3].toSint16();
		pos.x = argv[2].toSint16();
		g_sci->_gfxCursor->kernelSetPos(pos);
	}

	if ((argc >= 2) && (argv[1].toSint16() == 0)) {
		cursorId = -1;
	}

	g_sci->_gfxCursor->kernelSetShape(cursorId);
	return s->r_acc;
}

static reg_t kSetCursorSci11(EngineState *s, int argc, reg_t *argv) {
	Common::Point pos;
	Common::Point *hotspot = NULL;

	switch (argc) {
	case 1:
		switch (argv[0].toSint16()) {
		case 0:
			g_sci->_gfxCursor->kernelHide();
			break;
		case -1:
			g_sci->_gfxCursor->kernelClearZoomZone();
			break;
		case -2:
			g_sci->_gfxCursor->kernelResetMoveZone();
			break;
		default:
			g_sci->_gfxCursor->kernelShow();
			break;
		}
		break;
	case 2:
		pos.y = argv[1].toSint16();
		pos.x = argv[0].toSint16();
		
		g_sci->_gfxCursor->kernelSetPos(pos);
		break;
	case 4: {
		int16 top, left, bottom, right;

		if (getSciVersion() >= SCI_VERSION_2) {
			top = argv[1].toSint16();
			left = argv[0].toSint16();
			bottom = argv[3].toSint16();
			right = argv[2].toSint16();
		} else {
			top = argv[0].toSint16();
			left = argv[1].toSint16();
			bottom = argv[2].toSint16();
			right = argv[3].toSint16();
		}
		// bottom/right needs to be included into our movezone, because we compare it like any regular Common::Rect
		bottom++;
		right++;

		if ((right >= left) && (bottom >= top)) {
			Common::Rect rect = Common::Rect(left, top, right, bottom);
			g_sci->_gfxCursor->kernelSetMoveZone(rect);
		} else {
			warning("kSetCursor: Ignoring invalid mouse zone (%i, %i)-(%i, %i)", left, top, right, bottom);
		}
		break;
	}
	case 9: // case for kq5cd, we are getting calling with 4 additional 900d parameters
	case 5:
		hotspot = new Common::Point(argv[3].toSint16(), argv[4].toSint16());
		// Fallthrough
	case 3:
		if (g_sci->getPlatform() == Common::kPlatformMacintosh)
			g_sci->_gfxCursor->kernelSetMacCursor(argv[0].toUint16(), argv[1].toUint16(), argv[2].toUint16(), hotspot);
		else
			g_sci->_gfxCursor->kernelSetView(argv[0].toUint16(), argv[1].toUint16(), argv[2].toUint16(), hotspot);
		break;
	case 10:
		// Freddy pharkas, when using the whiskey glass to read the prescription (bug #3034973)
		g_sci->_gfxCursor->kernelSetZoomZone(argv[0].toUint16(), 
			Common::Rect(argv[1].toUint16(), argv[2].toUint16(), argv[3].toUint16(), argv[4].toUint16()),
			argv[5].toUint16(), argv[6].toUint16(), argv[7].toUint16(),
			argv[8].toUint16(), argv[9].toUint16());
		break;
	default :
		error("kSetCursor: Unhandled case: %d arguments given", argc);
		break;
	}
	return s->r_acc;
}

reg_t kSetCursor(EngineState *s, int argc, reg_t *argv) {
	switch (g_sci->_features->detectSetCursorType()) {
	case SCI_VERSION_0_EARLY:
		return kSetCursorSci0(s, argc, argv);
	case SCI_VERSION_1_1:
		return kSetCursorSci11(s, argc, argv);
	default:
		error("Unknown SetCursor type");
		return NULL_REG;
	}
}

reg_t kMoveCursor(EngineState *s, int argc, reg_t *argv) {
	Common::Point pos;
	if (argc == 2) {
		pos.y = argv[1].toSint16();
		pos.x = argv[0].toSint16();
		g_sci->_gfxCursor->kernelSetPos(pos);
	}
	return s->r_acc;
}

reg_t kPicNotValid(EngineState *s, int argc, reg_t *argv) {
	int16 newPicNotValid = (argc > 0) ? argv[0].toUint16() : -1;

	return make_reg(0, g_sci->_gfxScreen->kernelPicNotValid(newPicNotValid));
}

static Common::Rect getGraphRect(reg_t *argv) {
	int16 x = argv[1].toSint16();
	int16 y = argv[0].toSint16();
	int16 x1 = argv[3].toSint16();
	int16 y1 = argv[2].toSint16();
	if (x > x1) SWAP(x, x1);
	if (y > y1) SWAP(y, y1);
	return Common::Rect(x, y, x1, y1);
}

static Common::Point getGraphPoint(reg_t *argv) {
	int16 x = argv[1].toSint16();
	int16 y = argv[0].toSint16();
	return Common::Point(x, y);
}

reg_t kGraph(EngineState *s, int argc, reg_t *argv) {
	if (!s)
		return make_reg(0, getSciVersion());
	error("not supposed to call this");
}

reg_t kGraphGetColorCount(EngineState *s, int argc, reg_t *argv) {
	if (g_sci->getResMan()->isAmiga32color())
		return make_reg(0, 32);
	return make_reg(0, !g_sci->getResMan()->isVGA() ? 16 : 256);
}

reg_t kGraphDrawLine(EngineState *s, int argc, reg_t *argv) {
	int16 color = argv[4].toSint16();
	int16 priority = (argc > 5) ? argv[5].toSint16() : -1;
	int16 control = (argc > 6) ? argv[6].toSint16() : -1;

	// TODO: Find out why we get > 15 for color in EGA
	// FIXME: EGA? Which EGA? SCI0 or SCI1? Check the
	// workarounds inside kGraphFillBoxAny and kNewWindow
	if (!g_sci->getResMan()->isVGA() && !g_sci->getResMan()->isAmiga32color())
		color &= 0x0F;

	g_sci->_gfxPaint16->kernelGraphDrawLine(getGraphPoint(argv), getGraphPoint(argv + 2), color, priority, control);
	return s->r_acc;
}

reg_t kGraphSaveBox(EngineState *s, int argc, reg_t *argv) {
	Common::Rect rect = getGraphRect(argv);
	uint16 screenMask = argv[4].toUint16() & GFX_SCREEN_MASK_ALL;
	return g_sci->_gfxPaint16->kernelGraphSaveBox(rect, screenMask);
}

reg_t kGraphRestoreBox(EngineState *s, int argc, reg_t *argv) {
	// This may be called with a memoryhandle from SAVE_BOX or SAVE_UPSCALEDHIRES_BOX
	g_sci->_gfxPaint16->kernelGraphRestoreBox(argv[0]);
	return s->r_acc;
}

reg_t kGraphFillBoxBackground(EngineState *s, int argc, reg_t *argv) {
	Common::Rect rect = getGraphRect(argv);
	g_sci->_gfxPaint16->kernelGraphFillBoxBackground(rect);
	return s->r_acc;
}

reg_t kGraphFillBoxForeground(EngineState *s, int argc, reg_t *argv) {
	Common::Rect rect = getGraphRect(argv);
	g_sci->_gfxPaint16->kernelGraphFillBoxForeground(rect);
	return s->r_acc;
}

reg_t kGraphFillBoxAny(EngineState *s, int argc, reg_t *argv) {
	Common::Rect rect = getGraphRect(argv);
	int16 colorMask = argv[4].toUint16();
	int16 color = argv[5].toSint16();
	int16 priority = argv[6].toSint16(); // yes, we may read from stack sometimes here
	int16 control = argv[7].toSint16(); // sierra did the same

	// WORKAROUND: PQ3 EGA is setting invalid colors (above 0 - 15).
	// Colors above 15 are all white in SCI1 EGA games, which is why this was never
	// observed. We clip them all to (0, 15) instead, as colors above 15 are used
	// for the undithering algorithm in EGA games - bug #3048908.
	if (g_sci->getResMan()->getViewType() == kViewEga && getSciVersion() >= SCI_VERSION_1_EARLY)
		color &= 0x0F;

	g_sci->_gfxPaint16->kernelGraphFillBox(rect, colorMask, color, priority, control);
	return s->r_acc;
}

reg_t kGraphUpdateBox(EngineState *s, int argc, reg_t *argv) {
	Common::Rect rect = getGraphRect(argv);
	// argv[4] is the map (1 for visual, etc.)
	// argc == 6 on upscaled hires
	bool hiresMode = (argc > 5) ? true : false;
	g_sci->_gfxPaint16->kernelGraphUpdateBox(rect, hiresMode);
	return s->r_acc;
}

reg_t kGraphRedrawBox(EngineState *s, int argc, reg_t *argv) {
	Common::Rect rect = getGraphRect(argv);
	g_sci->_gfxPaint16->kernelGraphRedrawBox(rect);
	return s->r_acc;
}

// Seems to be only implemented for SCI0/SCI01 games
reg_t kGraphAdjustPriority(EngineState *s, int argc, reg_t *argv) {
	g_sci->_gfxPorts->kernelGraphAdjustPriority(argv[0].toUint16(), argv[1].toUint16());
	return s->r_acc;
}

reg_t kGraphSaveUpscaledHiresBox(EngineState *s, int argc, reg_t *argv) {
	Common::Rect rect = getGraphRect(argv);
	return g_sci->_gfxPaint16->kernelGraphSaveUpscaledHiresBox(rect);
}

reg_t kTextSize(EngineState *s, int argc, reg_t *argv) {
	int16 textWidth, textHeight;
	Common::String text = s->_segMan->getString(argv[1]);
	reg_t *dest = s->_segMan->derefRegPtr(argv[0], 4);
	int maxwidth = (argc > 3) ? argv[3].toUint16() : 0;
	int font_nr = argv[2].toUint16();

	if (!dest) {
		debugC(2, kDebugLevelStrings, "GetTextSize: Empty destination");
		return s->r_acc;
	}

	Common::String sep_str;
	const char *sep = NULL;
	if ((argc > 4) && (argv[4].segment)) {
		sep_str = s->_segMan->getString(argv[4]);
		sep = sep_str.c_str();
	}

	dest[0] = dest[1] = NULL_REG;

	if (text.empty()) { // Empty text
		dest[2] = dest[3] = make_reg(0, 0);
		debugC(2, kDebugLevelStrings, "GetTextSize: Empty string");
		return s->r_acc;
	}

	textWidth = dest[3].toUint16(); textHeight = dest[2].toUint16();

#ifdef ENABLE_SCI32
	if (!g_sci->_gfxText16) {
		// TODO: Implement this
		textWidth = 0; textHeight = 0;
	} else
#endif
		g_sci->_gfxText16->kernelTextSize(g_sci->strSplit(text.c_str(), sep).c_str(), font_nr, maxwidth, &textWidth, &textHeight);
	
	debugC(2, kDebugLevelStrings, "GetTextSize '%s' -> %dx%d", text.c_str(), textWidth, textHeight);
	dest[2] = make_reg(0, textHeight);
	dest[3] = make_reg(0, textWidth);
	return s->r_acc;
}

reg_t kWait(EngineState *s, int argc, reg_t *argv) {
	int sleep_time = argv[0].toUint16();

	s->wait(sleep_time);

	return s->r_acc;
}

reg_t kCoordPri(EngineState *s, int argc, reg_t *argv) {
	int16 y = argv[0].toSint16();

	if ((argc < 2) || (y != 1)) {
		return make_reg(0, g_sci->_gfxPorts->kernelCoordinateToPriority(y));
	} else {
		int16 priority = argv[1].toSint16();
		return make_reg(0, g_sci->_gfxPorts->kernelPriorityToCoordinate(priority));
	}
}

reg_t kPriCoord(EngineState *s, int argc, reg_t *argv) {
	int16 priority = argv[0].toSint16();

	return make_reg(0, g_sci->_gfxPorts->kernelPriorityToCoordinate(priority));
}

reg_t kDirLoop(EngineState *s, int argc, reg_t *argv) {
	kDirLoopWorker(argv[0], argv[1].toUint16(), s, argc, argv);

	return s->r_acc;
}

reg_t kCanBeHere(EngineState *s, int argc, reg_t *argv) {
	reg_t curObject = argv[0];
	reg_t listReference = (argc > 1) ? argv[1] : NULL_REG;

	reg_t canBeHere = g_sci->_gfxCompare->kernelCanBeHere(curObject, listReference);
	return make_reg(0, canBeHere.isNull() ? 1 : 0);
}

reg_t kCantBeHere(EngineState *s, int argc, reg_t *argv) {
	reg_t curObject = argv[0];
	reg_t listReference = (argc > 1) ? argv[1] : NULL_REG;
	
	reg_t canBeHere = g_sci->_gfxCompare->kernelCanBeHere(curObject, listReference);
	return canBeHere;
}

reg_t kIsItSkip(EngineState *s, int argc, reg_t *argv) {
	GuiResourceId viewId = argv[0].toSint16();
	int16 loopNo = argv[1].toSint16();
	int16 celNo = argv[2].toSint16();
	Common::Point position(argv[4].toUint16(), argv[3].toUint16());

	bool result = g_sci->_gfxCompare->kernelIsItSkip(viewId, loopNo, celNo, position);
	return make_reg(0, result);
}

reg_t kCelHigh(EngineState *s, int argc, reg_t *argv) {
	GuiResourceId viewId = argv[0].toSint16();
	if (viewId == -1)	// Happens in SCI32
		return NULL_REG;
	int16 loopNo = argv[1].toSint16();
	int16 celNo = (argc >= 3) ? argv[2].toSint16() : 0;
	int16 celHeight;

	celHeight = g_sci->_gfxCache->kernelViewGetCelHeight(viewId, loopNo, celNo);

	return make_reg(0, celHeight);
}

reg_t kCelWide(EngineState *s, int argc, reg_t *argv) {
	GuiResourceId viewId = argv[0].toSint16();
	if (viewId == -1)	// Happens in SCI32
		return NULL_REG;
	int16 loopNo = argv[1].toSint16();
	int16 celNo = (argc >= 3) ? argv[2].toSint16() : 0;
	int16 celWidth;

	celWidth = g_sci->_gfxCache->kernelViewGetCelWidth(viewId, loopNo, celNo);

	return make_reg(0, celWidth);
}

reg_t kNumLoops(EngineState *s, int argc, reg_t *argv) {
	reg_t object = argv[0];
	GuiResourceId viewId = readSelectorValue(s->_segMan, object, SELECTOR(view));
	int16 loopCount;

	loopCount = g_sci->_gfxCache->kernelViewGetLoopCount(viewId);

	debugC(2, kDebugLevelGraphics, "NumLoops(view.%d) = %d", viewId, loopCount);

	return make_reg(0, loopCount);
}

reg_t kNumCels(EngineState *s, int argc, reg_t *argv) {
	reg_t object = argv[0];
	GuiResourceId viewId = readSelectorValue(s->_segMan, object, SELECTOR(view));
	int16 loopNo = readSelectorValue(s->_segMan, object, SELECTOR(loop));
	int16 celCount;

	celCount = g_sci->_gfxCache->kernelViewGetCelCount(viewId, loopNo);

	debugC(2, kDebugLevelGraphics, "NumCels(view.%d, %d) = %d", viewId, loopNo, celCount);

	return make_reg(0, celCount);
}

reg_t kOnControl(EngineState *s, int argc, reg_t *argv) {
	Common::Rect rect;
	byte screenMask;
	int argBase = 0;

	if ((argc == 2) || (argc == 4)) {
		screenMask = GFX_SCREEN_MASK_CONTROL;
	} else {
		screenMask = argv[0].toUint16();
		argBase = 1;
	}
	rect.left = argv[argBase].toSint16();
	rect.top = argv[argBase + 1].toSint16();
	if (argc > 3) {
		rect.right = argv[argBase + 2].toSint16();
		rect.bottom = argv[argBase + 3].toSint16();
	} else {
		rect.right = rect.left + 1;
		rect.bottom = rect.top + 1;
	}
	uint16 result = g_sci->_gfxCompare->kernelOnControl(screenMask, rect);
	return make_reg(0, result);
}

#define K_DRAWPIC_FLAGS_MIRRORED			(1 << 14)
#define K_DRAWPIC_FLAGS_ANIMATIONBLACKOUT	(1 << 15)

reg_t kDrawPic(EngineState *s, int argc, reg_t *argv) {
	GuiResourceId pictureId = argv[0].toUint16();
	uint16 flags = 0;
	int16 animationNr = -1;
	bool animationBlackoutFlag = false;
	bool mirroredFlag = false;
	bool addToFlag = false;
	int16 EGApaletteNo = 0; // default needs to be 0

	if (argc >= 2) {
		flags = argv[1].toUint16();
		if (flags & K_DRAWPIC_FLAGS_ANIMATIONBLACKOUT)
			animationBlackoutFlag = true;
		animationNr = flags & 0xFF;
		if (flags & K_DRAWPIC_FLAGS_MIRRORED)
			mirroredFlag = true;
	}
	if (argc >= 3) {
		if (!argv[2].isNull())
			addToFlag = true;
		if (!g_sci->_features->usesOldGfxFunctions())
			addToFlag = !addToFlag;
	}
	if (argc >= 4)
		EGApaletteNo = argv[3].toUint16();

	g_sci->_gfxPaint16->kernelDrawPicture(pictureId, animationNr, animationBlackoutFlag, mirroredFlag, addToFlag, EGApaletteNo);

	return s->r_acc;
}

reg_t kBaseSetter(EngineState *s, int argc, reg_t *argv) {
	reg_t object = argv[0];

	g_sci->_gfxCompare->kernelBaseSetter(object);
	return s->r_acc;
}

reg_t kSetNowSeen(EngineState *s, int argc, reg_t *argv) {
	g_sci->_gfxCompare->kernelSetNowSeen(argv[0]);

	return s->r_acc;
}

// we are called on EGA/amiga games as well, this doesnt make sense.
//  doing this would actually break the system EGA/amiga palette
reg_t kPalette(EngineState *s, int argc, reg_t *argv) {
	if (!s)
		return make_reg(0, getSciVersion());
	error("not supposed to call this");
}

reg_t kPaletteSetFromResource(EngineState *s, int argc, reg_t *argv) {
	if (g_sci->getResMan()->isVGA()) {
		GuiResourceId resourceId = argv[0].toUint16();
		bool force = false;
		if (argc == 2)
			force = argv[1].toUint16() == 2 ? true : false;
		g_sci->_gfxPalette->kernelSetFromResource(resourceId, force);
	}
	return s->r_acc;
}

reg_t kPaletteSetFlag(EngineState *s, int argc, reg_t *argv) {
	if (g_sci->getResMan()->isVGA()) {
		uint16 fromColor = CLIP<uint16>(argv[0].toUint16(), 1, 255);
		uint16 toColor = CLIP<uint16>(argv[1].toUint16(), 1, 255);
		uint16 flags = argv[2].toUint16();
		g_sci->_gfxPalette->kernelSetFlag(fromColor, toColor, flags);
	}
	return s->r_acc;
}

reg_t kPaletteUnsetFlag(EngineState *s, int argc, reg_t *argv) {
	if (g_sci->getResMan()->isVGA()) {
		uint16 fromColor = CLIP<uint16>(argv[0].toUint16(), 1, 255);
		uint16 toColor = CLIP<uint16>(argv[1].toUint16(), 1, 255);
		uint16 flags = argv[2].toUint16();
		g_sci->_gfxPalette->kernelUnsetFlag(fromColor, toColor, flags);
	}
	return s->r_acc;
}

reg_t kPaletteSetIntensity(EngineState *s, int argc, reg_t *argv) {
	if (g_sci->getResMan()->isVGA()) {
		uint16 fromColor = CLIP<uint16>(argv[0].toUint16(), 1, 255);
		uint16 toColor = CLIP<uint16>(argv[1].toUint16(), 1, 255);
		uint16 intensity = argv[2].toUint16();
		bool setPalette = (argc < 4) ? true : (argv[3].isNull()) ? true : false;

		g_sci->_gfxPalette->kernelSetIntensity(fromColor, toColor, intensity, setPalette);
	}
	return s->r_acc;
}

reg_t kPaletteFindColor(EngineState *s, int argc, reg_t *argv) {
	if (g_sci->getResMan()->isVGA()) {
		uint16 r = argv[0].toUint16();
		uint16 g = argv[1].toUint16();
		uint16 b = argv[2].toUint16();
		return make_reg(0, g_sci->_gfxPalette->kernelFindColor(r, g, b));
	}
	return NULL_REG;
}

reg_t kPaletteAnimate(EngineState *s, int argc, reg_t *argv) {
	if (g_sci->getResMan()->isVGA()) {
		int16 argNr;
		bool paletteChanged = false;
		for (argNr = 0; argNr < argc; argNr += 3) {
			uint16 fromColor = argv[argNr].toUint16();
			uint16 toColor = argv[argNr + 1].toUint16();
			int16 speed = argv[argNr + 2].toSint16();
			if (g_sci->_gfxPalette->kernelAnimate(fromColor, toColor, speed))
				paletteChanged = true;
		}
		if (paletteChanged)
			g_sci->_gfxPalette->kernelAnimateSet();
	}
	return s->r_acc;
}

reg_t kPaletteSave(EngineState *s, int argc, reg_t *argv) {
	if (g_sci->getResMan()->isVGA()) {
		return g_sci->_gfxPalette->kernelSave();
	}
	return NULL_REG;
}

reg_t kPaletteRestore(EngineState *s, int argc, reg_t *argv) {
	if (g_sci->getResMan()->isVGA()) {
		g_sci->_gfxPalette->kernelRestore(argv[0]);
	}
	return argv[0];
}

reg_t kPalVary(EngineState *s, int argc, reg_t *argv) {
	if (!s)
		return make_reg(0, getSciVersion());
	error("not supposed to call this");
}

reg_t kPalVaryInit(EngineState *s, int argc, reg_t *argv) {
	GuiResourceId paletteId = argv[0].toUint16();
	uint16 ticks = argv[1].toUint16();
	uint16 stepStop = argc >= 3 ? argv[2].toUint16() : 64;
	uint16 direction = argc >= 4 ? argv[3].toUint16() : 1;
	if (g_sci->_gfxPalette->kernelPalVaryInit(paletteId, ticks, stepStop, direction))
		return SIGNAL_REG;
	return NULL_REG;
}

reg_t kPalVaryReverse(EngineState *s, int argc, reg_t *argv) {
	int16 ticks = argc >= 1 ? argv[0].toUint16() : -1;
	int16 stepStop = argc >= 2 ? argv[1].toUint16() : 0;
	int16 direction = argc >= 3 ? argv[2].toSint16() : -1;

	return make_reg(0, g_sci->_gfxPalette->kernelPalVaryReverse(ticks, stepStop, direction));
}

reg_t kPalVaryGetCurrentStep(EngineState *s, int argc, reg_t *argv) {
	return make_reg(0, g_sci->_gfxPalette->kernelPalVaryGetCurrentStep());
}

reg_t kPalVaryDeinit(EngineState *s, int argc, reg_t *argv) {
	g_sci->_gfxPalette->kernelPalVaryDeinit();
	return NULL_REG;
}

reg_t kPalVaryChangeTarget(EngineState *s, int argc, reg_t *argv) {
	GuiResourceId paletteId = argv[0].toUint16();
	int16 currentStep = g_sci->_gfxPalette->kernelPalVaryChangeTarget(paletteId);
	return make_reg(0, currentStep);
}

reg_t kPalVaryChangeTicks(EngineState *s, int argc, reg_t *argv) {
	uint16 ticks = argv[0].toUint16();
	g_sci->_gfxPalette->kernelPalVaryChangeTicks(ticks);
	return NULL_REG;
}

reg_t kPalVaryPauseResume(EngineState *s, int argc, reg_t *argv) {
	bool pauseState = !argv[0].isNull();
	g_sci->_gfxPalette->kernelPalVaryPause(pauseState);
	return NULL_REG;
}

reg_t kPalVaryUnknown(EngineState *s, int argc, reg_t *argv) {
	// Unknown (seems to be SCI32 exclusive)
	return NULL_REG;
}

reg_t kAssertPalette(EngineState *s, int argc, reg_t *argv) {
	GuiResourceId paletteId = argv[0].toUint16();

	g_sci->_gfxPalette->kernelAssertPalette(paletteId);
	return s->r_acc;
}

// Used to show hires character portraits in the Windows CD version of KQ6
reg_t kPortrait(EngineState *s, int argc, reg_t *argv) {
	uint16 operation = argv[0].toUint16();

	switch (operation) {
	case 0: { // load
		if (argc == 2) {
			Common::String resourceName = s->_segMan->getString(argv[1]);
			s->r_acc = g_sci->_gfxPaint16->kernelPortraitLoad(resourceName);
		} else {
			error("kPortrait(loadResource) called with unsupported argc %d", argc);
		}
		break;
	}
	case 1: { // show
		if (argc == 10) {
			Common::String resourceName = s->_segMan->getString(argv[1]);
			Common::Point position = Common::Point(argv[2].toUint16(), argv[3].toUint16());
			uint resourceNum = argv[4].toUint16();
			uint noun = argv[5].toUint16() & 0xff;
			uint verb = argv[6].toUint16() & 0xff;
			uint cond = argv[7].toUint16() & 0xff;
			uint seq = argv[8].toUint16() & 0xff;
			// argv[9] is usually 0??!!

			g_sci->_gfxPaint16->kernelPortraitShow(resourceName, position, resourceNum, noun, verb, cond, seq);
			return SIGNAL_REG;
		} else {
			error("kPortrait(show) called with unsupported argc %d", argc);
		}
		break;
	}
	case 2: { // unload
		if (argc == 2) {
			uint16 portraitId = argv[1].toUint16();
			g_sci->_gfxPaint16->kernelPortraitUnload(portraitId);
		} else {
			error("kPortrait(unload) called with unsupported argc %d", argc);
		}
		break;
	}
	default:
		error("kPortrait(%d), not implemented (argc = %d)", operation, argc);
	}

	return s->r_acc;
}

// Original top-left must stay on kControl rects, we adjust accordingly because
// sierra sci actually wont draw rects that are upside down (example: jones,
// when challenging jones - one button is a duplicate and also has lower-right
// which is 0, 0)
Common::Rect kControlCreateRect(int16 x, int16 y, int16 x1, int16 y1) {
	if (x > x1) x1 = x;
	if (y > y1) y1 = y;
	return Common::Rect(x, y, x1, y1);
}

void _k_GenericDrawControl(EngineState *s, reg_t controlObject, bool hilite) {
	int16 type = readSelectorValue(s->_segMan, controlObject, SELECTOR(type));
	int16 style = readSelectorValue(s->_segMan, controlObject, SELECTOR(state));
	int16 x = readSelectorValue(s->_segMan, controlObject, SELECTOR(nsLeft));
	int16 y = readSelectorValue(s->_segMan, controlObject, SELECTOR(nsTop));
	GuiResourceId fontId = readSelectorValue(s->_segMan, controlObject, SELECTOR(font));
	reg_t textReference = readSelector(s->_segMan, controlObject, SELECTOR(text));
	Common::String text;
	Common::Rect rect;
	TextAlignment alignment;
	int16 mode, maxChars, cursorPos, upperPos, listCount, i;
	int16 upperOffset, cursorOffset;
	GuiResourceId viewId;
	int16 loopNo;
	int16 celNo;
	int16 priority;
	reg_t listSeeker;
	Common::String *listStrings = NULL;
	const char **listEntries = NULL;
	bool isAlias = false;

	rect = kControlCreateRect(x, y,
				readSelectorValue(s->_segMan, controlObject, SELECTOR(nsRight)),
				readSelectorValue(s->_segMan, controlObject, SELECTOR(nsBottom)));

	if (!textReference.isNull())
		text = s->_segMan->getString(textReference);

	switch (type) {
	case SCI_CONTROLS_TYPE_BUTTON:
		debugC(2, kDebugLevelGraphics, "drawing button %04x:%04x to %d,%d", PRINT_REG(controlObject), x, y);
		g_sci->_gfxControls->kernelDrawButton(rect, controlObject, g_sci->strSplit(text.c_str(), NULL).c_str(), fontId, style, hilite);
		return;

	case SCI_CONTROLS_TYPE_TEXT:
		alignment = readSelectorValue(s->_segMan, controlObject, SELECTOR(mode));
		debugC(2, kDebugLevelGraphics, "drawing text %04x:%04x ('%s') to %d,%d, mode=%d", PRINT_REG(controlObject), text.c_str(), x, y, alignment);
		g_sci->_gfxControls->kernelDrawText(rect, controlObject, g_sci->strSplit(text.c_str()).c_str(), fontId, alignment, style, hilite);
		s->r_acc = g_sci->_gfxText16->allocAndFillReferenceRectArray();
		return;

	case SCI_CONTROLS_TYPE_TEXTEDIT:
		mode = readSelectorValue(s->_segMan, controlObject, SELECTOR(mode));
		maxChars = readSelectorValue(s->_segMan, controlObject, SELECTOR(max));
		cursorPos = readSelectorValue(s->_segMan, controlObject, SELECTOR(cursor));
		if (cursorPos > (int)text.size()) {
			// if cursor is outside of text, adjust accordingly
			cursorPos = text.size();
			writeSelectorValue(s->_segMan, controlObject, SELECTOR(cursor), cursorPos);
		}
		debugC(2, kDebugLevelGraphics, "drawing edit control %04x:%04x (text %04x:%04x, '%s') to %d,%d", PRINT_REG(controlObject), PRINT_REG(textReference), text.c_str(), x, y);
		g_sci->_gfxControls->kernelDrawTextEdit(rect, controlObject, g_sci->strSplit(text.c_str(), NULL).c_str(), fontId, mode, style, cursorPos, maxChars, hilite);
		return;

	case SCI_CONTROLS_TYPE_ICON:
		viewId = readSelectorValue(s->_segMan, controlObject, SELECTOR(view));
		{
			int l = readSelectorValue(s->_segMan, controlObject, SELECTOR(loop));
			loopNo = (l & 0x80) ? l - 256 : l;
			int c = readSelectorValue(s->_segMan, controlObject, SELECTOR(cel));
			celNo = (c & 0x80) ? c - 256 : c;
			// Check if the control object specifies a priority selector (like in Jones)
			if (lookupSelector(s->_segMan, controlObject, SELECTOR(priority), NULL, NULL) == kSelectorVariable)
				priority = readSelectorValue(s->_segMan, controlObject, SELECTOR(priority));
			else
				priority = -1;
		}
		debugC(2, kDebugLevelGraphics, "drawing icon control %04x:%04x to %d,%d", PRINT_REG(controlObject), x, y - 1);
		g_sci->_gfxControls->kernelDrawIcon(rect, controlObject, viewId, loopNo, celNo, priority, style, hilite);
		return;

	case SCI_CONTROLS_TYPE_LIST:
	case SCI_CONTROLS_TYPE_LIST_ALIAS:
		if (type == SCI_CONTROLS_TYPE_LIST_ALIAS)
			isAlias = true;

		maxChars = readSelectorValue(s->_segMan, controlObject, SELECTOR(x)); // max chars per entry
		cursorOffset = readSelectorValue(s->_segMan, controlObject, SELECTOR(cursor));
		if (SELECTOR(topString) != -1) {
			// Games from early SCI1 onwards use topString
			upperOffset = readSelectorValue(s->_segMan, controlObject, SELECTOR(topString));
		} else {
			// Earlier games use lsTop or brTop
			if (lookupSelector(s->_segMan, controlObject, SELECTOR(brTop), NULL, NULL) == kSelectorVariable)
				upperOffset = readSelectorValue(s->_segMan, controlObject, SELECTOR(brTop));
			else
				upperOffset = readSelectorValue(s->_segMan, controlObject, SELECTOR(lsTop));
		}

		// Count string entries in NULL terminated string list
		listCount = 0; listSeeker = textReference;
		while (s->_segMan->strlen(listSeeker) > 0) {
			listCount++;
			listSeeker.offset += maxChars;
		}

		// TODO: This is rather convoluted... It would be a lot cleaner
		// if sciw_new_list_control would take a list of Common::String
		cursorPos = 0; upperPos = 0;
		if (listCount) {
			// We create a pointer-list to the different strings, we also find out whats upper and cursor position
			listSeeker = textReference;
			listEntries = (const char**)malloc(sizeof(char *) * listCount);
			listStrings = new Common::String[listCount];
			for (i = 0; i < listCount; i++) {
				listStrings[i] = s->_segMan->getString(listSeeker);
				listEntries[i] = listStrings[i].c_str();
				if (listSeeker.offset == upperOffset)
					upperPos = i;
				if (listSeeker.offset == cursorOffset)
					cursorPos = i;
				listSeeker.offset += maxChars;
			}
		}

		debugC(2, kDebugLevelGraphics, "drawing list control %04x:%04x to %d,%d, diff %d", PRINT_REG(controlObject), x, y, SCI_MAX_SAVENAME_LENGTH);
		g_sci->_gfxControls->kernelDrawList(rect, controlObject, maxChars, listCount, listEntries, fontId, style, upperPos, cursorPos, isAlias, hilite);
		free(listEntries);
		delete[] listStrings;
		return;

	case SCI_CONTROLS_TYPE_DUMMY:
		// Actually this here does nothing at all, its required by at least QfG1/EGA that we accept this type
		return;

	default:
		error("unsupported control type %d", type);
	}
}

reg_t kDrawControl(EngineState *s, int argc, reg_t *argv) {
	reg_t controlObject = argv[0];
	Common::String objName = s->_segMan->getObjectName(controlObject);

	// Most of the time, we won't return anything to the caller
	//  but |r| textcodes will trigger creation of rects in memory and will then set s->r_acc
	s->r_acc = NULL_REG;

	// Disable the "Change Directory" button, as we don't allow the game engine to
	// change the directory where saved games are placed
	// "changeDirItem" is used in the import windows of QFG2&3
	if ((objName == "changeDirI") || (objName == "changeDirItem")) {
		int state = readSelectorValue(s->_segMan, controlObject, SELECTOR(state));
		writeSelectorValue(s->_segMan, controlObject, SELECTOR(state), (state | SCI_CONTROLS_STYLE_DISABLED) & ~SCI_CONTROLS_STYLE_ENABLED);
	}
	if (objName == "DEdit") {
		reg_t textReference = readSelector(s->_segMan, controlObject, SELECTOR(text));
		if (!textReference.isNull()) {
			Common::String text = s->_segMan->getString(textReference);
			if ((text == "a:hq1_hero.sav") || (text == "a:glory1.sav") || (text == "a:glory2.sav") || (text == "a:glory3.sav")) {
				// Remove "a:" from hero quest / quest for glory export default filenames
				text.deleteChar(0);
				text.deleteChar(0);
				s->_segMan->strcpy(textReference, text.c_str());
			}
		}
	}
	if (objName == "savedHeros") {
		// Import of QfG character files dialog is shown
		// display additional popup information before letting user use it
		reg_t changeDirButton = s->_segMan->findObjectByName("changeDirItem");
		if (!changeDirButton.isNull()) {
			// check if checkDirButton is still enabled, in that case we are called the first time during that room
			if (!(readSelectorValue(s->_segMan, changeDirButton, SELECTOR(state)) & SCI_CONTROLS_STYLE_DISABLED)) {
				showScummVMDialog("Characters saved inside ScummVM are shown "
						"automatically. Character files saved in the original "
						"interpreter need to be put inside ScummVM's saved games "
						"directory and a prefix needs to be added depending on which "
						"game it was saved in: 'qfg1-' for Quest for Glory 1, 'qfg2-' "
						"for Quest for Glory 2. Example: 'qfg2-thief.sav'.");
			}
		}
		s->_chosenQfGImportItem = readSelectorValue(s->_segMan, controlObject, SELECTOR(mark));
	}

	_k_GenericDrawControl(s, controlObject, false);
	return s->r_acc;
}

reg_t kHiliteControl(EngineState *s, int argc, reg_t *argv) {
	reg_t controlObject = argv[0];

	_k_GenericDrawControl(s, controlObject, true);
	return s->r_acc;
}

reg_t kEditControl(EngineState *s, int argc, reg_t *argv) {
	reg_t controlObject = argv[0];
	reg_t eventObject = argv[1];

	if (!controlObject.isNull()) {
		int16 controlType = readSelectorValue(s->_segMan, controlObject, SELECTOR(type));

		switch (controlType) {
		case SCI_CONTROLS_TYPE_TEXTEDIT:
			// Only process textedit controls in here
			g_sci->_gfxControls->kernelTexteditChange(controlObject, eventObject);
		}
	}
	return s->r_acc;
}

reg_t kAddToPic(EngineState *s, int argc, reg_t *argv) {
	GuiResourceId viewId;
	int16 loopNo;
	int16 celNo;
	int16 leftPos, topPos, priority, control;

	switch (argc) {
	// Is this ever really gets called with 0 parameters, we need to set _picNotValid!!
	//case 0:
	//	break;
	case 1:
		if (argv[0].isNull())
			return s->r_acc;
		g_sci->_gfxAnimate->kernelAddToPicList(argv[0], argc, argv);
		break;
	case 7:
		viewId = argv[0].toUint16();
		loopNo = argv[1].toSint16();
		celNo = argv[2].toSint16();
		leftPos = argv[3].toSint16();
		topPos = argv[4].toSint16();
		priority = argv[5].toSint16();
		control = argv[6].toSint16();
		g_sci->_gfxAnimate->kernelAddToPicView(viewId, loopNo, celNo, leftPos, topPos, priority, control);
		break;
	default:
		error("kAddToPic with unsupported parameter count %d", argc);
	}
	return s->r_acc;
}

reg_t kGetPort(EngineState *s, int argc, reg_t *argv) {
	return g_sci->_gfxPorts->kernelGetActive();
}

reg_t kSetPort(EngineState *s, int argc, reg_t *argv) {
	uint16 portId;
	Common::Rect picRect;
	int16 picTop, picLeft;
	bool initPriorityBandsFlag = false;

	switch (argc) {
	case 1:
		portId = argv[0].toSint16();
		g_sci->_gfxPorts->kernelSetActive(portId);
		break;

	case 7:
		initPriorityBandsFlag = true;
	case 6:
		picRect.top = argv[0].toSint16();
		picRect.left = argv[1].toSint16();
		picRect.bottom = argv[2].toSint16();
		picRect.right = argv[3].toSint16();
		picTop = argv[4].toSint16();
		picLeft = argv[5].toSint16();
		g_sci->_gfxPorts->kernelSetPicWindow(picRect, picTop, picLeft, initPriorityBandsFlag);
		break;

	default:
		error("SetPort was called with %d parameters", argc);
		break;
	}
	return NULL_REG;
}

reg_t kDrawCel(EngineState *s, int argc, reg_t *argv) {
	GuiResourceId viewId = argv[0].toSint16();
	int16 loopNo = argv[1].toSint16();
	int16 celNo = argv[2].toSint16();
	uint16 x = argv[3].toUint16();
	uint16 y = argv[4].toUint16();
	int16 priority = (argc > 5) ? argv[5].toSint16() : -1;
	uint16 paletteNo = (argc > 6) ? argv[6].toUint16() : 0;
	bool hiresMode = false;
	reg_t upscaledHiresHandle = NULL_REG;
	uint16 scaleX = 128;
	uint16 scaleY = 128;

	if (argc > 7) {
		// this is either kq6 hires or scaling
		if (paletteNo > 0) {
			// it's scaling
			scaleX = argv[6].toUint16();
			scaleY = argv[7].toUint16();
			paletteNo = 0;
		} else {
			// KQ6 hires
			hiresMode = true;
			upscaledHiresHandle = argv[7];
		}
	}

	g_sci->_gfxPaint16->kernelDrawCel(viewId, loopNo, celNo, x, y, priority, paletteNo, scaleX, scaleY, hiresMode, upscaledHiresHandle);

	return s->r_acc;
}

reg_t kDisposeWindow(EngineState *s, int argc, reg_t *argv) {
	int windowId = argv[0].toSint16();
	bool reanimate = false;
	if ((argc != 2) || (argv[1].isNull()))
		reanimate = true;

	g_sci->_gfxPorts->kernelDisposeWindow(windowId, reanimate);
	return s->r_acc;
}

reg_t kNewWindow(EngineState *s, int argc, reg_t *argv) {
	Common::Rect rect1 (argv[1].toSint16(), argv[0].toSint16(), argv[3].toSint16(), argv[2].toSint16());
	Common::Rect rect2;
	int argextra = argc >= 13 ? 4 : 0; // Triggers in PQ3 and SCI1.1 games, argc 13 for DOS argc 15 for mac
	int	style = argv[5 + argextra].toSint16();
	int	priority = (argc > 6 + argextra) ? argv[6 + argextra].toSint16() : -1;
	int colorPen = (argc > 7 + argextra) ? argv[7 + argextra].toSint16() : 0;
	int colorBack = (argc > 8 + argextra) ? argv[8 + argextra].toSint16() : 255;

	// WORKAROUND: PQ3 EGA is setting invalid colors (above 0 - 15).
	// Colors above 15 are all white in SCI1 EGA games, which is why this was never
	// observed. We clip them all to (0, 15) instead, as colors above 15 are used
	// for the undithering algorithm in EGA games - bug #3048908.
	if (g_sci->getResMan()->getViewType() == kViewEga && getSciVersion() >= SCI_VERSION_1_EARLY) {
		colorPen &= 0x0F;
		colorBack &= 0x0F;
	}

	//	const char *title = argv[4 + argextra].segment ? kernel_dereference_char_pointer(s, argv[4 + argextra], 0) : NULL;
	if (argc>=13) {
		rect2 = Common::Rect (argv[5].toSint16(), argv[4].toSint16(), argv[7].toSint16(), argv[6].toSint16());
	}

	Common::String title;
	if (argv[4 + argextra].segment) {
		title = s->_segMan->getString(argv[4 + argextra]);
		title = g_sci->strSplit(title.c_str(), NULL);
	}

	return g_sci->_gfxPorts->kernelNewWindow(rect1, rect2, style, priority, colorPen, colorBack, title.c_str());
}

reg_t kAnimate(EngineState *s, int argc, reg_t *argv) {
	reg_t castListReference = (argc > 0) ? argv[0] : NULL_REG;
	bool cycle = (argc > 1) ? ((argv[1].toUint16()) ? true : false) : false;

	g_sci->_gfxAnimate->kernelAnimate(castListReference, cycle, argc, argv);

	return s->r_acc;
}

reg_t kShakeScreen(EngineState *s, int argc, reg_t *argv) {
	int16 shakeCount = (argc > 0) ? argv[0].toUint16() : 1;
	int16 directions = (argc > 1) ? argv[1].toUint16() : 1;

	g_sci->_gfxScreen->kernelShakeScreen(shakeCount, directions);
	return s->r_acc;
}

reg_t kDisplay(EngineState *s, int argc, reg_t *argv) {
	reg_t textp = argv[0];
	int index = (argc > 1) ? argv[1].toUint16() : 0;

	Common::String text;

	if (textp.segment) {
		argc--; argv++;
		text = s->_segMan->getString(textp);
	} else {
		argc--; argc--; argv++; argv++;
		text = g_sci->getKernel()->lookupText(textp, index);
	}

	return g_sci->_gfxPaint16->kernelDisplay(g_sci->strSplit(text.c_str()).c_str(), argc, argv);
}

reg_t kSetVideoMode(EngineState *s, int argc, reg_t *argv) {
	// This call is used for KQ6's intro. It has one parameter, which is 1 when
	// the intro begins, and 0 when it ends. It is suspected that this is
	// actually a flag to enable video planar memory access, as the video
	// decoder in KQ6 is specifically written for the planar memory model.
	// Planar memory mode access was used for VGA "Mode X" (320x240 resolution,
	// although the intro in KQ6 is 320x200).
	// Refer to http://en.wikipedia.org/wiki/Mode_X

	//warning("STUB: SetVideoMode %d", argv[0].toUint16());
	return s->r_acc;
}

// New calls for SCI11. Using those is only needed when using text-codes so that
// one is able to change font and/or color multiple times during kDisplay and
// kDrawControl
reg_t kTextFonts(EngineState *s, int argc, reg_t *argv) {
	g_sci->_gfxText16->kernelTextFonts(argc, argv);
	return s->r_acc;
}

reg_t kTextColors(EngineState *s, int argc, reg_t *argv) {
	g_sci->_gfxText16->kernelTextColors(argc, argv);
	return s->r_acc;
}

/**
 * Debug command, used by the SCI builtin debugger
 */
reg_t kShow(EngineState *s, int argc, reg_t *argv) {
	uint16 map = argv[0].toUint16();

	switch (map) {
	case 1:	// Visual, substituted by display for us
		g_sci->_gfxScreen->debugShowMap(3);
		break;
	case 2:	// Priority
		g_sci->_gfxScreen->debugShowMap(1);
		break;
	case 3:	// Control
	case 4:	// Control
		g_sci->_gfxScreen->debugShowMap(2);
		break;
	default:
		warning("Map %d is not available", map);
	}

	return s->r_acc;
}

reg_t kRemapColors(EngineState *s, int argc, reg_t *argv) {
	// TODO: This is all a stub/skeleton, thus we're invoking kStub() for now
	kStub(s, argc, argv);

	uint16 operation = argv[0].toUint16();

	switch (operation) {
	case 0:	{ // Initialize remapping to base. 0 turns remapping off.
		//int16 unk1 = (argc >= 2) ? argv[1].toSint16() : 0;
		}
		break;
	case 1:	{ // unknown
		// The demo of QFG4 calls this with 1+3 parameters, thus there are differences here
		//int16 unk1 = argv[1].toSint16();
		//int16 unk2 = argv[2].toSint16();
		//int16 unk3 = argv[3].toSint16();
		//uint16 unk4 = argv[4].toUint16();
		//uint16 unk5 = (argc >= 6) ? argv[5].toUint16() : 0;
		}
		break;
	case 2:	{ // remap by percent
		//int16 unk1 = argv[1].toSint16();
		//uint16 percent = argv[2].toUint16();
		//uint16 unk3 = (argc >= 4) ? argv[3].toUint16() : 0;
		}
		break;
	case 3:	{ // remap to gray
		//int16 unk1 = argv[1].toSint16();
		//int16 percent = argv[2].toSint16();	// 0 - 100
		//uint16 unk3 = (argc >= 4) ? argv[3].toUint16() : 0;
		}
		break;
	case 4:	{ // unknown
		//int16 unk1 = argv[1].toSint16();
		//uint16 unk2 = argv[2].toUint16();
		//uint16 unk3 = argv[3].toUint16();
		//uint16 unk4 = (argc >= 5) ? argv[4].toUint16() : 0;
		}
		break;
	case 5:	{ // increment color
		//int16 unk1 = argv[1].toSint16();
		//uint16 unk2 = argv[2].toUint16();
		}
		break;
	default:
		break;
	}

	return s->r_acc;
}

#ifdef ENABLE_SCI32

reg_t kIsHiRes(EngineState *s, int argc, reg_t *argv) {
	// Returns 0 if the screen width or height is less than 640 or 400,
	// respectively.
	if (g_system->getWidth() < 640 || g_system->getHeight() < 400)
		return make_reg(0, 0);

	return make_reg(0, 1);
}

// SCI32 variant, can't work like sci16 variants
reg_t kCantBeHere32(EngineState *s, int argc, reg_t *argv) {
//	reg_t curObject = argv[0];
//	reg_t listReference = (argc > 1) ? argv[1] : NULL_REG;
	
	return NULL_REG;
}

reg_t kAddScreenItem(EngineState *s, int argc, reg_t *argv) {
	g_sci->_gfxFrameout->kernelAddScreenItem(argv[0]);
	return s->r_acc;
}

reg_t kUpdateScreenItem(EngineState *s, int argc, reg_t *argv) {
	g_sci->_gfxFrameout->kernelUpdateScreenItem(argv[0]);
	return s->r_acc;
}

reg_t kDeleteScreenItem(EngineState *s, int argc, reg_t *argv) {
	g_sci->_gfxFrameout->kernelDeleteScreenItem(argv[0]);
	return s->r_acc;
}

reg_t kAddPlane(EngineState *s, int argc, reg_t *argv) {
	g_sci->_gfxFrameout->kernelAddPlane(argv[0]);
	return s->r_acc;
}

reg_t kDeletePlane(EngineState *s, int argc, reg_t *argv) {
	g_sci->_gfxFrameout->kernelDeletePlane(argv[0]);
	return s->r_acc;
}

reg_t kUpdatePlane(EngineState *s, int argc, reg_t *argv) {
	g_sci->_gfxFrameout->kernelUpdatePlane(argv[0]);
	return s->r_acc;
}

reg_t kRepaintPlane(EngineState *s, int argc, reg_t *argv) {
	g_sci->_gfxFrameout->kernelRepaintPlane(argv[0]);
	return s->r_acc;
}

reg_t kAddPicAt(EngineState *s, int argc, reg_t *argv) {
	reg_t planeObj = argv[0];
	GuiResourceId pictureId = argv[1].toUint16();
	int16 forWidth = argv[2].toSint16();
	// argv[3] seems to be 0 most of the time

	g_sci->_gfxFrameout->kernelAddPicAt(planeObj, forWidth, pictureId);
	return s->r_acc;
}

reg_t kGetHighPlanePri(EngineState *s, int argc, reg_t *argv) {
	return make_reg(0, g_sci->_gfxFrameout->kernelGetHighPlanePri());
}

reg_t kFrameOut(EngineState *s, int argc, reg_t *argv) {
	// This kernel call likely seems to be doing the screen updates,
	// as its called right after a view is updated

	// TODO
	g_sci->_gfxFrameout->kernelFrameout();

	return NULL_REG;
}

// Tests if the coordinate is on the passed object
reg_t kIsOnMe(EngineState *s, int argc, reg_t *argv) {
	uint16 x = argv[0].toUint16();
	uint16 y = argv[1].toUint16();
	reg_t targetObject = argv[2];
	uint16 illegalBits = argv[3].offset;
	Common::Rect nsRect;

	// we assume that x, y are local coordinates

	// Get the bounding rectangle of the object
	nsRect.left = readSelectorValue(s->_segMan, targetObject, SELECTOR(nsLeft));
	nsRect.top = readSelectorValue(s->_segMan, targetObject, SELECTOR(nsTop));
	nsRect.right = readSelectorValue(s->_segMan, targetObject, SELECTOR(nsRight));
	nsRect.bottom = readSelectorValue(s->_segMan, targetObject, SELECTOR(nsBottom));

	// nsRect top/left may be negative, adjust accordingly
	Common::Rect checkRect = nsRect;
	if (checkRect.top < 0)
		checkRect.top = 0;
	if (checkRect.left < 0)
		checkRect.left = 0;

	bool contained = checkRect.contains(x, y);
	if (contained && illegalBits) {
		// If illegalbits are set, we check the color of the pixel that got clicked on
		//  for now, we return false if the pixel is transparent
		//  although illegalBits may get differently set, don't know yet how this really works out
		uint16 viewId = readSelectorValue(s->_segMan, targetObject, SELECTOR(view));
		int16 loopNo = readSelectorValue(s->_segMan, targetObject, SELECTOR(loop));
		int16 celNo = readSelectorValue(s->_segMan, targetObject, SELECTOR(cel));
		if (g_sci->_gfxCompare->kernelIsItSkip(viewId, loopNo, celNo, Common::Point(x - nsRect.left, y - nsRect.top)))
			contained = false;
	}
	return make_reg(0, contained);
}

reg_t kCreateTextBitmap(EngineState *s, int argc, reg_t *argv) {
	// TODO: argument 0 is usually 0, and arguments 1 and 2 are usually 1
	switch (argv[0].toUint16()) {
	case 0: {
		if (argc != 4) {
			warning("kCreateTextBitmap(0): expected 4 arguments, got %i", argc);
			return NULL_REG;
		}
		reg_t object = argv[3];
		Common::String text = s->_segMan->getString(readSelector(s->_segMan, object, SELECTOR(text)));
		break;
	}
	default:
		warning("CreateTextBitmap(%d)", argv[0].toUint16());
	}

	return NULL_REG;
}

reg_t kRobot(EngineState *s, int argc, reg_t *argv) {

	int16 subop = argv[0].toUint16();

	switch (subop) {
		case 0: { // init
			int id = argv[1].toUint16();
			reg_t obj = argv[2];
			int16 flag = argv[3].toSint16();
			int16 x = argv[4].toUint16();
			int16 y = argv[5].toUint16();
			warning("kRobot(init), id %d, obj %04x:%04x, flag %d, x=%d, y=%d", id, PRINT_REG(obj), flag, x, y);
			GfxRobot *test = new GfxRobot(g_sci->getResMan(), g_sci->_gfxScreen, id);
			test->draw();
			delete test;

			}
			break;
		case 1:	// LSL6 hires (startup)
			// TODO
			return NULL_REG;	// an integer is expected
		case 4: {	// start
				int id = argv[1].toUint16();
				warning("kRobot(start), id %d", id);
			}
			break;
		case 8: // sync
			//warning("kRobot(sync), obj %04x:%04x", PRINT_REG(argv[1]));
			break;
		default:
			warning("kRobot(%d)", subop);
			break;
	}

	return s->r_acc;
}

reg_t kGetWindowsOption(EngineState *s, int argc, reg_t *argv) {
	uint16 windowsOption = argv[0].toUint16();
	switch (windowsOption) {
	case 0:
		// Title bar on/off in Phantasmagoria, we return 0 (off)
		return NULL_REG;
	default:
		warning("GetWindowsOption: Unknown option %d", windowsOption);
		return NULL_REG;
	}
}

reg_t kWinHelp(EngineState *s, int argc, reg_t *argv) {
	switch (argv[0].toUint16()) {
	case 1:
		// Load a help file
		// Maybe in the future we can implement this, but for now this message should suffice
		showScummVMDialog("Please use an external viewer to open the game's help file: " + s->_segMan->getString(argv[1]));
		break;
	case 2:
		// Looks like some init function
		break;
	default:
		warning("Unknown kWinHelp subop %d", argv[0].toUint16());
	}

	return s->r_acc;
}

/**
 * Used to programmatically mass set properties of the target plane
 */
reg_t kSetShowStyle(EngineState *s, int argc, reg_t *argv) {
	// TODO: This is all a stub/skeleton, thus we're invoking kStub() for now
	kStub(s, argc, argv);

	// showStyle matches the style selector of the associated plane object
	uint16 showStyle = argv[0].toUint16();	// 0 - 15
	reg_t planeObj = argv[1];
	//argv[2]
	//int16 priority = argv[3].toSint16();
	//argv[4]
	//argv[5]
	//argv[6]
	//argv[7]
	//int16 unk8 = (argc >= 9) ? argv[8].toSint16() : 0;

	if (showStyle > 15) {
		warning("kSetShowStyle: Illegal style %d for plane %04x:%04x", showStyle, PRINT_REG(planeObj));
		return s->r_acc;
	}

	return s->r_acc;
}

#endif

} // End of namespace Sci
