#include <PSGL/psgl.h>
#include <Cg/NV/cg.h>

#include "graphics/surface.h"
#include "graphics/pixelformat.h"

#include "common/rect.h"
#include "common/array.h"


class GLESTexture
{
public:
	static void initGLExtensions();

	GLESTexture();
	virtual ~GLESTexture();
	virtual void reinitGL();
	virtual void allocBuffer(GLuint width, GLuint height);
	const Graphics::Surface* surface_const() const { return &_surface; }
	GLuint width() const { return _surface.w; }
	GLuint height() const { return _surface.h; }
	GLuint texture_name() const { return _texture_name; }
	bool dirty() const { return _all_dirty || !_dirty_rect.isEmpty(); }
	virtual void updateBuffer(GLuint x, GLuint y, GLuint width, GLuint height, const void* buf, int pitch);
	virtual void fillBuffer(byte x);
	virtual void drawTexture() {
		drawTexture(0, 0, _surface.w, _surface.h);
	}
	virtual void drawTexture(GLshort x, GLshort y, GLshort w, GLshort h);
	virtual void _drawTexture(GLshort x, GLshort y, GLshort w, GLshort h);
	virtual Graphics::PixelFormat getFormat() = 0;
	virtual Graphics::Surface* lock();
	virtual void unlock();
	void setKeyColor(uint32 color);

protected:
	virtual byte bytesPerPixel() const = 0;
	virtual GLenum glFormat() const = 0;
	virtual GLenum glType() const = 0;
	virtual GLenum glInternalFormat() const = 0;
	//virtual size_t paletteSize() const { return 0; };
	void setDirty() {
		_all_dirty = true;
		_dirty_rect = Common::Rect();
	}
	void setDirtyRect(const Common::Rect& r) {
		if (!_all_dirty) {
			if (_dirty_rect.isEmpty())
				_dirty_rect = r;
			else
				_dirty_rect.extend(r);
		}
	}
	GLuint _texture_name;
	byte* _texture;
	Graphics::Surface _surface;
	bool _isLocked;
	GLuint _texture_width;
	GLuint _texture_height;
	bool _all_dirty;
	Common::Rect _dirty_rect;  // Covers dirty area
	int32 _keycolor;
};


// RGBA4444 texture
class GLES4444Texture : public GLESTexture {
protected:
	virtual byte bytesPerPixel() const { return 2; }
	virtual GLenum glInternalFormat() const { return GL_RGBA8; }
	virtual GLenum glFormat() const { return GL_RGBA; }
	virtual GLenum glType() const { return GL_UNSIGNED_SHORT_4_4_4_4; }
	virtual Graphics::PixelFormat getFormat() { return Graphics::PixelFormat(2, 4, 4, 4, 4, 12, 8, 4, 0);};
};

// RGBA4444 texture
class GLES8888Texture : public GLESTexture {
protected:
	virtual byte bytesPerPixel() const { return 4; }
	virtual GLenum glInternalFormat() const { return GL_RGBA8; }
	virtual GLenum glFormat() const { return GL_RGBA; }
	virtual GLenum glType() const { return GL_UNSIGNED_BYTE; }
	virtual Graphics::PixelFormat getFormat() { return Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0);};
};

// RGB565 texture
class GLES565Texture : public GLESTexture {
protected:
	virtual byte bytesPerPixel() const { return 2; }
	virtual GLenum glInternalFormat() const { return GL_RGB; }
	virtual GLenum glFormat() const { return GL_RGB; }
	virtual GLenum glType() const { return GL_UNSIGNED_SHORT_5_6_5; }
	virtual Graphics::PixelFormat getFormat() { return Graphics::PixelFormat(2, 5, 6, 5, 0, 11, 5, 0, 0);};
};

// RGB565 texture
class GLES555Texture : public GLESTexture {
protected:
	virtual byte bytesPerPixel() const { return 2; }
	virtual GLenum glInternalFormat() const { return GL_RGBA; }
	virtual GLenum glFormat() const { return GL_RGBA; }
	virtual GLenum glType() const { return GL_UNSIGNED_SHORT_5_5_5_1; }
	virtual Graphics::PixelFormat getFormat() { return Graphics::PixelFormat(2, 5, 5, 5, 0, 10, 5, 0, 0);};
	virtual void updateBuffer(GLuint x, GLuint y, GLuint width, GLuint height, const void* buf, int pitch);
	virtual Graphics::Surface* lock();
	virtual void unlock();
};

// RGB565 texture
class GLES5551Texture : public GLESTexture {
protected:
	virtual byte bytesPerPixel() const { return 2; }
	virtual GLenum glInternalFormat() const { return GL_RGBA; }
	virtual GLenum glFormat() const { return GL_RGBA; }
	virtual GLenum glType() const { return GL_UNSIGNED_SHORT_5_5_5_1; }
	virtual Graphics::PixelFormat getFormat() { return Graphics::PixelFormat(2, 5, 5, 5, 1, 11, 6, 1, 0);};
};

// RGB888 256-entry paletted texture
class GLESPaletteTexture : public GLESTexture
{
public:
	GLESPaletteTexture();
	virtual ~GLESPaletteTexture();
	virtual void allocBuffer(GLuint width, GLuint height);
	virtual void updateBuffer(GLuint x, GLuint y, GLuint width, GLuint height,
							  const void* buf, int pitch);

	void grabPalette(byte *colors, uint start, uint num);
	void updatePalette(const byte *colors, uint start, uint num);
	void setKeyColor(uint32 color);

	virtual void drawTexture()
	{
		drawTexture(0, 0, _surface.w, _surface.h);
	}
	virtual void drawTexture(GLshort x, GLshort y, GLshort w, GLshort h);
	virtual void fillBuffer(byte x);
	virtual Graphics::PixelFormat getFormat() { return Graphics::PixelFormat::createFormatCLUT8();};

protected:
	void initCG();
	virtual byte bytesPerPixel() const { return 1; }
	virtual GLenum glInternalFormat() const { return GL_LUMINANCE8; }
	virtual GLenum glFormat() const { return GL_LUMINANCE; }
	virtual GLenum glType() const { return GL_UNSIGNED_BYTE; }
	virtual GLenum glPaletteInternalFormat() const { return GL_RGBA8; }
	virtual GLenum glPaletteFormat() const { return GL_RGBA; }
	virtual GLenum glPaletteType() const { return GL_UNSIGNED_BYTE; }
	//virtual size_t paletteSize() const { return 256 * 4; };
	uint32* _palette;
	GLuint _palette_name;
	
	CGcontext ctx;
	CGprogram vprog;
	CGprogram fprog;
	CGparameter _texture_param;
	CGparameter _palette_param;
	CGparameter _mvp_param;
};
