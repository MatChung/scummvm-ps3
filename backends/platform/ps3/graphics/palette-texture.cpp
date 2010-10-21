#include "texture.h"



GLESPaletteTexture::GLESPaletteTexture() :
	GLESTexture(),
	_texture(NULL)
{
}

GLESPaletteTexture::~GLESPaletteTexture() {
	delete[] _texture;
}


void GLESPaletteTexture::allocBuffer(GLuint w, GLuint h)
{
	//net_send("GLESPaletteTexture::allocBuffer(%d,%d)\n",w,h);

	//CHECK_GL_ERROR();
	int bpp = bytesPerPixel();
	_surface.w = w;
	_surface.h = h;
	_surface.bytesPerPixel = bpp;

	if (w <= _texture_width && h <= _texture_height)
		// Already allocated a sufficiently large buffer
		return;

	_texture_width = _surface.w;
	_texture_height = _surface.h;

	net_send("GLESPaletteTexture::allocBufferXXX(%d,%d)\n",_texture_width,_texture_height);
	_surface.pitch = _texture_width * bpp;

	// Texture gets uploaded later (from drawTexture())

	byte* new_buffer = new byte[paletteSize() +
		_texture_width * _texture_height * bytesPerPixel()];
	
	if (_texture)
	{
		memcpy(new_buffer, _texture, paletteSize()); // preserve palette
		delete[] _texture;
	}
	_texture = new_buffer;
	_surface.pixels = _texture + paletteSize();
}

void GLESPaletteTexture::fillBuffer(byte x)
{
	//net_send("GLESPaletteTexture::fillBuffer(%d)-%X\n",&_surface);
	//net_send("GLESPaletteTexture::fillBuffer(%d)-%X\n",_surface.pixels);
	//net_send("GLESPaletteTexture::fillBuffer()0-%d,%d,%d\n",_surface.h , _surface.w , bytesPerPixel());
	assert(_surface.pixels);
	memset(_surface.pixels, x, _surface.pitch * _surface.h);
	setDirty();
}

void GLESPaletteTexture::updateBuffer(GLuint x, GLuint y,
									  GLuint w, GLuint h,
									  const void* buf, int pitch)
{
	_all_dirty = true;

	const byte* src = static_cast<const byte*>(buf);
	byte* dst = static_cast<byte*>(_surface.getBasePtr(x, y));
	do
	{
		memcpy(dst, src, w * bytesPerPixel());
		dst += _surface.pitch;
		src += pitch;
	} while (--h);
}

void GLESPaletteTexture::uploadTexture() const
{
	const size_t texture_size =
		paletteSize() + _texture_width * _texture_height * bytesPerPixel();
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, glType(),
						   _texture_width, _texture_height,
						   0, texture_size, _texture);
	//CHECK_GL_ERROR();
}

void GLESPaletteTexture::drawTexture(GLshort x, GLshort y, GLshort w, GLshort h)
{
	if (_all_dirty)
	{
		glBindTexture(GL_TEXTURE_2D, _texture_name);
		//CHECK_GL_ERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//CHECK_GL_ERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//CHECK_GL_ERROR();
		uploadTexture();
		_all_dirty = false;
	}

	GLESTexture::drawTexture(x, y, w, h);
}
