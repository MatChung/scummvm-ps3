#include "ps3.h"


extern Common::List<Graphics::PixelFormat> __formats;
static const OSystem::GraphicsMode s_supportedGraphicsModes[] = {
		{"default", "Default", 1},
		{0, 0, 0},
};

Common::List<Graphics::PixelFormat> OSystem_PS3::getSupportedFormats() const
{
	net_send("OSystem_PS3::getSupportedFormats()\n");
	return __formats;
}

Graphics::PixelFormat OSystem_PS3::getScreenFormat() const
{
	//net_send("OSystem_PS3::getScreenFormat()\n");
	return _currentScreenFormat;
}

const OSystem::GraphicsMode* OSystem_PS3::getSupportedGraphicsModes() const
{
	net_send("OSystem_PS3::getSupportedGraphicsModes()\n");
	return s_supportedGraphicsModes;
}


int OSystem_PS3::getDefaultGraphicsMode() const
{
	net_send("OSystem_PS3::getDefaultGraphicsMode()\n");
	return 0;
}

bool OSystem_PS3::setGraphicsMode(const char *mode)
{
	net_send("OSystem_PS3::setGraphicsMode()\n");
	return true;
}

bool OSystem_PS3::setGraphicsMode(int mode)
{
	net_send("OSystem_PS3::setGraphicsMode()\n");
	return true;
}

int OSystem_PS3::getGraphicsMode() const
{
	net_send("OSystem_PS3::getGraphicsMode()\n");
	return 0;
}
