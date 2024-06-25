#ifndef __AUDIO_SURROUND_DEMO__H
#define __AUDIO_SURROUND_DEMO__H
#include "media/effects_adj.h"
#include "audio_effect/audio_eff_default_parm.h"
#include "media/audio_surround.h"
#include "app_config.h"
#include "clock_cfg.h"
#include "audio_dec.h"

surround_hdl *surround_open_demo(u32 sur_name, u8 ch_type);
void surround_close_demo(surround_hdl *surround);
void audio_surround_effect_update_demo(u32 sur_name, surround_update_parm *parm, u8 bypass);

void audio_surround_effect_switch_demo(u32 sur_name, u8 eff_mode);

#endif
