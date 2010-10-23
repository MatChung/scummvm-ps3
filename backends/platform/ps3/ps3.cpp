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
#include "sound/ps3-sound.h"

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


Common::List<Graphics::PixelFormat> __formats;

void *thread_func(void *attr)
{
	int x=0;
	OSystem_PS3 *sys=(OSystem_PS3*)attr;
	while(true)
	{
		x++;
		if(x%500==0)
			net_send("thread_func\n");
		sys_timer_usleep(1000);
		sys->update();
	}
}


OSystem_PS3::OSystem_PS3(uint16 width, uint16 height)
{
	net_send("OSystem_PS3(%d, %d)\n",width,height);
	_isInitialized=false;
	_savefile = NULL;
	_mixer = NULL;
	_timer = NULL;
	_fsFactory = NULL;
	_shutdownRequested=false;
	_tv_screen_width=width;
	_tv_screen_height=height;
	
	net_send("OSystem_PS3::OSystem_PS3() fs init\n");
	_fsFactory = new Ps3FilesystemFactory();
	net_send("OSystem_PS3::OSystem_PS3() fs ready\n");

	__formats.push_back(Graphics::PixelFormat(4,8,8,8,8,0,8,16,24));
	__formats.push_back(Graphics::PixelFormat(2, 5, 5, 5, 0, 0, 5, 10, 0));


	pthread_create(&_thread,&_thread_attribs,thread_func,this);
}

OSystem_PS3::~OSystem_PS3()
{
	net_send("OSystem_PS3::~OSystem_PS3()\n");
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
	net_send("OSystem_PS3::initBackend()\n");
	if(_isInitialized)
		return;
	net_send("OSystem_PS3::initBackend()!\n");
	initGraphics();
	net_send("OSystem_PS3::initBackend()1\n");
	//graphics.setGraphicsMode(0);
	net_send("OSystem_PS3::initBackend()2\n");


	//_mixer->setReady(false);
	net_send("OSystem_PS3::initBackend()7\n");

	_startTime=sys_time_get_system_time();
	net_send("sys_time_get_system_time() %d\n",(int)_startTime);

	// Note that both the mixer and the timer manager are useless
	// this way; they need to be hooked into the system somehow to
	// be functional. Of course, can't do that in a PS3 backend :).

	if(_savefile==NULL)
	{
		net_send("OSystem_PS3::initBackend() save init\n");
		_savefile = new DefaultSaveFileManager();
		net_send("OSystem_PS3::initBackend() save ready\n");
	}

	if(_mixer==NULL)
	{
		net_send("OSystem_PS3::initBackend() mixer init\n");
		_mixer = new Audio::MixerImpl(this, 48000);
		assert(_mixer);
		_sound=new PS3Sound(_mixer);
		_sound->init();
		_mixer->setReady(true);
		net_send("OSystem_PS3::initBackend() mixer ready\n");
	}
	if(_timer==NULL)
	{
		net_send("OSystem_PS3::initBackend() timer init\n");
		_timer = new DefaultTimerManager();
		net_send("OSystem_PS3::initBackend() timer ready\n");
	}


	OSystem::initBackend();
	net_send("OSystem_PS3::initBackend()9\n");

	_isInitialized=true;
}

