#include "fb.h"

void blitPalette(u32 x_,u32 y_,u32 w,u32 h,u8 *buf, u32 *pal)
{
	buffer *b=buffers[currentBuffer];

	u32 cx=b->width/2;
	u32 cy=b->height/2;
	u32 cw=w/2;
	u32 ch=h/2;

	for(u32 y=0;y<h;y++)
	{
		u32 yoff=(cy-ch+y+y_)*b->width;
		for(u32 x=0;x<w;x++)
		{
			u32 xoff=(cx-cw+x+x_);
			b->ptr[yoff+xoff]=pal[buf[y*w+x]]>>8;
		}
	}
}

u32 translate(u8 *buf,Graphics::PixelFormat &fmt)
{
//	if(fmt.bytesPerPixel!=2)
//	{
//		printf("unknown format: %d\n",fmt.bytesPerPixel);
//		return 0xFF4400;
//	}
	uint8 r=0;
	uint8 g=0;
	uint8 b=0;
	fmt.colorToRGB((uint32)*((u16*)buf),r,g,b);

	return r<<16|g<<8|b;
}

void blitRGB(u32 x_,u32 y_,u32 w,u32 h,u8 *buf,Graphics::PixelFormat &fmt)
{
	buffer *b=buffers[currentBuffer];

	u32 cx=b->width/2;
	u32 cy=b->height/2;
	u32 cw=w/2;
	u32 ch=h/2;

	for(u32 y=0;y<h;y++)
	{
		u32 yoff=(cy-ch+y+y_)*b->width;
		for(u32 x=0;x<w;x++)
		{
			u32 xoff=(cx-cw+x+x_);
			b->ptr[yoff+xoff]=translate(&buf[(y*w+x)*fmt.bytesPerPixel],fmt);
		}
	}
}
