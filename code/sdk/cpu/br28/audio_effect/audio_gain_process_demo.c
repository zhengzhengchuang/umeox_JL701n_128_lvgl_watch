#include "audio_gain_process_demo.h"
#include "app_config.h"
#include "audio_eff_default_parm.h"

#if defined(GAIN_PROCESS_EN)&&GAIN_PROCESS_EN
#if TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
Gain_Process_TOOL_SET rl_gain_parm[mode_add];//rl通道gain parm
Gain_Process_TOOL_SET gain_parm[mode_add];//音乐模式尾部的增益调节
#endif
#if AUDIO_VBASS_CONFIG
Gain_Process_TOOL_SET vbass_prev_gain_parm[mode_add];
#endif
struct aud_gain_process *audio_gain_open_demo(u16 gain_name, u8 channel)
{
    /* if ((gain_name != AEID_MUSIC_VBASS_PREV_GAIN) && (gain_name != AEID_AUX_VBASS_PREV_GAIN)) { */
    /* return NULL; */
    /* } */
    if (gain_name == AEID_MIC_GAIN) {
        return NULL;
    }
    struct aud_gain_parm parm = {0};

    float gain[4] = {0};
    u8 bypass = 0;
    u8 tar = 0;
    if (gain_name == AEID_MUSIC_GAIN) {
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
        gain[0] = gain_parm[0].parm.gain[0];
        gain[1] = gain_parm[0].parm.gain[1];
        gain[2] = gain_parm[0].parm.gain[2];
        gain[3] = gain_parm[0].parm.gain[3];
        bypass = gain_parm[0].is_bypass;
        parm.divide = 2;
#else
        return NULL;
#endif
    } else if (gain_name == AEID_MUSIC_RL_GAIN) {
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
        gain[0] = rl_gain_parm[0].parm.gain[0];
        gain[1] = rl_gain_parm[0].parm.gain[1];
        gain[2] = rl_gain_parm[0].parm.gain[2];
        gain[3] = rl_gain_parm[0].parm.gain[3];
        bypass = rl_gain_parm[0].is_bypass;
        parm.divide = 2;
#else
        return NULL;
#endif
    } else if (gain_name == AEID_AUX_GAIN) {
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
        gain[0] = gain_parm[1].parm.gain[0];
        gain[1] = gain_parm[1].parm.gain[1];
        gain[2] = gain_parm[1].parm.gain[2];
        gain[3] = gain_parm[1].parm.gain[2];
        bypass = gain_parm[1].is_bypass;
        parm.divide = 2;
#else
        return NULL;
#endif
#endif
    } else if (gain_name == AEID_AUX_RL_GAIN) {
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
        gain[0] = rl_gain_parm[1].parm.gain[0];
        gain[1] = rl_gain_parm[1].parm.gain[1];
        gain[2] = rl_gain_parm[1].parm.gain[2];
        gain[3] = rl_gain_parm[1].parm.gain[3];
        bypass = rl_gain_parm[1].is_bypass;
        parm.divide = 2;
#else
        return NULL;
#endif
#endif
    } else if (gain_name == AEID_MIC_GAIN) {
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
#if 0
        u8 mode = get_mic_eff_mode();
        gain[0] = eff_mode[mode].gain_parm.parm.gain[0];
        gain[1] = eff_mode[mode].gain_parm.parm.gain[0];
        bypass = eff_mode[mode].gain_parm.is_bypass;
#endif
#endif
    } else if (gain_name == AEID_MUSIC_VBASS_PREV_GAIN) {
#if AUDIO_VBASS_CONFIG
        gain[0] = vbass_prev_gain_parm[0].parm.gain[0];
        gain[1] = vbass_prev_gain_parm[0].parm.gain[0];
        bypass = vbass_prev_gain_parm[0].is_bypass;
#endif
    } else if (gain_name == AEID_AUX_VBASS_PREV_GAIN) {

#if AUDIO_VBASS_CONFIG
#if LINEIN_MODE_SOLE_EQ_EN
        gain[0] = vbass_prev_gain_parm[1].parm.gain[0];
        gain[1] = vbass_prev_gain_parm[1].parm.gain[0];
        bypass = vbass_prev_gain_parm[1].is_bypass;
#endif
#endif
    }

    parm.gain[0] = gain[0];
    parm.gain[1] = gain[1];
    parm.gain[2] = gain[2];
    parm.gain[3] = gain[3];
    parm.channel = channel;
    parm.indata_inc = (channel == 1) ? 1 : 2;
    parm.outdata_inc = (channel == 1) ? 1 : 2;
    parm.bit_wide = 0; //16bit_wide
    parm.gain_name = gain_name;
    struct aud_gain_process *hdl = audio_gain_process_open(&parm);
    audio_gain_process_bypass(parm.gain_name, bypass);
    return hdl;
}


void audio_gain_close_demo(struct aud_gain_process *hdl)
{
    if (!hdl) {
        return;
    }
    audio_gain_process_close(hdl);
    hdl = NULL;
}

void audio_gain_update_parm(u16 gain_name, void *parm, int bypass)
{
    audio_gain_process_update(gain_name, parm);
    audio_gain_process_bypass(gain_name, bypass);
}
#endif


