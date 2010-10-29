
#include <sys/memory.h>
#include "common/events.h"

#define PS3_MAX_VKEY_CHARS 256

class PS3VKeyboard
{
public:
	PS3VKeyboard();

	void show();
	void finish();
	void kill();
	bool pollEvent(Common::Event &event);


private:
	sys_memory_container_t _container_id;
	bool _isShown;
	wchar_t _buffer[PS3_MAX_VKEY_CHARS+1];
	char _cbuffer[PS3_MAX_VKEY_CHARS+1];
	int _sent;
	uint64_t _nextsend;
};
