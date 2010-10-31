#pragma once

#include "../ps3.h"
#include <cell/audio.h>
#include <sys/sys_time.h>


class PS3Sound
{
public:
	PS3Sound(Audio::MixerImpl *sys);
	void init();
	void play();


private:
	bool playOneBlock();

	byte *_buffer;
	Audio::MixerImpl *_mixer;
	system_time_t _lastquery;

	uint32_t _audio_port;
	float *_audio_buffer;
	float *_audio_buffer_end;
	sys_addr_t _audio_read_index_addr;
	unsigned int _audio_block_index;
	unsigned int _audio_last_read_block;
	uint32_t _stored_blocks;
};
