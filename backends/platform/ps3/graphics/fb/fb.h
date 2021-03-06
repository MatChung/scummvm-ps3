/*
 * fb.h
 *
 *  Created on: 05.11.2010
 *      Author: lousy
 */

#ifndef FB_H_
#define FB_H_

#include <psl1ght/types.h>
#include <rsx/gcm.h>
#include <rsx/reality.h>
#include <sysutil/video.h>
#include "graphics/pixelformat.h"

typedef struct {
	int height;
	int width;
	uint32_t *ptr;
	// Internal stuff
	uint32_t offset;
} buffer;

extern gcmContextData *context; // Context to keep track of the RSX buffer.

extern VideoResolution res; // Screen Resolution

extern int currentBuffer;
extern buffer *buffers[2]; // The buffer we will be drawing into.


// external for now
void init_screen();
void flip(s32 buffer);
void waitFlip();

void blitPalette(u32 x,u32 y,u32 w,u32 h,u8 *buf, u32 *pal);
void blitRGB(u32 x,u32 y,u32 w,u32 h,u8 *buf,Graphics::PixelFormat &fmt);

#endif /* FB_H_ */
