#ifndef __ESCO_SYCNTS__H
#define __ESCO_SYCNTS__H
#include "asm/dac.h"
#include "media/includes.h"
#include "asm/gpio.h"
#include "media/audio_syncts.h"
#include "media/bt_audio_timestamp.h"
#include "media/a2dp_syncts.h"


int esco_decoder_syncts_setup(struct decoder_syncts *hdl, u32 in_sample_rate, u32 out_sample_rate, u8 ch_num, u32 esco_len);
void esco_decoder_syncts_free(struct decoder_syncts *hdl);
int esco_decoder_set_timestamp(struct audio_decoder *decoder, u16 hash);
#endif




