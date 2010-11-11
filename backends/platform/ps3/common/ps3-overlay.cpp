#include "../ps3.h"
#include <sys/types.h>


void OSystem_PS3::showOverlay()
{
	printf("OSystem_PS3::showOverlay()\n");
	_show_overlay=true;

	_pad.setResolution(_overlay_screen.getWidth(),_overlay_screen.getHeight());
}

void OSystem_PS3::hideOverlay()
{
	printf("OSystem_PS3::hideOverlay()\n");
	_show_overlay=false;

	_pad.setResolution(_game_screen.getWidth(),_game_screen.getHeight());
}

void OSystem_PS3::clearOverlay()
{
	printf("OSystem_PS3::clearOverlay()\n");
	_overlay_screen.fillBuffer(0);
}

void OSystem_PS3::grabOverlay(OverlayColor *buf, int pitch)
{
	printf("OSystem_PS3::grabOverlay(0x%08X, %d)\n",(u32)(u64)buf,pitch);

	const Graphics::Surface* surface = _overlay_screen.surface_const();
	int h = surface->h;
	byte *src = (byte *)surface->pixels;
	do {
		memcpy(buf, src, surface->w * 2);
		src += surface->pitch;
		buf += pitch;
	} while (--h);
}

void OSystem_PS3::copyRectToOverlay(const OverlayColor *buf, int pitch, int x, int y, int w, int h)
{
	printf("OSystem_PS3::copyRectToOverlay(0x%08X, %d, %d, %d, %d, %d)\n",(unsigned int)(unsigned long)buf,pitch,x,y,w,h);
	_overlay_screen.updateBuffer(x,y,w,h,buf,pitch*2);
}

int16 OSystem_PS3::getOverlayHeight()
{
	printf("OSystem_PS3::getOverlayHeight()\n");
	return _overlay_screen.getHeight();
}

int16 OSystem_PS3::getOverlayWidth()
{
	printf("OSystem_PS3::getOverlayWidth()\n");
	return _overlay_screen.getWidth();
}

Graphics::PixelFormat OSystem_PS3::getOverlayFormat() const
{
	printf("OSystem_PS3::getOverlayFormat()\n");
	return _overlay_screen.getFormat();
}
