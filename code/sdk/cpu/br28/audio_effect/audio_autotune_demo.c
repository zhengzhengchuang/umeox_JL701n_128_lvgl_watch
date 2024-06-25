#include "audio_autotune_demo.h"

#if defined(TCFG_MIC_AUTOTUNE_ENABLE) &&TCFG_MIC_AUTOTUNE_ENABLE
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
#undef AUDIO_AUTIOTUNE_ENABLE
#define AUDIO_AUTIOTUNE_ENABLE 1
#endif
#endif

#ifndef AUDIO_AUTIOTUNE_ENABLE
#define AUDIO_AUTIOTUNE_ENABLE 0
#endif

#if AUDIO_AUTIOTUNE_ENABLE
autotune_hdl *audio_autotune_open_demo(u32 autotune_name, u32 sample_rate)
{
    AUTOTUNE_PARM parm = {0};
    u8 bypass = 0;
    if (autotune_name == AEID_MIC_AUTOTUNE) {
#if defined(TCFG_MIC_AUTOTUNE_ENABLE) &&TCFG_MIC_AUTOTUNE_ENABLE
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
        u8 mode = get_mic_eff_mode();
        memcpy(&parm, &eff_mode[mode].autotune_parm.parm, sizeof(AUTOTUNE_PARM));
        bypass = eff_mode[mode].autotune_parm.is_bypass;
#endif
#endif
    }

    autotune_hdl *hdl = NULL;
    hdl = audio_autotune_open(autotune_name, &parm, sample_rate);
    audio_autotune_bypass(autotune_name, bypass);

    /* void autotune_test(void *p); */
    /* sys_timer_add(NULL, autotune_test, 3000); */
    return hdl;
}

void audio_autotune_close_demo(autotune_hdl *hdl)
{
    audio_autotune_close(hdl);
}


void audio_autotune_update_demo(u32 autotune_name, AUTOTUNE_PARM *parm, u32 bypass)
{
    audio_autotune_update_parm(autotune_name, parm);
    audio_autotune_bypass(autotune_name, bypass);
}


void autotune_test(void *p)
{
    static u8 mode = MODE_C_MAJOR;
    AUTOTUNE_PARM parm = {0};
    parm.mode = mode;
    audio_autotune_update_demo(AEID_MIC_AUTOTUNE, &parm, 0);
    printf("========mode %d\n", mode);
    if (++mode >= MODE_B_MAJOR) {
        mode = MODE_B_MAJOR;
    }

}
#endif
