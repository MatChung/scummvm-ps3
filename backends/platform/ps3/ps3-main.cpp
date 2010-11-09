#include "ps3.h"
#include "base/main.h"
#include <stdio.h>
#include <psl1ght/lv2/timer.h>
#include "graphics/fb/fb.h"
#include <lv2/process.h>

OSystem *OSystem_PS3_create()
{
	printf("OSystem_PS3::OSystem_PS3_create()\n");

	return new OSystem_PS3();
}


int main(int argc, const char* argv[])
{
	printf("INIT!\n");

	g_system = OSystem_PS3_create();
	assert(g_system);
	
	scummvm_main(argc, argv);

	printf("SHUTDOWN!\n");
	g_system->quit();

	//sysProcessExitSpawn2(SELF_PATH, (const char**)launchargv, (const char**)launchenvp, NULL, 0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M);
	return 0;
}
