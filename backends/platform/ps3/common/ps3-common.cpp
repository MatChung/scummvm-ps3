#include "../ps3.h"
#include <psl1ght/lv2/timer.h>
#include <time.h>


bool OSystem_PS3::pollEvent(Common::Event &event)
{
	//TODO: Eventqueue
	return false;
}

uint32 OSystem_PS3::getMillis()
{
	//TODO: msec timer
	return 0;
}

void OSystem_PS3::delayMillis(uint msecs)
{
	lv2TimerUSleep(msecs*1000);
}

OSystem::MutexRef OSystem_PS3::createMutex()
{
	//TODO: threads
	return NULL;
}

void OSystem_PS3::lockMutex(MutexRef mutex)
{
	//TODO: threads
}

void OSystem_PS3::unlockMutex(MutexRef mutex)
{
	//TODO: threads
}

void OSystem_PS3::deleteMutex(MutexRef mutex)
{
	//TODO: threads
}

Common::SaveFileManager *OSystem_PS3::getSavefileManager()
{
	return _savefile;
}

Audio::Mixer *OSystem_PS3::getMixer()
{
	return _mixer;
}

void OSystem_PS3::getTimeAndDate(TimeDate &t) const
{/*
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
	return _timer;
}

FilesystemFactory *OSystem_PS3::getFilesystemFactory()
{
	return _fsFactory;
}
