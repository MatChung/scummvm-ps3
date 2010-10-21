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
		_game_texture = new GLESPaletteTexture();
	else
		_game_texture->reinitGL();

	net_send("PS3GL::init()_overlay_texture\n");
	if (!_overlay_texture)
		_overlay_texture = new GLES565Texture();
	else
		_overlay_texture->reinitGL();

	net_send("PS3GL::init()_mouse_texture\n");
	if (!_mouse_texture)
		_mouse_texture = new GLESPaletteATexture();
	else
		_mouse_texture->reinitGL();

	glViewport(0, 0, 1920, 1080);

	net_send("PS3GL::init()glMatrixMode\n");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, 1920, 1080, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	net_send("PS3GL::init()clearFocusRectangle\n");
	//	CHECK_GL_ERROR();
	showMouse(true);
}

void OSystem_PS3::initSize(uint width, uint height, const Graphics::PixelFormat *format)
{
	net_send("OSystem_PS3::initSize()\n");
	_pad.setResolution(width,height);
	net_send("PS3GL::initSize(%d,%d",width,height);
	if(format!=NULL)
		net_send(",%d,%d,%d,%d)\n",format->rBits(),format->gBits(),format->bBits(),format->aBits());
	else
		net_send(")\n");

	//_egl_surface_width=width;
	//_egl_surface_height=height;
	_game_texture->allocBuffer(width, height);

	// Cap at 320x200 or the ScummVM themes abort :/
	GLuint overlay_width = MIN((int)width, 640);
	GLuint overlay_height = MIN((int)height, 400);
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
	updateFrame();
}
