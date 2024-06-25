#include "audio_noise_gate_demo.h"

#if (defined(MUSIC_NOISE_GATE_EN)&&MUSIC_NOISE_GATE_EN)  || (defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE)
#undef AUDIO_NOISE_GATE_ENABLE
#define AUDIO_NOISE_GATE_ENABLE 1
#endif

#ifndef AUDIO_NOISE_GATE_ENABLE
#define AUDIO_NOISE_GATE_ENABLE 0
#endif
#if MUSIC_NOISE_GATE_EN
NoiseGateParam_TOOL_SET music_noisegate_parm[mode_add];
#endif
NOISEGATE_API_STRUCT *audio_noisegate_open_demo(u32 ns_name, u32 sample_rate, u32 ch_num)
{
#if AUDIO_NOISE_GATE_ENABLE
    NoiseGateParam_TOOL_SET *parm = NULL;

#if MUSIC_NOISE_GATE_EN
    if (ns_name == AEID_MUSIC_NS_GATE) {
        parm = &music_noisegate_parm[nor_label];
    } else if (ns_name == AEID_AUX_NS_GATE) {
        parm = &music_noisegate_parm[aux_label];
    }
#endif
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    if (ns_name == AEID_MIC_NS_GATE) {
        u8 mode = get_mic_eff_mode();
        struct eff_parm *mic_eff = &eff_mode[mode];
        parm = 	&mic_eff->noise_gate_parm;
    }
#endif


    if (!parm) {
        return NULL;
    }
    NoiseGateParam noisegate_parm = {0};
    noisegate_parm.attackTime = parm->parm.attackTime;
    noisegate_parm.releaseTime = parm->parm.releaseTime;
    noisegate_parm.threshold = parm->parm.threshold;
    noisegate_parm.low_th_gain = parm->parm.low_th_gain;
    noisegate_parm.sampleRate = sample_rate;
    noisegate_parm.channel = ch_num;

    NOISEGATE_API_STRUCT *hdl = audio_noisegate_open(ns_name,  &noisegate_parm);
    audio_noisegate_bypass(ns_name, parm->is_bypass);
    return hdl;
#else
    return NULL;
#endif
}

void audio_noisegate_close_demo(NOISEGATE_API_STRUCT *hdl)
{
#if AUDIO_NOISE_GATE_ENABLE
    audio_noisegate_close(hdl);
#endif
}

void audio_noisegate_update_demo(u32 ns_name, noisegate_update_param *parm, u8 bypass)
{
#if AUDIO_NOISE_GATE_ENABLE
    audio_noisegate_update(ns_name, parm);
    audio_noisegate_bypass(ns_name, bypass);
#endif
}
