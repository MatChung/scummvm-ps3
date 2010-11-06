#include "../ps3.h"
#include "../graphics/fb/fb.h"
#include "backends/saves/default/default-saves.h"
#include "backends/timer/default/default-timer.h"
#include "../filesystem/ps3-fs-factory.h"

Common::List<Graphics::PixelFormat> __formats;

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

	printf("  OSystem::initBackend()\n");
	OSystem::initBackend();
	printf("  initBackend FINISHED\n");
}

void OSystem_PS3::quit()
{
	printf("OSystem_PS3::quit()\n");
	exit(0);
}
