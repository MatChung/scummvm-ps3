#ifndef PS3_H_
#define PS3_H_


#include "input/ps3-pad.h"
#include "backends/base-backend.h"
#include "sound/mixer_intern.h"
#include "graphics/ps3-surface.h"
#include <psl1ght/types.h>

class OSystem_PS3 : public BaseBackend
{
public:
	//init / quit
	OSystem_PS3();
	virtual ~OSystem_PS3();
	virtual void initBackend();
	virtual void quit();

	//features
	virtual bool hasFeature(Feature f);
	virtual void setFeatureState(Feature f, bool enable);
	virtual bool getFeatureState(Feature f);

	//graphics query
	virtual const GraphicsMode *getSupportedGraphicsModes() const;
	virtual int getDefaultGraphicsMode() const;
	bool setGraphicsMode(const char *name);
	virtual bool setGraphicsMode(int mode);
	virtual int getGraphicsMode() const;
	virtual Common::List<Graphics::PixelFormat> getSupportedFormats() const;
	virtual Graphics::PixelFormat getScreenFormat() const;

	//graphics common
	virtual void initSize(uint width, uint height, const Graphics::PixelFormat *format);
	virtual void updateScreen();
	virtual void setShakePos(int shakeOffset);

	//game screen
	virtual int16 getHeight();
	virtual int16 getWidth();
	virtual void setPalette(const byte *colors, uint start, uint num);
	virtual void grabPalette(byte *colors, uint start, uint num);
	virtual void copyRectToScreen(const byte *buf, int pitch, int x, int y, int w, int h);
	virtual Graphics::Surface *lockScreen();
	virtual void unlockScreen();

	//overlay
	virtual void showOverlay();
	virtual void hideOverlay();
	virtual void clearOverlay();
	virtual void grabOverlay(OverlayColor *buf, int pitch);
	virtual void copyRectToOverlay(const OverlayColor *buf, int pitch, int x, int y, int w, int h);
	virtual int16 getOverlayHeight();
	virtual int16 getOverlayWidth();
	virtual Graphics::PixelFormat getOverlayFormat() const;

	//mouse
	virtual bool showMouse(bool visible);
	virtual void warpMouse(int x, int y);
	virtual void setMouseCursor(const byte *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, int cursorTargetScale, const Graphics::PixelFormat *format);
	virtual void disableCursorPalette(bool disable);
	virtual void setCursorPalette(const byte *colors, uint start, uint num);

	//common system
	virtual bool pollEvent(Common::Event &event);
	virtual uint32 getMillis();
	virtual void delayMillis(uint msecs);
	virtual MutexRef createMutex(void);
	virtual void lockMutex(MutexRef mutex);
	virtual void unlockMutex(MutexRef mutex);
	virtual void deleteMutex(MutexRef mutex);
	virtual Common::SaveFileManager *getSavefileManager();
	virtual Audio::Mixer *getMixer();
	virtual void getTimeAndDate(TimeDate &t) const;
	virtual Common::TimerManager *getTimerManager();
	FilesystemFactory *getFilesystemFactory();

	bool running(){return true;};
	void threadUpdate();
protected:
	Common::SaveFileManager *_savefile;
	Audio::MixerImpl *_mixer;
	Common::TimerManager *_timer;
	FilesystemFactory *_fsFactory;

	int _shake_offset;
	bool _show_overlay;
	Common::Point _mouse_hotspot;
	Common::Point _mouse_pos;
	int _mouse_targetscale;
	int _mouse_keycolor;
	bool _show_mouse;
	bool _use_mouse_palette;
	bool _mouse_is_palette;

	int _current_graphics_mode;
	bool _stretch_to_full;
	bool _aspectRatioCorrection;

	PS3Texture _game_screen;
	PS3Texture _overlay_screen;
	PS3Texture _mouse_screen;

	PS3Pad _pad;

	//helper for thread function
	u32 opd[2];
};


#endif /* PS3_H_ */
