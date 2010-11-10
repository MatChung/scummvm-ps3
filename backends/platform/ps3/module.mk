MODULE := backends/platform/ps3

MODULE_OBJS := \
	ps3-main.o \
	audio/ps3-audio.o \
	common/ps3-common.o \
	common/ps3-features.o \
	common/ps3-game.o \
	common/ps3-graphics.o \
	common/ps3-graphicsquery.o \
	common/ps3-mouse.o \
	common/ps3-overlay.o \
	common/ps3-system.o \
	filesystem/ps3-fs-factory.o \
	filesystem/ps3-fs-node.o \
	graphics/ps3-surface.o \
	graphics/fb/fb.o \
	graphics/fb/simpleblit.o \
	input/ps3-pad.o \
	
	
# We don't use rules.mk but rather manually update OBJS and MODULE_DIRS.
MODULE_OBJS := $(addprefix $(MODULE)/, $(MODULE_OBJS))
OBJS := $(MODULE_OBJS) $(OBJS)
MODULE_DIRS += $(sort $(dir $(MODULE_OBJS)))
