#include "ps3.h"
// PS3 includes
#include <PSGL/psgl.h>
#include <sys/spu_initialize.h>
#include <sys/paths.h>
#include <cell/sysmodule.h>

#include <cell/pad.h>
#include "base/main.h"
#include "netdbg/net.h"

float pos[] = { -1,-1,0,  -1,1,0,  1,-1,0,  1,1,0, };
static bool want_to_quit = false;	// true when I need to quit

void drawAxisAlignedLine(float cntrX, float cntrY, float halfWidth, float halfHeight)
{
	glPushMatrix();
	glTranslatef(cntrX,cntrY,0);
	glScalef(halfWidth,halfHeight,1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glPopMatrix();
}

void drawGrid(const int numLines)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, pos);

	const float hw=0.005f, hh=1.0f;
	float dwh = 2.0f/numLines;

	glColor4f(1,1,1,1);
	for (float x=-1+(dwh*0.5f); x<1; x+=dwh)
		drawAxisAlignedLine(x,0,hw,hh);
	for (float y=-1+(dwh*0.5f); y<1; y+=dwh)
		drawAxisAlignedLine(0,y,hh,hw);

	glColor4f(1,0,0,1);
	drawAxisAlignedLine(-1,0,hw,hh);
	drawAxisAlignedLine(1,0,hw,hh);
	drawAxisAlignedLine(0,-1,hh,hw);
	drawAxisAlignedLine(0,1,hh,hw);
	drawAxisAlignedLine(0,0,hw*2,hw*2);
}

void drawRotatingGrid(const int numSteps)
{
	float zRot=0;
	for (int j=0; j<numSteps; j++)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
		glRotatef(zRot,0,0,1);

		drawGrid(20);
		zRot+=0.01f; if (zRot>360) zRot-=360;

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		psglSwap();
	}
}

void initGraphics(PSGLdevice *device)
{
	// get render target buffer dimensions and set viewport
	GLuint renderWidth, renderHeight;
	psglGetRenderBufferDimensions(device,&renderWidth,&renderHeight);

	glViewport(0, 0, renderWidth, renderHeight);

	// get display aspect ratio (width / height) and set projection
	// (it is important to use this value and NOT renderWidth/renderHeight since
	// pixel ratios do not necessarily match the 16/9 or 4/3 display aspect ratios)
	GLfloat aspectRatio = psglGetDeviceAspectRatio(device);  

	float l=aspectRatio, r=-l, b=-1, t=1;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(l,r,b,t,0,1);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glDisable(GL_CULL_FACE);

	// PSGL doesn't clear the screen on startup, so let's do that here.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	psglSwap();
}

// Given an priority ordered array of desired resolutions (from most desired to least), chooses
// the first mode that is available on the current display device.
//
// The list of modes are chosen from the following standard modes:
//   
//   CELL_VIDEO_OUT_RESOLUTION_480   (720x480)
//   CELL_VIDEO_OUT_RESOLUTION_576   (720x576)
//   CELL_VIDEO_OUT_RESOLUTION_720   (1280x720)
//   CELL_VIDEO_OUT_RESOLUTION_1080  (1920x1080)
//
// or these modes that allow a lower resolution buffer to automatically be 
// upscaled (in hardware) to the 1920x1080 standard:
//
//   CELL_VIDEO_OUT_RESOLUTION_1600x1080
//   CELL_VIDEO_OUT_RESOLUTION_1440x1080
//   CELL_VIDEO_OUT_RESOLUTION_1280x1080
//   CELL_VIDEO_OUT_RESOLUTION_960x1080
//
// if none of the desired resolutions are available, 0 is returned
//
// example, choose 1920x1080 if possible, but otherwise try for 1280x720:
//
//   unsigned int resolutions[] = { CELL_VIDEO_OUT_RESOLUTION_1080, CELL_VIDEO_OUT_RESOLUTION_720 };
//   int numResolutions = 2;
static int chooseBestResolution(const unsigned int *resolutions, unsigned int numResolutions)
{
	unsigned int bestResolution=0;
	for (unsigned int i=0; bestResolution==0 && i<numResolutions; i++)
		if( cellVideoOutGetResolutionAvailability(CELL_VIDEO_OUT_PRIMARY,resolutions[i],CELL_VIDEO_OUT_ASPECT_AUTO,0) )
			bestResolution = resolutions[i];
	return bestResolution;
}

