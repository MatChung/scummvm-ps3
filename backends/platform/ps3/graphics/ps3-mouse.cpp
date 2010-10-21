#include "../ps3.h"


bool OSystem_PS3::showMouse(bool visible)
{
	net_send("PS3GL::showMouse(%d)\n",visible);
	_show_mouse=visible;
	return visible;
}

void OSystem_PS3::warpMouse(int x, int y)
{
	//net_send("PS3GL::warpMouse(%d, %d)\n",x,y);
	_mouse_pos.x=x;
	_mouse_pos.y=y;

	updateScreen();
}

void OSystem_PS3::setMouseCursor(const byte *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, int cursorTargetScale, const Graphics::PixelFormat *format)
{
	net_send("PS3GL::setMouseCursor(%p, %u, %u, %d, %d, %d, %d, %p)",
		  buf, w, h, hotspotX, hotspotY, (int)keycolor, cursorTargetScale,
		  format);

	assert(keycolor < 256);

	_mouse_texture->allocBuffer(w, h);

	// Update palette alpha based on keycolor
	byte* palette = _mouse_texture->palette();
	int i = 256;
	do {
		palette[3] = 0xff;
		palette += 4;
	} while (--i);
	palette = _mouse_texture->palette();
	palette[keycolor*4 + 3] = 0x00;
	_mouse_texture->updateBuffer(0, 0, w, h, buf, w);

	_mouse_hotspot = Common::Point(hotspotX, hotspotY);
	_mouse_targetscale = cursorTargetScale;

	draw();
}
void OSystem_PS3::_setCursorPalette(const byte *colors,
					uint start, uint num) {
	net_send("PS3GL::_setCursorPalette(%d, %d, %d)\n",colors,start,num);
	byte* palette = _mouse_texture->palette() + start*4;
	do {
		for (int i = 0; i < 3; ++i)
			palette[i] = colors[i];
		// Leave alpha untouched to preserve keycolor

		palette += 4;
		colors += 4;
	} while (--num);
}

void OSystem_PS3::disableCursorPalette(bool disable)
{
	net_send("PS3GL::disableCursorPalette(%d)\n",disable);
	_use_mouse_palette=!disable;
}

void OSystem_PS3::setCursorPalette(const byte *colors, uint start, uint num)
{
	net_send("PS3GL::setCursorPalette(%d, %d, %d)\n",colors,start,num);
	_setCursorPalette(colors,start,num);
}
