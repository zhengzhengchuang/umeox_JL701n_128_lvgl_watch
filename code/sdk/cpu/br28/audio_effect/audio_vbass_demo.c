#include "audio_vbass_demo.h"
#include "app_config.h"
#include "audio_eff_default_parm.h"
//24M mips
#if AUDIO_VBASS_CONFIG
VirtualBass_TOOL_SET vbass_parm[mode_add];

vbass_hdl *audio_vbass_open_demo(u32 vbass_name, u32 sample_rate, u8 ch_num)
{
    VirtualBassParam parm = {0};
    u8 tar = 0;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (vbass_name == AEID_AUX_VBASS) {
        tar = 1;
    }
#endif

    u8 bypass  = vbass_parm[tar].is_bypass;
    parm.ratio = vbass_parm[tar].parm.ratio;
    parm.boost = vbass_parm[tar].parm.boost;
    parm.fc    = vbass_parm[tar].parm.fc;
    parm.channel = ch_num;
    parm.SampleRate = sample_rate;
    //printf("vbass ratio %d, boost %d, fc %d, channel %d, SampleRate %d\n", parm.ratio, parm.boost, parm.fc,parm.channel, parm.SampleRate);
    vbass_hdl *vbass = audio_vbass_open(vbass_name, &parm);
    audio_vbass_bypass(vbass_name, bypass);
    clock_add(DEC_VBASS_CLK);
    return vbass;
}


void audio_vbass_close_demo(vbass_hdl *vbass)
{
    if (vbass) {
        audio_vbass_close(vbass);
        vbass = NULL;
    }
    clock_remove(DEC_VBASS_CLK);
}



void audio_vbass_update_demo(u32 vbass_name, VirtualBassUdateParam *parm, u32 bypass)
{
    audio_vbass_parm_update(vbass_name, parm);
    audio_vbass_bypass(vbass_name, bypass);
}



#endif
