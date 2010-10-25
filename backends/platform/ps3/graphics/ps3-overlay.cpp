#include "../ps3.h"


void OSystem_PS3::showOverlay()
{
	if(!_show_overlay)
		net_send("OSystem_PS3::showOverlay()\n");
	_show_overlay = true;
}

void OSystem_PS3::hideOverlay()
{
	if(_show_overlay)
		net_send("OSystem_PS3::hideOverlay()\n");
	_show_overlay = false;
}

void OSystem_PS3::clearOverlay()
{
	net_send("OSystem_PS3::clearOverlay()\n");
	if(_overlay_texture!=NULL)
		_overlay_texture->fillBuffer(0);
}

Graphics::PixelFormat OSystem_PS3::getOverlayFormat() const
{
	net_send("OSystem_PS3::getOverlayFormat()\n");
	return Graphics::createPixelFormat<565>();
}

void OSystem_PS3::grabOverlay(OverlayColor *buf, int pitch)
{
	net_send("OSystem_PS3::grabOverlay()\n");
	// We support overlay alpha blending, so the pixel data here
	// shouldn't actually be used.	Let's fill it with zeros, I'm sure
	// it will be fine...
	//net_send("OSystem_PS3::grabOverlay()=%X\n",_overlay_texture);
	//net_send("OSystem_PS3::grabOverlay()=%X\n",_overlay_texture->surface_const());
	const Graphics::Surface* surface = _overlay_texture->surface_const();
	//net_send("OSystem_PS3::grabOverlay()=%X\n",surface);
	//assert(surface->bytesPerPixel == sizeof(buf[0]));
	int h = surface->h;
	byte *src = (byte *)surface->pixels;
	do {
		memcpy(buf, src, surface->w * 2);
		src += surface->pitch;
		buf += pitch;
	} while (--h);
}


void OSystem_PS3::copyRectToOverlay(const OverlayColor *buf, int pitch, int x, int y, int w, int h)
{
	net_send("OSystem_PS3::copyRectToOverlay(%d,%d,%d,%d,%d)\n",pitch,x,y,w,h);

	/*
	const Graphics::Surface* surface = _overlay_texture->surface_const();
	//assert(surface->bytesPerPixel == sizeof(buf[0]));

	int *temp=new int[w*h];
	//Graphics::crossBlit((byte*)temp,(byte*)buf,w*4,pitch,w,h,get8888Format(),getOverlayFormat());
	//const byte* src = static_cast<const byte*>(buf);
	//net_send("converting...\n");
	
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
			//net_send("%d,%d,%d,%d  ",r,g,b,a);
		}
		//net_send("\n");
	}*/

	// This 'pitch' is pixels not bytes
	_overlay_texture->updateBuffer(x, y, w, h, buf, pitch * sizeof(buf[0]));
//	delete[] temp;
}

int16 OSystem_PS3::getOverlayHeight()
{
	net_send("OSystem_PS3::getOverlayHeight(%d)\n",_overlay_texture->height());
	return _overlay_texture->height();
}

int16 OSystem_PS3::getOverlayWidth()
{
	net_send("OSystem_PS3::getOverlayWidth(%d)\n",_overlay_texture->width());
	return _overlay_texture->width();
}
