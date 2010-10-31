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
_updatesPerFrame(0),
_palette_dirty(0)
{
	CHECK_GL_ERROR();
	glGenTextures(1, &_texture_name);
	CHECK_GL_ERROR();
	glGenTextures(1, &_palette_name);
	CHECK_GL_ERROR();

	_palette=new uint32[256];
	memset(_palette,0,sizeof(uint32)*256);

	glBindTexture(GL_TEXTURE_2D, _palette_name);
	CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	CHECK_GL_ERROR();
	glTexImage1D(0,glPaletteInternalFormat(),256,0,glPaletteFormat(),glPaletteType(),_palette);
	CHECK_GL_ERROR();
	glFlush();
	CHECK_GL_ERROR();

	initCG();
	CHECK_GL_ERROR();

	// This all gets reset later in allocBuffer:
	_surface.w = 0;
	_surface.h = 0;
	_surface.pitch = 0;
	_surface.pixels = NULL;
	_surface.bytesPerPixel = 0;

	_pixelFormat=Graphics::PixelFormat::createFormatCLUT8();
	_internalFormat=getFormatFromPixelFormat();

	_debug=false;

	allDirty();
}

GLESTexture::~GLESTexture()
{
	net_send("GLESTexture::~GLESTexture(%u)\n", _texture_name);
	CHECK_GL_ERROR();
	net_send("    delete GL texture\n");
	if(_texture_name>0)
		glDeleteTextures(1, &_texture_name);
	net_send("    delete GL palette\n");
	if(_palette_name>0)
		glDeleteTextures(1, &_palette_name);
	net_send("    delete mem texture\n");
	if(_texture)
		delete []_texture;
	net_send("    delete mem palette\n");
	if(_palette)
		delete []_palette;
	CHECK_GL_ERROR();
	net_send("    destroyed\n");
}

void GLESTexture::reinitGL()
{
	net_send("GLESTexture::reinitGL()\n");
	CHECK_GL_ERROR();
	_texture_width = 0;
	_texture_height = 0;
	CHECK_GL_ERROR();
}

void GLESTexture::setFormat(Graphics::PixelFormat &format)
{
	if(format!=_pixelFormat)
	{
		_texture_width = 0;
		_texture_height = 0;
		_surface.w=0;
		_surface.h=0;
		_surface.bytesPerPixel=0;
		_pixelFormat=format;
		_internalFormat=getFormatFromPixelFormat();
		//allocBuffer(_surface.w,_surface.h);
	}
}

void GLESTexture::allocBuffer(GLuint w, GLuint h)
{
	//net_send("GLESTexture::allocBuffer(%d,%d)\n",w,h);
	CHECK_GL_ERROR();
	_surface.w = w;
	_surface.h = h;
	_surface.bytesPerPixel = _pixelFormat.bytesPerPixel;

	if (w == _texture_width && h == _texture_height)
		// Already allocated a sufficiently large buffer
		return;


	_texture_width = (MAX(w,_texture_width));
	_texture_height = (MAX(h,_texture_height));

	net_send("GLESTexture::allocBuffer(%d,%d,%d)\n",w,h,_surface.bytesPerPixel);

	if (_texture)
	{
		net_send("    delete old texture\n");
		delete[] _texture;
	}

	net_send("    alloc new texture\n");
	_texture = new byte[_texture_width * _texture_height * _surface.bytesPerPixel];
	_surface.pixels = _texture;
	_surface.pitch = _texture_width * _surface.bytesPerPixel;

	net_send("    gl init\n");

	// Allocate room for the texture now, but pixel data gets uploaded
	// later (perhaps with multiple TexSubImage2D operations).
	CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	CHECK_GL_ERROR();
	if(_current_filter==0 || _surface.bytesPerPixel==1)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

void GLESTexture::setBuffer(GLuint w, GLuint h, const void* buf)
{
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	if(_texture_width==w && _texture_height==h)
	{
		_surface.w = w;
		_surface.h = h;
		_surface.bytesPerPixel = _pixelFormat.bytesPerPixel;
		_surface.pitch = _texture_width * _surface.bytesPerPixel;
		memcpy(_texture,buf,_texture_width * _texture_height * _surface.bytesPerPixel);
		glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat(),
			_texture_width, _texture_height,
			0, glFormat(), glType(), _texture);
	}
	else
	{
		_texture_width=w;
		_texture_height=h;
		_surface.w = w;
		_surface.h = h;
		_surface.bytesPerPixel = _pixelFormat.bytesPerPixel;
		_surface.pitch = _texture_width * _surface.bytesPerPixel;

		if (_texture)
		{
			delete[] _texture;
		}

		_texture = new byte[_texture_width * _texture_height * _surface.bytesPerPixel];
		_surface.pixels = _texture;

		memcpy(_texture,buf,_texture_width * _texture_height * _surface.bytesPerPixel);

		glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat(),
			_texture_width, _texture_height,
			0, glFormat(), glType(), _texture);
	}

	clearDirty();
}

