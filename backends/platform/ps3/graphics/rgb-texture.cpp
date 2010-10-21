#include "base/main.h"
#include "graphics/surface.h"
#include "texture.h"

//#include <PSGL/gl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>


#include "common/rect.h"
#include "common/array.h"
#include "common/util.h"
#include "common/tokenizer.h"


void GLESTexture::initGLExtensions()
{
}
GLESTexture::GLESTexture() :
	_texture_width(0),
	_texture_height(0),
	_all_dirty(true)
{
	glGenTextures(1, &_texture_name);
	// This all gets reset later in allocBuffer:
	_surface.w = 0;
	_surface.h = 0;
	_surface.pitch = _texture_width;
	_surface.pixels = NULL;
	_surface.bytesPerPixel = 0;
}

GLESTexture::~GLESTexture() {
	debug("Destroying texture %u", _texture_name);
	glDeleteTextures(1, &_texture_name);
}

void GLESTexture::reinitGL() {
	glGenTextures(1, &_texture_name);
	setDirty();
}

void GLESTexture::allocBuffer(GLuint w, GLuint h)
{
	net_send("GLESTexture::allocBuffer(%d,%d)\n",w,h);
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

	net_send("GLESTexture::allocBufferXXX(%d,%d)\n",_texture_width,_texture_height);
	_surface.pitch = _texture_width * bpp;

	// Allocate room for the texture now, but pixel data gets uploaded
	// later (perhaps with multiple TexSubImage2D operations).
	//CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	//CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//CHECK_GL_ERROR();
	glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat(),
		     _texture_width, _texture_height,
		     0, glFormat(), glType(), NULL);
	//CHECK_GL_ERROR();
}

void GLESTexture::updateBuffer(GLuint x, GLuint y, GLuint w, GLuint h,
							   const void* buf, int pitch) {
	//ENTER("updateBuffer(%u, %u, %u, %u, %p, %d, %d)\n", x, y, w, h, buf, pitch, _texture_name);
	glBindTexture(GL_TEXTURE_2D, _texture_name);

	setDirtyRect(Common::Rect(x, y, x+w, y+h));

	if (static_cast<int>(w) * bytesPerPixel() == pitch)
	{
		//net_send("GLESTexture::updateBuffer()=AA\n");
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h,
						glFormat(), glType(), buf);

		//CHECK_GL_ERROR();
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
			//CHECK_GL_ERROR();
			++y;
			src += pitch;
		} while (--h);
	}
}
void GLESTexture::fillBuffer(byte x) {
	byte *tmpbuf=new byte[_surface.h * _surface.w * bytesPerPixel()];
	memset(tmpbuf, 0, _surface.h * _surface.w * bytesPerPixel());
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _surface.w, _surface.h,
					glFormat(), glType(), tmpbuf);
		//CHECK_GL_ERROR();
	setDirty();
	delete[] tmpbuf;
}

void GLESTexture::drawTexture(GLshort x, GLshort y, GLshort w, GLshort h)
{
	glBindTexture(GL_TEXTURE_2D, _texture_name);

	const GLfloat tex_width = _surface.w/((float)_texture_width);
	const GLfloat tex_height = _surface.h/((float)_texture_height);

	const GLfloat texcoords[] = {
		0, 0,
		tex_width, 0,
		0, tex_height,
		tex_width, tex_height,
	};
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

	const GLfloat vertices[] = {
		x,	 y,
		x+w, y,
		x,	 y+h,
		x+w, y+h,
	};
	glVertexPointer(2, GL_FLOAT, 0, vertices);

	assert(ARRAYSIZE(vertices) == ARRAYSIZE(texcoords));

	//net_send("GLESTexture::drawTexture() - ");
	//net_send("%d, %d, %d, %d, %d, %d\n",tex_width,tex_height,x,y,w,h);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, ARRAYSIZE(vertices)/2);

	_all_dirty = false;
	_dirty_rect = Common::Rect();
}

