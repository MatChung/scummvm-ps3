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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/engines/sword25/gfx/animationresource.cpp $
 * $Id: animationresource.cpp 53758 2010-10-24 01:52:27Z fingolfin $
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

#include "sword25/gfx/animationresource.h"

#include "sword25/kernel/kernel.h"
#include "sword25/package/packagemanager.h"
#include "sword25/gfx/bitmapresource.h"

namespace Sword25 {

#define BS_LOG_PREFIX "ANIMATIONRESOURCE"

namespace {
const int   DEFAULT_FPS = 10;
const int   MIN_FPS     = 1;
const int   MAX_FPS     = 200;
}

AnimationResource::AnimationResource(const Common::String &filename) :
		Resource(filename, Resource::TYPE_ANIMATION),
		Common::XMLParser(),
		_valid(false) {
	// Get a pointer to the package manager
	_pPackage = Kernel::getInstance()->getPackage();
	BS_ASSERT(_pPackage);

	// Switch to the folder the specified Xml fiile is in
	Common::String oldDirectory = _pPackage->getCurrentDirectory();
	if (getFileName().contains('/')) {
		Common::String dir = Common::String(getFileName().c_str(), strrchr(getFileName().c_str(), '/'));
		_pPackage->changeDirectory(dir);
	}

	// Load the contents of the file
	uint fileSize;
	char *xmlData = _pPackage->getXmlFile(getFileName(), &fileSize);
	if (!xmlData) {
		BS_LOG_ERRORLN("Could not read \"%s\".", getFileName().c_str());
		return; 
	}

	// Parse the contents
	if (!loadBuffer((const byte *)xmlData, fileSize))
		return;

	_valid = parse();
	close();
	free(xmlData);

	// Switch back to the previous folder
	_pPackage->changeDirectory(oldDirectory);

	// Give an error message if there weren't any frames specified
	if (_frames.empty()) {
		BS_LOG_ERRORLN("\"%s\" does not have any frames.", getFileName().c_str());
		return;
	}

	// Pre-cache all the frames
	if (!precacheAllFrames()) {
		BS_LOG_ERRORLN("Could not precache all frames of \"%s\".", getFileName().c_str());
		return;
	}

	// Post processing to compute animation features
	if (!computeFeatures()) {
		BS_LOG_ERRORLN("Could not determine the features of \"%s\".", getFileName().c_str());
		return;
	}

	_valid = true;
}

bool AnimationResource::parseBooleanKey(Common::String s, bool &result) {
	s.toLowercase();
	if (!strcmp(s.c_str(), "true"))
		result = true;
	else if (!strcmp(s.c_str(), "false"))
		result = false;
	else
		return false;
	return true;
}

bool AnimationResource::parserCallback_animation(ParserNode *node) {
	if (!parseIntegerKey(node->values["fps"], 1, &_FPS) || (_FPS < MIN_FPS) || (_FPS > MAX_FPS)) {
		return parserError("Illegal or missing fps attribute in <animation> tag in \"%s\". Assuming default (\"%d\").",
		                 getFileName().c_str(), DEFAULT_FPS);
	}

	// Loop type value
	const char *loopTypeString = node->values["type"].c_str();
	
	if (strcmp(loopTypeString, "oneshot") == 0) {
		_animationType = Animation::AT_ONESHOT;
	} else if (strcmp(loopTypeString, "loop") == 0) {
		_animationType = Animation::AT_LOOP;
	} else if (strcmp(loopTypeString, "jojo") == 0) {
		_animationType = Animation::AT_JOJO;
	} else {
		BS_LOG_WARNINGLN("Illegal type value (\"%s\") in <animation> tag in \"%s\". Assuming default (\"loop\").",
				loopTypeString, getFileName().c_str());
		_animationType = Animation::AT_LOOP;
	}

	// Calculate the milliseconds required per frame
	// FIXME: Double check variable naming. Based on the constant, it may be microseconds
	_millisPerFrame = 1000000 / _FPS;

	return true;
}

bool AnimationResource::parserCallback_frame(ParserNode *node) {
	Frame frame;

	const char *fileString = node->values["file"].c_str();
	if (!fileString) {
		BS_LOG_ERRORLN("<frame> tag without file attribute occurred in \"%s\".", getFileName().c_str());
		return false;
	}
	frame.fileName = _pPackage->getAbsolutePath(fileString);
	if (frame.fileName.empty()) {
		BS_LOG_ERRORLN("Could not create absolute path for file specified in <frame> tag in \"%s\": \"%s\".", 
			getFileName().c_str(), fileString);
		return false;
	}

	const char *actionString = node->values["action"].c_str();
	if (actionString)
		frame.action = actionString;

	const char *hotspotxString = node->values["hotspotx"].c_str();
	const char *hotspotyString = node->values["hotspoty"].c_str();
	if ((!hotspotxString && hotspotyString) ||
	        (hotspotxString && !hotspotyString))
		BS_LOG_WARNINGLN("%s attribute occurred without %s attribute in <frame> tag in \"%s\". Assuming default (\"0\").",
		                 hotspotxString ? "hotspotx" : "hotspoty",
		                 !hotspotyString ? "hotspoty" : "hotspotx",
		                 getFileName().c_str());

	frame.hotspotX = 0;
	if (hotspotxString && !parseIntegerKey(hotspotxString, 1, &frame.hotspotX))
		BS_LOG_WARNINGLN("Illegal hotspotx value (\"%s\") in frame tag in \"%s\". Assuming default (\"%s\").",
		                 hotspotxString, getFileName().c_str(), frame.hotspotX);

	frame.hotspotY = 0;
	if (hotspotyString && !parseIntegerKey(hotspotyString, 1, &frame.hotspotY))
		BS_LOG_WARNINGLN("Illegal hotspoty value (\"%s\") in frame tag in \"%s\". Assuming default (\"%s\").",
		                 hotspotyString, getFileName().c_str(), frame.hotspotY);

	Common::String flipVString = node->values["flipv"];
	if (!flipVString.empty()) {
		if (!parseBooleanKey(flipVString, frame.flipV)) {
			BS_LOG_WARNINGLN("Illegal flipv value (\"%s\") in <frame> tag in \"%s\". Assuming default (\"false\").",
			                 flipVString.c_str(), getFileName().c_str());
			frame.flipV = false;
		}
	} else
		frame.flipV = false;

	Common::String flipHString = node->values["fliph"];
	if (!flipHString.empty()) {
		if (!parseBooleanKey(flipVString, frame.flipV)) {
			BS_LOG_WARNINGLN("Illegal fliph value (\"%s\") in <frame> tag in \"%s\". Assuming default (\"false\").",
			                 flipHString.c_str(), getFileName().c_str());
			frame.flipH = false;
		}
	} else
		frame.flipH = false;

	_frames.push_back(frame);
	return true;
}

AnimationResource::~AnimationResource() {
}

bool AnimationResource::precacheAllFrames() const {
	Common::Array<Frame>::const_iterator iter = _frames.begin();
	for (; iter != _frames.end(); ++iter) {
		if (!Kernel::getInstance()->getResourceManager()->precacheResource((*iter).fileName)) {
			BS_LOG_ERRORLN("Could not precache \"%s\".", (*iter).fileName.c_str());
			return false;
		}
	}

	return true;
}

bool AnimationResource::computeFeatures() {
	BS_ASSERT(_frames.size());

	// Alle Features werden als vorhanden angenommen
	_scalingAllowed = true;
	_alphaAllowed = true;
	_colorModulationAllowed = true;

	// Alle Frame durchgehen und alle Features deaktivieren, die auch nur von einem Frame nicht unterstützt werden.
	Common::Array<Frame>::const_iterator iter = _frames.begin();
	for (; iter != _frames.end(); ++iter) {
		BitmapResource *pBitmap;
		if (!(pBitmap = static_cast<BitmapResource *>(Kernel::getInstance()->getResourceManager()->requestResource((*iter).fileName)))) {
			BS_LOG_ERRORLN("Could not request \"%s\".", (*iter).fileName.c_str());
			return false;
		}

		if (!pBitmap->isScalingAllowed())
			_scalingAllowed = false;
		if (!pBitmap->isAlphaAllowed())
			_alphaAllowed = false;
		if (!pBitmap->isColorModulationAllowed())
			_colorModulationAllowed = false;

		pBitmap->release();
	}

	return true;
}

} // End of namespace Sword25
