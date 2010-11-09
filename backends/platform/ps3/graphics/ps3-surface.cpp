#include "ps3-surface.h"
#include "fb/fb.h"

PS3Texture::PS3Texture() :
	_isLocked(false),
	_palette_dirty(0)
{
	_palette=new u32[256];
	memset(_palette,0,sizeof(u32)*256);

	// This all gets reset later in allocBuffer:
	_surface.w = 0;
	_surface.h = 0;
	_surface.pitch = 0;
	_surface.pixels = NULL;
	_surface.bytesPerPixel = 0;

	_pixelFormat=Graphics::PixelFormat::createFormatCLUT8();
	_internalFormat=getFormatFromPixelFormat();

	clearDirty();
}

PS3Texture::~PS3Texture()
{
	printf("GLESTexture::~GLESTexture()\n");
	printf("    delete mem texture\n");
	if(_texture)
		delete []_texture;
	printf("    delete mem palette\n");
	if(_palette)
		delete []_palette;
	printf("    destroyed\n");
}

void PS3Texture::setFormat(Graphics::PixelFormat &format)
{
	if(format!=_pixelFormat)
	{
		_surface.w=0;
		_surface.h=0;
		_surface.bytesPerPixel=0;
		_pixelFormat=format;
		_internalFormat=getFormatFromPixelFormat();
	}
}

void PS3Texture::draw(u32 x,u32 y,u32 w,u32 h)
{
	if(_pixelFormat.bytesPerPixel==1)
		blitPalette(_surface.w,_surface.h,_texture, _palette);
	else
		blitRGB(_surface.w,_surface.h,_texture,_pixelFormat);

}

void PS3Texture::allocBuffer(u32 w, u32 h)
{
	if (w == _surface.w && h == _surface.w)
		// Already allocated a sufficiently large buffer
		return;

	_surface.w = w;
	_surface.h = h;
	_surface.bytesPerPixel = _pixelFormat.bytesPerPixel;

	printf("GLESTexture::allocBuffer(%d,%d,%d)\n",w,h,_surface.bytesPerPixel);

	if (_texture)
	{
		printf("    delete old texture\n");
		delete[] _texture;
	}

	printf("    alloc new texture\n");
	_texture = new byte[_surface.w * _surface.h * _surface.bytesPerPixel];
	_surface.pixels = _texture;
	_surface.pitch = _surface.w * _surface.bytesPerPixel;

	fillBuffer(0);
}

void PS3Texture::fillBuffer(byte x)
{
	allDirty();

	memset(_surface.pixels, x, _surface.pitch * _surface.h);
}

void PS3Texture::updateBuffer(u32 x, u32 y, u32 w, u32 h, const void* buf, int pitch)
{
	x=MIN(x,((u32)_surface.w));
	y=MIN(y,((u32)_surface.h));
	w=MIN(w,((u32)_surface.w-x));
	h=MIN(h,((u32)_surface.h-y));

	_dirty_top=MIN(y,_dirty_top);
	_dirty_bottom=MAX(y+h,_dirty_bottom);

	switch(_internalFormat)
	{
	case FORMAT_555:
		{
			const uint16* src = static_cast<const uint16*>(buf);
			uint16* dst = static_cast<uint16*>(_surface.getBasePtr(x, y));

			do
			{
				for(u32 xx=0;xx<w;xx++)
				{
					if(src[xx]==_keycolor)
						dst[xx]=0;
					else
						dst[xx]=(src[xx]<<1)|1;
				}
				dst += _surface.pitch/2;// 2 because uint16
				src += pitch/2;// 2 because uint16
			} while (--h);
		}
		break;
	case FORMAT_4444:
	case FORMAT_565:
	case FORMAT_5551:
		{
			const uint16* src = static_cast<const uint16*>(buf);
			uint16* dst = static_cast<uint16*>(_surface.getBasePtr(x, y));

			do
			{
				for(u32 xx=0;xx<w;xx++)
				{
					if(src[xx]==_keycolor)
						dst[xx]=0;
					else
						dst[xx]=(src[xx]);
				}
				dst += _surface.pitch/2;// 2 because uint16
				src += pitch/2;// 2 because uint16
			} while (--h);
		}
		break;
	case FORMAT_8888:
		{
			const uint32* src = static_cast<const uint32*>(buf);
			uint32* dst = static_cast<uint32*>(_surface.getBasePtr(x, y));

			do
			{
				for(u32 xx=0;xx<w;xx++)
				{
					if(src[xx]==_keycolor)
						dst[xx]=0;
					else
						dst[xx]=(src[xx]);
				}
				dst += _surface.pitch/4;// 4 because uint32
				src += pitch/4;// 4 because uint32
			} while (--h);
		}
		break;
	case FORMAT_PALETTE:
		{
			const uint8* src = static_cast<const uint8*>(buf);
			uint8* dst = &_texture[x+y*_surface.w];

			do
			{
				dst = &_texture[x+y*_surface.w];
				for(u32 xx=0;xx<w;xx++)
				{
					dst[xx]=src[xx];
				}
				//memcpy(dst,src,w);
				dst += _surface.w;
				src += pitch;
				y++;
			} while (--h);
		}
		break;
	}
}

