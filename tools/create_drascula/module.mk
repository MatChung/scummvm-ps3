# $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/tools/create_drascula/module.mk $
# $Id: module.mk 32598 2008-06-07 18:59:12Z sev $

MODULE := tools/create_drascula

MODULE_OBJS := \
	create_drascula.o

# Set the name of the executable
TOOL_EXECUTABLE := create_drascula

# Include common rules
include $(srcdir)/rules.mk
