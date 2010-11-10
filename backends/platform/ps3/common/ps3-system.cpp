#include "../ps3.h"
#include "../graphics/fb/fb.h"
#include "backends/saves/default/default-saves.h"
#include "backends/timer/default/default-timer.h"
#include "../filesystem/ps3-fs-factory.h"
#include "graphics/colormasks.h"
#include <lv2/time.h>
#include <psl1ght/lv2/thread.h>
#include <unistd.h>

Common::List<Graphics::PixelFormat> __formats;

static void thread_func(u64 arg)
{
	printf("thread_func START!\n");
	OSystem_PS3 *sys=(OSystem_PS3*)arg;

	u64 c=sys_time_get_system_time();

	while(sys->running())
	{
		u64 n=sys_time_get_system_time();

		if(n-10*1000>c)//every 10ms
		{
			c=n;
			sys->threadUpdate();
		}
		// reduce this if there are sound problems (MT32 is still too CPU heavy)
		//sys_timer_usleep(10);
		//sys->soundUpdate();
	}

	printf("thread_func EXIT!\n");
	printf("thread_func EXIT!\n");
	printf("thread_func EXIT!\n");

	sys_ppu_thread_exit(0);
}



OSystem_PS3::OSystem_PS3()
{
	_savefile = NULL;
	_mixer = NULL;
	_timer = NULL;
	_mouse_keycolor=255;
	_use_mouse_palette=0;
	_mouse_is_palette=true;

	_fsFactory = new Ps3FilesystemFactory();

	// RGBA8888
	//__formats.push_back(Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0));
	// Palette - most games
	//__formats.push_back(Graphics::PixelFormat::createFormatCLUT8());
	// RGB565 - overlay
	__formats.push_back(Graphics::PixelFormat(2, 5, 6, 5, 0, 11, 5, 0, 0));
	// RGB555 - fmtowns
	//__formats.push_back(Graphics::PixelFormat(2, 5, 5, 5, 0, 10, 5, 0, 0));
	// RGB5551 - unknown
	//__formats.push_back(Graphics::PixelFormat(2, 5, 5, 5, 1, 11, 6, 1, 0));
	// RGB4444 - overlay if alpha enabled
	//__formats.push_back(Graphics::PixelFormat(2, 4, 4, 4, 4, 12, 8, 4, 0));

	sys_ppu_thread_t id;
	u64 thread_arg = (u64)this;
	u32 priority = 1500;
	size_t stack_size = 0x100000;
	char thread_name[] = "My Thread";
	//opd32 opd;

	opd[0]=((u64*)thread_func)[0];
	opd[1]=((u64*)thread_func)[1];
	sys_ppu_thread_create_ex(&id, (opd32*)opd, thread_arg, priority, stack_size, THREAD_JOINABLE, thread_name);
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
	printf("OSystem_PS3::initBackend()\n");
	printf("  init_screen()\n");
	init_screen();
	printf("  ~init_screen()\n");


	if(_savefile==NULL)
	{
		printf("  save init\n");
		_savefile = new DefaultSaveFileManager();
		printf("  ~save init\n");
	}

	if(_mixer==NULL)
	{
		printf("  mixer create\n");
		Audio::MixerImpl *mixer=new Audio::MixerImpl(this, 48000);
		assert(mixer);
		printf("  sound create\n");
		//PS3Sound *sound=new PS3Sound(mixer);
		printf("  sound init\n");
		//sound->init();
		mixer->setReady(true);
		_mixer = mixer;
		//_sound = sound;
		printf("  ~mixer init()\n");
	}
	if(_timer==NULL)
	{
		printf("  timer create\n");
		_timer = new DefaultTimerManager();
		printf("  ~timer create\n");
	}

	Graphics::PixelFormat pp=Graphics::PixelFormat::createFormatCLUT8();
	Graphics::PixelFormat prgb=Graphics::createPixelFormat<565>();
	_overlay_screen.setFormat(prgb);
	_game_screen.setFormat(pp);
	_mouse_screen.setFormat(pp);

	printf("  OSystem::initBackend()\n");
	OSystem::initBackend();
	printf("  initBackend FINISHED\n");
}

void OSystem_PS3::quit()
{
	printf("OSystem_PS3::quit()\n");
	exit(0);
}

void OSystem_PS3::threadUpdate()
{
	_pad.frame();

	if(_timer!=NULL)
		((DefaultTimerManager*)_timer)->handler();
}

