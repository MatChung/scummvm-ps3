#ifndef BACKENDS_PLATFORM_PS3_PS3_H
#define BACKENDS_PLATFORM_PS3_PS3_H

#include "pad.h"
#include "backends/base-backend.h"
#include "sound/mixer_intern.h"
#include "graphics/colormasks.h"
#include "graphics/video.h"
//#include "graphics/ps3gl.h"


class OSystem_PS3 : public BaseBackend
{
protected:
	Common::SaveFileManager *_savefile;
	Audio::MixerImpl *_mixer;
	Common::TimerManager *_timer;
	FilesystemFactory *_fsFactory;

	system_time_t _startTime;

	//PS3GL graphics;
	PS3Pad _pad;
	bool _isInitialized;
	bool _shutdownRequested;

	void updateFrame();
public:

	OSystem_PS3();
	virtual ~OSystem_PS3();

	virtual void initBackend();

	virtual bool hasFeature(Feature f);
	virtual void setFeatureState(Feature f, bool enable);
	virtual bool getFeatureState(Feature f);
	virtual const GraphicsMode *getSupportedGraphicsModes() const;
	virtual int getDefaultGraphicsMode() const;
	bool setGraphicsMode(const char *name);
	virtual bool setGraphicsMode(int mode);
	virtual int getGraphicsMode() const;
	virtual Common::List<Graphics::PixelFormat> getSupportedFormats() const;
	virtual Graphics::PixelFormat getScreenFormat() const;

	virtual void initSize(uint width, uint height, const Graphics::PixelFormat *format);
	virtual int16 getHeight();
	virtual int16 getWidth();
	virtual void setPalette(const byte *colors, uint start, uint num);
	virtual void grabPalette(byte *colors, uint start, uint num);
	virtual void copyRectToScreen(const byte *buf, int pitch, int x, int y, int w, int h);
	virtual void updateScreen();
	virtual Graphics::Surface *lockScreen();
	virtual void unlockScreen();
	virtual void setShakePos(int shakeOffset);

	virtual void showOverlay();
	virtual void hideOverlay();
	virtual void clearOverlay();
	virtual void grabOverlay(OverlayColor *buf, int pitch);
	virtual void copyRectToOverlay(const OverlayColor *buf, int pitch, int x, int y, int w, int h);
	virtual int16 getOverlayHeight();
	virtual int16 getOverlayWidth();
	virtual Graphics::PixelFormat getOverlayFormat() const;

	virtual bool showMouse(bool visible);
	virtual void warpMouse(int x, int y);
	virtual void setMouseCursor(const byte *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, int cursorTargetScale, const Graphics::PixelFormat *format);

	virtual bool pollEvent(Common::Event &event);
	virtual uint32 getMillis();
	virtual void delayMillis(uint msecs);

	virtual MutexRef createMutex(void);
	virtual void lockMutex(MutexRef mutex);
	virtual void unlockMutex(MutexRef mutex);
	virtual void deleteMutex(MutexRef mutex);

	virtual void quit();

	virtual Common::SaveFileManager *getSavefileManager();
	virtual Audio::Mixer *getMixer();
	virtual void getTimeAndDate(TimeDate &t) const;
	virtual Common::TimerManager *getTimerManager();
	FilesystemFactory *getFilesystemFactory();

	virtual bool allowMapping() const { return false; }


	void initGraphics();

protected:
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

#endif
