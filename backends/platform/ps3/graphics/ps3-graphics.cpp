#include "../ps3.h"
#include <PSGL/psgl.h>

void OSystem_PS3::initGraphics()
{
	net_send("PS3GL::init()\n");
	GLESTexture::initGLExtensions();

	// Turn off anything that looks like 3D ;)
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glDisable(GL_DITHER);
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_TEXTURE_2D);

	net_send("PS3GL::init()_game_texture\n");
	if (!_game_texture)
	{
		_game_texture = new GLESPaletteTexture();
		_game_texture_palette = (GLESPaletteTexture*)_game_texture;
	}
	else
		_game_texture->reinitGL();

	net_send("PS3GL::init()_overlay_texture\n");
	if (!_overlay_texture)
		_overlay_texture = new GLES565Texture();
	else
		_overlay_texture->reinitGL();

	net_send("PS3GL::init()_mouse_texture\n");
	if (!_mouse_texture)
		_mouse_texture = new GLESPaletteTexture();
	else
		_mouse_texture->reinitGL();

	glViewport(0, 0, _tv_screen_width, _tv_screen_height);

	net_send("PS3GL::init()glMatrixMode\n");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, _tv_screen_width, _tv_screen_height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	net_send("PS3GL::init()clearFocusRectangle\n");
	//	CHECK_GL_ERROR();
	showMouse(true);
}

void OSystem_PS3::initSize(uint width, uint height, const Graphics::PixelFormat *format)
{
	net_send("OSystem_PS3::initSize(%d,%d)\n",width,height);
	_pad.setResolution(width,height);

	Graphics::PixelFormat newFormat;
	if(format==NULL)
		newFormat = Graphics::PixelFormat::createFormatCLUT8();
	else
		newFormat = *format;

	if(_game_texture->getFormat()!=newFormat)
	{
		if(_game_texture!=NULL)
			delete _game_texture;
		_game_texture=createTextureFromPixelFormat(newFormat);

		if(newFormat.bytesPerPixel==1)
			_game_texture_palette=(GLESPaletteTexture*)_game_texture;
		else
			_game_texture_palette=NULL;
	}

	//_egl_surface_width=width;
	//_egl_surface_height=height;
	_game_texture->allocBuffer(width, height);

	// Cap at 320x200 or the ScummVM themes abort :/
	GLuint overlay_width = MIN((int)width, 320);
	GLuint overlay_height = MIN((int)height, 200);
	_overlay_texture->allocBuffer(overlay_width, overlay_height);

	// Don't know mouse size yet - it gets reallocated in
	// setMouseCursor.  We need the palette allocated before
	// setMouseCursor however, so just take a guess at the desired
	// size (it's small).
	_mouse_texture->allocBuffer(20, 20);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(-10, width+10, height+20, -7, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	_currentScreenFormat=newFormat;
}

void OSystem_PS3::draw()
{
	//net_send("OSystem_PS3::draw()\n");
	updateScreen();
}

void OSystem_PS3::updateScreen()
{
	//net_send("OSystem_PS3::updateScreen()\n");
	glClearColorx(0, 0, 0, 1 << 16);
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();

	if (_shake_offset != 0)
	{
		// Move everything up by _shake_offset (game) pixels
		glTranslatef(0, -_shake_offset, 0);
	}

	//net_send("PS3GL::updateScreen() - drawGame1\n");
	_game_texture->drawTexture(0, 0,
		_game_texture->width(), _game_texture->height());

	//CHECK_GL_ERROR();

	if (_show_overlay)
	{
		//net_send("PS3GL::updateScreen() - _show_overlay\n");
		_overlay_texture->drawTexture(0, 0,
			_game_texture->width(),
			_game_texture->height());
		//CHECK_GL_ERROR();
	}

	if (_show_mouse)
	{
		//net_send("PS3GL::updateScreen() - _show_mouse\n");
		glPushMatrix();

		glTranslatef(_mouse_pos.x-_mouse_hotspot.x,
			_mouse_pos.y-_mouse_hotspot.y,
			0);

		GLshort w=_mouse_texture->width();
		GLshort h=_mouse_texture->height();
		_mouse_texture->drawTexture(0,0,w,h);

		glPopMatrix();
	}

	glPopMatrix();

	//CHECK_GL_ERROR();

	psglSwap();
}

GLESTexture *OSystem_PS3::createTextureFromPixelFormat(Graphics::PixelFormat &format)
{
	if(format.bytesPerPixel==1)
		return new GLESPaletteTexture();

	if(format.bytesPerPixel==2)
	{
		if(format.rBits()==4 && format.gBits()==4 && format.bBits()==4 && format.aBits()==4)
			return new GLES4444Texture();
		if(format.rBits()==5 && format.gBits()==6 && format.bBits()==5 && format.aBits()==0)
			return new GLES565Texture();
	}

	if(format.bytesPerPixel==4)
	{
		if(format.rBits()==8 && format.gBits()==8 && format.bBits()==8 && format.aBits()==8)
			return new GLES8888Texture();
	}

	net_send("OSystem_PS3::createTextureFromPixelFormat - NO VALID TEXTURE FOUND\n");
	net_send("  bytesPerPixel: %d\n",format.bytesPerPixel);
	net_send("  rBits:         %d\n",format.rBits());
	net_send("  gBits:         %d\n",format.gBits());
	net_send("  bBits:         %d\n",format.bBits());
	net_send("  aBits:         %d\n",format.aBits());
	return NULL;
}
