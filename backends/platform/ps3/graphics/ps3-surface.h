/*
 * ps3-surface.h
 *
 *  Created on: 09.11.2010
 *      Author: lousy
 */

#ifndef PS3SURFACE_H_
#define PS3SURFACE_H_

#include "graphics/surface.h"
#include "graphics/pixelformat.h"

#include "common/rect.h"
#include "common/array.h"

#include <psl1ght/types.h>

class PS3Texture
{
	enum TextureFormat
	{
		FORMAT_PALETTE,
		FORMAT_4444,
		FORMAT_565,
		FORMAT_555,
		FORMAT_5551,
		FORMAT_8888,
	};
public:
	static void initGLExtensions();

	PS3Texture();
	~PS3Texture();
	void allocBuffer(u32 width, u32 height);

	uint16 getWidth() const { return _surface.w; }
	uint16 getHeight() const { return _surface.h; }
	Graphics::PixelFormat getFormat() const {return _pixelFormat;};
	void setFormat(Graphics::PixelFormat &format);
	const Graphics::Surface* surface_const() const { return &_surface; }

	//void setBuffer(u32 width, u32 height, const void* buf);
	void updateBuffer(u32 x, u32 y, u32 w, u32 h, const void* buf, int pitch);
	void fillBuffer(byte x);
	Graphics::Surface* lock();
	void unlock();

	void grabPalette(byte *colors, uint start, uint num);
	void updatePalette(const byte *colors, uint start, uint num);
	void setKeyColor(uint32 color);

	void draw() { draw(0, 0, _surface.w, _surface.h); }
	void draw(u32 x,u32 y,u32 w,u32 h);

protected:
	void allDirty(){_dirty_top=0;_dirty_bottom=_surface.h;};
	void clearDirty(){_dirty_top=99999;_dirty_bottom=0;};

	TextureFormat getFormatFromPixelFormat();

	Graphics::Surface _surface;
	uint32 _keycolor;
	Graphics::PixelFormat _pixelFormat;
	TextureFormat _internalFormat;
	uint32 _renderType;

	byte* _texture;
	u32* _palette;

	u32  _dirty_top;
	u32  _dirty_bottom;
	bool _palette_dirty;

	bool _isLocked;
};

#endif /* PS3SURFACE_H_ */