Graphics::Surface* PS3Texture::lock()
{
	_isLocked=true;

	switch(_internalFormat)
	{
	case FORMAT_555:
		{
			uint16* dst = static_cast<uint16*>(_surface.pixels);

			for(uint16 y=0;y<_surface.h;y++)
			{
				for(uint16 x=0;x<_surface.w;x++)
				{
					dst[y*_surface.pitch/2+x]=(dst[y*_surface.pitch/2+x]>>1);
				}
			}
		}
		break;
	default:
		break;
	}

	return &_surface;
}

void PS3Texture::unlock()
{
	_isLocked=false;

	switch(_internalFormat)
	{
	case FORMAT_555:
		{
			uint16* dst = static_cast<uint16*>(_surface.pixels);
			for(uint16 y=0;y<_surface.h;y++)
			{
				for(uint16 x=0;x<_surface.w;x++)
				{
					dst[y*_surface.pitch/2+x]=(dst[y*_surface.pitch/2+x]<<1)|1;
				}
			}
		}
		break;
	default:
		break;
	}

	allDirty();
}


void PS3Texture::setKeyColor(uint32 color)
{
	_keycolor=color;
	_palette_dirty=true;
}

void PS3Texture::grabPalette(byte *colors, uint start, uint num)
{
	uint32* dst = (uint32*)(colors);
	for(uint i=start;i<start+num;i++)
	{
		*dst=_palette[i];
		dst++;
	}
}

void PS3Texture::updatePalette(const byte *colors, uint start, uint num)
{
	const uint32* src = (uint32*)(colors);
	for(uint i=start;i<start+num;i++)
	{
		_palette[i]=(*src)|0xFF;
		src++;
	}

	_palette_dirty=true;
}

PS3Texture::TextureFormat PS3Texture::getFormatFromPixelFormat()
{
	if(_pixelFormat.bytesPerPixel==1)
		return FORMAT_PALETTE;

	if(_pixelFormat.bytesPerPixel==2)
	{
		if(_pixelFormat.rBits()==4 && _pixelFormat.gBits()==4 && _pixelFormat.bBits()==4 && _pixelFormat.aBits()==4)
			return FORMAT_4444;
		if(_pixelFormat.rBits()==5 && _pixelFormat.gBits()==6 && _pixelFormat.bBits()==5 && _pixelFormat.aBits()==0)
			return FORMAT_565;
		if(_pixelFormat.rBits()==5 && _pixelFormat.gBits()==5 && _pixelFormat.bBits()==5 && _pixelFormat.aBits()==0)
			return FORMAT_555;
		if(_pixelFormat.rBits()==5 && _pixelFormat.gBits()==5 && _pixelFormat.bBits()==5 && _pixelFormat.aBits()==1)
			return FORMAT_5551;
	}

	if(_pixelFormat.bytesPerPixel==4)
	{
		if(_pixelFormat.rBits()==8 && _pixelFormat.gBits()==8 && _pixelFormat.bBits()==8 && _pixelFormat.aBits()==8)
			return FORMAT_8888;
	}

	printf("GLESTexture::getFormatFromPixelFormat: UNKNOWN FORMAT\n");
	printf("  bytesPerPixel: %d\n",_pixelFormat.bytesPerPixel);
	printf("  rBits:         %d\n",_pixelFormat.rBits());
	printf("  gBits:         %d\n",_pixelFormat.gBits());
	printf("  bBits:         %d\n",_pixelFormat.bBits());
	printf("  aBits:         %d\n",_pixelFormat.aBits());
	printf("  rShift:        %d\n",_pixelFormat.rShift);
	printf("  gShift:        %d\n",_pixelFormat.gShift);
	printf("  bShift:        %d\n",_pixelFormat.bShift);
	printf("  aShift:        %d\n",_pixelFormat.aShift);
	return FORMAT_8888;
}
