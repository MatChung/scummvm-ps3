/* ScummVM - Graphic Adventure Engine
*
* ScummVM is the legal property of its developers, whose names
* are too numerous to list here. Please refer to the COPYRIGHT
* file distributed with this source distribution.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
* $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/backends/platform/PS3/PS3.cpp $
* $Id: PS3.cpp 48238 2010-03-11 23:39:51Z fingolfin $
*
*/

#include "ps3.h"
#include "pad.h"


#include <unistd.h>

#include "common/rect.h"
#include "common/events.h"

#include "backends/saves/default/default-saves.h"
#include "backends/timer/default/default-timer.h"
#include <pthread.h>


#include <sys/sys_time.h>
#include <sys/timer.h>

//#include <sysutil/sysutil_sysparam.h>  // used for cellVideoOutGetResolutionAvailability() and videoOutIsReady()


/*
* Include header files needed for the getFilesystemFactory() method.
*/
#include "backends/fs/ps3/ps3-fs-factory.h"



#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netex/net.h>
#include <netex/ifctl.h>
#include <netex/errno.h>


static Common::List<Graphics::PixelFormat> __formats;

OSystem_PS3::OSystem_PS3()
{
	printf("OSystem_PS3()\n");
	_isInitialized=false;
	_savefile = NULL;
	_mixer = NULL;
	_timer = NULL;
	_fsFactory = NULL;
	_shutdownRequested=false;
	
	printf("OSystem_PS3::OSystem_PS3() fs init\n");
	_fsFactory = new Ps3FilesystemFactory();
	printf("OSystem_PS3::OSystem_PS3() fs ready\n");

	//__formats.
}

OSystem_PS3::~OSystem_PS3()
{
	printf("OSystem_PS3::~OSystem_PS3()\n");
	if(_savefile!=NULL)
		delete _savefile;
	if(_mixer!=NULL)
		delete _mixer;
	if(_timer!=NULL)
		delete _timer;
	if(_fsFactory!=NULL)
		delete _fsFactory;
}

void OSystem_PS3::initBackend()
{
	if(_isInitialized)
		return;
	printf("OSystem_PS3::initBackend()!\n");
	graphics.init();
	printf("OSystem_PS3::initBackend()1\n");
	graphics.setGraphicsMode(0);
	printf("OSystem_PS3::initBackend()2\n");


	//_mixer->setReady(false);
	printf("OSystem_PS3::initBackend()7\n");

	_startTime=sys_time_get_system_time();
	printf("sys_time_get_system_time() %d\n",_startTime);

	// Note that both the mixer and the timer manager are useless
	// this way; they need to be hooked into the system somehow to
	// be functional. Of course, can't do that in a PS3 backend :).

	if(_savefile==NULL)
	{
		printf("OSystem_PS3::initBackend() save init\n");
		_savefile = new DefaultSaveFileManager();
		printf("OSystem_PS3::initBackend() save ready\n");
	}

	if(_mixer==NULL)
	{
		printf("OSystem_PS3::initBackend() mixer init\n");
		_mixer = new Audio::MixerImpl(this, 22100);
		assert(_mixer);
		_mixer->setReady(true);
		printf("OSystem_PS3::initBackend() mixer ready\n");
	}
	if(_timer==NULL)
	{
		printf("OSystem_PS3::initBackend() timer init\n");
		_timer = new DefaultTimerManager();
		printf("OSystem_PS3::initBackend() timer ready\n");
	}


	OSystem::initBackend();
	printf("OSystem_PS3::initBackend()9\n");

	_isInitialized=true;
}

Common::List<Graphics::PixelFormat> OSystem_PS3::getSupportedFormats() const
{
	return __formats;
}

Graphics::PixelFormat OSystem_PS3::getScreenFormat() const
{
}

bool OSystem_PS3::hasFeature(Feature f)
{
	printf("OSystem_PS3::hasFeature(%d)\n",f);
	switch(f)
	{
	case kFeatureFullscreenMode:
		return false;
	case kFeatureAspectRatioCorrection:
		return true;
	case kFeatureVirtualKeyboard:
		return false;
		//return true;
	case kFeatureCursorHasPalette:
		return true;
	case kFeatureOverlaySupportsAlpha:
		return false;
	case kFeatureIconifyWindow:
		return false;
	case kFeatureDisableKeyFiltering:
		return false;
	}
	return false;
}

void OSystem_PS3::setFeatureState(Feature f, bool enable)
{
	printf("OSystem_PS3::setFeatureState(%d,%d)\n",f,enable);
}

bool OSystem_PS3::getFeatureState(Feature f)
{
	printf("OSystem_PS3::getFeatureState(%d)\n",f);
	return false;
}

