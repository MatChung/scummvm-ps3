#include "../ps3.h"

extern Common::List<Graphics::PixelFormat> __formats;

const OSystem::GraphicsMode *OSystem_PS3::getSupportedGraphicsModes() const
{
	printf("OSystem_PS3::getSupportedGraphicsModes()\n");
	return NULL;
}

int OSystem_PS3::getDefaultGraphicsMode() const
{
	printf("OSystem_PS3::getDefaultGraphicsMode()\n");
	return 0;
}

bool OSystem_PS3::setGraphicsMode(const char *name)
{
	printf("OSystem_PS3::setGraphicsMode(%s)\n",name);
	return false;
}

bool OSystem_PS3::setGraphicsMode(int mode)
{
	printf("OSystem_PS3::setGraphicsMode(%d)\n",mode);
	return false;
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

