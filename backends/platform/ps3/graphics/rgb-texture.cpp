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
	_texture_width(0),
	_texture_height(0),
	_all_dirty(true)
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
	_texture=NULL;
}

GLESTexture::~GLESTexture() {
	debug("Destroying texture %u", _texture_name);
	CHECK_GL_ERROR();
	if(_texture_name>0)
		glDeleteTextures(1, &_texture_name);
	CHECK_GL_ERROR();
}

void GLESTexture::reinitGL() {
	net_send("GLESTexture::reinitGL()\n");
	CHECK_GL_ERROR();
	glGenTextures(1, &_texture_name);
	CHECK_GL_ERROR();
	setDirty();
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

	_texture_width = _surface.w;
	_texture_height = _surface.h;

	net_send("GLESTexture::allocBufferXXX(%d,%d,%d)\n",_texture_width,_texture_height,bpp);
	_surface.pitch = _texture_width * bpp;

	if (_texture)
		delete[] _texture;
	net_send("GLESTexture::allocBufferYYY(%d,%d,%d)\n",_texture_width,_texture_height,bpp);

	_texture = new byte[_texture_width * _texture_height * bpp];
	_surface.pixels = _texture;
	net_send("GLESTexture::allocBufferZZZ(%d,%d,%d)\n",_texture_width,_texture_height,bpp);

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
	///net_send("GLESTexture::updateBuffer(%d,%d,%d,%d)\n",x,y,w,h);
	_all_dirty = true;

	const byte* src = static_cast<const byte*>(buf);
	byte* dst = static_cast<byte*>(_surface.getBasePtr(x, y));
	CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	CHECK_GL_ERROR();
	GLuint horig=h;
	do
	{
		memcpy(dst, src, w * bytesPerPixel());
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y+horig-h, w, 1,
			glFormat(), glType(), dst);
		dst += _surface.pitch;
		src += pitch;
	} while (--h);
	CHECK_GL_ERROR();

	glFlush();
	CHECK_GL_ERROR();
	_updatesPerFrame++;
}
void GLES555Texture::updateBuffer(GLuint x, GLuint y, GLuint w, GLuint h,
							   const void* buf, int pitch)
{
	net_send("GLESTexture::updateBuffer(%d,%d,%d,%d)\n",x,y,w,h);
	_all_dirty = true;
	byte bpp=bytesPerPixel();

	const uint16* src = static_cast<const uint16*>(buf);
	uint16* dst = static_cast<uint16*>(_surface.getBasePtr(x, y));
	CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	CHECK_GL_ERROR();
	GLuint horig=h;
	do
	{
		for(GLuint xx=0;xx<w;xx++)
		{
			dst[xx]=(src[xx]<<1)|1;
			//if(x==0 && y==0 && w==16 &&horig==16)
			//	net_send("%x ",src[xx]);
		}
		//if(x==0 && y==0 && w==16 &&horig==16)
		//	net_send("\n");
		//memcpy(dst, src, w * bytesPerPixel());
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y+horig-h, w, 1,
			glFormat(), glType(), dst);
		dst += _surface.pitch/bpp;
		src += pitch/bpp;
	} while (--h);
	CHECK_GL_ERROR();

	glFlush();
	CHECK_GL_ERROR();
	_updatesPerFrame++;
}
/*	//ENTER("updateBuffer(%u, %u, %u, %u, %p, %d, %d)\n", x, y, w, h, buf, pitch, _texture_name);
	CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	CHECK_GL_ERROR();

	setDirtyRect(Common::Rect(x, y, x+w, y+h));

	if (static_cast<int>(w) * bytesPerPixel() == pitch)
	{
		//net_send("GLESTexture::updateBuffer()=AA\n");
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h,
						glFormat(), glType(), buf);

		CHECK_GL_ERROR();
	}
	else
	{
		//net_send("GLESTexture::updateBuffer()=BB\n");
		// GLES removed the ability to specify pitch, so we
		// have to do this row by row.
		const byte* src = static_cast<const byte*>(buf);
		do {
			//net_send("GLESTexture::updateBuffer(linecopy): %d\n",h);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y,
							w, 1, glFormat(), glType(), src);
			CHECK_GL_ERROR();
			++y;
			src += pitch;
		} while (--h);
	}
	glFlush();
	CHECK_GL_ERROR();
}*/

void GLESTexture::fillBuffer(byte x)
{
	net_send("GLESTexture::fillBuffer(%d)\n",x);
	assert(_surface.pixels);
	memset(_surface.pixels, x, _surface.pitch * _surface.h);
	CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	CHECK_GL_ERROR();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _surface.w, _surface.h,
		glFormat(), glType(), _surface.pixels);
	CHECK_GL_ERROR();

	glFlush();
	CHECK_GL_ERROR();
	setDirty();
	_updatesPerFrame++;
}

void GLESTexture::drawTexture(GLshort x, GLshort y, GLshort w, GLshort h)
{
	CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	CHECK_GL_ERROR();
	_drawTexture(x,y,w,h);
}
void GLESTexture::_drawTexture(GLshort x, GLshort y, GLshort w, GLshort h)
{
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

	_all_dirty = false;
	_dirty_rect = Common::Rect();

	if(_updatesPerFrame>2)
		net_send("GLESTexture::_updatesPerFrame(%d)\n",_updatesPerFrame);
	_updatesPerFrame=0;
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
	CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	CHECK_GL_ERROR();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _surface.w, _surface.h,
		glFormat(), glType(), _surface.pixels);
	CHECK_GL_ERROR();
	_isLocked=false;

	glFlush();
	CHECK_GL_ERROR();
	_updatesPerFrame++;
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
	CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	CHECK_GL_ERROR();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _surface.w, _surface.h,
		glFormat(), glType(), _surface.pixels);
	CHECK_GL_ERROR();
	_isLocked=false;

	glFlush();
	CHECK_GL_ERROR();
	_updatesPerFrame++;
}

void GLESTexture::setKeyColor(uint32 color)
{
	//net_send("GLESPaletteTexture::setKeyColor(%d)\n",color);
	//_keycolor=color;
}
