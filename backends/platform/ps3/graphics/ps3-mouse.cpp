#include "../ps3.h"


bool OSystem_PS3::showMouse(bool visible)
{
	if(_show_mouse!=visible)
		net_send("OSystem_PS3::showMouse(%d)\n",visible);
	_show_mouse=visible;
	return visible;
}

void OSystem_PS3::warpMouse(int x, int y)
{
	//net_send("OSystem_PS3::warpMouse(%d, %d)\n",x,y);
	_mouse_pos.x=x;
	_mouse_pos.y=y;

	//updateScreen();
}

void OSystem_PS3::setMouseCursor(const byte *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, int cursorTargetScale, const Graphics::PixelFormat *format)
{
	net_send("OSystem_PS3::setMouseCursor(%p, %u, %u, %d, %d, %d, %d, %p)\n",
		  buf, w, h, hotspotX, hotspotY, (int)keycolor, cursorTargetScale,
		  format);

	if(w<1 || h<1)
		return;

	Graphics::PixelFormat newFormat;
	if(format==NULL)
		newFormat = Graphics::PixelFormat::createFormatCLUT8();
	else
		newFormat = *format;

	if(_mouse_texture->getFormat()!=newFormat)
	{
		Graphics::PixelFormat format2=_mouse_texture->getFormat();
		net_send("    MISMATCH!!:\n");
		net_send("      bytesPerPixel: %d\n",format2.bytesPerPixel);
		net_send("      rBits:         %d\n",format2.rBits());
		net_send("      gBits:         %d\n",format2.gBits());
		net_send("      bBits:         %d\n",format2.bBits());
		net_send("      aBits:         %d\n",format2.aBits());
		net_send("      rShift:        %d\n",format2.rShift);
		net_send("      gShift:        %d\n",format2.gShift);
		net_send("      bShift:        %d\n",format2.bShift);
		net_send("      aShift:        %d\n",format2.aShift);

		if(_mouse_texture!=NULL)
			delete _mouse_texture;

		_mouse_texture=createTextureFromPixelFormat(newFormat);

		if(newFormat.bytesPerPixel==1)
		{
			//if(_mouse_texture->height()!=h || _mouse_texture->width()!=w)
			_mouse_texture->allocBuffer(w, h);
			_mouse_texture->updatePalette((byte*)_tempMousePalette,0,256);
		}
	}


	//assert(keycolor < 256);

	if(_mouse_texture->height()<h || _mouse_texture->width()<w)
		_mouse_texture->allocBuffer(w, h);

	// Update palette alpha based on keycolor
	/*byte* palette = _mouse_texture->palette();
	int i = 256;
	do {
		palette[3] = 0xff;
		palette += 4;
	} while (--i);*/
	//palette = _mouse_texture->palette();
	//palette[keycolor*4 + 3] = 0x00;
	_mouse_texture->updateBuffer(0, 0, w, h, buf, w*newFormat.bytesPerPixel);
	_mouse_texture->setKeyColor(keycolor);
	_mouse_keycolor=keycolor;

	_mouse_hotspot = Common::Point(hotspotX, hotspotY);
	_mouse_targetscale = cursorTargetScale;

	if(newFormat.bytesPerPixel==1)
		_mouse_is_palette=true;
	else
		_mouse_is_palette=false;

	//draw();
}
void OSystem_PS3::_setCursorPalette(const byte *colors,
					uint start, uint num) {
	//net_send("OSystem_PS3::_setCursorPalette(%d, %d, %d)\n",start,num,_mouse_is_palette);

	if(!_mouse_is_palette)
	{
		memcpy(&_tempMousePalette[start],colors,sizeof(uint32)*num);
		return;
	}

	_mouse_texture->updatePalette(colors,start,num);
	_mouse_texture->setKeyColor(_mouse_keycolor);
/*
	byte* palette = _mouse_texture->palette() + start*4;
	do {
		for (int i = 0; i < 3; ++i)
			palette[i] = colors[i];
		// Leave alpha untouched to preserve keycolor

		palette += 4;
		colors += 4;
	} while (--num);*/
}

void OSystem_PS3::disableCursorPalette(bool disable)
{
	net_send("OSystem_PS3::disableCursorPalette(%d)\n",disable);
	_use_mouse_palette=!disable;
}

void OSystem_PS3::setCursorPalette(const byte *colors, uint start, uint num)
{
	net_send("OSystem_PS3::setCursorPalette(%d, %d, %d)\n",colors,start,num);
	_setCursorPalette(colors,start,num);
}
