#include "../ps3.h"


bool OSystem_PS3::showMouse(bool visible)
{
	if(_show_mouse!=visible)
		printf("OSystem_PS3::showMouse(%d)\n",visible);
	_show_mouse=visible;
	return visible;
}

void OSystem_PS3::warpMouse(int x, int y)
{
	printf("OSystem_PS3::warpMouse(%d, %d)\n",x,y);
	_mouse_pos.x=x;
	_mouse_pos.y=y;
}

void OSystem_PS3::setMouseCursor(const byte *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, int cursorTargetScale, const Graphics::PixelFormat *format)
{
	printf("OSystem_PS3::setMouseCursor(%d, %d, %d, %d, %d, %d, %d)\n",w,h,hotspotX,hotspotY,keycolor,cursorTargetScale,(u32)(u64)format);
}

void OSystem_PS3::disableCursorPalette(bool disable)
{
	printf("OSystem_PS3::disableCursorPalette(%d)\n",disable);
	_use_mouse_palette=!disable;
}

void OSystem_PS3::setCursorPalette(const byte *colors, uint start, uint num)
{
	printf("OSystem_PS3::setCursorPalette(%d, %d)\n",start,num);
	//_setCursorPalette(colors,start,num);
}

