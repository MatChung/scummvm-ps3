MODULE := backends/platform/ps3

MODULE_OBJS := \
	ps3main.o \
	graphics.o \
	ps3.o \
	input/ps3-pad.o \
	input/ps3-vkey.o \
	graphics/ps3-game.o \
	graphics/ps3-graphics.o \
	graphics/ps3-mouse.o \
	graphics/ps3-overlay.o \
	graphics/rgb-texture.o \
	netdbg/net.o \
	sound/ps3-sound.o

# We don't use rules.mk but rather manually update OBJS and MODULE_DIRS.
MODULE_OBJS := $(addprefix $(MODULE)/, $(MODULE_OBJS))
OBJS := $(MODULE_OBJS) $(OBJS)
MODULE_DIRS += $(sort $(dir $(MODULE_OBJS)))
