#include "ps3gl.h"
#include "common/util.h"
#include "graphics/conversion.h"
#include <sysutil/sysutil_sysparam.h>
#include <sys/timer.h>
#include <PSGL/psgl.h>

#include <sys/spu_initialize.h>
#include <sys/paths.h>

#include <sysutil/sysutil_sysparam.h>  // used for cellVideoOutGetResolutionAvailability() and videoOutIsReady()


static const OSystem::GraphicsMode s_supportedGraphicsModes[] = {
		{"default", "Default", 1},
		{0, 0, 0},
};

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
		printf("%s:%d: GL error: %s", file, line, getGlErrStr(error));
}
void PS3GL::init()
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
	CHECK_GL_ERROR();
	_force_redraw=true;
	showMouse(true);
}
bool PS3GL::setGraphicsMode(int mode)
{
	return true;
}

void PS3GL::initSize(uint width, uint height, const Graphics::PixelFormat *format)
{
	printf("PS3GL::initSize(%d,%d,%X)\n",width,height,format);

	//_egl_surface_width=width;
	//_egl_surface_height=height;
	_game_texture->allocBuffer(width, height);

	// Cap at 320x200 or the ScummVM themes abort :/
	GLuint overlay_width = MIN((int)_egl_surface_width, 640);
	GLuint overlay_height = MIN((int)_egl_surface_height, 400);
	_overlay_texture->allocBuffer(overlay_width, overlay_height);

	// Don't know mouse size yet - it gets reallocated in
	// setMouseCursor.  We need the palette allocated before
	// setMouseCursor however, so just take a guess at the desired
	// size (it's small).
	_mouse_texture->allocBuffer(20, 20);
}
void PS3GL::_setCursorPalette(const byte *colors,
					uint start, uint num) {
	byte* palette = _mouse_texture->palette() + start*4;
	do {
		for (int i = 0; i < 3; ++i)
			palette[i] = colors[i];
		// Leave alpha untouched to preserve keycolor

		palette += 4;
		colors += 4;
	} while (--num);
}

void PS3GL::setPalette(const byte *colors, uint start, uint num)
{
	if (!_use_mouse_palette)
		_setCursorPalette(colors, start, num);

	byte* palette = _game_texture->palette() + start*3;
	do {
		for (int i = 0; i < 3; ++i)
			palette[i] = colors[i];
		palette += 3;
		colors += 4;
	} while (--num);
}

void PS3GL::grabPalette(byte *colors, uint start, uint num)
{
	const byte* palette = _game_texture->palette_const() + start*3;
	do {
		for (int i = 0; i < 3; ++i)
			colors[i] = palette[i];
		colors[3] = 0xff;  // alpha

		palette += 3;
		colors += 4;
	} while (--num);
}

