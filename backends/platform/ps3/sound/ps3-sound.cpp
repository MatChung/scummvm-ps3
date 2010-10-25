#include "ps3-sound.h"

#define AUDIO_CHANNEL_COUNT CELL_AUDIO_PORT_2CH
#define AUDIO_BLOCK_COUNT CELL_AUDIO_BLOCK_32
//snes uses 512?
#define AUDIO_BLOCK_SIZE 256


PS3Sound::PS3Sound(Audio::MixerImpl *mixer)
{
	_mixer=mixer;
	_lastquery=0;
	_audio_last_read_block=0xFFFFFFFF;
	_audio_block_index=0;
	_buffer=new byte[AUDIO_BLOCK_SIZE*4];
}


void PS3Sound::init()
{
	net_send("PS3Sound::init()\n");
	CellAudioPortParam params;
	CellAudioPortConfig config;

	cellAudioInit();

	params.nChannel = AUDIO_CHANNEL_COUNT;
	params.nBlock = AUDIO_BLOCK_COUNT;
	params.attr = 0;

	int ret=cellAudioPortOpen(&params, &_audio_port);
	if (ret != CELL_OK)
	{
		net_send("  cellAudioPortOpen() failed\n");
		cellAudioQuit();
		return;
	}

	cellAudioGetPortConfig(_audio_port, &config);
	_audio_buffer = (float *)config.portAddr;
	_audio_buffer_end = (float *)(config.portAddr + config.portSize);
	_audio_read_index_addr = config.readIndexAddr;

	cellAudioPortStart(_audio_port);
	net_send("  success: %X,%X,%X\n",_audio_buffer,_audio_buffer_end,_audio_read_index_addr);
}

void PS3Sound::play()
{
	while(playOneBlock());
}
//static int __counterr=0;
bool PS3Sound::playOneBlock()
{
	unsigned int current_block = *(uint64_t *)_audio_read_index_addr;

	if (current_block == _audio_last_read_block)
	{
		//__counterr++;
		return false;
	}
	if(_audio_block_index != current_block)
	{
		//__counterr++;
		return false;
	}

	//net_send("PS3Sound::playOneBlock(%d)\n",__counterr);
	//__counterr=0;

	_mixer->mixCallback(_buffer,AUDIO_BLOCK_SIZE*4);
	int16 *samples = (int16 *)_buffer;
	//while (S9xGetSampleCount() >= 512 && _audio_block_index != current_block)
	{
		//S9xMixSamples((uint8 *)samples, 256 * 2);

		float *buf = _audio_buffer + AUDIO_CHANNEL_COUNT * AUDIO_BLOCK_SIZE * _audio_block_index;
		for (unsigned int i = 0; i < AUDIO_BLOCK_SIZE * AUDIO_CHANNEL_COUNT; ++i)
		{
			buf[i] = ((float)samples[i])/32768.0f;
		}

		_audio_block_index = (_audio_block_index + 1) % AUDIO_BLOCK_COUNT;
	}

	_audio_last_read_block = current_block;

	//net_send("  current_block=%d\n",current_block);
	//net_send("  _audio_last_read_block=%d\n",_audio_last_read_block);
	//net_send("  _audio_block_index=%d\n",_audio_block_index);
	//net_send("  _audio_last_read_block=%d\n",_audio_last_read_block);
	//net_send("  _audio_last_read_block=%d\n",_audio_last_read_block);

	return true;
}
