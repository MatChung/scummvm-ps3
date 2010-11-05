#include "ps3.h"
#include "base/main.h"
#include <stdio.h>

OSystem *OSystem_PS3_create()
{
	printf("OSystem_PS3::OSystem_PS3_create()\n");

	return new OSystem_PS3();
}


int main(int argc, const char* argv[])
{
	g_system = OSystem_PS3_create();

	assert(g_system);
	

	scummvm_main(argc, argv);

	printf("SHUTDOWN!\n");
	g_system->quit();

	return 0;
}
