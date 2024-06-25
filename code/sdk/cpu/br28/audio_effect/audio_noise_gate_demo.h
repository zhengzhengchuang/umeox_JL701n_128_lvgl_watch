#include "media/effects_adj.h"
#include "media/audio_noisegate.h"
#include "audio_eff_default_parm.h"


NOISEGATE_API_STRUCT *audio_noisegate_open_demo(u32 ns_name, u32 sample_rate, u32 ch_num);
void audio_noisegate_close_demo(NOISEGATE_API_STRUCT *hdl);
void audio_noisegate_update_demo(u32 ns_name, noisegate_update_param *parm, u8 bypass);
