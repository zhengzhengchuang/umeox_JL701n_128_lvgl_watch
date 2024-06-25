#ifndef __AUDIO_AUTOTUNE_DEMO_H
#define __AUDIO_AUTOTUNE_DEMO_H
#include "media/audio_autotune.h"
#include "media/effects_adj.h"
#include "audio_eff_default_parm.h"
#include "effect_parm.h"


autotune_hdl *audio_autotune_open_demo(u32 autotune_name, u32 sample_rate);
void audio_autotune_close_demo(autotune_hdl *hdl);
void audio_autotune_update_demo(u32 autotune_name, AUTOTUNE_PARM *parm, u32 bypass);
#endif