const OSystem::GraphicsMode* OSystem_PS3::getSupportedGraphicsModes() const
{
	printf("OSystem_PS3::getSupportedGraphicsModes()\n");
	return graphics.getSupportedGraphicsModes();
}


int OSystem_PS3::getDefaultGraphicsMode() const
{
	printf("OSystem_PS3::getDefaultGraphicsMode()\n");
	return graphics.getDefaultGraphicsMode();
}

bool OSystem_PS3::setGraphicsMode(const char *mode)
{
	printf("OSystem_PS3::setGraphicsMode()\n");
	return graphics.setGraphicsMode(0);
}

bool OSystem_PS3::setGraphicsMode(int mode)
{
	printf("OSystem_PS3::setGraphicsMode()\n");
	return graphics.setGraphicsMode(0);
}

int OSystem_PS3::getGraphicsMode() const
{
	printf("OSystem_PS3::getGraphicsMode()\n");
	return graphics.getGraphicsMode();
}

void OSystem_PS3::initSize(uint width, uint height, const Graphics::PixelFormat *format)
{
	printf("OSystem_PS3::initSize()\n");
	_pad.setResolution(width,height);
	return graphics.initSize(width,height,format);
}

int16 OSystem_PS3::getHeight()
{
	printf("OSystem_PS3::getHeight()\n");
	return graphics.getHeight();
}

int16 OSystem_PS3::getWidth()
{
	printf("OSystem_PS3::getWidth()\n");
	return graphics.getWidth();
}

void OSystem_PS3::setPalette(const byte *colors, uint start, uint num)
{
	printf("OSystem_PS3::setPalette()\n");
	return graphics.setPalette(colors,start,num);
}

void OSystem_PS3::grabPalette(byte *colors, uint start, uint num)
{
	printf("OSystem_PS3::grabPalette()\n");
	return graphics.grabPalette(colors,start,num);
}

void OSystem_PS3::copyRectToScreen(const byte *buf, int pitch, int x, int y, int w, int h)
{
	printf("OSystem_PS3::copyRectToScreen(%x,%d,%d,%d,%d,%d)\n",buf,pitch,x,y,w,h);
	return graphics.copyRectToScreen(buf,pitch,x,y,w,h);
}

void OSystem_PS3::updateScreen()
{
	//printf("OSystem_PS3::updateScreen()\n");
	return graphics.updateScreen();
}

Graphics::Surface *OSystem_PS3::lockScreen()
{
	printf("OSystem_PS3::lockScreen()\n");
	return graphics.lockScreen();
}

void OSystem_PS3::unlockScreen()
{
	printf("OSystem_PS3::unlockScreen()\n");
	return graphics.unlockScreen();
}

void OSystem_PS3::setShakePos(int shakeOffset)
{
	printf("OSystem_PS3::setShakePos()\n");
	return graphics.setShakePos(shakeOffset);
}

void OSystem_PS3::showOverlay()
{
	printf("OSystem_PS3::showOverlay()\n");
	return graphics.showOverlay();
}

void OSystem_PS3::hideOverlay()
{
	printf("OSystem_PS3::hideOverlay()\n");
	return graphics.hideOverlay();
}

void OSystem_PS3::clearOverlay()
{
	printf("OSystem_PS3::clearOverlay()\n");
	return graphics.clearOverlay();
}

void OSystem_PS3::grabOverlay(OverlayColor *buf, int pitch)
{
	printf("OSystem_PS3::grabOverlay()\n");
	return graphics.grabOverlay(buf,pitch);
}

void OSystem_PS3::copyRectToOverlay(const OverlayColor *buf, int pitch, int x, int y, int w, int h)
{
	printf("OSystem_PS3::copyRectToOverlay()\n");
	return graphics.copyRectToOverlay(buf,pitch,x,y,w,h);
}

int16 OSystem_PS3::getOverlayHeight()
{
	//printf("OSystem_PS3::getOverlayHeight()=%d\n",graphics.getOverlayHeight());
	return graphics.getOverlayHeight();
}

int16 OSystem_PS3::getOverlayWidth()
{
	//printf("OSystem_PS3::getOverlayWidth()=%d\n",graphics.getOverlayWidth());
	return graphics.getOverlayWidth();
}


bool OSystem_PS3::showMouse(bool visible)
{
	printf("OSystem_PS3::showMouse()\n");
	return graphics.showMouse(visible);
	return true;
}

void OSystem_PS3::warpMouse(int x, int y)
{
	//printf("OSystem_PS3::warpMouse()\n");
	return graphics.warpMouse(x,y);
}

