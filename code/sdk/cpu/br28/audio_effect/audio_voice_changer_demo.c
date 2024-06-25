#include "audio_voice_changer_demo.h"
#include "app_config.h"
#include "audio_effect/audio_eff_default_parm.h"

#undef AUDIO_VOICE_CHANGER_ENABLE
#if defined(TCFG_MIC_VOICE_CHANGER_ENABLE) && TCFG_MIC_VOICE_CHANGER_ENABLE
#define AUDIO_VOICE_CHANGER_ENABLE 1
#else
#define AUDIO_VOICE_CHANGER_ENABLE 0
#endif

#if AUDIO_VOICE_CHANGER_ENABLE
VoiceChangerParam_TOOL_SET music_voice_changer = {//大叔声
    .is_bypass = 0,
    .parm.effect_v = EFFECT_VOICECHANGE_PITCHSHIFT,
    .parm.shiftv = 130,
    .parm.formant_shift = 100,
};

voice_changer_hdl *audio_voice_changer_open_demo(u32 voice_changer_name, u32 sample_rate)
{
    VOICECHANGER_PARM parm = {0};
    u32 bypass = 0;
    if (voice_changer_name == AEID_MIC_VOICE_CHANGER) {
#if defined(TCFG_MIC_VOICE_CHANGER_ENABLE) && TCFG_MIC_VOICE_CHANGER_ENABLE
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
        u8 mode = get_mic_eff_mode();
        memcpy(&parm, &eff_mode[mode].voicechanger_parm.parm, sizeof(VOICECHANGER_PARM));
        bypass = eff_mode[mode].voicechanger_parm.is_bypass;
#endif
#endif
    } else if (voice_changer_name == AEID_MUSIC_VOICE_CHANGER) {
        memcpy(&parm, &music_voice_changer.parm, sizeof(VOICECHANGER_PARM));
        bypass = music_voice_changer.is_bypass;
    }
    voice_changer_hdl *hdl = audio_voice_changer_open(&parm, sample_rate, voice_changer_name);
    audio_voice_changer_bypass(voice_changer_name, bypass);
    return hdl;
}


void audio_voice_changer_close_demo(voice_changer_hdl *hdl)
{
    audio_voice_changer_close(hdl);
}

void audio_voice_changer_update_demo(u32 voice_changer_name, VOICECHANGER_PARM *parm, u32 bypass)
{
    audio_voice_changer_update_parm(voice_changer_name, parm);
    audio_voice_changer_bypass(voice_changer_name, bypass);
}

//变声模式切换
void audio_voice_changer_mode_switch(u32 voice_changer_name, VOICE_CHANGER_MODE voice_changer_mode)
{
    VOICECHANGER_PARM parm = {0};
    switch (voice_changer_mode) {
    case VOICE_CHANGER_NONE://原声
        audio_voice_changer_bypass(voice_changer_name, RUN_BYPASS);
        break;
    case VOICE_CHANGER_UNCLE://大叔
        audio_voice_changer_bypass(voice_changer_name, RUN_NORMAL);
        parm.shiftv = 130;
        parm.formant_shift = 100;//无效参数
        parm.effect_v = EFFECT_VOICECHANGE_PITCHSHIFT;
        audio_voice_changer_update_parm(voice_changer_name, &parm);
        break;
    case VOICE_CHANGER_GODDESS://女神
        audio_voice_changer_bypass(voice_changer_name, RUN_NORMAL);
        parm.shiftv = 56;
        parm.formant_shift = 90;
        parm.effect_v = EFFECT_VOICECHANGE_SPECTRUM;
        audio_voice_changer_update_parm(voice_changer_name, &parm);
        break;
    case VOICE_CHANGER_BABY://娃娃音
        audio_voice_changer_bypass(voice_changer_name, RUN_NORMAL);
        parm.shiftv = 50;
        parm.formant_shift = 100;//无效参数
        parm.effect_v = EFFECT_VOICECHANGE_PITCHSHIFT;
        audio_voice_changer_update_parm(voice_changer_name, &parm);
        break;
    case VOICE_CHANGER_MAGIC://魔音女声
        audio_voice_changer_bypass(voice_changer_name, RUN_NORMAL);
        parm.shiftv = 75;
        parm.formant_shift = 80;
        parm.effect_v = EFFECT_VOICECHANGE_PITCHSHIFT;
        audio_voice_changer_update_parm(voice_changer_name, &parm);
        break;
    case VOICE_CHANGER_MONSTER://怪兽音
        audio_voice_changer_bypass(voice_changer_name, RUN_NORMAL);
        parm.shiftv = 160;
        parm.formant_shift = 100;//无效参数
        parm.effect_v = EFFECT_VOICECHANGE_PITCHSHIFT;
        audio_voice_changer_update_parm(voice_changer_name, &parm);
        break;
    case VOICE_CHANGER_DONALD_DUCK://唐老鸭
        audio_voice_changer_bypass(voice_changer_name, RUN_NORMAL);
        parm.shiftv = 60;
        parm.formant_shift = 170;
        parm.effect_v = EFFECT_VOICECHANGE_CARTOON;
        audio_voice_changer_update_parm(voice_changer_name, &parm);
        break;
    case VOICE_CHANGER_MINIONS://小黄人
        audio_voice_changer_bypass(voice_changer_name, RUN_NORMAL);
        parm.shiftv = 50;
        parm.formant_shift = 60;
        parm.effect_v = EFFECT_VOICECHANGE_CARTOON;
        audio_voice_changer_update_parm(voice_changer_name, &parm);
        break;
    case VOICE_CHANGER_ROBOT://机器音
        audio_voice_changer_bypass(voice_changer_name, RUN_NORMAL);
        parm.shiftv = 70;
        parm.formant_shift = 80;
        parm.effect_v = EFFECT_VOICECHANGE_ROBORT;
        audio_voice_changer_update_parm(voice_changer_name, &parm);
        break;
    case VOICE_CHANGER_WHISPER://气音
        audio_voice_changer_bypass(voice_changer_name, RUN_NORMAL);
        parm.shiftv = 70;
        parm.formant_shift = 80;
        parm.effect_v = EFFECT_VOICECHANGE_WHISPER;
        audio_voice_changer_update_parm(voice_changer_name, &parm);
        break;
    case VOICE_CHANGER_MELODY://固定旋律音
        audio_voice_changer_bypass(voice_changer_name, RUN_NORMAL);
        parm.shiftv = 70;
        parm.formant_shift = 80;
        parm.effect_v = EFFECT_VOICECHANGE_MELODY;
        audio_voice_changer_update_parm(voice_changer_name, &parm);
        break;
    case VOICE_CHANGER_FEEDBACK://调制音
        audio_voice_changer_bypass(voice_changer_name, RUN_NORMAL);
        parm.shiftv = 150;
        parm.formant_shift = 80;
        parm.effect_v = EFFECT_VOICECHANGE_FEEDBACK;
        audio_voice_changer_update_parm(voice_changer_name, &parm);
        break;
    default:
        break;
    }
}
#endif
