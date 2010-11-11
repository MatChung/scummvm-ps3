#include "../ps3.h"

extern Common::List<Graphics::PixelFormat> __formats;
static const OSystem::GraphicsMode s_supportedGraphicsModes[] = {
		{"default", "Stretched", 0},
//		{"aspect", "Aspect correct", 1},
//		{"default-bi", "Stretched Bilinear (does NOT work in most games)", 2},
//		{"aspect-bi", "Aspect correct Bilinear (does NOT work in most games)", 3},
		{0, 0, 0},
};

const OSystem::GraphicsMode *OSystem_PS3::getSupportedGraphicsModes() const
{
	printf("OSystem_PS3::getSupportedGraphicsModes()\n");
	return s_supportedGraphicsModes;
}

int OSystem_PS3::getDefaultGraphicsMode() const
{
	printf("OSystem_PS3::getDefaultGraphicsMode()\n");
	return 0;
}

bool OSystem_PS3::setGraphicsMode(const char *name)
{
	printf("OSystem_PS3::setGraphicsMode(%s)\n",name);
	return true;
}

bool OSystem_PS3::setGraphicsMode(int mode)
{
	printf("OSystem_PS3::setGraphicsMode(%d)\n",mode);
	return true;
}

int OSystem_PS3::getGraphicsMode() const
{
	printf("OSystem_PS3::getGraphicsMode()\n");
	return 0;
}

Common::List<Graphics::PixelFormat> OSystem_PS3::getSupportedFormats() const
{
	printf("OSystem_PS3::getSupportedFormats()\n");
	return __formats;
}

Graphics::PixelFormat OSystem_PS3::getScreenFormat() const
{
	printf("OSystem_PS3::getScreenFormat()\n");
	return _game_screen.getFormat();
}

