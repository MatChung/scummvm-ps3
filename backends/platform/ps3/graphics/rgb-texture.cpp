#include "base/main.h"
#include "graphics/surface.h"
#include "texture.h"

#include <PSGL/psgl.h>
//#include <GLES/gl.h>
//#include <GLES/glext.h>


#include "common/rect.h"
#include "common/array.h"
#include "common/util.h"
#include "common/tokenizer.h"

#if 1
#define CHECK_GL_ERROR() checkGlError(__FILE__, __LINE__)
static const char* getGlErrStr(GLenum error) {
	switch (error) {
	case GL_NO_ERROR:		   return "GL_NO_ERROR";
	case GL_INVALID_ENUM:	   return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:	   return "GL_INVALID_VALUE";
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
		net_send("%s:%d: GL error: %s\n", file, line, getGlErrStr(error));
}
#else
#define CHECK_GL_ERROR() do {} while (false)
#endif

void GLESTexture::initGLExtensions()
{
}
GLESTexture::GLESTexture() :
	_texture(NULL),
	_isLocked(false),
	_texture_width(0),
	_texture_height(0),
	_dirty_top(99999),
	_dirty_bottom(0),
	_updatesPerFrame(0)
{
	CHECK_GL_ERROR();
	glGenTextures(1, &_texture_name);
	CHECK_GL_ERROR();
	// This all gets reset later in allocBuffer:
	_surface.w = 0;
	_surface.h = 0;
	_surface.pitch = _texture_width;
	_surface.pixels = NULL;
	_surface.bytesPerPixel = 0;
}

GLESTexture::~GLESTexture()
{
	net_send("GLESTexture::~GLESTexture(%u)\n", _texture_name);
	CHECK_GL_ERROR();
	net_send("    delete GL texture\n");
	if(_texture_name>0)
		glDeleteTextures(1, &_texture_name);
	net_send("    delete mem texture\n");
	if(_texture)
		delete []_texture;
	CHECK_GL_ERROR();
	net_send("    destroyed\n");
}

void GLESTexture::reinitGL()
{
	net_send("GLESTexture::reinitGL()\n");
	CHECK_GL_ERROR();
	//glGenTextures(1, &_texture_name);
	CHECK_GL_ERROR();
}

void GLESTexture::allocBuffer(GLuint w, GLuint h)
{
	//net_send("GLESTexture::allocBuffer(%d,%d)\n",w,h);
	CHECK_GL_ERROR();
	int bpp = bytesPerPixel();
	_surface.w = w;
	_surface.h = h;
	_surface.bytesPerPixel = bpp;

	if (w <= _texture_width && h <= _texture_height)
		// Already allocated a sufficiently large buffer
		return;

	_texture_width = w;
	_texture_height = h;

	net_send("GLESTexture::allocBuffer(%d,%d)\n",w,h);

	if (_texture)
	{
		net_send("    delete old texture\n");
		delete[] _texture;
	}

	net_send("    alloc new texture\n");
	_texture = new byte[_texture_width * _texture_height * bpp];
	_surface.pixels = _texture;
	_surface.pitch = _texture_width * bpp;

	net_send("    gl init\n");

	// Allocate room for the texture now, but pixel data gets uploaded
	// later (perhaps with multiple TexSubImage2D operations).
	CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	CHECK_GL_ERROR();
	glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat(),
		     _texture_width, _texture_height,
		     0, glFormat(), glType(), NULL);
	CHECK_GL_ERROR();
	glFlush();
	CHECK_GL_ERROR();
	fillBuffer(0);
}

void GLESTexture::updateBuffer(GLuint x, GLuint y, GLuint w, GLuint h,
							   const void* buf, int pitch)
{
	net_send("GLESTexture::updateBuffer(%d,%d,%d,%d)\n",x,y,w,h);

	const byte* src = static_cast<const byte*>(buf);
	byte* dst = static_cast<byte*>(_surface.getBasePtr(x, y));

	_dirty_top=MIN(y,_dirty_top);
	_dirty_bottom=MAX(y+h,_dirty_bottom);

	do
	{
		memcpy(dst, src, w * bytesPerPixel());
		dst += _surface.pitch;
		src += pitch;
	} while (--h);
}
void GLES555Texture::updateBuffer(GLuint x, GLuint y, GLuint w, GLuint h,
							   const void* buf, int pitch)
{
	//net_send("GLESTexture::updateBuffer(%d,%d,%d,%d)\n",x,y,w,h);

	_dirty_top=MIN(y,_dirty_top);
	_dirty_bottom=MAX(y+h,_dirty_bottom);

	const uint16* src = static_cast<const uint16*>(buf);
	uint16* dst = static_cast<uint16*>(_surface.getBasePtr(x, y));

	do
	{
		for(GLuint xx=0;xx<w;xx++)
		{
			dst[xx]=(src[xx]<<1)|1;
		}
		dst += _surface.pitch/2;// 2 because uint16
		src += pitch/2;// 2 because uint16
	} while (--h);
}