void GLESTexture::updateBuffer(GLuint x, GLuint y, GLuint w, GLuint h,
							   const void* buf, int pitch)
{
	if(_debug)
		net_send("GLESTexture::updateBuffer(%d,%d,%d,%d,%d)\n",x,y,w,h,pitch);

	x=MIN(x,((GLuint)_surface.w));
	y=MIN(y,((GLuint)_surface.h));
	w=MIN(w,((GLuint)_surface.w-x));
	h=MIN(h,((GLuint)_surface.h-y));
	if(_debug)
		net_send("                         (%d,%d,%d,%d,%d)\n",x,y,w,h,pitch);
	if(_debug)
		net_send("                         (%d,%d,%d,%d,%d)\n",_texture_width,_texture_height,_surface.w,_surface.h,_surface.pitch);

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
				for(GLuint xx=0;xx<w;xx++)
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
				for(GLuint xx=0;xx<w;xx++)
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
				for(GLuint xx=0;xx<w;xx++)
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
		if(_debug)
			net_send("    palette\n");
		{
			const uint8* src = static_cast<const uint8*>(buf);
			uint8* dst = &_texture[x+y*_texture_width];

			do
			{
				dst = &_texture[x+y*_texture_width];
				for(GLuint xx=0;xx<w;xx++)
				{
					dst[xx]=src[xx];
				}
				//memcpy(dst,src,w);
				dst += _texture_width;
				src += pitch;
				y++;
			} while (--h);
		}
		break;
	}
}

void GLESTexture::fillBuffer(byte x)
{
	if(_debug)
		net_send("GLESTexture::fillBuffer(%d)\n",x);

	allDirty();

	memset(_surface.pixels, x, _surface.pitch * _surface.h);
}

void GLESTexture::drawTexture(GLshort x, GLshort y, GLshort w, GLshort h)
{
	//net_send("GLESTexture::drawTexture %d (%d,%d,%d,%d)\n",_texture_name,x,y,w,h);
	CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	CHECK_GL_ERROR();

	if(_dirty_top<_dirty_bottom)
	{
		if(_dirty_bottom>_surface.h)
			_dirty_bottom=_surface.h;


		if(_debug)
			net_send("    glTexSubImage2D(%d,%d,%d,%d)\n",0,_dirty_top,_texture_width, _dirty_bottom-_dirty_top);
		glTexSubImage2D(GL_TEXTURE_2D, 0,
			0, _dirty_top, // x,y
			_texture_width, _dirty_bottom-_dirty_top,//w,h
			glFormat(), glType(), _surface.getBasePtr(0,_dirty_top));
		CHECK_GL_ERROR();
		glFlush();
		CHECK_GL_ERROR();

		clearDirty();
	}

	if(_palette_dirty)
	{
		//net_send("    glTexSubImage2D(%d,%d,%d,%d) pal\n",0,0,256, 1);
		if(_keycolor<256)
			_palette[_keycolor]=0;
		//net_send("    1\n",0,0,256, 1);
		CHECK_GL_ERROR();
		//net_send("    2\n",0,0,256, 1);
		glBindTexture(GL_TEXTURE_2D, _palette_name);
		//net_send("    3\n",0,0,256, 1);
		CHECK_GL_ERROR();
		//net_send("    4\n",0,0,256, 1);
		glTexSubImage1D(0,0,256,glPaletteFormat(),glPaletteType(),_palette);
		//net_send("    5\n",0,0,256, 1);
		CHECK_GL_ERROR();
		//net_send("    6\n",0,0,256, 1);
		glFlush();
		//net_send("    7\n",0,0,256, 1);
		CHECK_GL_ERROR();
		//net_send("    8\n",0,0,256, 1);
		_palette_dirty=false;
		//net_send("    9\n",0,0,256, 1);
	}

	//net_send("    drawing...\n");
	switch(_internalFormat)
	{
	case FORMAT_PALETTE:
		{
			CHECK_GL_ERROR();

			//CG stuff enable
			cgGLEnableProfile(CG_PROFILE_SCE_VP_RSX);
			cgGLBindProgram(_vprog);
			cgGLSetStateMatrixParameter(_mvp_param, CG_GL_MODELVIEW_PROJECTION_MATRIX,
				CG_GL_MATRIX_IDENTITY);
			cgGLEnableProfile(CG_PROFILE_SCE_FP_RSX);
			cgGLBindProgram(_fprog);
			cgGLSetTextureParameter(_texture_param,_texture_name);
			cgGLEnableTextureParameter(_texture_param);
			cgGLSetTextureParameter(_palette_param,_palette_name);
			cgGLEnableTextureParameter(_palette_param);


			CHECK_GL_ERROR();
			//draw
			GLESTexture::_drawTexture(x, y, w*1, h);
			CHECK_GL_ERROR();

			//CG stuff disable
			cgGLDisableTextureParameter(_texture_param);
			cgGLDisableTextureParameter(_palette_param);
			cgGLDisableProfile(CG_PROFILE_SCE_VP_RSX);
			cgGLDisableProfile(CG_PROFILE_SCE_FP_RSX);
			CHECK_GL_ERROR();
			//CG stuff disable finished
		}
		break;
	default:
		_drawTexture(x,y,w,h);
		break;
	}
	//net_send("    finished\n");
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

	if(_debug)
	{
		net_send("GLESTexture::drawTexture() - ");
		net_send("    %f, %f, %d, %d, %d, %d\n",tex_width,tex_height,x,y,w,h);
		net_send("    %d, %d, %d, %d\n",_surface.w,_texture_width,_surface.h,_texture_height);
	}

	CHECK_GL_ERROR();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, ARRAYSIZE(vertices)/2);
	CHECK_GL_ERROR();
}

