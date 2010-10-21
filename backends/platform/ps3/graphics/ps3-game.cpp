#include "../ps3.h"


int16 OSystem_PS3::getHeight()
{
	net_send("OSystem_PS3::getHeight()\n");
	return _game_texture->height();
}

int16 OSystem_PS3::getWidth()
{
	net_send("OSystem_PS3::getWidth()\n");
	return _game_texture->width();
}

void OSystem_PS3::grabPalette(byte *colors, uint start, uint num)
{
	net_send("OSystem_PS3::grabPalette()\n");
	_game_texture->grabPalette(colors,start,num);/*
	const byte* palette = _game_texture->palette_const() + start*3;
	do {
		for (int i = 0; i < 3; ++i)
			colors[i] = palette[i];
		colors[3] = 0xff;  // alpha

		palette += 3;
		colors += 4;
	} while (--num);*/
}

void OSystem_PS3::copyRectToScreen(const byte *buf, int pitch, int x, int y, int w, int h)
{
	//net_send("OSystem_PS3::copyRectToScreen()\n");
	_game_texture->updateBuffer(x, y, w, h, buf, pitch);
	draw();
}

Graphics::Surface *OSystem_PS3::lockScreen()
{
	net_send("OSystem_PS3::lockScreen()\n");
	Graphics::Surface* surface = _game_texture->lock();
	//assert(surface->pixels);
	return surface;
}

void OSystem_PS3::unlockScreen()
{
	net_send("OSystem_PS3::unlockScreen()\n");
	_game_texture->unlock();
	//assert(_game_texture->dirty());
	draw();
}

void OSystem_PS3::setPalette(const byte *colors, uint start, uint num)
{
	//net_send("OSystem_PS3::setPalette()\n");
	if (_use_mouse_palette)
		_setCursorPalette(colors, start, num);

	_game_texture->updatePalette(colors, start, num);
/*
	byte* palette = _game_texture->palette() + start*3;
	do {
		for (int i = 0; i < 3; ++i)
			palette[i] = colors[i];
		palette += 3;
		colors += 4;
	} while (--num);*/
}
