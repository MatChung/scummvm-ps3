#ifndef BACKENDS_PLATFORM_PS3_PS3_H
#define BACKENDS_PLATFORM_PS3_PS3_H

#include "pad.h"
#include "backends/base-backend.h"
#include "sound/mixer_intern.h"
#include "graphics/colormasks.h"
#include "graphics/texture.h"
#include <pthread.h>
//#include "graphics/ps3gl.h"

class PS3Sound;

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
	PS3Sound *_sound;
	bool _isInitialized;
	bool _shutdownRequested;

	pthread_t _thread;
	pthread_attr_t _thread_attribs;

	uint16 _tv_screen_width;
	uint16 _tv_screen_height;

public:

	OSystem_PS3(uint16 width, uint16 height);
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
	virtual void disableCursorPalette(bool disable);
	virtual void setCursorPalette(const byte *colors, uint start, uint num);

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


	void update();
	void initGraphics();
	bool running();
	void requestQuit();

protected:
	float zRot;
	unsigned int deviceWidth, deviceHeight;

	GLESTexture* _game_texture;
	GLESPaletteTexture* _game_texture_palette;
	int _shake_offset;
	//Common::Rect _focus_rect;
	bool _full_screen_dirty;

	GLES565Texture* _overlay_texture;
	bool _show_overlay;
	bool _running;

	// Mouse layer
	GLESPaletteTexture* _mouse_texture;
	Common::Point _mouse_hotspot;
	Common::Point _mouse_pos;
	int _mouse_targetscale;
	int _mouse_keycolor;
	bool _show_mouse;
	bool _use_mouse_palette;

	void _setCursorPalette(const byte *colors, uint start, uint num);

	void draw();

	Graphics::PixelFormat _currentScreenFormat;


	GLESTexture *createTextureFromPixelFormat(Graphics::PixelFormat &format);
};

#endif