void GLESTexture::setFilter(int filter)
{
	if(_current_filter==filter)
		return;

	_current_filter=filter;

	glBindTexture(GL_TEXTURE_2D, _texture_name);
	if(filter==0)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

Graphics::Surface* GLESTexture::lock()
{
	if(_debug)
		net_send("GLESTexture::lock()\n");
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

void GLESTexture::unlock()
{
	if(_debug)
		net_send("GLESTexture::unlock()\n");
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
/*
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
}*/

void GLESTexture::setKeyColor(uint32 color)
{
	if(_debug)
		net_send("GLESTexture::setKeyColor(%d)\n",color);
	_keycolor=color;
	_palette_dirty=true;
}

void GLESTexture::grabPalette(byte *colors, uint start, uint num)
{
	if(_debug)
		net_send("GLESTexture::grabPalette(%d,%d)\n",start,num);

	uint32* dst = (uint32*)(colors);
	for(uint i=start;i<start+num;i++)
	{
		*dst=_palette[i];
		dst++;
	}
}

void GLESTexture::updatePalette(const byte *colors, uint start, uint num)
{
	if(_debug)
		net_send("GLESTexture::updatePalette(%d,%d)\n",start,num);
	const uint32* src = (uint32*)(colors);
	for(uint i=start;i<start+num;i++)
	{
		_palette[i]=(*src)|0xFF;
		src++;
	}

	_palette_dirty=true;
}

GLESTexture::GLESTextureFormat GLESTexture::getFormatFromPixelFormat()
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

	net_send("GLESTexture::getFormatFromPixelFormat: UNKNOWN FORMAT\n");
	net_send("  bytesPerPixel: %d\n",_pixelFormat.bytesPerPixel);
	net_send("  rBits:         %d\n",_pixelFormat.rBits());
	net_send("  gBits:         %d\n",_pixelFormat.gBits());
	net_send("  bBits:         %d\n",_pixelFormat.bBits());
	net_send("  aBits:         %d\n",_pixelFormat.aBits());
	net_send("  rShift:        %d\n",_pixelFormat.rShift);
	net_send("  gShift:        %d\n",_pixelFormat.gShift);
	net_send("  bShift:        %d\n",_pixelFormat.bShift);
	net_send("  aShift:        %d\n",_pixelFormat.aShift);
	return FORMAT_8888;
}

GLenum GLESTexture::glFormat() const
{
	switch(_internalFormat)
	{
	case FORMAT_PALETTE:
		return GL_LUMINANCE;
	case FORMAT_4444:
		return GL_RGBA;
	case FORMAT_565:
		return GL_RGB;
	case FORMAT_555:
		return GL_RGBA;
	case FORMAT_5551:
		return GL_RGBA;
	case FORMAT_8888:
		return GL_RGBA;
	}
	return GL_RGBA;
}

GLenum GLESTexture::glType() const
{
	switch(_internalFormat)
	{
	case FORMAT_PALETTE:
		return GL_UNSIGNED_BYTE;
	case FORMAT_4444:
		return GL_UNSIGNED_SHORT_4_4_4_4;
	case FORMAT_565:
		return GL_UNSIGNED_SHORT_5_6_5;
	case FORMAT_555:
		return GL_UNSIGNED_SHORT_5_5_5_1;
	case FORMAT_5551:
		return GL_UNSIGNED_SHORT_5_5_5_1;
	case FORMAT_8888:
		return GL_RGBA;
	}
	return GL_RGBA;
}

GLenum GLESTexture::glInternalFormat()
{
	switch(_internalFormat)
	{
	case FORMAT_PALETTE:
		return GL_LUMINANCE8;
	case FORMAT_4444:
		return GL_RGBA8;
	case FORMAT_565:
		return GL_RGB;
	case FORMAT_555:
		return GL_RGBA8;
	case FORMAT_5551:
		return GL_RGBA8;
	case FORMAT_8888:
		return GL_RGBA8;
	}
	return GL_RGBA8;
}

void GLESTexture::glTexImage1D(GLint level_, GLint internalformat_, GLsizei width_, GLint border_, GLenum format_, GLenum type_, const GLvoid *pixels_)
{
	glTexImage2D(GL_TEXTURE_2D, level_, internalformat_, width_, 1, border_, format_, type_, pixels_);
}

void GLESTexture::glTexSubImage1D(GLint level_, GLint xoffset_, GLsizei width_, GLenum format_, GLenum type_, const GLvoid *pixels_)
{
	glTexSubImage2D(GL_TEXTURE_2D,
		level_,
		xoffset_,
		0,
		width_,
		1,
		format_,
		type_,
		pixels_);
}

static const char *cg_vert="void main (float4 position : POSITION,     // Local-space position\n\
						   float2 diffuseTexCoord : TEXCOORD0,    // Diffuse texture coordinate\n\
						   \n\
						   uniform float4x4 modelViewProj,           // Local-to-clip matrix\n\
						   \n\
						   out float4 ePosition : POSITION,      // Clip-space position\n\
						   out float2 oDiffuseTexCoord : TEXCOORD0)     // Diffuse texture coordinate\n\
						   {\n\
						   \n\
						   // Compute the clip-space position\n\
						   ePosition   = mul (modelViewProj, position);  \n\
						   \n\
						   // Transfer data as-is to the fragment shader\n\
						   oDiffuseTexCoord   = diffuseTexCoord;\n\
						   }\n\
						   ";
static const char *cg_frag="void main (float2 diffuseTexCoord : TEXCOORD0,	// Interpolated texture coordinate from vertex shader\n\
						   \n\
						   uniform sampler2D palette, // Diffuse texture map\n\
						   uniform sampler2D index, // Diffuse texture map\n\
						   \n\
						   out float4 oColor      : COLOR)      // Final fragment color\n\
						   {\n\
						   float2 colorIndex = tex2D (index, diffuseTexCoord).xx;\n\
						   colorIndex.y=0;\n\
						   colorIndex.x*=255.0;\n\
						   colorIndex.x+=0.5;\n\
						   colorIndex.x/=256.0;\n\
						   oColor = tex2D (palette, colorIndex);\n\
						   \n\
						   //oColor = color;\n\
						   }\n\
						   ";

void GLESTexture::initCG()
{
	net_send("GLESPaletteTexture::initCG(ctx): ");
	_ctx = cgCreateContext();
	net_send("%X\n",_ctx);


	net_send("GLESPaletteTexture::initCG(vprog): ");
	_vprog = cgCreateProgram(_ctx,
		CG_SOURCE,
		cg_vert,
		CG_PROFILE_SCE_VP_RSX,
		"main",
		NULL);
	net_send("%X\n",_vprog);


	net_send("GLESPaletteTexture::initCG(fprog): ");
	_fprog = cgCreateProgram(_ctx,
		CG_SOURCE,
		cg_frag,
		CG_PROFILE_SCE_FP_RSX,
		"main",
		NULL);
	_texture_param = cgGetNamedParameter(_fprog, "index");
	_palette_param = cgGetNamedParameter(_fprog, "palette");
	_mvp_param = cgGetNamedParameter(_vprog, "modelViewProj");
}

void GLESTexture::shutdownCG()
{
	cgDestroyParameter(_texture_param);
	cgDestroyParameter(_palette_param);
	cgDestroyParameter(_mvp_param);

	cgDestroyProgram(_fprog);
	cgDestroyProgram(_vprog);

	cgDestroyContext(_ctx);
}

void GLESTexture::setDebug(bool set)
{
	_debug=set;
}
