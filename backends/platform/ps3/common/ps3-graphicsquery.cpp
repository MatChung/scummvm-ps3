#include "../ps3.h"
#include "../graphics/fb/fb.h"



void OSystem_PS3::initSize(uint width, uint height, const Graphics::PixelFormat *format)
{
	printf("OSystem_PS3::initSize(%d,%d)\n",width,height);

	Graphics::PixelFormat newFormat;
	if(format==NULL)
		newFormat = Graphics::PixelFormat::createFormatCLUT8();
	else
		newFormat = *format;


	if(_game_screen.getFormat()!=newFormat)
	{
		//if(_game_texture!=NULL)
		//	delete _game_texture;
		//_game_texture=createTextureFromPixelFormat(newFormat);
		_game_screen.setFormat(newFormat);
	}

	//_egl_surface_width=width;
	//_egl_surface_height=height;
	_game_screen.allocBuffer(width, height);
	_game_screen.fillBuffer(0);

	// Cap at 320x200 or the ScummVM themes abort :/
	u32 overlay_width = MIN((int)width*2, 320*2);
	u32 overlay_height = MIN((int)height*2, 200*2);
	_overlay_screen.allocBuffer(overlay_width, overlay_height);
	_overlay_screen.fillBuffer(0);

	// Don't know mouse size yet - it gets reallocated in
	// setMouseCursor.  We need the palette allocated before
	// setMouseCursor however, so just take a guess at the desired
	// size (it's small).
	//_mouse_texture->setDebug(true);
	_mouse_screen.allocBuffer(20, 20);
	_mouse_screen.fillBuffer(0);
}

void OSystem_PS3::updateScreen()
{
	printf("OSystem_PS3::updateScreen()\n");
	waitFlip(); // Wait for the last flip to finish, so we can draw to the old buffer
	//drawFrame(buffers[currentBuffer], frame++); // Draw into the unused buffer

	if(!_show_overlay)
		_game_screen.draw(0, 0, _game_screen.getWidth(), _game_screen.getHeight());
	else
		_overlay_screen.draw(0, 0, _overlay_screen.getWidth(), _overlay_screen.getHeight());

	if (_show_mouse)
		_mouse_screen.draw(_mouse_pos.x-_mouse_hotspot.x,_mouse_pos.y-_mouse_hotspot.y,_mouse_screen.getWidth(), _mouse_screen.getHeight());


	flip(currentBuffer); // Flip buffer onto screen
	currentBuffer = !currentBuffer;
}

void OSystem_PS3::setShakePos(int shakeOffset)
{
	printf("OSystem_PS3::setShakePos(%d)\n",shakeOffset);
	if (_shake_offset != shakeOffset) {
		_shake_offset = shakeOffset;
	}
}
