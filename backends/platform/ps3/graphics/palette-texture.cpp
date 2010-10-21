#include "texture.h"
#include <Cg/NV/cgGL.h>



GLESPaletteTexture::GLESPaletteTexture() :
GLESTexture(),
_texture(NULL)
{
	glGenTextures(1, &_palette_name);
	glGenTextures(1, &_texture_name);
	_palette=new uint32[256];
	memset(_palette,0,sizeof(uint32)*256);
	glBindTexture(GL_TEXTURE_2D, _palette_name);
	//CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//CHECK_GL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, glPaletteInternalFormat(),
		256, 1,
		0, glPaletteFormat(), glPaletteType(), _palette);
	glFlush();
	_isLocked=false;

	initCG();
}

GLESPaletteTexture::~GLESPaletteTexture()
{
	delete[] _texture;
	delete[] _palette;
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


	if (_texture)
		delete[] _texture;

	_texture = new byte[_texture_width * _texture_height * bpp];
	_surface.pixels = _texture;

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
}

void GLESPaletteTexture::fillBuffer(byte x)
{
	//net_send("GLESPaletteTexture::fillBuffer(%d)-%X\n",&_surface);
	//net_send("GLESPaletteTexture::fillBuffer(%d)-%X\n",_surface.pixels);
	//net_send("GLESPaletteTexture::fillBuffer()0-%d,%d,%d\n",_surface.h , _surface.w , bytesPerPixel());
	assert(_surface.pixels);
	memset(_surface.pixels, x, _surface.pitch * _surface.h);
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _surface.w, _surface.h,
		glFormat(), glType(), _surface.pixels);
	//CHECK_GL_ERROR();
	setDirty();
}

void GLESPaletteTexture::updateBuffer(GLuint x, GLuint y,
									  GLuint w, GLuint h,
									  const void* buf, int pitch)
{
	_all_dirty = true;

	const byte* src = static_cast<const byte*>(buf);
	byte* dst = static_cast<byte*>(_surface.getBasePtr(x, y));
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	GLuint horig=h;
	do
	{
		memcpy(dst, src, w * bytesPerPixel());
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y+horig-h, w, 1,
			glFormat(), glType(), dst);
		dst += _surface.pitch;
		src += pitch;
	} while (--h);
}

void GLESPaletteTexture::drawTexture(GLshort x, GLshort y, GLshort w, GLshort h)
{
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, _texture_name);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, _palette_name);
	//uploadTexture();

	//CG stuff enable
	cgGLEnableProfile(CG_PROFILE_SCE_VP_RSX);
	cgGLBindProgram(vprog);
	cgGLSetStateMatrixParameter(_mvp_param, CG_GL_MODELVIEW_PROJECTION_MATRIX,
		CG_GL_MATRIX_IDENTITY);
	cgGLEnableProfile(CG_PROFILE_SCE_FP_RSX);
	cgGLBindProgram(fprog);
	cgGLSetTextureParameter(_texture_param,_texture_name);
	cgGLEnableTextureParameter(_texture_param);
	cgGLSetTextureParameter(_palette_param,_palette_name);
	cgGLEnableTextureParameter(_palette_param);


	//draw
	GLESTexture::_drawTexture(x, y, w*1, h);

	//CG stuff disable
	cgGLDisableTextureParameter(_texture_param);
	cgGLDisableTextureParameter(_palette_param);
	cgGLDisableProfile(CG_PROFILE_SCE_VP_RSX);
	cgGLDisableProfile(CG_PROFILE_SCE_FP_RSX);
	//CG stuff disable finished
}

void GLESPaletteTexture::grabPalette(byte *colors, uint start, uint num)
{
	net_send("GLESPaletteTexture::grabPalette(%d,%d)\n",start, num);
	for(uint i=start;i<start+num;i++)
	{
		uint32 col=_palette[i];
		colors[i*4+0]=(col>>24)&0xff;
		colors[i*4+1]=(col>>16)&0xff;
		colors[i*4+2]=(col>>8)&0xff;
	}
}

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
}

void GLESPaletteTexture::updatePalette(const byte *colors, uint start, uint num)
{
	//net_send("GLESPaletteTexture::updatePalette(%d,%d)\n",start, num);
	for(uint i=start;i<start+num;i++)
	{
		uint32 r=colors[i*4+0];
		uint32 g=colors[i*4+1];
		uint32 b=colors[i*4+2];
		_palette[i]=(r<<24)|(g<<16)|(b<<8)|(0xff);
		//net_send("    %d, %d, %d, %X\n",r,g,b,_palette[i]);
	}

	//dumpPalette(_palette,start,num);

	glBindTexture(GL_TEXTURE_2D, _palette_name);
	glTexSubImage2D(GL_TEXTURE_2D, 0, start, 0, num, 1,
		glPaletteFormat(), glPaletteType(), &_palette[start]);
}

void GLESPaletteTexture::setKeyColor(uint32 color)
{
	net_send("GLESPaletteTexture::setKeyColor(%d)\n",color);
	_palette[color]=0;
	glBindTexture(GL_TEXTURE_2D, _palette_name);
	glTexSubImage2D(GL_TEXTURE_2D, 0, color, 0, 1, 1,
		glPaletteFormat(), glPaletteType(), &_palette[color]);
}

Graphics::Surface* GLESPaletteTexture::lock()
{
	_isLocked=true;
	return &_surface;
}

void GLESPaletteTexture::unlock()
{
	glBindTexture(GL_TEXTURE_2D, _texture_name);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _surface.w, _surface.h,
		glFormat(), glType(), _surface.pixels);
	_isLocked=false;
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
	ctx = cgCreateContext();
	net_send("%X\n",ctx);


	net_send("GLESPaletteTexture::initCG(vprog): ");
	vprog = cgCreateProgram(ctx,
		CG_SOURCE,
		cg_vert,
		CG_PROFILE_SCE_VP_RSX,
		"main",
		NULL);
	net_send("%X\n",vprog);


	net_send("GLESPaletteTexture::initCG(fprog): ");
	fprog = cgCreateProgram(ctx,
		CG_SOURCE,
		cg_frag,
		CG_PROFILE_SCE_FP_RSX,
		"main",
		NULL);
	_texture_param = cgGetNamedParameter(fprog, "index");
	_palette_param = cgGetNamedParameter(fprog, "palette");
	_mvp_param = cgGetNamedParameter(vprog, "modelViewProj");
	net_send("%X,%X,%X\n",fprog,_texture_param,_palette_param);


}
