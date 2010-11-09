#include "../ps3.h"
#include "../graphics/fb/fb.h"



void OSystem_PS3::initSize(uint width, uint height, const Graphics::PixelFormat *format)
{
	printf("OSystem_PS3::initSize(%d,%d)\n",width,height);
}

void OSystem_PS3::updateScreen()
{
	printf("OSystem_PS3::updateScreen()\n");
	waitFlip(); // Wait for the last flip to finish, so we can draw to the old buffer
	//drawFrame(buffers[currentBuffer], frame++); // Draw into the unused buffer
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
