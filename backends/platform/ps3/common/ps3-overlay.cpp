#include "../ps3.h"
#include <sys/types.h>


void OSystem_PS3::showOverlay()
{
	printf("OSystem_PS3::showOverlay()\n");
}

void OSystem_PS3::hideOverlay()
{
	printf("OSystem_PS3::hideOverlay()\n");
}

void OSystem_PS3::clearOverlay()
{
	printf("OSystem_PS3::clearOverlay()\n");
}

void OSystem_PS3::grabOverlay(OverlayColor *buf, int pitch)
{
	printf("OSystem_PS3::grabOverlay(0x%08X, %d)\n",(unsigned int)(unsigned long)buf,pitch);
}

void OSystem_PS3::copyRectToOverlay(const OverlayColor *buf, int pitch, int x, int y, int w, int h)
{
	printf("OSystem_PS3::updateScreen(0x%08X, %d, %d, %d, %d, %d)\n",(unsigned int)(unsigned long)buf,pitch,x,y,w,h);
}

int16 OSystem_PS3::getOverlayHeight()
{
	printf("OSystem_PS3::getOverlayHeight()\n");
	return 240;
}

int16 OSystem_PS3::getOverlayWidth()
{
	printf("OSystem_PS3::getOverlayWidth()\n");
	return 320;
}

Graphics::PixelFormat OSystem_PS3::getOverlayFormat() const
{
	printf("OSystem_PS3::getOverlayFormat()\n");
	return Graphics::PixelFormat(2,5,6,5,0,11,5,0,0);
}
