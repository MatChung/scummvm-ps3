#ifndef PS3_H_
#define PS3_H_


#include "backends/base-backend.h"

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
};


#endif /* PS3_H_ */