// Given one of the valid video resolution IDs, assigns the associated dimensions in w and h.
// If the video resolution ID is invalid, 0 is returned, 1 if valid
static int getResolutionWidthHeight(const unsigned int resolutionId, unsigned int &w, unsigned int &h)
{
	switch(resolutionId)
	{
	case CELL_VIDEO_OUT_RESOLUTION_480       : w=720;  h=480;  return(1);
	case CELL_VIDEO_OUT_RESOLUTION_576       : w=720;  h=576;  return(1);
	case CELL_VIDEO_OUT_RESOLUTION_720       : w=1280; h=720;  return(1);
	case CELL_VIDEO_OUT_RESOLUTION_1080      : w=1920; h=1080; return(1);
	case CELL_VIDEO_OUT_RESOLUTION_1600x1080 : w=1600; h=1080; return(1);
	case CELL_VIDEO_OUT_RESOLUTION_1440x1080 : w=1440; h=1080; return(1);
	case CELL_VIDEO_OUT_RESOLUTION_1280x1080 : w=1280; h=1080; return(1);
	case CELL_VIDEO_OUT_RESOLUTION_960x1080  : w=960;  h=1080; return(1);
	};
	printf("getResolutionWidthHeight: resolutionId %d not a valid video mode\n",resolutionId);
	return(0);
}

// Checks if the video output device is ready for initialization by psglInit.
// Call this before calling psglInit, until it returns true. This is mainly used to make
// sure HDMI devices are turned on and connected before calling psglInit. psglInit
// will busy wait until the device is ready, so repeatedly calling this allows 
// processing while waiting. For non-HDMI devices, this routine always returns true.
bool videoOutIsReady()
{
	CellVideoOutState videoState;
	cellVideoOutGetState(CELL_VIDEO_OUT_PRIMARY, 0, &videoState);
	return( videoState.state==CELL_VIDEO_OUT_OUTPUT_STATE_ENABLED );
}

void initModules()
{
	int ret;
	ret = cellSysmoduleLoadModule (CELL_SYSMODULE_FS);
}

void shutdownModules()
{
	int ret;
	ret = cellSysmoduleUnloadModule (CELL_SYSMODULE_FS);
}








static PSGLcontext *context=NULL;
static PSGLdevice *device=NULL;


static void gfxSysutilCallback(uint64_t status, uint64_t param,
			       void *userdata)
{
	(void) param;
	(void) userdata;
	switch (status) {
	case CELL_SYSUTIL_REQUEST_EXITGAME:
		printf("CELL_SYSUTIL_REQUEST_EXITGAME");
		printf("CELL_SYSUTIL_REQUEST_EXITGAME");
		printf("CELL_SYSUTIL_REQUEST_EXITGAME");
		printf("CELL_SYSUTIL_REQUEST_EXITGAME");
		printf("CELL_SYSUTIL_REQUEST_EXITGAME");
		printf("CELL_SYSUTIL_REQUEST_EXITGAME");
		printf("CELL_SYSUTIL_REQUEST_EXITGAME");
		printf("CELL_SYSUTIL_REQUEST_EXITGAME");
		want_to_quit = true;
		psglDestroyContext(context);
		psglDestroyDevice(device);
		psglExit();
		net_shutdown();
		shutdownModules();
		exit(0);
		break;
	case CELL_SYSUTIL_DRAWING_BEGIN:
	case CELL_SYSUTIL_DRAWING_END:
		break;
	default:
		printf
		    ("Graphics common: Unknown status received: 0x%llx\n",
		     status);
	}
}




#include "netdbg/net.h"


