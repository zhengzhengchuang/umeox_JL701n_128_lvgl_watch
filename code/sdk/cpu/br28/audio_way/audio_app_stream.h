

#ifndef _AUDIO_APP_STREAM_H_
#define _AUDIO_APP_STREAM_H_

#include "audio_stream.h"

void audio_app_stream_init(void);
struct audio_stream_entry *audio_app_stream_get_entry(void);
u32 audio_app_stream_get_out_way(void);

int audio_app_stream_switch_way(u32 close_way, u32 open_way);

void audio_reverb_stream_dac_init(void);
struct audio_stream_entry *audio_reverb_stream_dac_get_entry(void);
void audio_reverb_stream_dac_uninit(void);
#endif /*_AUDIO_APP_STREAM_H_*/

