#include "../ps3.h"


int16 OSystem_PS3::getHeight()
{
	return _game_texture->height();
}

int16 OSystem_PS3::getWidth()
{
	return _game_texture->width();
}

void OSystem_PS3::grabPalette(byte *colors, uint start, uint num)
{
	const byte* palette = _game_texture->palette_const() + start*3;
	do {
		for (int i = 0; i < 3; ++i)
			colors[i] = palette[i];
		colors[3] = 0xff;  // alpha

		palette += 3;
		colors += 4;
	} while (--num);
}

void OSystem_PS3::copyRectToScreen(const byte *buf, int pitch, int x, int y, int w, int h)
{
	_game_texture->updateBuffer(x, y, w, h, buf, pitch);
	draw();
}

Graphics::Surface *OSystem_PS3::lockScreen()
{
	Graphics::Surface* surface = _game_texture->surface();
	//assert(surface->pixels);
	return surface;
}

void OSystem_PS3::unlockScreen()
{
	//assert(_game_texture->dirty());
	draw();
}

void OSystem_PS3::setPalette(const byte *colors, uint start, uint num)
{
	if (!_use_mouse_palette)
		_setCursorPalette(colors, start, num);

	byte* palette = _game_texture->palette() + start*3;
	do {
		for (int i = 0; i < 3; ++i)
			palette[i] = colors[i];
		palette += 3;
		colors += 4;
	} while (--num);
}
