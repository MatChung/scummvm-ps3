/* This file is automatically generated by configure */
/* DO NOT EDIT MANUALLY */

#ifndef CONFIG_H
#define CONFIG_H


#undef SCUMM_LITTLE_ENDIAN
#ifndef SCUMM_BIG_ENDIAN
  #define SCUMM_BIG_ENDIAN
#endif
#undef HAVE_X86
#define SCUMM_NEED_ALIGNMENT
#undef USE_MT32EMU
#undef USE_RGB_COLOR
#undef USE_SCALERS
#undef USE_HQ_SCALERS
#undef USE_INDEO3
#undef USE_VORBIS
#undef USE_TREMOR
#undef USE_FLAC
#undef USE_MAD
#undef USE_ALSA
#undef USE_PNG
#undef USE_THEORADEC
#undef USE_SEQ_MIDI
#undef USE_TIMIDITY
#undef USE_ZLIB
#undef USE_MPEG2
#undef USE_FLUIDSYNTH
#undef USE_READLINE
#undef USE_TEXT_CONSOLE
#undef USE_NASM
#define USE_TRANSLATION
#define USE_DETECTLANG

/* Data types */
typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef signed char int8;
typedef signed short int16;
typedef signed int int32;

/* Plugin settings */
/* -> plugins disabled */

#define getenv(x) NULL
#define DISABLE_TOWNS_DUAL_LAYER_MODE
#include "backends/platform/ps3/netdbg/net.h"
//#define printf(fmt, ...) net_send(fmt, ##__VA_ARGS__)


#endif /* CONFIG_H */
