# $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/tools/skycpt/module.mk $
# $Id: module.mk 48148 2010-02-27 17:02:58Z jvprat $

MODULE := tools/skycpt

MODULE_OBJS := \
	AsciiCptCompile.o \
	KmpSearch.o \
	TextFile.o \
	cptcompiler.o \
	cpthelp.o \
	idFinder.o

# Set the name of the executable
TOOL_EXECUTABLE := skycpt

# Include common rules
include $(srcdir)/rules.mk