void PS3GL::copyRectToScreen(const byte *buf, int pitch, int x, int y, int w, int h)
{
	_game_texture->updateBuffer(x, y, w, h, buf, pitch);
	draw();
}
static inline GLfixed xdiv(int numerator, int denominator) {
	assert(numerator < (1<<16));
	return (numerator << 16) / denominator;
}
void PS3GL::updateScreen()
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
		glScalex(xdiv(_egl_surface_width, _focus_rect.width()),
				 xdiv(_egl_surface_height, _focus_rect.height()),
				 1 << 16);
		glTranslatex(-_focus_rect.left << 16, -_focus_rect.top << 16, 0);
		glScalex(xdiv(_game_texture->width(), _egl_surface_width),
				 xdiv(_game_texture->height(), _egl_surface_height),
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

		_overlay_texture->drawTexture(0, 0,
									  w,
									  h);
		_mouse_texture->drawTexture(0,0,w*2,h*2);

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

Graphics::Surface *PS3GL::lockScreen()
{
	Graphics::Surface* surface = _game_texture->surface();
	//assert(surface->pixels);
	return surface;
}

void PS3GL::unlockScreen()
{
	//assert(_game_texture->dirty());
	draw();
}

void PS3GL::setShakePos(int shakeOffset)
{
	if (_shake_offset != shakeOffset) {
		_shake_offset = shakeOffset;
		_force_redraw = true;
	}
}


void PS3GL::showOverlay()
{
	_show_overlay = true;
	_force_redraw = true;
}

void PS3GL::hideOverlay()
{
	_show_overlay = false;
	_force_redraw = true;
}

void PS3GL::clearOverlay()
{
	printf("PS3GL::clearOverlay()=%X\n",_overlay_texture);
	if(_overlay_texture!=NULL)
	_overlay_texture->fillBuffer(0);
}

void PS3GL::grabOverlay(OverlayColor *buf, int pitch)
{
	// We support overlay alpha blending, so the pixel data here
	// shouldn't actually be used.	Let's fill it with zeros, I'm sure
	// it will be fine...
	printf("PS3GL::grabOverlay()=%X\n",_overlay_texture);
	printf("PS3GL::grabOverlay()=%X\n",_overlay_texture->surface_const());
	const Graphics::Surface* surface = _overlay_texture->surface_const();
	printf("PS3GL::grabOverlay()=%X\n",surface);
	//assert(surface->bytesPerPixel == sizeof(buf[0]));
	int h = surface->h;
	do {
		memset(buf, 0, surface->w * sizeof(buf[0]));
		buf += pitch;  // This 'pitch' is pixels not bytes
	} while (--h);
}


void PS3GL::copyRectToOverlay(const OverlayColor *buf, int pitch, int x, int y, int w, int h)
{/*
	const Graphics::Surface* surface = _overlay_texture->surface_const();
	//assert(surface->bytesPerPixel == sizeof(buf[0]));

	int *temp=new int[w*h];
	//Graphics::crossBlit((byte*)temp,(byte*)buf,w*4,pitch,w,h,get8888Format(),getOverlayFormat());
	//const byte* src = static_cast<const byte*>(buf);
	//printf("converting...\n");
	
	for(int __y=0;__y<h;__y++)
	{
		for(int __x=0;__x<w;__x++)
		{
			OverlayColor scol=buf[__y*pitch+__x];
			int r=((scol>>12)&0xF)<<4;
			int g=((scol>>8)&0xF)<<4;
			int b=((scol>>4)&0xF)<<4;
			int a=((scol>>0)&0xF)<<4;
			temp[__y*w+__x]=(r<<24)|(g<<16)|(b<<8)|(a<<0);
			//printf("%d,%d,%d,%d  ",r,g,b,a);
		}
		//printf("\n");
	}*/

	// This 'pitch' is pixels not bytes
	_overlay_texture->updateBuffer(x, y, w, h, buf, pitch * sizeof(buf[0]));
//	delete[] temp;
}

int16 PS3GL::getOverlayHeight()
{
	return _overlay_texture->height();
}

int16 PS3GL::getOverlayWidth()
{
	return _overlay_texture->width();
}



const OSystem::GraphicsMode *PS3GL::getSupportedGraphicsModes() const
{
	return s_supportedGraphicsModes;
}

int PS3GL::getDefaultGraphicsMode() const
{
	return 0;
}

bool PS3GL::setGraphicsMode(const char *name)
{
	return setGraphicsMode(0);
}

int PS3GL::getGraphicsMode() const
{
	return 0;
}


int16 PS3GL::getHeight()
{
	return _game_texture->height();
}

int16 PS3GL::getWidth()
{
	return _game_texture->width();
}

void PS3GL::draw()
{
	updateScreen();
}

bool PS3GL::showMouse(bool visible)
{
	printf("PS3GL::showMouse(%d)\n",visible);
	//_show_mouse=visible;
	return visible;
}

void PS3GL::warpMouse(int x, int y)
{
	//printf("PS3GL::warpMouse(%d, %d)\n",x,y);
	_mouse_pos.x=x;
	_mouse_pos.y=y;

	updateScreen();
}

void PS3GL::setMouseCursor(const byte *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, int cursorTargetScale, const Graphics::PixelFormat *format)
{
	//printf("PS3GL::setMouseCursor(%p, %u, %u, %d, %d, %d, %d, %p)",
	//	  buf, w, h, hotspotX, hotspotY, (int)keycolor, cursorTargetScale,
	//	  format);

	assert(keycolor < 256);

	_mouse_texture->allocBuffer(w, h);

	// Update palette alpha based on keycolor
	byte* palette = _mouse_texture->palette();
	int i = 256;
	do {
		palette[3] = 0xff;
		palette += 4;
	} while (--i);
	palette = _mouse_texture->palette();
	palette[keycolor*4 + 3] = 0x00;
	_mouse_texture->updateBuffer(0, 0, w, h, buf, w);

	_mouse_hotspot = Common::Point(hotspotX, hotspotY);
	_mouse_targetscale = cursorTargetScale;

	draw();
}

