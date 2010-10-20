#include "backends/base-backend.h"
#include "graphics/colormasks.h"
#include "video.h"


class PS3GL
{
public:
	PS3GL(){deviceWidth=0;deviceHeight=0;};

	void init();


	const OSystem::GraphicsMode *getSupportedGraphicsModes() const;
	int getDefaultGraphicsMode() const;
	bool setGraphicsMode(const char *name);
	bool setGraphicsMode(int mode);
	int getGraphicsMode() const;
	void initSize(uint width, uint height, const Graphics::PixelFormat *format);
	int16 getHeight();
	int16 getWidth();
	void setPalette(const byte *colors, uint start, uint num);
	void grabPalette(byte *colors, uint start, uint num);
	void copyRectToScreen(const byte *buf, int pitch, int x, int y, int w, int h);
	void updateScreen();
	Graphics::Surface *lockScreen();
	void unlockScreen();
	void setShakePos(int shakeOffset);

	void showOverlay();
	void hideOverlay();
	void clearOverlay();
	void grabOverlay(OverlayColor *buf, int pitch);
	void copyRectToOverlay(const OverlayColor *buf, int pitch, int x, int y, int w, int h);
	int16 getOverlayHeight();
	int16 getOverlayWidth();
	virtual bool showMouse(bool visible);

	virtual void warpMouse(int x, int y);
	virtual void setMouseCursor(const byte *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, int cursorTargetScale, const Graphics::PixelFormat *format);

private:
	int16 _egl_surface_width;
	int16 _egl_surface_height;
	float zRot;
	unsigned int deviceWidth, deviceHeight;

	GLESPaletteTexture* _game_texture;
	int _shake_offset;
	Common::Rect _focus_rect;
	bool _full_screen_dirty;

	GLES565Texture* _overlay_texture;
	bool _show_overlay;

	// Mouse layer
	GLESPaletteATexture* _mouse_texture;
	Common::Point _mouse_hotspot;
	Common::Point _mouse_pos;
	int _mouse_targetscale;
	bool _show_mouse;
	bool _use_mouse_palette;
	bool _force_redraw;

	void _setCursorPalette(const byte *colors, uint start, uint num);

	void draw();
};