void initGL()
{
	// Check if the video output device is ready BEFORE calling psglInit.
	// This will make sure HDMI devices are turned on and connected, and allow
	// background processing while waiting (psglInit will busy wait until ready).
	// Non-HDMI video out is always "ready", so they pass through this loop trivially.
	while(!videoOutIsReady())
	{
		// do your background processing here until the video is ready
		net_send("video not ready!\n");
	};
	net_send("VIDEO READY!\n");

	PSGLinitOptions initOpts = 
	{
enable: PSGL_INIT_MAX_SPUS | PSGL_INIT_INITIALIZE_SPUS,
maxSPUs: 1,
initializeSPUs: false,
persistentMemorySize: 0,
transientMemorySize: 0,
errorConsole: 0,
fifoSize: 1* 1024*1024,  
hostMemorySize: 64* 1024*1024,  // 128 mbs for host memory 
	};
	psglInit(&initOpts);

	// (1) create array of all desired resolutions in priority order (most desired to least).
	//     In this example, we choose 1080p if possible, then 960x1080-to-1920x1080 horizontal scaling,
	//     and in the worst case, 720p.
	const unsigned int resolutions[] = { CELL_VIDEO_OUT_RESOLUTION_1080, CELL_VIDEO_OUT_RESOLUTION_960x1080, CELL_VIDEO_OUT_RESOLUTION_720, CELL_VIDEO_OUT_RESOLUTION_576, CELL_VIDEO_OUT_RESOLUTION_480 };
	const int numResolutions = sizeof(resolutions)/sizeof(resolutions[0]);

	// (2) loop through the modes and grab the first available
	int bestResolution = chooseBestResolution(resolutions,numResolutions);

	// (3) get the chosen video mode's pixel dimensions
	unsigned int deviceWidth=0, deviceHeight=0;
	getResolutionWidthHeight(bestResolution,deviceWidth,deviceHeight);

	// (3) if desired resolution is available, create the PSGL device and context

	if (bestResolution)
	{
		printf("%d x %d is available...\n",deviceWidth,deviceHeight);

		// (4) create the PSGL device based on the selected resolution mode
		PSGLdeviceParameters params;
		params.enable = PSGL_DEVICE_PARAMETERS_COLOR_FORMAT | PSGL_DEVICE_PARAMETERS_DEPTH_FORMAT | PSGL_DEVICE_PARAMETERS_MULTISAMPLING_MODE;
		params.colorFormat = GL_ARGB_SCE;
		params.depthFormat = GL_DEPTH_COMPONENT24;
		params.multisamplingMode = GL_MULTISAMPLING_NONE_SCE;

		params.enable |= PSGL_DEVICE_PARAMETERS_WIDTH_HEIGHT;
		params.width = deviceWidth;
		params.height = deviceHeight;

		device = psglCreateDeviceExtended(&params);

		// (5) create context
		context = psglCreateContext();
		psglMakeCurrent(context, device);
		psglResetCurrentContext();

		// Init PSGL and draw
		initGraphics(device);

		// Destroy the context, then the device (before psglExit)
	}
	else
	{
		printf("%d x %d is NOT available...\n",deviceWidth,deviceHeight);
		shutdownModules();
		exit(0);
	}
}

static CellPadInfo info;
void initInput()
{
	int ret;
	
	ret=cellPadInit(7);

	ret=cellPadGetInfo(&info);

	for(int i=0;i<7;i++)
	{
		ret=cellPadClearBuf(i);
	}
}


void shutdownInput()
{
	cellPadEnd();
}


OSystem *OSystem_PS3_create()
{
	printf("OSystem_PS3::OSystem_PS3_create()\n");
	return new OSystem_PS3();
}


void setupHomeDir(const char *argv0);

int main(int argc, char *argv[])
{
	net_init();

	setupHomeDir(argv[0]);

	#ifdef SCUMM_BIG_ENDIAN
	net_send("SCUMM_BIG_ENDIAN!\n");
	#endif
	#ifdef SCUMM_LITTLE_ENDIAN
	net_send("SCUMM_LITTLE_ENDIAN!\n");
	#endif

	for(int i=0;i<=argc;i++)
	{
		printf("%s\n",argv[i]);
	}

	net_send("STARTUP!\n");
	sys_spu_initialize(6, 1);
	initModules();

	initGL();
	initInput();

	//drawRotatingGrid(1000);


	g_system = OSystem_PS3_create();
	assert(g_system);

	cellSysutilRegisterCallback(0, gfxSysutilCallback, NULL);

	// Invoke the actual ScummVM main entry point:
	int res = scummvm_main(argc, argv);
	//g_system->quit();       // TODO: Consider removing / replacing this!

	psglDestroyContext(context);
	psglDestroyDevice(device);
	psglExit();
	net_shutdown();
	shutdownModules();
	exit(res);
}
