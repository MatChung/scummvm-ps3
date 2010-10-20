#include "video.h"
#if 0
#define ENTER(args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, args)
#else
#define ENTER(args...) printf(args)/**/
#endif

#if 1
#define CHECK_GL_ERROR() checkGlError(__FILE__, __LINE__)
static const char* getGlErrStr(GLenum error) {
	switch (error) {
	case GL_NO_ERROR:		   return "GL_NO_ERROR";
	case GL_INVALID_ENUM:	   return "GL_INVALID_ENUM";
	case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
	case GL_STACK_OVERFLOW:	   return "GL_STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW:   return "GL_STACK_UNDERFLOW";
	case GL_OUT_OF_MEMORY:	   return "GL_OUT_OF_MEMORY";
	}

	static char buf[40];
	snprintf(buf, sizeof(buf), "(Unknown GL error code 0x%x)", error);
	return buf;
}
static void checkGlError(const char* file, int line) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		printf("%s:%d: GL error: %s\n", file, line, getGlErrStr(error));
}
#else
#define CHECK_GL_ERROR() do {} while (false)
#endif

static bool npot_supported=true;

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
	//printf("GLESPaletteTexture::allocBuffer(%d,%d)\n",w,h);

	CHECK_GL_ERROR();
	int bpp = bytesPerPixel();
	_surface.w = w;
	_surface.h = h;
	_surface.bytesPerPixel = bpp;

	if (w <= _texture_width && h <= _texture_height)
		// Already allocated a sufficiently large buffer
		return;

	if (npot_supported)
	{
		_texture_width = _surface.w;
		_texture_height = _surface.h;
	}
	else
	{
		//_texture_width = nextHigher2(_surface.w);
		//_texture_height = nextHigher2(_surface.h);
	}
	printf("GLESPaletteTexture::allocBufferXXX(%d,%d)\n",_texture_width,_texture_height);
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
	//printf("GLESPaletteTexture::fillBuffer(%d)-%X\n",&_surface);
	//printf("GLESPaletteTexture::fillBuffer(%d)-%X\n",_surface.pixels);
	//printf("GLESPaletteTexture::fillBuffer()0-%d,%d,%d\n",_surface.h , _surface.w , bytesPerPixel());
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
	CHECK_GL_ERROR();
}

void GLESPaletteTexture::drawTexture(GLshort x, GLshort y, GLshort w, GLshort h)
{
	if (_all_dirty)
	{
		glBindTexture(GL_TEXTURE_2D, _texture_name);
		CHECK_GL_ERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		CHECK_GL_ERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		CHECK_GL_ERROR();
		uploadTexture();
		_all_dirty = false;
	}

	GLESTexture::drawTexture(x, y, w, h);
}
