#include "ps3.h"
#include "base/main.h"
#include <stdio.h>
#include <psl1ght/lv2/timer.h>

OSystem *OSystem_PS3_create()
{
	printf("OSystem_PS3::OSystem_PS3_create()\n");

	return new OSystem_PS3();
}


int main(int argc, const char* argv[])
{
	printf("INIT!\n");
	g_system = OSystem_PS3_create();
	printf("1\n");

	assert(g_system);
	
	printf("1\n");

	lv2TimerUSleep(10*1000*1000);

	scummvm_main(argc, argv);

	printf("SHUTDOWN!\n");
	g_system->quit();

	return 0;
}
