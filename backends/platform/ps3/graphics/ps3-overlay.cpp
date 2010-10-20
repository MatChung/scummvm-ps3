#include "../ps3.h"


void OSystem_PS3::showOverlay()
{
	_show_overlay = true;
	_force_redraw = true;
}

void OSystem_PS3::hideOverlay()
{
	_show_overlay = false;
	_force_redraw = true;
}

void OSystem_PS3::clearOverlay()
{
	//printf("PS3GL::clearOverlay()=%X\n",_overlay_texture);
	if(_overlay_texture!=NULL)
	_overlay_texture->fillBuffer(0);
}

Graphics::PixelFormat OSystem_PS3::getOverlayFormat() const
{
	return Graphics::createPixelFormat<565>();
}

 void OSystem_PS3::grabOverlay(OverlayColor *buf, int pitch)
{
	// We support overlay alpha blending, so the pixel data here
	// shouldn't actually be used.	Let's fill it with zeros, I'm sure
	// it will be fine...
	//printf("PS3GL::grabOverlay()=%X\n",_overlay_texture);
	//printf("PS3GL::grabOverlay()=%X\n",_overlay_texture->surface_const());
	const Graphics::Surface* surface = _overlay_texture->surface_const();
	//printf("PS3GL::grabOverlay()=%X\n",surface);
	//assert(surface->bytesPerPixel == sizeof(buf[0]));
	int h = surface->h;
	do {
		memset(buf, 0, surface->w * sizeof(buf[0]));
		buf += pitch;  // This 'pitch' is pixels not bytes
	} while (--h);
}


void OSystem_PS3::copyRectToOverlay(const OverlayColor *buf, int pitch, int x, int y, int w, int h)
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

int16 OSystem_PS3::getOverlayHeight()
{
	return _overlay_texture->height();
}

int16 OSystem_PS3::getOverlayWidth()
{
	return _overlay_texture->width();
}