void GLESTexture::fillBuffer(byte x)
{
	net_send("GLESTexture::fillBuffer(%d)\n",x);

	_dirty_top=0;
	_dirty_bottom=_surface.h;

	memset(_surface.pixels, x, _surface.pitch * _surface.h);
}

void GLESTexture::drawTexture(GLshort x, GLshort y, GLshort w, GLshort h)
{
	net_send("GLESTexture::drawTexture %d (%d,%d,%d,%d)\n",_texture_name,x,y,w,h);
	CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	CHECK_GL_ERROR();

	if(_dirty_top<_dirty_bottom)
	{
		net_send("    glTexSubImage2D(%d,%d,%d,%d)\n",0,_dirty_top,_texture_width, _dirty_bottom-_dirty_top);
		//glTexSubImage2D(GL_TEXTURE_2D, 0,
		//	0, _dirty_top, // x,y
		//	_texture_width, _dirty_bottom-_dirty_top,//w,h
		//	glFormat(), glType(), _surface.getBasePtr(0,_dirty_top));
		glTexSubImage2D(GL_TEXTURE_2D, 0,
			0, 0, // x,y
			_texture_width, _texture_height,//w,h
			glFormat(), glType(), _surface.getBasePtr(0,0));
		CHECK_GL_ERROR();
		glFlush();
		CHECK_GL_ERROR();

		_dirty_bottom=0;
		_dirty_top=99999;
	}

	_drawTexture(x,y,w,h);
}
void GLESTexture::_drawTexture(GLshort x, GLshort y, GLshort w, GLshort h)
{
	net_send("GLESTexture::drawTexture(%d,%d,%d,%d)\n",x,y,w,h);
	const GLfloat tex_width = _surface.w/((float)_texture_width);
	const GLfloat tex_height = _surface.h/((float)_texture_height);

	const GLfloat texcoords[] = {
		0, 0,
		tex_width, 0,
		0, tex_height,
		tex_width, tex_height,
	};
	CHECK_GL_ERROR();
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
	CHECK_GL_ERROR();

	const GLfloat vertices[] = {
		x,	 y,
		x+w, y,
		x,	 y+h,
		x+w, y+h,
	};
	CHECK_GL_ERROR();
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	CHECK_GL_ERROR();

	assert(ARRAYSIZE(vertices) == ARRAYSIZE(texcoords));

	//net_send("GLESTexture::drawTexture() - ");
	//net_send("%d, %d, %d, %d, %d, %d\n",tex_width,tex_height,x,y,w,h);

	CHECK_GL_ERROR();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, ARRAYSIZE(vertices)/2);
	CHECK_GL_ERROR();
}

Graphics::Surface* GLESTexture::lock()
{
	net_send("GLESTexture::lock()\n");
	_isLocked=true;
	return &_surface;
}

void GLESTexture::unlock()
{
	net_send("GLESTexture::unlock()\n");
	_isLocked=false;

	_dirty_top=0;
	_dirty_bottom=_surface.h;
}

Graphics::Surface *GLES555Texture::lock()
{
	//net_send("GLES555Texture::lock()\n");
	uint16* dst = static_cast<uint16*>(_surface.pixels);

	for(uint16 y=0;y<_surface.h;y++)
	{
		for(uint16 x=0;x<_surface.w;x++)
		{
			dst[y*_surface.pitch/2+x]=(dst[y*_surface.pitch/2+x]>>1);
		}
	}
	return &_surface;
}

void GLES555Texture::unlock()
{
	//net_send("GLES555Texture::unlock()\n");
	uint16* dst = static_cast<uint16*>(_surface.pixels);
	for(uint16 y=0;y<_surface.h;y++)
	{
		for(uint16 x=0;x<_surface.w;x++)
		{
			dst[y*_surface.pitch/2+x]=(dst[y*_surface.pitch/2+x]<<1)|1;
		}
	}
	_isLocked=false;

	_dirty_top=0;
	_dirty_bottom=_surface.h;
}

void GLESTexture::setKeyColor(uint32 color)
{
	net_send("GLESTexture::setKeyColor(%d)\n",color);
	net_send("    ERROR\n");
	net_send("    ERROR\n");
	net_send("    ERROR\n");
	net_send("    ERROR\n");
	net_send("    ERROR\n");
}

void GLESTexture::grabPalette(byte *colors, uint start, uint num)
{
	net_send("GLESTexture::grabPalette(%d,%d)\n",start,num);
	net_send("    ERROR\n");
	net_send("    ERROR\n");
	net_send("    ERROR\n");
	net_send("    ERROR\n");
	net_send("    ERROR\n");
}

void GLESTexture::updatePalette(const byte *colors, uint start, uint num)
{
	net_send("GLESTexture::updatePalette(%d,%d)\n",start,num);
	net_send("    ERROR\n");
	net_send("    ERROR\n");
	net_send("    ERROR\n");
	net_send("    ERROR\n");
	net_send("    ERROR\n");
}
