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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/tools/create_project/visualstudio.h $
 * $Id: visualstudio.h 54425 2010-11-23 11:47:52Z littleboy $
 *
 */

#ifndef TOOLS_CREATE_PROJECT_VISUALSTUDIO_H
#define TOOLS_CREATE_PROJECT_VISUALSTUDIO_H

#include "msvc.h"

namespace CreateProjectTool {

class VisualStudioProvider : public MSVCProvider {
public:
	VisualStudioProvider(StringList &global_warnings, std::map<std::string, StringList> &project_warnings, const int version);

protected:
	void createProjectFile(const std::string &name, const std::string &uuid, const BuildSetup &setup, const std::string &moduleDir,
	                       const StringList &includeList, const StringList &excludeList);

	void writeFileListToProject(const FileNode &dir, std::ofstream &projectFile, const int indentation,
	                            const StringList &duplicate, const std::string &objPrefix, const std::string &filePrefix);

	void writeReferences(std::ofstream &output);

	void outputGlobalPropFile(std::ofstream &properties, int bits, const StringList &defines, const std::string &prefix);

	void createBuildProp(const BuildSetup &setup, bool isRelease, bool isWin32, bool enableAnalysis);

	const char *getProjectExtension();
	const char *getPropertiesExtension();
	int getVisualStudioVersion();
};

} // End of CreateProjectTool namespace

#endif // TOOLS_CREATE_PROJECT_VISUALSTUDIO_H