void OSystem_PS3::setMouseCursor(const byte *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, int cursorTargetScale, const Graphics::PixelFormat *format)
{
	//printf("OSystem_PS3::setMouseCursor()\n");
	return graphics.setMouseCursor(buf,w,h,hotspotX,hotspotY,keycolor,cursorTargetScale,format);
}


bool OSystem_PS3::pollEvent(Common::Event &event)
{
	bool ret=_pad.pollEvent(event);
	if(ret==true)
	{
		//printf("OSystem_PS3::pollEvent() got Event: %d,%d,%d\n",event.type,event.mouse.x,event.mouse.y);

		if(event.type==Common::EVENT_MOUSEMOVE)
			warpMouse(event.mouse.x,event.mouse.y);

		return true;
	}
	else
	{
		updateFrame();
	}

	if(_shutdownRequested)
	{
		printf("OSystem_PS3::pollEvent(want_to_quit)\n");
		event.type=Common::EVENT_QUIT;
		//_shutdownRequested=false;
		return true;
	}
	//printf("OSystem_PS3::pollEvent()\n");
	return false;
}

uint32 OSystem_PS3::getMillis()
{
	//printf("OSystem_PS3::getMillis()\n");
	return (sys_time_get_system_time()-_startTime)/1000;
}

void OSystem_PS3::delayMillis(uint msecs)
{
	//printf("OSystem_PS3::delayMillis()\n");
	sys_timer_usleep(msecs * 1000);
}

OSystem::MutexRef OSystem_PS3::createMutex(void)
{
	printf("OSystem_PS3::createMutex()");
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

	pthread_mutex_t *mutex = new pthread_mutex_t;
	if (pthread_mutex_init(mutex, &attr) != 0) {
		warning("pthread_mutex_init() failed");
		delete mutex;
		printf("=NULL\n");
		return NULL;
	}
	printf("=%X\n",mutex);
	return (MutexRef)mutex;
}

void OSystem_PS3::lockMutex(MutexRef mutex)
{
	//printf("OSystem_PS3::lockMutex()\n");
	if (pthread_mutex_lock((pthread_mutex_t*)mutex) != 0)
		warning("pthread_mutex_lock() failed");
}

void OSystem_PS3::unlockMutex(MutexRef mutex)
{
	//printf("OSystem_PS3::unlockMutex()\n");
	if (pthread_mutex_unlock((pthread_mutex_t*)mutex) != 0)
		warning("pthread_mutex_unlock() failed");
}

void OSystem_PS3::deleteMutex(MutexRef mutex)
{
	printf("OSystem_PS3::deleteMutex()\n");
	pthread_mutex_t* m = (pthread_mutex_t*)mutex;
	if (pthread_mutex_destroy(m) != 0)
		warning("pthread_mutex_destroy() failed");
	else
		delete m;
}

void OSystem_PS3::quit()
{
	printf("OSystem_PS3::quit()\n");
}

Common::SaveFileManager *OSystem_PS3::getSavefileManager()
{
	printf("OSystem_PS3::getSavefileManager()=%d\n",_savefile);
	//assert(_savefile);
	return _savefile;
}

Audio::Mixer *OSystem_PS3::getMixer()
{
	if(_mixer==NULL)
	{
		printf("OSystem_PS3::initBackend()3\n");
		_mixer = new Audio::MixerImpl(this, 22050);
	}

	printf("OSystem_PS3::getMixer()=%d\n",_mixer);
	//assert(_mixer);
	return _mixer;
}

Common::TimerManager *OSystem_PS3::getTimerManager()
{
	printf("OSystem_PS3::getTimerManager()=%d\n",_timer);
	//assert(_timer);
	return _timer;
}

void OSystem_PS3::getTimeAndDate(TimeDate &t) const
{
	printf("OSystem_PS3::getTimeAndDate()\n");
	time_t now=time(NULL);
	std::tm *time=localtime(&now);

	if(time==NULL)
		return;

	t.tm_sec=time->tm_sec;
	t.tm_min=time->tm_min;
	t.tm_hour=time->tm_hour;
	t.tm_mday=time->tm_mday;
	t.tm_mon=time->tm_mon;
	t.tm_year=time->tm_year;
}

FilesystemFactory *OSystem_PS3::getFilesystemFactory()
{
	printf("OSystem_PS3::getFilesystemFactory()=%d\n",_fsFactory);
	//assert(_fsFactory);
	return _fsFactory;
}

void OSystem_PS3::updateFrame()
{
	_pad.frame();
	//printf("OSystem_PS3::updateFrame()\n");
	if(_timer!=NULL)
		((DefaultTimerManager*)_timer)->handler();

	byte samples[1024];
	int len=1024/4;
	if(_mixer!=NULL)
		_mixer->mixCallback(samples, len);
}


