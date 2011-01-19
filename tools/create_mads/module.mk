# $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/tools/create_mads/module.mk $
# $Id: module.mk 54393 2010-11-20 10:00:24Z dreammaster $

MODULE := tools/create_mads

MODULE_OBJS := \
	main.o \
	parser.o

# Set the name of the executable
TOOL_EXECUTABLE := create_mads

# Include common rules
include $(srcdir)/rules.mk
