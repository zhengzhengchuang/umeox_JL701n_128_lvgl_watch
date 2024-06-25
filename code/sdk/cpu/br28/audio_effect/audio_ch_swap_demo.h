#ifndef __AUDIO_CH_SWAP_DEMO__H
#define __AUDIO_CH_SWAP_DEMO__H


#include "media/effects_adj.h"
#include "media/audio_ch_swap.h"



struct audio_ch_swap *audio_ch_swap_open_demo(u32 swap_name, u8 channel);
void audio_ch_swap_close_demo(struct audio_ch_swap *hdl);
void audio_ch_swap_update_parm(u16 swap_name, void *parm, int bypass);
#endif
