#include "ps3.h"


extern Common::List<Graphics::PixelFormat> __formats;
static const OSystem::GraphicsMode s_supportedGraphicsModes[] = {
		{"default", "Stretched", 0},
		{"aspect", "Aspect correct", 1},
		{"default-bi", "Stretched Bilinear (does NOT work in most games)", 2},
		{"aspect-bi", "Aspect correct Bilinear (does NOT work in most games)", 3},
		{0, 0, 0},
};

static const int numGraphicsModes=4;

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
	for(int i=0;i<numGraphicsModes;i++)
	{
		if(strcmp(mode,s_supportedGraphicsModes[i].name)==0)
		{
			return setGraphicsMode(i);
		}
	}
	return false;
}

bool OSystem_PS3::setGraphicsMode(int mode)
{
	net_send("OSystem_PS3::setGraphicsMode(%d)\n",mode);
	net_send("OSystem_PS3::setGraphicsMode(%d)\n",mode);
	net_send("OSystem_PS3::setGraphicsMode(%d)\n",mode);
	net_send("OSystem_PS3::setGraphicsMode(%d)\n",mode);
	net_send("OSystem_PS3::setGraphicsMode(%d)\n",mode);

	_current_graphics_mode=mode;

	switch(mode)
	{
	case 0:
		_stretch_to_full=true;
		_game_texture->setFilter(0);
		_mouse_texture->setFilter(0);
		_overlay_texture->setFilter(0);
		return true;
	case 1:
		_stretch_to_full=false;
		_game_texture->setFilter(0);
		_mouse_texture->setFilter(0);
		_overlay_texture->setFilter(0);
		return true;
	case 2:
		_stretch_to_full=true;
		_game_texture->setFilter(1);
		_mouse_texture->setFilter(1);
		_overlay_texture->setFilter(1);
		return true;
	case 3:
		_stretch_to_full=false;
		_game_texture->setFilter(1);
		_mouse_texture->setFilter(1);
		_overlay_texture->setFilter(1);
		return true;
	}

	return false;
}

int OSystem_PS3::getGraphicsMode() const
{
	net_send("OSystem_PS3::getGraphicsMode(%d)\n",_current_graphics_mode);
	net_send("OSystem_PS3::getGraphicsMode()\n");
	net_send("OSystem_PS3::getGraphicsMode()\n");
	net_send("OSystem_PS3::getGraphicsMode()\n");
	return _current_graphics_mode;
}