bool OSystem_PS3::hasFeature(Feature f)
{
	net_send("OSystem_PS3::hasFeature(%d)\n",f);
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
void OSystem_PS3::setShakePos(int shakeOffset)
{
	if (_shake_offset != shakeOffset) {
		_shake_offset = shakeOffset;
	}
}

void OSystem_PS3::setFeatureState(Feature f, bool enable)
{
	net_send("OSystem_PS3::setFeatureState(%d,%d)\n",f,enable);
}

bool OSystem_PS3::getFeatureState(Feature f)
{
	net_send("OSystem_PS3::getFeatureState(%d)\n",f);

	return false;
}

bool OSystem_PS3::pollEvent(Common::Event &event)
{
	//net_send("OSystem_PS3::pollEvent()\n");
	bool ret=_pad.pollEvent(event);
	if(ret==true)
	{
		//net_send("OSystem_PS3::pollEvent() got Event: %d,%d,%d\n",event.type,event.mouse.x,event.mouse.y);

		if(event.type==Common::EVENT_MOUSEMOVE)
			warpMouse(event.mouse.x,event.mouse.y);

		return true;
	}

	if(_shutdownRequested)
	{
		net_send("OSystem_PS3::pollEvent(want_to_quit)\n");
		event.type=Common::EVENT_QUIT;
		//_shutdownRequested=false;
		return true;
	}
	//net_send("OSystem_PS3::pollEvent()\n");
	return false;
}

uint32 OSystem_PS3::getMillis()
{
	//net_send("OSystem_PS3::getMillis()\n");
	return (sys_time_get_system_time()-_startTime)/1000;
}

void OSystem_PS3::delayMillis(uint msecs)
{
	//if(msecs!=10)
	//	net_send("OSystem_PS3::delayMillis(%d)\n",msecs);
	sys_timer_usleep(msecs * 1000);
}

OSystem::MutexRef OSystem_PS3::createMutex(void)
{
	net_send("OSystem_PS3::createMutex()");
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

	pthread_mutex_t *mutex = new pthread_mutex_t;
	if (pthread_mutex_init(mutex, &attr) != 0) {
		warning("pthread_mutex_init() failed");
		delete mutex;
		net_send("=NULL\n");
		return NULL;
	}
	net_send("=%X\n",(int)mutex);
	return (MutexRef)mutex;
}

void OSystem_PS3::lockMutex(MutexRef mutex)
{
	//net_send("OSystem_PS3::lockMutex()\n");
	int ret=pthread_mutex_lock((pthread_mutex_t*)mutex);
	if (ret != 0)
		net_send("pthread_mutex_lock() failed:%d\n",ret);
}

void OSystem_PS3::unlockMutex(MutexRef mutex)
{
	//net_send("OSystem_PS3::unlockMutex()\n");
	int ret=pthread_mutex_unlock((pthread_mutex_t*)mutex);
	if (ret != 0)
		net_send("pthread_mutex_unlock() failed:%d\n",ret);
}

void OSystem_PS3::deleteMutex(MutexRef mutex)
{
	net_send("OSystem_PS3::deleteMutex()\n");
	pthread_mutex_t* m = (pthread_mutex_t*)mutex;
	if (pthread_mutex_destroy(m) != 0)
		warning("pthread_mutex_destroy() failed");
	else
		delete m;
}

void OSystem_PS3::quit()
{
	net_send("OSystem_PS3::quit()\n");
}

Common::SaveFileManager *OSystem_PS3::getSavefileManager()
{
	net_send("OSystem_PS3::getSavefileManager()=%d\n",(int)_savefile);
	//assert(_savefile);
	return _savefile;
}

Audio::Mixer *OSystem_PS3::getMixer()
{
	net_send("OSystem_PS3::getMixer()=%d\n",_mixer);
	//assert(_mixer);
	return _mixer;
}

Common::TimerManager *OSystem_PS3::getTimerManager()
{
	net_send("OSystem_PS3::getTimerManager()=%d\n",_timer);
	//assert(_timer);
	return _timer;
}

void OSystem_PS3::getTimeAndDate(TimeDate &t) const
{
	net_send("OSystem_PS3::getTimeAndDate()\n");
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
	net_send("OSystem_PS3::getFilesystemFactory()=%d\n",_fsFactory);
	//assert(_fsFactory);
	return _fsFactory;
}


byte samples[1024*1024];
int len=1024*1024/4;
uint32 lastquery=0;

void OSystem_PS3::update()
{
	//net_send("OSystem_PS3::updateFrame()\n");
	_pad.frame();
	if(_timer!=NULL)
		((DefaultTimerManager*)_timer)->handler();

	if(lastquery==0)
		lastquery=getMillis();

	if(_sound!=NULL)
	{
		_sound->play();
	}
}


