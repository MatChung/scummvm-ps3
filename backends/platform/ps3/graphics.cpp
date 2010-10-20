#include "ps3.h"


extern Common::List<Graphics::PixelFormat> __formats;
static const OSystem::GraphicsMode s_supportedGraphicsModes[] = {
		{"default", "Default", 1},
		{0, 0, 0},
};

Common::List<Graphics::PixelFormat> OSystem_PS3::getSupportedFormats() const
{
	printf("OSystem_PS3::getSupportedFormats()\n");
	return __formats;
}

Graphics::PixelFormat OSystem_PS3::getScreenFormat() const
{
	printf("OSystem_PS3::getScreenFormat()\n");
	return Graphics::PixelFormat(4,8,8,8,8,0,8,16,24);
}

const OSystem::GraphicsMode* OSystem_PS3::getSupportedGraphicsModes() const
{
	printf("OSystem_PS3::getSupportedGraphicsModes()\n");
	return s_supportedGraphicsModes;
}


int OSystem_PS3::getDefaultGraphicsMode() const
{
	printf("OSystem_PS3::getDefaultGraphicsMode()\n");
	return 0;
}

bool OSystem_PS3::setGraphicsMode(const char *mode)
{
	printf("OSystem_PS3::setGraphicsMode()\n");
	return true;
}

bool OSystem_PS3::setGraphicsMode(int mode)
{
	printf("OSystem_PS3::setGraphicsMode()\n");
	return true;
}

int OSystem_PS3::getGraphicsMode() const
{
	printf("OSystem_PS3::getGraphicsMode()\n");
	return 0;
}

void OSystem_PS3::initSize(uint width, uint height, const Graphics::PixelFormat *format)
{
	printf("OSystem_PS3::initSize()\n");
	_pad.setResolution(width,height);
	printf("PS3GL::initSize(%d,%d,%d,%d,%d,%d)\n",width,height,format->rBits(),format->gBits(),format->bBits(),format->aBits());

	//_egl_surface_width=width;
	//_egl_surface_height=height;
	_game_texture->allocBuffer(width, height);

	// Cap at 320x200 or the ScummVM themes abort :/
	GLuint overlay_width = MIN((int)_egl_surface_width, 640);
	GLuint overlay_height = MIN((int)_egl_surface_height, 400);
	_overlay_texture->allocBuffer(overlay_width, overlay_height);

	// Don't know mouse size yet - it gets reallocated in
	// setMouseCursor.  We need the palette allocated before
	// setMouseCursor however, so just take a guess at the desired
	// size (it's small).
	_mouse_texture->allocBuffer(20, 20);
}
