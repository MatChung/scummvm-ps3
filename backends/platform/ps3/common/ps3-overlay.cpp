#include "../ps3.h"


void OSystem_PS3::showOverlay()
{
}

void OSystem_PS3::hideOverlay()
{
}

void OSystem_PS3::clearOverlay()
{
}

void OSystem_PS3::grabOverlay(OverlayColor *buf, int pitch)
{
}

void OSystem_PS3::copyRectToOverlay(const OverlayColor *buf, int pitch, int x, int y, int w, int h)
{
}

int16 OSystem_PS3::getOverlayHeight()
{
	return 320;
}

int16 OSystem_PS3::getOverlayWidth()
{
	return 200;
}

Graphics::PixelFormat OSystem_PS3::getOverlayFormat() const
{
	return Graphics::PixelFormat::createFormatCLUT8();
}
