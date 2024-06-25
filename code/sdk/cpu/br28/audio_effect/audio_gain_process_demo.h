#ifndef __AUDIO_GAIN_DEMO__H
#define __AUDIO_GAIN_DEMO__H


#include "media/effects_adj.h"
#include "media/audio_gain_process.h"

struct aud_gain_process *audio_gain_open_demo(u16 gain_name, u8 channel);
void audio_gain_close_demo(struct aud_gain_process *hdl);
void audio_gain_update_parm(u16 gain_name, void *parm, int bypass);
#endif
