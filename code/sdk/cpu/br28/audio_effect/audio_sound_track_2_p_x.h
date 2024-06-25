#ifndef _SOUND_TRACK_2_P_X_H_
#define _SOUND_TRACK_2_P_X_H_

#include "app_config.h"
#include "clock_cfg.h"
#include "media/includes.h"
#include "asm/includes.h"
#include "media/eq_config.h"
#include "media/effects_adj.h"
#include "audio_effect/audio_eff_default_parm.h"
#include "application/audio_vocal_tract_synthesis.h"
#include "audio_config.h"

//低通总增益调节
void low_bass_set_global_gain(float left_global_gain, float right_global_gain);
//高阶低通滤波器系数回调
int low_bass_eq_get_filter_info_demo(void *_eq, int sr, struct audio_eq_filter_info *info);

//左右声道叠加后除2,再对左或者右声道做反相处理
int stream_rl_rr_mix_data_handler(struct audio_stream_entry *entry,  struct audio_data_frame *in);
#endif/*_SOUND_TRACK_2_P_X_H_*/




