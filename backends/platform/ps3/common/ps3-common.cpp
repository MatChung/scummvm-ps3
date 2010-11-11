#include "../ps3.h"
#include <psl1ght/lv2/timer.h>
#include <time.h>
#include <lv2/time.h>


bool OSystem_PS3::pollEvent(Common::Event &event)
{
	bool ret=false;
	ret=_pad.pollEvent(event);
	if(ret==true)
	{
		//net_send("OSystem_PS3::pollEvent() got Event: %d,%d,%d\n",event.type,event.mouse.x,event.mouse.y);

		if(event.type==Common::EVENT_MOUSEMOVE)
		{
			_mouse_pos.x=event.mouse.x;
			_mouse_pos.y=event.mouse.y;
			//warpMouse(event.mouse.x,event.mouse.y);
		}

		return true;
	}

	return false;
	//printf("OSystem_PS3::pollEvent()\n");
	//TODO: Eventqueue
	return false;
}

uint32 OSystem_PS3::getMillis()
{
	//printf("OSystem_PS3::getMillis()\n");
	return sys_time_get_system_time()/1000;
}

void OSystem_PS3::delayMillis(uint msecs)
{
	//printf("OSystem_PS3::delayMillis(%d)\n",msecs);
	lv2TimerUSleep(msecs*1000);
}

OSystem::MutexRef OSystem_PS3::createMutex()
{
	printf("OSystem_PS3::createMutex()\n");
	//TODO: threads
	return NULL;
}

void OSystem_PS3::lockMutex(MutexRef mutex)
{
	printf("OSystem_PS3::lockMutex()\n");
	//TODO: threads
}

void OSystem_PS3::unlockMutex(MutexRef mutex)
{
	printf("OSystem_PS3::unlockMutex()\n");
	//TODO: threads
}

void OSystem_PS3::deleteMutex(MutexRef mutex)
{
	printf("OSystem_PS3::deleteMutex()\n");
	//TODO: threads
}

Common::SaveFileManager *OSystem_PS3::getSavefileManager()
{
	printf("OSystem_PS3::getSavefileManager()\n");
	return _savefile;
}

Audio::Mixer *OSystem_PS3::getMixer()
{
	printf("OSystem_PS3::getMixer()\n");
	return _mixer;
}

void OSystem_PS3::getTimeAndDate(TimeDate &t) const
{
	printf("OSystem_PS3::getTimeAndDate()\n");
/*
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	t.tm_hour=timeinfo->tm_hour;
	t.tm_mday=timeinfo->tm_mday;
	t.tm_min=timeinfo->tm_min;
	t.tm_mon=timeinfo->tm_mon;
	t.tm_sec=timeinfo->tm_sec;
	t.tm_year=timeinfo->tm_year;
	*/
}

Common::TimerManager *OSystem_PS3::getTimerManager()
{
	printf("OSystem_PS3::getTimerManager()\n");
	return _timer;
}

FilesystemFactory *OSystem_PS3::getFilesystemFactory()
{
	printf("OSystem_PS3::getFilesystemFactory()\n");
	return _fsFactory;
}
