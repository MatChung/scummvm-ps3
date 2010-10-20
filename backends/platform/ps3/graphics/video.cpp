/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/backends/platform/null/null.cpp $
 * $Id: null.cpp 34912 2008-11-06 15:02:50Z fingolfin $
 *
 */

#include "base/main.h"
#include "graphics/surface.h"
#include "video.h"

//#include <PSGL/gl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>


#include "common/rect.h"
#include "common/array.h"
#include "common/util.h"
#include "common/tokenizer.h"


#undef LOG_TAG
#define LOG_TAG "ScummVM-video"

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

void GLESTexture::initGLExtensions() {
/*	const char* ext_string =
		reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG,
				"Extensions: %s", ext_string);
	Common::StringTokenizer tokenizer(ext_string, " ");
	while (!tokenizer.empty()) {
		Common::String token = tokenizer.nextToken();
		if (token == "GL_ARB_texture_non_power_of_two")
			npot_supported = true;
#ifdef GL_OES_draw_texture
		if (token == "GL_OES_draw_texture")
			draw_tex_supported = true;
#endif
	}*/
}
static bool npot_supported=true;
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

GLuint nextHigher2(uint16 i)
{
	GLuint ret=1;

	while(ret<i)
		ret<<=2;

	return ret;
}

void GLESTexture::allocBuffer(GLuint w, GLuint h)
{
	printf("GLESTexture::allocBuffer(%d,%d)\n",w,h);
	CHECK_GL_ERROR();
	int bpp = bytesPerPixel();
	_surface.w = w;
	_surface.h = h;
	_surface.bytesPerPixel = bpp;

	if (w <= _texture_width && h <= _texture_height)
		// Already allocated a sufficiently large buffer
		return;

	if (npot_supported) {
		_texture_width = _surface.w;
		_texture_height = _surface.h;
	} else {
		_texture_width = nextHigher2(_surface.w);
		_texture_height = nextHigher2(_surface.h);
	}
	printf("GLESTexture::allocBufferXXX(%d,%d)\n",_texture_width,_texture_height);
	_surface.pitch = _texture_width * bpp;

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
}

void GLESTexture::updateBuffer(GLuint x, GLuint y, GLuint w, GLuint h,
							   const void* buf, int pitch) {
	ENTER("updateBuffer(%u, %u, %u, %u, %p, %d, %d)\n", x, y, w, h, buf, pitch, _texture_name);
	glBindTexture(GL_TEXTURE_2D, _texture_name);

	setDirtyRect(Common::Rect(x, y, x+w, y+h));

	if (static_cast<int>(w) * bytesPerPixel() == pitch)
	{
		printf("GLESTexture::updateBuffer()=AA\n");
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h,
						glFormat(), glType(), buf);

		CHECK_GL_ERROR();
	}
	else
	{
		printf("GLESTexture::updateBuffer()=BB\n");
		// GLES removed the ability to specify pitch, so we
		// have to do this row by row.
		const byte* src = static_cast<const byte*>(buf);
		do {
			//printf("GLESTexture::updateBuffer(linecopy): %d\n",h);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y,
							w, 1, glFormat(), glType(), src);
			CHECK_GL_ERROR();
			++y;
			src += pitch;
		} while (--h);
	}
}

static inline GLfixed xdiv(int numerator, int denominator) {
	assert(numerator < (1<<16));
	return (numerator << 16) / denominator;
}

void GLESTexture::fillBuffer(byte x) {
	printf("GLESTexture::fillBuffer(%d)-%X\n",x,this);
	printf("GLESTexture::fillBuffer()-%X\n",&_surface);
	printf("GLESTexture::fillBuffer()-%X\n",_surface.pixels);
	printf("GLESTexture::fillBuffer()0-%d,%d,%d\n",_surface.h , _surface.w , bytesPerPixel());
	byte *tmpbuf=new byte[_surface.h * _surface.w * bytesPerPixel()];
	printf("GLESTexture::fillBuffer()1\n");
	memset(tmpbuf, 0, _surface.h * _surface.w * bytesPerPixel());
	printf("GLESTexture::fillBuffer()2-%d\n",_texture_name);
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	printf("GLESTexture::fillBuffer()3\n");
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _surface.w, _surface.h,
					glFormat(), glType(), tmpbuf);
		CHECK_GL_ERROR();
	printf("GLESTexture::fillBuffer()4\n");
	setDirty();
	printf("GLESTexture::fillBuffer()5\n");
	delete[] tmpbuf;
}

void GLESTexture::drawTexture(GLshort x, GLshort y, GLshort w, GLshort h) {
	glBindTexture(GL_TEXTURE_2D, _texture_name);

#ifdef GL_OES_draw_texture
	// Great extension, but only works under specific conditions.
	// Still a work-in-progress - disabled for now.
	if (false && draw_tex_supported && paletteSize() == 0) {
		//glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		const GLint crop[4] = {0, _surface.h, _surface.w, -_surface.h};
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop);
		glDrawTexiOES(x, y, 0, w, h);
	} else
#endif
	{
		const GLfixed tex_width = xdiv(_surface.w, _texture_width);
		const GLfixed tex_height = xdiv(_surface.h, _texture_height);
		const GLfixed texcoords[] = {
			0, 0,
			tex_width, 0,
			0, tex_height,
			tex_width, tex_height,
		};
		glTexCoordPointer(2, GL_FIXED, 0, texcoords);

		const GLshort vertices[] = {
			x,	 y,
			x+w, y,
			x,	 y+h,
			x+w, y+h,
		};
		glVertexPointer(2, GL_SHORT, 0, vertices);

		assert(ARRAYSIZE(vertices) == ARRAYSIZE(texcoords));

		//printf("GLESTexture::drawTexture() - ");
		//printf("%d, %d, %d, %d, %d, %d\n",tex_width,tex_height,x,y,w,h);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, ARRAYSIZE(vertices)/2);
	}

	_all_dirty = false;
	_dirty_rect = Common::Rect();
}

