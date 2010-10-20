#include "../ps3.h"
#include <PSGL/psgl.h>

void OSystem_PS3::initGraphics()
{
	printf("PS3GL::init()\n");
	GLESTexture::initGLExtensions();
	_egl_surface_width=320;
	_egl_surface_height=200;

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

	printf("PS3GL::init()_game_texture\n");
	if (!_game_texture)
		_game_texture = new GLESPaletteTexture();
	else
		_game_texture->reinitGL();

	printf("PS3GL::init()_overlay_texture\n");
	if (!_overlay_texture)
		_overlay_texture = new GLES565Texture();
	else
		_overlay_texture->reinitGL();

	printf("PS3GL::init()_mouse_texture\n");
	if (!_mouse_texture)
		_mouse_texture = new GLESPaletteATexture();
	else
		_mouse_texture->reinitGL();

	glViewport(0, 0, 1920, 1080);

	printf("PS3GL::init()glMatrixMode\n");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, _egl_surface_width, _egl_surface_height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	printf("PS3GL::init()clearFocusRectangle\n");
	//	CHECK_GL_ERROR();
	_force_redraw=true;
	showMouse(true);
}

void OSystem_PS3::draw()
{
	updateScreen();
}

void OSystem_PS3::updateScreen()
{
	//printf("PS3GL::updateScreen()\n");
	/*if (!_force_redraw &&
	!_game_texture->dirty() &&
	!_overlay_texture->dirty() &&
	!_mouse_texture->dirty())
	return;

	*/
	//printf("PS3GL::updateScreen()\n");

	_force_redraw = true;

	glPushMatrix();

	if (_shake_offset != 0 ||
		(!_focus_rect.isEmpty() &&
		!Common::Rect(_game_texture->width(),
		_game_texture->height()).contains(_focus_rect))) {
			// These are the only cases where _game_texture doesn't
			// cover the entire screen.
			glClearColorx(0, 0, 0, 1 << 16);
			glClear(GL_COLOR_BUFFER_BIT);

			// Move everything up by _shake_offset (game) pixels
			glTranslatex(0, -_shake_offset << 16, 0);
	}

	if (_focus_rect.isEmpty()) {
		//printf("PS3GL::updateScreen() - drawGame1\n");
		_game_texture->drawTexture(0, 0,
			_egl_surface_width, _egl_surface_height);
	} else {
		//printf("PS3GL::updateScreen() - drawGame2\n");
		glPushMatrix();
		glScalef(_egl_surface_width/ _focus_rect.width(),
			_egl_surface_height/ _focus_rect.height(),
			1);
		glTranslatex(-_focus_rect.left << 16, -_focus_rect.top << 16, 0);
		glScalex(_game_texture->width()/ _egl_surface_width,
			_game_texture->height()/ _egl_surface_height,
			1 << 16);
		_game_texture->drawTexture(0, 0,
			_egl_surface_width, _egl_surface_height);
		glPopMatrix();
	}

	//CHECK_GL_ERROR();

	if (_show_overlay) {
		//printf("PS3GL::updateScreen() - _show_overlay\n");
		_overlay_texture->drawTexture(0, 0,
			_egl_surface_width,
			_egl_surface_height);
		//CHECK_GL_ERROR();
	}

	/*if (_show_mouse) */{
		//printf("PS3GL::updateScreen() - _show_mouse\n");
		glPushMatrix();

		glTranslatef(-_mouse_hotspot.x+_mouse_pos.x,
			-_mouse_hotspot.y+_mouse_pos.y,
			0);

		// Scale up ScummVM -> OpenGL (pixel) coordinates
		int texwidth, texheight;
		if (_show_overlay) {
			texwidth = getOverlayWidth();
			texheight = getOverlayHeight();
		} else {
			texwidth = getWidth();
			texheight = getHeight();
		}
		//glScalex(xdiv(_egl_surface_width, texwidth),
		//		 xdiv(_egl_surface_height, texheight),
		//		 1 << 16);

		// Note the extra half texel to position the mouse in
		// the middle of the x,y square:
		//const Common::Point& mouse = getEventManager()->getMousePos();
		//glTranslatex((mouse.x << 16) | 1 << 15,
		//			 (mouse.y << 16) | 1 << 15, 0);

		// Mouse targetscale just seems to make the cursor way
		// too big :/
		//glScalex(_mouse_targetscale << 16, _mouse_targetscale << 16,
		//	 1 << 16);

		GLshort w=_mouse_texture->width();
		GLshort h=_mouse_texture->height();
		//printf("PS3GL::updateScreen() - _show_mouse (%d,%d,%d,%d)\n",-_mouse_hotspot.x+_mouse_pos.x,-_mouse_hotspot.y+_mouse_pos.y,w,h);

		//_overlay_texture->drawTexture(0, 0, w, h);
		_mouse_texture->drawTexture(0,0,w,h);

		glPopMatrix();
	}

	glPopMatrix();

	//CHECK_GL_ERROR();

	/*	JNIEnv* env = JNU_GetEnv();
	if (!env->CallBooleanMethod(_back_ptr, MID_swapBuffers)) {
	// Context lost -> need to reinit GL
	destroyScummVMSurface();
	setupScummVMSurface();
	}*/
	psglSwap();
}
