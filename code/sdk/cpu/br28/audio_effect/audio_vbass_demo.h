
#ifndef _AUDIO_VBASS_DEMO_H
#define _AUDIO_VBASS_DEMO_H

#include "audio_vbass_demo.h"
#include "media/effects_adj.h"
#include "audio_effect/audio_eff_default_parm.h"
#include "app_config.h"
#include "media/audio_vbass.h"
#include "clock_cfg.h"


vbass_hdl *audio_vbass_open_demo(u32 vbass_name, u32 sample_rate, u8 ch_num);
void audio_vbass_close_demo(vbass_hdl *vbass);
void audio_vbass_update_demo(u32 vbass_name, VirtualBassUdateParam *parm, u32 bypass);


#endif
