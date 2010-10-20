# $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/tools/create_lure/module.mk $
# $Id: module.mk 48148 2010-02-27 17:02:58Z jvprat $

MODULE := tools/create_lure

MODULE_OBJS := \
	create_lure_dat.o \
	process_actions.o

# Set the name of the executable
TOOL_EXECUTABLE := create_lure

# Include common rules
include $(srcdir)/rules.mk
