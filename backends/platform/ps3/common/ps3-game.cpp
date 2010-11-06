#include "../ps3.h"


int16 OSystem_PS3::getHeight()
{
	return 320;
}

int16 OSystem_PS3::getWidth()
{
	return 200;
}

void OSystem_PS3::setPalette(const byte *colors, uint start, uint num)
{
}

void OSystem_PS3::grabPalette(byte *colors, uint start, uint num)
{
}

void OSystem_PS3::copyRectToScreen(const byte *buf, int pitch, int x, int y, int w, int h)
{
}

Graphics::Surface *OSystem_PS3::lockScreen()
{
	return NULL;
}

void OSystem_PS3::unlockScreen()
{
}
