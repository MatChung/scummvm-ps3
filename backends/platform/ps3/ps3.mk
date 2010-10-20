MODULE := backends/platform/ps3

MODULE_OBJS := \
	ps3v.o

# We don't use rules.mk but rather manually update OBJS and MODULE_DIRS.
MODULE_DIRS += \
	backends/platform/ps3/
OBJS := $(addprefix $(MODULE)/, $(MODULE_OBJS)) $(OBJS)

# Include common rules
include $(srcdir)/rules.mk


