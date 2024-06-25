#ifndef __EFFECT_PARM_H_
#define __EFFECT_PARM_H_

#include "mic_effect.h"
#include "media/effects_adj.h"
#include "audio_effect/audio_voice_changer_demo.h"

void  mic_eff_analyze_data(u8 tar_mode);
int get_eq_module_index(u16 module_name);
int get_drc_module_index(u16 module_name);
void set_mic_reverb_mode_by_id(u8 mode);

u8 get_mic_eff_mode();
#endif
