#include "texture.h"
#include <Cg/NV/cgGL.h>

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


GLESPaletteTexture::GLESPaletteTexture() :
GLESTexture(),
_palette_dirty(0)
{
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
	glTexImage2D(GL_TEXTURE_2D, 0, glPaletteInternalFormat(),
		256, 1,
		0, glPaletteFormat(), glPaletteType(), NULL);
	CHECK_GL_ERROR();
	glFlush();
	CHECK_GL_ERROR();

	initCG();
	CHECK_GL_ERROR();
}

GLESPaletteTexture::~GLESPaletteTexture()
{
	net_send("GLESPaletteTexture::~GLESPaletteTexture(%u)\n", _palette_name);
	CHECK_GL_ERROR();
	net_send("    delete GL texture\n");
	if(_palette_name>0)
		glDeleteTextures(1, &_palette_name);
	CHECK_GL_ERROR();

	net_send("    delete mem texture\n");
	if(_palette)
		delete[] _palette;

	net_send("    shutdown CG\n");
	shutdownCG();
	net_send("    destroyed\n");
}

/* not needed?
void GLESPaletteTexture::reinitGL()
{
	net_send("GLESPaletteTexture::reinitGL()\n");
	CHECK_GL_ERROR();
	//glGenTextures(1, &_texture_name);
	CHECK_GL_ERROR();
}

void GLESPaletteTexture::allocBuffer(GLuint w, GLuint h)
{
	//net_send("GLESPaletteTexture::allocBuffer(%d,%d)\n",w,h);

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

	net_send("GLESPaletteTexture::allocBuffer(%d,%d)\n",w,h);

	if (_texture)
	{
		net_send("    delete old texture\n");
		delete[] _texture;
	}

	net_send("    alloc new texture\n");
	_texture = new byte[_texture_width * _texture_height * bpp];
	_surface.pixels = _texture;
	CHECK_GL_ERROR();
	net_send("GLESPaletteTexture::allocBufferYYY(%d,%d)\n",_texture_width,_texture_height);

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
	net_send("  finished\n");
}

void GLESPaletteTexture::fillBuffer(byte x)
{
	//net_send("GLESPaletteTexture::fillBuffer(%d)-%X\n",&_surface);
	//net_send("GLESPaletteTexture::fillBuffer(%d)-%X\n",_surface.pixels);
	//net_send("GLESPaletteTexture::fillBuffer()0-%d,%d,%d\n",_surface.h , _surface.w , bytesPerPixel());
	net_send("GLESPaletteTexture::fillBuffer(%d)\n",x);
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
	_updatesPerFrame++;
}
void GLESPaletteTexture::updateBuffer(GLuint x, GLuint y,
									  GLuint w, GLuint h,
									  const void* buf, int pitch)
{
	//net_send("GLESPaletteTexture::updateBuffer(%d,%d,%d,%d)\n",x,y,w,h);

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
*/

void GLESPaletteTexture::drawTexture(GLshort x, GLshort y, GLshort w, GLshort h)
{
	net_send("GLESPaletteTexture::drawTexture(%d,%d,%d,%d)\n",x,y,w,h);
	if(_dirty_top<_dirty_bottom)
	{
		net_send("    glTexSubImage2D(%d,%d,%d,%d)\n",0,_dirty_top,_texture_width, _dirty_bottom-_dirty_top);
		CHECK_GL_ERROR();
		glBindTexture(GL_TEXTURE_2D, _texture_name);
		CHECK_GL_ERROR();
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, _dirty_top,
		//	_texture_width, _dirty_bottom-_dirty_top, glFormat(), glType(), _texture);
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

	if(_palette_dirty)
	{
		net_send("    glTexSubImage2D(%d,%d,%d,%d) pal\n",0,0,256, 1);
		CHECK_GL_ERROR();
		glBindTexture(GL_TEXTURE_2D, _palette_name);
		CHECK_GL_ERROR();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
			256, 1, glPaletteFormat(), glPaletteType(), _palette);
		CHECK_GL_ERROR();
		glFlush();
		CHECK_GL_ERROR();

		_palette_dirty=false;
	}


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

void GLESPaletteTexture::grabPalette(byte *colors, uint start, uint num)
{
	net_send("GLESPaletteTexture::grabPalette(%d,%d)\n",start, num);
	uint32* dst = (uint32*)(colors);
	for(uint i=start;i<start+num;i++)
	{
		*dst=_palette[i];
		dst++;
	}
}
/*
void dumpPalette(const uint32 *colors, uint start, uint num)
{
	net_send("GLESPaletteTexture::dumpPalette(%d,%d)\n",start, num);
	for(uint i=start;i<start+num;i++)
	{
		uint32 col=colors[i];
		uint32 r=(col>>24)&0xff;
		uint32 g=(col>>16)&0xff;
		uint32 b=(col>>8)&0xff;
		byte r1=(col>>24)&0xff;
		byte g1=(col>>16)&0xff;
		byte b1=(col>>8)&0xff;
		net_send("    %d, %d, %d",r,g,b);
		net_send("    %d, %d, %d\n",r1,g1,b1);
	}
}*/

void GLESPaletteTexture::updatePalette(const byte *colors, uint start, uint num)
{
	net_send("GLESPaletteTexture::updatePalette(%d,%d)\n",start, num);
	const uint32* src = (uint32*)(colors);
	for(uint i=start;i<start+num;i++)
	{
		_palette[i]=(*src)|0xFF;
		src++;
	}

	_palette_dirty=true;
}

void GLESPaletteTexture::setKeyColor(uint32 color)
{
	net_send("GLESPaletteTexture::setKeyColor(%d)\n",color);
	//_keycolor=color;
	_palette[color]=0;
	_palette_dirty=true;
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
						   oColor = tex2D (palette, colorIndex);\n\
						   \n\
						   //oColor = color;\n\
						   }\n\
						   ";

void GLESPaletteTexture::initCG()
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

void GLESPaletteTexture::shutdownCG()
{
	cgDestroyParameter(_texture_param);
	cgDestroyParameter(_palette_param);
	cgDestroyParameter(_mvp_param);

	cgDestroyProgram(_fprog);
	cgDestroyProgram(_vprog);

	cgDestroyContext(_ctx);
}
