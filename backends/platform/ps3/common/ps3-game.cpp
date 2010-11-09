#include "../ps3.h"


int16 OSystem_PS3::getHeight()
{
	printf("OSystem_PS3::getSupportedGraphicsModes()\n");
	return _game_screen.getHeight();
}

int16 OSystem_PS3::getWidth()
{
	printf("OSystem_PS3::getSupportedGraphicsModes()\n");
	return _game_screen.getWidth();
}

void OSystem_PS3::setPalette(const byte *colors, uint start, uint num)
{
	printf("OSystem_PS3::getSupportedGraphicsModes()\n");
	_game_screen.updatePalette(colors,start,num);
}

void OSystem_PS3::grabPalette(byte *colors, uint start, uint num)
{
	printf("OSystem_PS3::getSupportedGraphicsModes()\n");
	_game_screen.grabPalette(colors,start,num);
}

void OSystem_PS3::copyRectToScreen(const byte *buf, int pitch, int x, int y, int w, int h)
{
	printf("OSystem_PS3::getSupportedGraphicsModes()\n");
	_game_screen.updateBuffer(x,y,w,h,buf,pitch);
}

Graphics::Surface *OSystem_PS3::lockScreen()
{
	printf("OSystem_PS3::getSupportedGraphicsModes()\n");
	return _game_screen.lock();
}

void OSystem_PS3::unlockScreen()
{
	printf("OSystem_PS3::getSupportedGraphicsModes()\n");
	_game_screen.unlock();
}
