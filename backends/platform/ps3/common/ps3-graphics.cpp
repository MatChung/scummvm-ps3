#include "../ps3.h"

extern Common::List<Graphics::PixelFormat> __formats;

const OSystem::GraphicsMode *OSystem_PS3::getSupportedGraphicsModes() const
{
	return NULL;
}

int OSystem_PS3::getDefaultGraphicsMode() const
{
	return 0;
}

bool OSystem_PS3::setGraphicsMode(const char *name)
{
	return false;
}

bool OSystem_PS3::setGraphicsMode(int mode)
{
	return false;
}

int OSystem_PS3::getGraphicsMode() const
{
	return 0;
}

Common::List<Graphics::PixelFormat> OSystem_PS3::getSupportedFormats() const
{
	return __formats;
}

Graphics::PixelFormat OSystem_PS3::getScreenFormat() const
{
	return Graphics::PixelFormat::createFormatCLUT8();
}

