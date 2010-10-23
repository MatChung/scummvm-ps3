MODULE := backends/platform/ps3

MODULE_OBJS := \
	graphics.o \
	pad.o \
	ps3.o \
	ps3main.o \
	graphics/ps3-game.o \
	graphics/ps3-graphics.o \
	graphics/ps3-mouse.o \
	graphics/ps3-overlay.o \
	graphics/rgb-texture.o \
	graphics/palette-texture.o \
	netdbg/net.o \
	sound/ps3-sound.o

# We don't use rules.mk but rather manually update OBJS and MODULE_DIRS.
MODULE_OBJS := $(addprefix $(MODULE)/, $(MODULE_OBJS))
OBJS := $(MODULE_OBJS) $(OBJS)
MODULE_DIRS += $(sort $(dir $(MODULE_OBJS)))
