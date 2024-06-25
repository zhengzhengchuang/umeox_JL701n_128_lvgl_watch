#include "audio_effect/audio_gain_process_demo.h"
#include "audio_effect/audio_voice_changer_demo.h"
#include "audio_effect/audio_vbass_demo.h"
#include "audio_effect/audio_eff_default_parm.h"
#include "audio_effect/audio_noise_gate_demo.h"
#include "audio_effect/audio_surround_demo.h"
#include "media/effects_adj.h"
#include "cfg_tool.h"
#include "app_config.h"
#include "audio_mic/effect_parm.h"
#include "audio_effect/audio_eq_drc_demo.h"

#if defined(TCFG_EQ_ENABLE) && TCFG_EQ_ENABLE
#define LOG_TAG     "[EFFECT]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#include "debug.h"
#ifdef SUPPORT_MS_EXTENSIONS
//#pragma bss_seg(".audio_mic_stream_bss")
//#pragma data_seg(".audio_mic_stream_data")
#pragma const_seg(".audio_effect_adj_const")
#pragma code_seg(".audio_effect_adj_code")
#endif

u8 EFF_CFG_FILE_NAME[64] = SDFILE_RES_ROOT_PATH"eq_cfg_hw.bin";
const u8 eff_sdk_name[16] 		= "AC701N";
const u8 eff_eq_ver[4] 			= {1, 0, 0, 0};


#if defined(SOUND_TRACK_2_P_X_CH_CONFIG) &&SOUND_TRACK_2_P_X_CH_CONFIG && (EQ_SECTION_MAX == 32)
#define MUSIC_EQ_NSECTION_SPC mSECTION_MAX
#else
#define MUSIC_EQ_NSECTION_SPC  music_eq_nsection
#endif

/*drc后扩展eq支持的在线调音段数*/
#define EXT_EQ_NSECTION             5
/*高低音eq支持在线调音的段数*/
#define BASS_AND_TREBLE_EQ_NSECTION 10
/*动态EQ模块前eq支持的在线调音段数*/
#define MUSIC_EQ2_NSECTION          10
//每个模块id对应一个group_id(功能id)

struct mode_list mlist[] = {
    {AEID_ESCO_DL_EQ, phone_eq_nsection, 2,    {EFF_PHONE_WIDEBAND_EQ, EFF_PHONE_NARROWBAND_EQ}},

    {AEID_ESCO_UL_EQ,  phone_eq_nsection, 2,    {EFF_AEC_WIDEBAND_EQ, EFF_AEC_NARROWBAND_EQ}},
    {AEID_MUSIC_EQ,  MUSIC_EQ_NSECTION_SPC, 2,    {EFF_MUSIC_EQ, EFF_MUSIC_FR_EQ}},
    {AEID_MUSIC_EQ2,     MUSIC_EQ2_NSECTION,       1,    {EFF_MUSIC_EQ2}},
    {AEID_MUSIC_EXTEQ, EXT_EQ_NSECTION, 1,    {EFF_MUSIC_EXT_EQ}},
    {AEID_MUSIC_RL_EQ,  music_eq_nsection, 2,    {EFF_MUSIC_RL_EQ, EFF_MUSIC_RR_EQ}},
    {AEID_MIC_EQ0,  mic_eq_nsection, 1,    {EFF_MIC_EQ0}},
    {AEID_MIC_EQ1,  mic_eq_nsection, 1,    {EFF_MIC_EQ1}},
    {AEID_MIC_EQ2,  mic_eq_nsection, 1,    {EFF_MIC_EQ2}},
    {AEID_MIC_EQ3,  mic_eq_nsection, 1,    {EFF_MIC_EQ3}},
    {AEID_MIC_EQ4,  mic_eq_nsection, 1,    {EFF_MIC_EQ4}},
    {AEID_HIGH_BASS_EQ,  BASS_AND_TREBLE_EQ_NSECTION, 1,    {EFF_MUSIC_HIGH_BASS_EQ}},
#if TCFG_DRC_ENABLE
    {AEID_ESCO_DL_DRC,   0,  2,     {EFF_PHONE_WIDEBAND_DRC, EFF_PHONE_NARROWBAND_DRC}},
    {AEID_ESCO_UL_DRC,   0, 2,    {EFF_AEC_WIDEBAND_DRC, EFF_AEC_NARROWBAND_DRC}},


    {AEID_MUSIC_DRC,     0, 5,    {EFF_MUSIC_LOW_DRC, EFF_MUSIC_MID_DRC, EFF_MUSIC_HIGH_DRC, EFF_MUSIC_WHOLE_DRC, EFF_MUSIC_CROSSOVER }},   //最后一个group_id存方分频器
    {AEID_MUSIC_FR_DRC,  0, 1,    {0x2004}},
    {AEID_MUSIC_RL_DRC,  0, 4,    {EFF_MUSIC_RL_LOW_DRC, EFF_MUSIC_RL_MID_DRC, EFF_MUSIC_RL_HIGH_DRC, EFF_MUSIC_RL_WHOLE_DRC }},
    {AEID_MUSIC_RR_DRC,  0, 1,    {0x2008}},


    {AEID_MIC_DRC0,      0, 1,    {EFF_MIC_DRC0}},
    {AEID_MIC_DRC1,      0, 1,    {EFF_MIC_DRC1}},
    {AEID_MIC_DRC2,      0, 1,    {EFF_MIC_DRC2}},
    {AEID_MIC_DRC3,      0, 1,    {EFF_MIC_DRC3}},
    {AEID_MIC_DRC4,      0, 1,    {EFF_MIC_DRC4}},
    {AEID_HIGH_BASS_DRC, 0, 1,    {EFF_MUSIC_HIGH_BASS_DRC}},

#endif
};

#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
struct mode_list aux_list[] = {
    {AEID_AUX_EQ,            MUSIC_EQ_NSECTION_SPC,       2,    {EFF_AUX_EQ, EFF_MUSIC_FR_EQ}},
    {AEID_AUX_EQ2,           MUSIC_EQ2_NSECTION,          1,    {EFF_MUSIC_EQ2}},
    {AEID_AUX_EXTEQ,         EXT_EQ_NSECTION,             1,    {EFF_MUSIC_EXT_EQ}},
    {AEID_AUX_EXTEQ2,        EXT_EQ_NSECTION,             1,    {EFF_MUSIC_EXT_EQ2}},
    {AEID_AUX_RL_EQ,         music_eq_nsection,           2,    {EFF_MUSIC_RL_EQ, EFF_MUSIC_RR_EQ}},
    {AEID_AUX_HIGH_BASS_EQ,  BASS_AND_TREBLE_EQ_NSECTION, 1,    {EFF_MUSIC_HIGH_BASS_EQ}},

#if TCFG_DRC_ENABLE
    {AEID_AUX_DRC,           0, 5,    {EFF_AUX_DRC, EFF_MUSIC_MID_DRC, EFF_MUSIC_HIGH_DRC, EFF_MUSIC_WHOLE_DRC, EFF_MUSIC_CROSSOVER }},   //最后一个group_id存方分频器
    {AEID_AUX_FR_DRC,        0, 1,    {0x2004}},
    {AEID_AUX_RL_DRC,        0, 5,    {EFF_MUSIC_RL_LOW_DRC, EFF_MUSIC_RL_MID_DRC, EFF_MUSIC_RL_MID_DRC, EFF_MUSIC_RL_HIGH_DRC, EFF_MUSIC_RL_WHOLE_DRC }},
    {AEID_AUX_RR_DRC,        0, 1,    {0x2008}},
    {AEID_AUX_HIGH_BASS_DRC, 0, 1,    {EFF_MUSIC_HIGH_BASS_DRC}},
#endif
};
#endif
int get_index_by_group_id(u16 group_id, u8 tar)
{
    if (!tar) {
        for (int i = 0; i < ARRAY_SIZE(mlist); i++) {
            struct mode_list *list = (struct mode_list *)&mlist[i];
            for (int j = 0; j < list->group_num; j++) {
                if (list->group_id[j] == group_id) {
                    return i;
                }
            }
        }
    } else if (tar == 1) {
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
        for (int i = 0; i < ARRAY_SIZE(aux_list); i++) {
            struct mode_list *list = (struct mode_list *)&aux_list[i];
            for (int j = 0; j < list->group_num; j++) {
                if (list->group_id[j] == group_id) {
                    return i;
                }
            }
        }
#endif
    }
    log_e("get_ tab index error\n");
    return -1;
}
void set_list_nsection(u8 tar, u8 index, u8 nsection)
{
    if (!tar) {
        mlist[index].nsection = nsection;
    } else {
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
        aux_list[index].nsection = nsection;
#endif
    }
}
int get_module_name(u16 group_id, u8 tar)
{
    if (!tar) {
        for (int i = 0; i < ARRAY_SIZE(mlist); i++) {
            struct mode_list *list = (struct mode_list *)&mlist[i];
            for (int j = 0; j < list->group_num; j++) {
                if (list->group_id[j] == group_id) {
                    return list->module_name;
                }
            }
        }
    } else if (tar == 1) {
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
        for (int i = 0; i < ARRAY_SIZE(aux_list); i++) {
            struct mode_list *list = (struct mode_list *)&aux_list[i];
            for (int j = 0; j < list->group_num; j++) {
                if (list->group_id[j] == group_id) {
                    return list->module_name;
                }
            }
        }
#endif
    }
#if PARM_DEBUG
    log_w("get_module_name error id 0x%x, tar %d\n", group_id, tar);
#endif
    return 0;
}
int get_module_name_and_index(u16 group_id, u16 *index, u8 tar)
{
    if (!tar) {
        for (int i = 0; i < ARRAY_SIZE(mlist); i++) {
            struct mode_list *list = (struct mode_list *)&mlist[i];
            for (int j = 0; j < list->group_num; j++) {
                if (list->group_id[j] == group_id) {
                    *index = j;
                    return list->module_name;
                }
            }
        }
    } else if (tar == 1) {
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
        for (int i = 0; i < ARRAY_SIZE(aux_list); i++) {
            struct mode_list *list = (struct mode_list *)&aux_list[i];
            for (int j = 0; j < list->group_num; j++) {
                if (list->group_id[j] == group_id) {
                    *index = j;
                    return list->module_name;
                }
            }
        }
#endif
    }
#if PARM_DEBUG
    log_w("get_module_name and index error id 0x%x, tar %d\n", group_id, tar);
#endif
    return 0;
}

int get_group_id(u16 module_name, u8 tar)
{
    for (int i = 0; i < ARRAY_SIZE(mlist); i++) {
        struct mode_list *list = (struct mode_list *)&mlist[i];
        if (list->module_name == module_name) { //
            return list->group_id[tar];
        }
    }
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    for (int i = 0; i < ARRAY_SIZE(aux_list); i++) {
        struct mode_list *list = (struct mode_list *)&aux_list[i];
        if (list->module_name == module_name) { //
            return list->group_id[tar];
        }
    }
#endif
    log_e("get_module_id error\n");
    return 0;
}
struct mode_list *get_group_list(u16 module_name)
{
    for (int i = 0; i < ARRAY_SIZE(mlist); i++) {
        struct mode_list *list = (struct mode_list *)&mlist[i];
        if (list->module_name == module_name) { //
            return list;
        }
    }
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    for (int i = 0; i < ARRAY_SIZE(aux_list); i++) {
        struct mode_list *list = (struct mode_list *)&aux_list[i];
        if (list->module_name == module_name) { //
            return list;
        }
    }
#endif
    return NULL;
}
int get_eq_nsection(u16 module_name)
{
    for (int i = 0; i < ARRAY_SIZE(mlist); i++) {
        struct mode_list *list = (struct mode_list *)&mlist[i];
        if (list->module_name == module_name) {
            return list->nsection;
        }
    }
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    for (int i = 0; i < ARRAY_SIZE(aux_list); i++) {
        struct mode_list *list = (struct mode_list *)&aux_list[i];
        if (list->module_name == module_name) {
            return list->nsection;
        }
    }
#endif
    log_e("get_eq_nsection error\n");
    return 0;
}

int get_eq_seg(u16 module_name)
{
    return 0;
}
void get_eff_mode(u16 mode_id, u16 group_id, u8 *mode_index, u8 *drc_index)//获取混响模式的index
{
    *mode_index = mode_id - mic_mode_seq0;
    *drc_index = group_id - EFF_MIC_DRC0;
}
int get_phone_mode(u16 group_id)
{
    return group_id - EFF_PHONE_WIDEBAND_EQ;
}

#define REPLY_TO_TOOL  (0)
#define EFF_CFG_FILE_ID  (0x3)//音效配置项源文件id

static u8 eff_mode_save = 0;
static u16 alive_timer = 0;
static u32 alive_cnt = 0;

int music_surround_effect_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if AUDIO_SURROUND_CONFIG
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 sur_name = AEID_MUSIC_SURROUND;
    u8 update = 1;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        sur_name = AEID_AUX_SURROUND;
        if (!get_cur_sur_hdl_by_name(sur_name)) {
            update = 0;
        }
    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }

    memcpy(&sur_parm[tar], &packet->data[1], sizeof(SurroundEffect_TOOL_SET));
    SurroundEffect_TOOL_SET *parmt = &sur_parm[tar];
    if (update) {
        audio_surround_effect_update_demo(sur_name, &parmt->parm, parmt->is_bypass);
    }

#if PARM_DEBUG
    surround_update_parm *parm = &parmt->parm;
    log_debug("music surround effect  surround_type %d,rotatestep %d, damping %d, feedback %d, roomsize %d\n",
              parm->surround_type, parm->rotatestep, parm->damping, parm->feedback, parm->roomsize);
#endif
#endif
    return 0;
}

/*
 *混响mic tunning_eq在线调试数据获取解析配置
 * */
int mic_tunning_eq_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE&&BASS_AND_TREBLE_ENABLE
    u8 mode_id = packet->data[0];
    struct tunning_eq_tool *_parm = &eff_mode[mode_id - mic_mode_seq0].tunning_eq_parm; //增益
    memcpy(_parm, &packet->data[1], sizeof(eff_mode[mode_id - mic_mode_seq0].tunning_eq_parm));
#if PARM_DEBUG
    for (int i = 0; i < 3; i++) {
        struct tunning_eq_seg *parm = &_parm->seg[i];
        log_debug("mic parm->index %d, parm->filt_type %d, parm->center_freq %d, parm->q 0x%x, parm->min_gain 0x%x,parm->max_gain 0x%x\n", i, parm->filt_type, parm->center_freq, *(int *)&parm->q, *(int *)&parm->min_gain, *(int *)&parm->max_gain);
    }
#endif

#endif
    return 0;
}

/*
 *music、aux  tunning_eq在线调试数据获取解析配置
 * */

int tunning_eq_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_EQ_ENABLE) && TCFG_EQ_ENABLE && TCFG_AUDIO_OUT_EQ_ENABLE &&BASS_AND_TREBLE_ENABLE
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 eq_name = AEID_HIGH_BASS_EQ;
    u8 update = 1;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        eq_name = AEID_AUX_HIGH_BASS_EQ;
        if (!get_cur_eq_hdl_by_name(eq_name)) {
            update = 0;
        }
    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }

    memcpy(&tunning_eq[tar], &packet->data[1], sizeof(struct tunning_eq_tool));
    if (update) {
    }

#if PARM_DEBUG
    struct tunning_eq_tool *_parm = &tunning_eq[tar];
    for (int i = 0; i < 3; i++) {
        struct tunning_eq_seg *parm = &_parm->seg[i];
        log_debug("parm->index %d, parm->filt_type %d, parm->center_freq %d, parm->q 0x%x, parm->min_gain 0x%x,parm->max_gain 0x%x\n", i, parm->filt_type, parm->center_freq, *(int *)&parm->q, *(int *)&parm->min_gain, *(int *)&parm->max_gain);
    }
#endif

#endif
    return 0;

}
int music_noise_gate_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if MUSIC_NOISE_GATE_EN
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 ns_name = AEID_MUSIC_NS_GATE;
    u8 update = 1;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        ns_name = AEID_AUX_NS_GATE;
        if (!get_cur_noisegate_hdl_by_name(ns_name)) {
            update = 0;
        }
    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }

    memcpy(&music_noisegate_parm[tar], &packet->data[1], sizeof(NoiseGateParam_TOOL_SET));
    NoiseGateParam_TOOL_SET *parmt = &music_noisegate_parm[tar];
    if (update) {
        audio_noisegate_update_demo(ns_name, &parmt->parm, parmt->is_bypass);
    }

#if PARM_DEBUG
    noisegate_update_param *parm = &parmt->parm;
    log_debug("music noise gate attact_time %d,releasetime %d, threshold %d, low_th_gain %d\n",
              parm->attackTime, parm->releaseTime, parm->threshold, parm->low_th_gain);
#endif
#endif
    return 0;
}


int vbass_prev_gain_process_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if AUDIO_VBASS_CONFIG
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 gain_name = AEID_MUSIC_VBASS_PREV_GAIN;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        gain_name = AEID_AUX_VBASS_PREV_GAIN;
        if (!get_cur_gain_hdl_by_name(gain_name)) {
            memcpy(&vbass_prev_gain_parm[tar], &packet->data[1], sizeof(Gain_Process_TOOL_SET));
            return 0;
        }
    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }
    memcpy(&vbass_prev_gain_parm[tar], &packet->data[1], sizeof(Gain_Process_TOOL_SET));
    audio_gain_update_parm(gain_name, &vbass_prev_gain_parm[tar].parm, vbass_prev_gain_parm[tar].is_bypass);
#if PARM_DEBUG
    log_debug("vbass_prev_gain_parm[%d].is_bypass %d, gain 0x%x", tar, vbass_prev_gain_parm[tar].is_bypass, *(int *)&vbass_prev_gain_parm[tar].parm.gain[0]);
    log_debug("vbass_prev_gain_parm[%d].is_bypass %d, gain 0x%x", tar, vbass_prev_gain_parm[tar].is_bypass, *(int *)&vbass_prev_gain_parm[tar].parm.gain[1]);
#endif
#endif
    return 0;
}

int music_ch_swap_process_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(CH_SWAP_OLD) && CH_SWAP_OLD
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 swap_name = AEID_MUSIC_CH_SWAP;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        swap_name = AEID_AUX_CH_SWAP;
        if (!get_cur_swap_hdl_by_name(swap_name)) {
            memcpy(&music_ch_swap[tar], &packet->data[1], sizeof(ChannelSwap_TOOL_SET));
            return 0;
        }
    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }
    memcpy(&music_ch_swap[tar], &packet->data[1], sizeof(ChannelSwap_TOOL_SET));
    audio_ch_swap_update_parm(swap_name, NULL, music_ch_swap[tar].is_bypass);
#if PARM_DEBUG
    log_debug("music_ch_swap[%d].is_bypass %d%x", tar, music_ch_swap[tar].is_bypass);
#endif
#endif
    return 0;
}
int music_vbass_parm_ananlyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if AUDIO_VBASS_CONFIG
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 vbass_name = AEID_MUSIC_VBASS;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        vbass_name = AEID_AUX_VBASS;
        if (!get_cur_vbass_hdl_by_name(vbass_name)) {
            memcpy(&vbass_parm[tar], &packet->data[1], sizeof(VirtualBass_TOOL_SET));
            return 0;
        }
    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }

    memcpy(&vbass_parm[tar], &packet->data[1], sizeof(VirtualBass_TOOL_SET));
    audio_vbass_update_demo(vbass_name, &vbass_parm[tar].parm, vbass_parm[tar].is_bypass);
#if PARM_DEBUG
    log_debug("is_bypass %d, ratio %d, boost %d, fc %d\n", vbass_parm[tar].is_bypass, vbass_parm[tar].parm.ratio, vbass_parm[tar].parm.boost, vbass_parm[tar].parm.fc);
#endif
#endif
    return 0;
}


int mic_voice_changer_parm_ananlyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
#if defined(TCFG_MIC_VOICE_CHANGER_ENABLE) && TCFG_MIC_VOICE_CHANGER_ENABLE
    u8 mode_id = packet->data[0];
    VoiceChangerParam_TOOL_SET *changer_parm = &eff_mode[mode_id - mic_mode_seq0].voicechanger_parm;
    memcpy(changer_parm, &packet->data[1], sizeof(eff_mode[mode_id - mic_mode_seq0].voicechanger_parm));
    audio_voice_changer_update_demo(AEID_MIC_VOICE_CHANGER, &changer_parm->parm, changer_parm->is_bypass);
#if PARM_DEBUG
    log_debug("effect_v %d, shiftv %d, formant_shift %d, is_bypass %d\n", changer_parm->parm.effect_v, changer_parm->parm.shiftv, changer_parm->parm.formant_shift, changer_parm->is_bypass);
#endif
#endif
#endif
    return 0;
}

int mic_mix_gain_parm_ananlyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    u8 mode_id = packet->data[0];
    Mix_TOOL_SET *mix_gain_parm = &eff_mode[mode_id - mic_mode_seq0].mix_gain;
    memcpy(mix_gain_parm, &packet->data[1], sizeof(eff_mode[mode_id - mic_mode_seq0].mix_gain));
#if PARM_DEBUG
    log_debug("gain0: 0x%x, gain1:0x%x, gain2:0x%x\n", mix_gain_parm->gain1, mix_gain_parm->gain2, mix_gain_parm->gain3);
#endif
#endif
    return 0;
}

int mic_gain_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if 0//GAIN_PROCESS_EN
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    u8 mode_id = packet->data[0];
    Gain_Process_TOOL_SET *gain_parm = &eff_mode[mode_id - mic_mode_seq0].gain_parm; //增益
    memcpy(gain_parm, &packet->data[1], sizeof(eff_mode[mode_id - mic_mode_seq0].gain_parm));
    audio_gain_update_parm(AEID_MIC_GAIN, &gain_parm->parm, eff_mode[mode_id - mic_mode_seq0].gain_parm.is_bypass);
#if PARM_DEBUG
    log_debug("gain_parm.is_bypass %d, gain 0x%x", gain_parm->is_bypass, *(int *)&gain_parm->parm.gain);
#endif
#endif
#endif
    return 0;
}

int gain_process_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 gain_name = AEID_MUSIC_GAIN;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        gain_name = AEID_AUX_GAIN;
        if (!get_cur_gain_hdl_by_name(gain_name)) {
            memcpy(&gain_parm[tar], &packet->data[1], sizeof(Gain_Process_TOOL_SET));
            return 0;
        }
    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }

    memcpy(&gain_parm[tar], &packet->data[1], sizeof(Gain_Process_TOOL_SET));
    audio_gain_update_parm(gain_name, &gain_parm[tar].parm, gain_parm[tar].is_bypass);
#if PARM_DEBUG
    log_debug("gain_parm.is_bypass %d, gain 0x%x", gain_parm[tar].is_bypass, *(int *)&gain_parm[tar].parm.gain[0]);
    log_debug("gain_parm.is_bypass %d, gain 0x%x", gain_parm[tar].is_bypass, *(int *)&gain_parm[tar].parm.gain[1]);
    log_debug("gain_parm.is_bypass %d, gain 0x%x", gain_parm[tar].is_bypass, *(int *)&gain_parm[tar].parm.gain[2]);
    log_debug("gain_parm.is_bypass %d, gain 0x%x", gain_parm[tar].is_bypass, *(int *)&gain_parm[tar].parm.gain[3]);
#endif
#endif
    return 0;
}

int rl_gain_process_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{

#if (TCFG_AUDIO_DAC_CONNECT_MODE == DAC_OUTPUT_FRONT_LR_REAR_LR)
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 gain_name = AEID_MUSIC_RL_GAIN;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        gain_name = AEID_AUX_RL_GAIN;
        if (!get_cur_gain_hdl_by_name(gain_name)) {
            memcpy(&rl_gain_parm[tar], &packet->data[1], sizeof(Gain_Process_TOOL_SET));
            return 0;
        }

    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }

    memcpy(&rl_gain_parm[tar], &packet->data[1], sizeof(Gain_Process_TOOL_SET));
    audio_gain_update_parm(gain_name, &rl_gain_parm[tar].parm, rl_gain_parm[tar].is_bypass);
#if PARM_DEBUG
    log_debug("rl_gain_parm.is_bypass %d, gain 0x%x", rl_gain_parm[tar].is_bypass, *(int *)&rl_gain_parm[tar].parm.gain[0]);
    log_debug("rl_gain_parm.is_bypass %d, gain 0x%x", rl_gain_parm[tar].is_bypass, *(int *)&rl_gain_parm[tar].parm.gain[1]);
    log_debug("rl_gain_parm.is_bypass %d, gain 0x%x", rl_gain_parm[tar].is_bypass, *(int *)&rl_gain_parm[tar].parm.gain[2]);
    log_debug("rl_gain_parm.is_bypass %d, gain 0x%x", rl_gain_parm[tar].is_bypass, *(int *)&rl_gain_parm[tar].parm.gain[3]);
#endif
#endif
#endif
    return 0;
}

void dynamic_eq_printf(DynamicEQParam_TOOL_SET *dy_parm)
{
#if PARM_DEBUG
    log_debug("dy_parm->is_bypass %d\n", dy_parm->is_bypass);
    for (int i = 0; i < dy_parm->nSection; i++) {
        log_debug("fc %d , Q 0x%x, gain:0x%x, type %d, attacktime %d, releaseTime %d, rmsTime %d, threshold 0x%x, ratio 0x%x, noisegate_threshold 0x%x, fixGain %x, algorithm %d\n",
                  dy_parm->effect_param[i].fc,
                  *(int *)&dy_parm->effect_param[i].Q,
                  *(int *)&dy_parm->effect_param[i].gain,
                  dy_parm->effect_param[i].type,
                  dy_parm->effect_param[i].attackTime,
                  dy_parm->effect_param[i].releaseTime,
                  dy_parm->effect_param[i].rmsTime,
                  *(int *)&dy_parm->effect_param[i].threshold,
                  *(int *)&dy_parm->effect_param[i].ratio,
                  *(int *)&dy_parm->effect_param[i].noisegate_threshold,
                  *(int *)&dy_parm->effect_param[i].fixGain,
                  dy_parm->effect_param[i].algorithm);
    }

    log_debug("nSection %d, detect_mode %d\n", dy_parm->nSection, dy_parm->detect_mode);
#endif
}

/*
 *动态eq 参数设置更新
 *tar:0 music 模式的  1:AUX模式的（AUX独立音效时有效）
 * */
static void dynamic_eq_update_parm(u32 dy_eq_name, u8 tar)
{
#if TCFG_DYNAMIC_EQ_ENABLE
    DynamicEQParam_TOOL_SET *dy_parm = &dynamic_eq[tar];//&packet->data[1];
    DynamicEQEffectParam *parm1 = dy_parm->effect_param;

    DynamicEQParam parm2 = {0};
    parm2.nSection = dy_parm->nSection;
    parm2.detect_mode = dy_parm->detect_mode;

    DynamicEQDetectionParam detectParm[2] = {0};
    detectParm[0].fc = dy_parm->effect_param[0].fc;
    detectParm[1].fc = dy_parm->effect_param[1].fc;

    dynamic_eq_printf(&dynamic_eq[tar]);
    audio_dynamic_eq_detection_update_parm(dy_eq_name, detectParm, dy_parm->is_bypass);//动态eq检测更新

    audio_dynamic_eq_update_parm(dy_eq_name, (void *)parm1, (void *)&parm2, dy_parm->is_bypass); //动态eq更新

#endif
}

int dynamic_eq_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if TCFG_DYNAMIC_EQ_ENABLE
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 dy_eq_name = AEID_MUSIC_DYNAMIC_EQ;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        dy_eq_name = AEID_AUX_DYNAMIC_EQ;
        if (!get_cur_dynamic_eq_hdl_by_name(dy_eq_name)) {
            memcpy(&dynamic_eq[tar], &packet->data[1], sizeof(DynamicEQParam_TOOL_SET));
            return 0;
        }
    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }

    memcpy(&dynamic_eq[tar], &packet->data[1], sizeof(DynamicEQParam_TOOL_SET));
    dynamic_eq_update_parm(dy_eq_name, tar);
#endif
    return 0;
}

int phone_eq_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_EQ_ENABLE) && TCFG_EQ_ENABLE && (TCFG_AEC_UL_EQ_ENABLE || TCFG_PHONE_EQ_ENABLE)
    u32 eq_name = get_module_name(packet->cmd, 0);//查询到相应模块
    float global_gain = 0;
    u8 mode_id = packet->data[0];
    log_debug("mode_id %d\n", mode_id);
    struct eq_seg_info *seg = (struct eq_seg_info *)&packet->data[1];

    if (seg->index == (u16) - 1) {
        memcpy(&global_gain, &seg->iir_type, sizeof(float));
        log_info("global_gain 0x%x\n", *(int *)&global_gain);
    }

    void *tar_seg = NULL;
    u8 tar = packet->cmd - EFF_PHONE_WIDEBAND_EQ;
    log_debug("phone_mode tar %d\n", tar);
    if (seg->index == (u16) - 1) {
        phone_mode[tar].eq_parm.global_gain = global_gain;
    } else {
        tar_seg = &phone_mode[tar].eq_parm.seg[seg->index];
    }
    if (seg->index != (u16) - 1) {
#if PARM_DEBUG
        log_info("idx:%d, iir:%d, frq:%d, gain:0x%x, q:0x%x \n", seg->index, seg->iir_type, seg->freq, *(int *)&seg->gain, *(int *)&seg->q);
#endif
        memcpy(tar_seg, seg, sizeof(struct eq_seg_info));
        if (!bt_phone_dec_is_running()) {
            return 0;
        }
        cur_eq_set_update(eq_name, tar_seg, get_eq_nsection(eq_name), 0);
    } else {
        if (!bt_phone_dec_is_running()) {
            return 0;
        }
        cur_eq_set_global_gain(eq_name, global_gain);
    }
#endif

    return 0;
}

int low_pass_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(SOUND_TRACK_2_P_X_CH_CONFIG) &&SOUND_TRACK_2_P_X_CH_CONFIG&& HIGH_GRADE_LOW_PASS_FILTER_EN
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 eq_name = AEID_MUSIC_RL_EQ;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        eq_name = AEID_AUX_RL_EQ;
        if (!get_cur_eq_hdl_by_name(eq_name)) {
            memcpy(&low_pass_parm[tar], &packet->data[1], sizeof(LowPassParam_TOOL_SET));
            return 0;
        }
    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }

    memcpy(&low_pass_parm[tar], &packet->data[1], sizeof(LowPassParam_TOOL_SET));
    cur_eq_set_update(eq_name, NULL, 1, 0);
#if PARM_DEBUG
    log_info("low_pass_parm.is_bypass %d\n", low_pass_parm[tar].is_bypass);
    struct advance_iir *low_p = &low_pass_parm[tar].low_pass;
    log_info("low_p->fc %d, low_p->order %d, low_p->type %d\n", low_p->fc, low_p->order, low_p->type);
#endif
#endif
    return 0;
}
int mic_eq_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    u32 eq_name = get_module_name(packet->cmd, 0);//查询到相应模块
    float global_gain = 0;
    u8 mode_id = packet->data[0];
    log_debug("mode_id %d\n", mode_id);
    struct eq_seg_info *seg = (struct eq_seg_info *)&packet->data[1];

    if (seg->index == (u16) - 1) {
        memcpy(&global_gain, &seg->iir_type, sizeof(float));
        log_info("global_gain 0x%x\n", *(int *)&global_gain);
    }

    void *tar_seg = NULL;
    u8 tar = packet->cmd - EFF_MIC_EQ0;
    log_debug("eff_mode tar %d\n", tar);
    if (seg->index == (u16) - 1) {
        eff_mode[mode_id - mic_mode_seq0].eq_parm[tar].global_gain = global_gain;
    } else {
        tar_seg = &eff_mode[mode_id - mic_mode_seq0].eq_parm[tar].seg[seg->index];
    }
    if (seg->index != (u16) - 1) {
#if PARM_DEBUG
        log_info("idx:%d, iir:%d, frq:%d, gain:0x%x, q:0x%x \n", seg->index, seg->iir_type, seg->freq, *(int *)&seg->gain, *(int *)&seg->q);
#endif
        memcpy(tar_seg, seg, sizeof(struct eq_seg_info));
        if (!mic_effect_get_status()) {
            return 0;
        }

        cur_eq_set_update(eq_name, tar_seg, get_eq_nsection(eq_name), 1);
    } else {
        if (!mic_effect_get_status()) {
            return 0;
        }

        cur_eq_set_global_gain(eq_name, global_gain);
    }
#endif

    return 0;
}

int music_eq_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_EQ_ENABLE) && TCFG_EQ_ENABLE
    float global_gain = 0;
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 eq_name = get_module_name(packet->cmd, tar);//查询到相应模块
    u8 update = 1;
    u8 right_ch = 0;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        eq_name = get_module_name(packet->cmd, tar);//查询到相应模块
        if (!get_cur_eq_hdl_by_name(eq_name)) {
            update = 0;
            /* return 0; */
        }
    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }

    log_debug("mode_id %d ,eq_name %d\n", mode_id, eq_name);
    struct eq_seg_info *seg = (struct eq_seg_info *)&packet->data[1];

    if (seg->index == (u16) - 1) {
        memcpy(&global_gain, &seg->iir_type, sizeof(float));
        log_info("global_gain 0x%x\n", *(int *)&global_gain);
    }

    void *tar_seg = NULL;
    if (packet->cmd == EFF_MUSIC_EQ) {
        if (seg->index == (u16) - 1) {
            music_mode[tar].eq_parm.global_gain = global_gain;
        } else {
            tar_seg = &music_mode[tar].eq_parm.seg[seg->index];
        }
    } else if (packet->cmd == EFF_MUSIC_EQ2) {
#if TCFG_DYNAMIC_EQ_ENABLE
        if (seg->index == (u16) - 1) {
            music_eq2_parm[tar].global_gain = global_gain;
        } else {
            tar_seg = &music_eq2_parm[tar].seg[seg->index];
        }
#endif
    } else if (packet->cmd == EFF_MUSIC_HIGH_BASS_EQ) {
#if defined(TCFG_EQ_ENABLE) && TCFG_EQ_ENABLE && TCFG_AUDIO_OUT_EQ_ENABLE
        if (seg->index == (u16) - 1) {
            high_bass_eq_parm[tar].global_gain = global_gain;
        } else {
            tar_seg = &high_bass_eq_parm[tar].seg[seg->index];
            memcpy(tar_seg, seg, sizeof(struct eq_seg_info));
            if (update) {
                cur_eq_set_update(eq_name, tar_seg, get_eq_nsection(eq_name) + 2, 0); //工具段数 + 高低音段数2
            }
            return 0;
        }
#endif
    } else if (packet->cmd == EFF_AUX_EQ) {
        if (seg->index == (u16) - 1) {
            music_mode[tar].eq_parm.global_gain = global_gain;
        } else {
            tar_seg = &music_mode[tar].eq_parm.seg[seg->index];
        }
    } else if (packet->cmd == EFF_MUSIC_RL_EQ) {
#if (TCFG_AUDIO_DAC_CONNECT_MODE == DAC_OUTPUT_FRONT_LR_REAR_LR)
        if (seg->index == (u16) - 1) {
            rl_eq_parm[tar].global_gain = global_gain;
        } else {
            tar_seg = &rl_eq_parm[tar].seg[seg->index];
        }
#endif
    } else if (packet->cmd == EFF_MUSIC_FR_EQ) {
#if (TCFG_AUDIO_DAC_CONNECT_MODE == DAC_OUTPUT_FRONT_LR_REAR_LR)
        if (seg->index == (u16) - 1) {
            fr_eq_parm[tar].global_gain = global_gain;
        } else {
            tar_seg = &fr_eq_parm[tar].seg[seg->index];
        }
#endif
        right_ch = 1;
    } else if (packet->cmd == EFF_MUSIC_RR_EQ) {
#if (TCFG_AUDIO_DAC_CONNECT_MODE == DAC_OUTPUT_FRONT_LR_REAR_LR)
        if (seg->index == (u16) - 1) {
            rr_eq_parm[tar].global_gain = global_gain;
        } else {
            tar_seg = &rr_eq_parm[tar].seg[seg->index];
        }
#endif
        right_ch = 1;
    } else if (packet->cmd == EFF_MUSIC_EXT_EQ) {
#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
        if (seg->index == (u16) - 1) {
            muisc_ext_eq[tar].global_gain = global_gain;
        } else {
            tar_seg = &muisc_ext_eq[tar].seg[seg->index];
        }
#endif
    }
    if ((seg->index != (u16) - 1) && tar_seg) {
#if PARM_DEBUG
        log_info("idx:%d, iir:%d, frq:%d, gain:0x%x, q:0x%x \n", seg->index, seg->iir_type, seg->freq, *(int *)&seg->gain, *(int *)&seg->q);
#endif
        memcpy(tar_seg, seg, sizeof(struct eq_seg_info));
        if (update) {
            if (right_ch) {
                cur_eq_right_ch_set_update(eq_name, tar_seg, get_eq_nsection(eq_name), 0);
            } else {
                cur_eq_set_update(eq_name, tar_seg, get_eq_nsection(eq_name), 0);
            }
        }
    } else {
        if (update) {
            if (right_ch) {
                cur_eq_right_ch_set_global_gain(eq_name, global_gain);
            } else {
                cur_eq_set_global_gain(eq_name, global_gain);
            }
        }
    }
#endif
    return 0;
}


int mic_wdrc_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    u16 type = 0; //0:low  1:mid  2:high ,3:other band, 4:crossover
    u32 drc_name = get_module_name_and_index(packet->cmd, &type, 0);
    type = 0;//混响没有多带drc
    wdrc_struct_TOOL_SET *wdrc_parm = (wdrc_struct_TOOL_SET *)&packet->data[1];
    void *tar_wdrc = NULL;

    log_debug("wdrc_parm type %d\n", type);
    u8 mode_id = packet->data[0];
    u8 tar = packet->cmd - EFF_MIC_DRC0;
    tar_wdrc = &eff_mode[mode_id - mic_mode_seq0].drc_parm[tar];
    memcpy(tar_wdrc, wdrc_parm, sizeof(wdrc_struct_TOOL_SET));

    wdrc_printf(tar_wdrc);
    cur_drc_set_update(drc_name, type, tar_wdrc);
    cur_drc_set_bypass(drc_name, type, wdrc_parm->is_bypass);
#endif

    return 0;
}

int high_bass_wdrc_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE && TCFG_AUDIO_OUT_DRC_ENABLE
    u16 type = 0; //0:low  1:mid  2:high ,3:other band, 4:crossover
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 drc_name = get_module_name_and_index(packet->cmd, &type, tar);
    u8 update = 1;
    wdrc_struct_TOOL_SET *wdrc_parm = (wdrc_struct_TOOL_SET *)&packet->data[1];
    void *tar_wdrc = NULL;
#if LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        drc_name = get_module_name_and_index(packet->cmd, &type, tar);
        if (!get_cur_drc_hdl_by_name(drc_name)) {
            update = 0;
            /* return 0; */
        }

    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }
    log_debug("wdrc_parm type %d\n", type);
    tar_wdrc = &high_bass_drc_parm[tar];
    memcpy(tar_wdrc, wdrc_parm, sizeof(wdrc_struct_TOOL_SET));

    wdrc_printf(tar_wdrc);
    if (update) {
        cur_drc_set_update(drc_name, type, tar_wdrc);
        cur_drc_set_bypass(drc_name, type, wdrc_parm->is_bypass);
    }
#endif
    return 0;
}

int music_wdrc_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_DRC_ENABLE)&&TCFG_DRC_ENABLE
    u16 type = 0; //0:low  1:mid  2:high ,3:other band, 4:crossover
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 drc_name = get_module_name_and_index(packet->cmd, &type, tar);
    u8 update = 1;
#if LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        drc_name = get_module_name_and_index(packet->cmd, &type, tar);
        if (!get_cur_drc_hdl_by_name(drc_name)) {
            update = 0;
            /* return 0; */
        }
    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }

    struct mode_list *list = get_group_list(drc_name);
    wdrc_struct_TOOL_SET *wdrc_parm = (wdrc_struct_TOOL_SET *)&packet->data[1];
    void *tar_wdrc = NULL;

    log_debug("wdrc_parm type %d\n", type);
    if (type == (list->group_num - 1)) { ////最后一个存分频器系数
#if defined(MULTI_BAND_DRC) && MULTI_BAND_DRC
        CrossOverParam_TOOL_SET *parm = (CrossOverParam_TOOL_SET *)&packet->data[1];
        tar_wdrc = &music_mode[tar].drc_parm.crossover;
        memcpy(tar_wdrc, parm, sizeof(CrossOverParam_TOOL_SET));
        if (update) {
            cur_crossover_set_update(drc_name, tar_wdrc);
        }
        log_debug("way_num %d, N %d, low_freq %d, high_freq %d\n", parm->way_num, parm->N, parm->low_freq, parm->high_freq);
#endif
        return 0;
    } else {
        tar_wdrc = &music_mode[tar].drc_parm.wdrc_parm[type];
    }

    memcpy(tar_wdrc, wdrc_parm, sizeof(wdrc_struct_TOOL_SET));

    wdrc_printf(tar_wdrc);
    if (update) {
        cur_drc_set_update(drc_name, type, tar_wdrc);
        cur_drc_set_bypass(drc_name, type, wdrc_parm->is_bypass);
    }
#endif
    return 0;
}


int music_rl_wdrc_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if (TCFG_AUDIO_DAC_CONNECT_MODE == DAC_OUTPUT_FRONT_LR_REAR_LR)
    u16 type = 0; //0:low  1:mid  2:high ,3:other band, 4:crossover
    u8 mode_id = packet->data[0];
    u8 tar = 0;
    u32 drc_name = get_module_name_and_index(packet->cmd, &type, tar);
    u8 update = 1;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    if (linein_mode_seq == mode_id) {
        tar = 1;
        drc_name = get_module_name_and_index(packet->cmd, &type, tar);
        if (!get_cur_drc_hdl_by_name(drc_name)) {
            update = 0;
            /* return 0; */
        }
    } else
#endif
        if (music_mode_seq0 != mode_id) {
            return 0;
        }

    struct mode_list *list = get_group_list(drc_name);


    wdrc_struct_TOOL_SET *wdrc_parm = (wdrc_struct_TOOL_SET *)&packet->data[1];
    void *tar_wdrc = NULL;

    log_debug("wdrc_parm type %d\n", type);
    if (type == (list->group_num - 1)) { ////最后一个存分频器系数
#if defined(MULTI_BAND_DRC) && MULTI_BAND_DRC
        CrossOverParam_TOOL_SET *parm = (CrossOverParam_TOOL_SET *)&packet->data[1];
        tar_wdrc = &rl_drc_parm[tar].crossover;
        memcpy(tar_wdrc, parm, sizeof(CrossOverParam_TOOL_SET));
        if (update) {
            cur_crossover_set_update(drc_name, tar_wdrc);
        }
        log_debug("way_num %d, N %d, low_freq %d, high_freq %d\n", parm->way_num, parm->N, parm->low_freq, parm->high_freq);
#endif
        return 0;
    } else {
        tar_wdrc = &rl_drc_parm[tar].wdrc_parm[type];
    }

    memcpy(tar_wdrc, wdrc_parm, sizeof(wdrc_struct_TOOL_SET));

    wdrc_printf(tar_wdrc);
    if (update) {
        cur_drc_set_update(drc_name, type, tar_wdrc);
        cur_drc_set_bypass(drc_name, type, wdrc_parm->is_bypass);
    }
#endif
    return 0;
}


int phone_wdrc_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_DRC_ENABLE)&&TCFG_DRC_ENABLE && TCFG_PHONE_DRC_ENABLE
    u16 type = 0; //0:low  1:mid  2:high ,3:other band, 4:crossover
    u32 drc_name = get_module_name_and_index(packet->cmd, &type, 0);
    wdrc_struct_TOOL_SET *wdrc_parm = (wdrc_struct_TOOL_SET *)&packet->data[1];
    type = 0;//通话下行使用type位置做更新
    void *tar_wdrc = NULL;
    log_debug("wdrc_parm type %d\n", type);
    u8 tar = packet->cmd - EFF_PHONE_WIDEBAND_DRC;
    tar_wdrc = &phone_mode[tar].drc_parm;
    memcpy(tar_wdrc, wdrc_parm, sizeof(wdrc_struct_TOOL_SET));
    wdrc_printf(tar_wdrc);
    cur_drc_set_update(drc_name, type, tar_wdrc);
    cur_drc_set_bypass(drc_name, type, wdrc_parm->is_bypass);
#endif
    return 0;
}


int noise_gate_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    u8 mode_id = packet->data[0];
    memcpy(&eff_mode[mode_id - mic_mode_seq0].noise_gate_parm, &packet->data[1], sizeof(NoiseGateParam_TOOL_SET));
    NoiseGateParam_TOOL_SET *parmt = &eff_mode[mode_id - mic_mode_seq0].noise_gate_parm;
    noisegate_update_parm(&parmt->parm, parmt->is_bypass);

#if PARM_DEBUG
    noisegate_update_param *parm = &parmt->parm;
    log_debug("noise gate attact_time %d,releasetime %d, threshold %d, low_th_gain %d\n",
              parm->attackTime, parm->releaseTime, parm->threshold, parm->low_th_gain);
#endif
#endif
    return 0;
}

int howling_pitch_shift_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    u8 mode_id = packet->data[0];
    memcpy(&eff_mode[mode_id - mic_mode_seq0].howlingps_parm, &packet->data[1], sizeof(HowlingPs_PARM_TOOL_SET));
    HowlingPs_PARM_TOOL_SET *parmt = &eff_mode[mode_id - mic_mode_seq0].howlingps_parm;
    howling_pitch_shift_update_parm(&parmt->parm, parmt->is_bypass);
#if PARM_DEBUG
    HOWLING_PITCHSHIFT_PARM *parm = &parmt->parm;
    log_debug("ps_parm %d, fe_parm %d, effect_v %d\n", parm->ps_parm, parm->fe_parm, parm->effect_v);
#endif
#endif
    return 0;
}
int notchhowling_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    u8 mode_id = packet->data[0];
    memcpy(&eff_mode[mode_id - mic_mode_seq0].notchhowling_parm, &packet->data[1], sizeof(NotchHowlingParam_TOOL_SET));
    NotchHowlingParam_TOOL_SET *parmt = &eff_mode[mode_id - mic_mode_seq0].notchhowling_parm;
    notchhowline_update_parm(&parmt->parm, parmt->is_bypass);

#if PARM_DEBUG
    NotchHowlingParam *parm = &parmt->parm;
    log_debug("parmt->is_bypass %d\n", parmt->is_bypass);
    log_debug("Q 0x%x, gain 0x%x, fade_n %d\n", *(int *)&parm->Q, *(int *)&parm->gain, parm->fade_n);
#endif
#endif
    return 0;
}


int reverb_parm_analyze(EFF_ONLINE_PACKET *packet)
{
#if 0
    u8 mode_id = packet->data[0];
    memcpy(&eff_mode[mode_id - mic_mode_seq0].reverb_parm, &packet->data[1], sizeof(REVERBN_PARM_TOOL_SET));
    REVERBN_PARM_TOOL_SET *parmt = &eff_mode[mode_id - mic_mode_seq0].reverb_parm;
    reverb_update_parm(&parmt->parm);
    REVERBN_PARM_SET *parm = &parmt->parm;
    log_debug("dry %d,\n wet %d,\n delay %d,\n rot60 %d,\n Erwet %d,\nErfactor %d,\n Ewidth %d,\n Ertolate %d,\nErbasslpf %d,\nErbassB %d,\n predelay %d,\nallpassfeedback %d,\ndiffusion %d,\n dampinglpf %d,\n basslpf %d,\n bassB %d,\n lowcut %d\n",
              parm->dry,
              parm->wet,
              parm->delay,
              parm->rot60,
              parm->Erwet,
              parm->Erfactor,
              parm->Ewidth,
              parm->Ertolate,
              parm->Erbasslpf,
              parm->ErbassB,
              parm->predelay,
              parm->allpassfeedback,
              parm->diffusion,
              parm->dampinglpf,
              parm->basslpf,
              parm->bassB,
              parm->lowcut
             );
#endif
    return 0;
}

int plate_reverb_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    u8 mode_id = packet->data[0];
    memcpy(&eff_mode[mode_id - mic_mode_seq0].plate_reverb_parm, &packet->data[1], sizeof(Plate_reverb_TOOL_SET));
    Plate_reverb_TOOL_SET *parmt = &eff_mode[mode_id - mic_mode_seq0].plate_reverb_parm;
    plate_reverb_update_parm(&parmt->parm, parmt->is_bypass);

#if PARM_DEBUG
    Plate_reverb_parm *parm = &parmt->parm;
    log_debug("wet %d\n dry %d\n pre_delay %d\n highcutoff %d\n diffusion %d\n decayfactor %d\n highfrequencydamping %d\n modulate %d\n roomsize %d\n", parm->wet, parm->dry, parm->pre_delay, parm->highcutoff, parm->diffusion, parm->decayfactor, parm->highfrequencydamping, parm->modulate, parm->roomsize);
#endif
#endif
    return 0;
}


int echo_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    u8 mode_id = packet->data[0];
    memcpy(&eff_mode[mode_id - mic_mode_seq0].echo_parm, &packet->data[1], sizeof(EF_ECHO_TOOL_SET));
    EF_ECHO_TOOL_SET *parmt = &eff_mode[mode_id - mic_mode_seq0].echo_parm;
    echo_updata_parm(&parmt->parm, parmt->is_bypass);
#if PARM_DEBUG
    ECHO_PARM_SET *parm = &parmt->parm;
    log_debug("delay %d, decayval %d, filt_enable %d, lpf_cutoff %d, wetgain %d, drygain %d\n",
              parm->delay, parm->decayval, parm->filt_enable, parm->lpf_cutoff, parm->wetgain, parm->drygain);
#endif
#endif
    return 0;
}



void eff_send_packet(void *priv, u32 id, u8 *packet, int size, u8 sq)
{
#if (TCFG_CFG_TOOL_ENABLE || TCFG_ONLINE_ENABLE)
    all_assemble_package_send_to_pc(id, sq, packet, size);
#endif
}

void alive_timer_send_packet(void *p)
{
    u32 T = 2;
    u8 sq  = 0;
    eff_send_packet(NULL, T, (u8 *)"empty", 5, sq);
    if (alive_cnt++ > 15) { //连接超时，删除timer(工具可能关闭了)
        if (alive_timer) {
            sys_timer_del(alive_timer);
            alive_timer = 0;
        }
        alive_cnt = 0;
    }
}


int eff_tool_get_cfg_file_size(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
    struct file_s {
        int id;
        int fileid;
    };
    struct file_s fs;
    memcpy(&fs, packet, sizeof(struct file_s));
    u32 file_size  = 0;
    if (fs.fileid == EFF_CFG_FILE_ID) {
        FILE *file = NULL;
        file = fopen((const char *)EFF_CFG_FILE_NAME, "r");
        if (!file) {
            log_error("EFF_CFG_FILE_NAME err %s\n", EFF_CFG_FILE_NAME);
        } else {
            file_size = flen(file);
            fclose(file);
        }
        eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)&file_size, sizeof(u32), sq);
    }
    return 1;
}
int eff_tool_get_cfg_file_data(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
    struct file_s {
        int id;
        int fileid;
        int offset;
        int size;
    };
    struct file_s fs;
    memcpy(&fs, packet, sizeof(struct file_s));
    if (fs.fileid == (int)EFF_CFG_FILE_ID) {
        FILE *file = NULL;
        file = fopen((const char *)EFF_CFG_FILE_NAME, "r");
        if (!file) {
            return -EINVAL;
        }

        fseek(file, fs.offset, SEEK_SET);
        u8 *data = malloc(fs.size);
        if (!data) {
            fclose(file);
            return -EINVAL;
        }
        int ret = fread(file, data, fs.size);
        if (ret != fs.size) {
        }
        fclose(file);
        eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)data, fs.size, sq);
        free(data);
    }
    return 1;
}


static s32 eff_online_update(void *_packet)
{
    u8 *ptr = _packet;
    u8 id = ptr[0];
    u8 sq = ptr[1];
    int res = -1;
    EFF_ONLINE_PACKET *packet = (EFF_ONLINE_PACKET *)&ptr[2];
    struct cmd_interface *p = NULL;
#if PARM_DEBUG
    if (packet->cmd != 0x4) {//离线查询
        log_debug(" id 0x%x, sq %d\n", id, sq);
        log_debug("_cmd:0x%x mode_id %d\n", packet->cmd, packet->data[0]);
    }
#endif
    if (!alive_timer) {
        alive_timer = sys_timer_add(NULL, alive_timer_send_packet, 900);//定时往上位机推送，可能对工具离线的消息有帮助
    }
    list_for_each_cmd_interface(p) {
        if (p->cmd == packet->cmd) {
            if (p->cmd_deal) {
                res = p->cmd_deal(packet, REPLY_TO_TOOL, sq);
            }
        }
    }
    return res;
}

int eff_tool_get_version(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if TCFG_EFFECT_TOOL_ENABLE
    struct _ver_info {
        char sdkname[16];
        u8 eqver[4];
    };
    struct _ver_info _ver_info = {0};
    memcpy(_ver_info.sdkname, eff_sdk_name, sizeof(eff_sdk_name));
    memcpy(_ver_info.eqver, eff_eq_ver, sizeof(eff_eq_ver));
    eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)&_ver_info, sizeof(struct _ver_info), sq);
#endif
    return 1;
}
int eff_tool_set_channge_mode(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if TCFG_EFFECT_TOOL_ENABLE
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    struct cmd {
        int id;
        int modeId;
    };
    struct cmd cmd;
    memcpy(&cmd, packet, sizeof(struct cmd));
    log_debug("cmd.modeId %d\n", cmd.modeId);
    if ((cmd.modeId >= mic_mode_seq0) && (cmd.modeId <= mic_mode_seq7)) { //混响0~7
        set_mic_reverb_mode_by_id(cmd.modeId - mic_mode_seq0);
    }
#endif
    eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)"OK", 2, sq);
#endif
    return 1;
}
//在线检测应答
int eff_tool_set_inquire(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if TCFG_EFFECT_TOOL_ENABLE
    //eff_send_packet(NULL, id, (u8 *)"OK", 2, sq);//OK表示需要重传，NO表示不需要重传,ER还是表示未知命令
    eff_send_packet(NULL, id, (u8 *)"NO", 2, sq);//OK表示需要重传，NO表示不需要重传,ER还是表示未知命令
    alive_cnt = 0;
#endif
    return 1;
}

int eff_tool_resync_parm_begin(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if TCFG_EFFECT_TOOL_ENABLE
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    eff_mode_save = get_mic_eff_mode();
#endif
    eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)"OK", 2, sq);
#endif
    return 1;
}

int eff_tool_resync_parm_end(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
#if TCFG_EFFECT_TOOL_ENABLE
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
    u8 eff_mode = eff_mode_save;
    set_mic_reverb_mode_by_id(eff_mode);//还原混响的模式
#endif
    eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)"OK", 2, sq);
#endif
    return 1;
}


static void effect_tool_callback(u8 *_packet, u32 size)
{
#if TCFG_EFFECT_TOOL_ENABLE
    int res = 0;
    u8 *ptr = _packet;
    u8 id = ptr[0];
    u8 sq = ptr[1];
    u8 *packet = (u8 *)&ptr[2];
    ASSERT(((int)packet & 0x3) == 0, "buf %x size %d\n", (unsigned int)packet, size - 2);

    res = eff_online_update(ptr);
    if (!res) {
        log_debug("Ack");
        eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)"OK", 2, sq);
    } else if (res != 1) {
        log_debug("Nack");
        eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)"ER", 2, sq);
    }
#endif
}

#if TCFG_EFFECT_TOOL_ENABLE
REGISTER_DETECT_TARGET(eff_adj_target) = {
    .id         = 0x11,//EFF_CONFIG_ID,
    .tool_message_deal   = effect_tool_callback,
};

//在线调试不进power down
static u8 effect_tool_idle_query(void)
{
#if TCFG_EFFECT_TOOL_ENABLE
    return 0;
#endif
    return 1;
}

REGISTER_LP_TARGET(effect_adj_lp_target) = {
    .name = "effect_adj",
    .is_idle = effect_tool_idle_query,
};
#endif

#define INDEX_HEAD  0xFFFD
int eff_file_exist()
{
    FILE *file = fopen((const char *)EFF_CFG_FILE_NAME, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

int eff_file_analyze(u32 mode_id, u16 group_id, void *data_buf, u32 buf_len)
{
#if PARM_DEBUG
    log_debug("mode_id 0x%x, group_id 0x%x\n", mode_id, group_id);
#endif
    int ret = 0;
    u8 *crc_buf = NULL;
    FILE *file = NULL;
    file = fopen((const char *)EFF_CFG_FILE_NAME, "r");
    if (!file) {
        log_error("eff file open err\n");
        return  -ENOENT;
    }
    u8 FV = 0;
    if (1 != fread(file, &FV, 1)) {
        ret = -1;

        goto err_exit;
    }

    u8 ver[4] = {0};
    if (4 != fread(file, ver, 4)) {
        ret = -2;
        goto err_exit;
    }

    if (memcmp(ver, eff_eq_ver, sizeof(eff_eq_ver))) {
        log_error("eff ver err \n");
        log_error_hexdump((unsigned char *)ver, 4);
        fseek(file, 0, SEEK_SET);
        ret = -3;
        goto err_exit;
    }

    u16 index_head = 0;//索引头
    if (2 != fread(file, &index_head, 2)) {
        ret = -4;
        goto err_exit;
    }

    if (index_head != INDEX_HEAD) {
        ret = -5;
        goto err_exit;
    }
    u16 index_len = 0;//索引区域长度
    if (2 != fread(file, &index_len, 2)) {
        ret = -6;
        goto err_exit;
    }
    u8 mode_num = 0;//模式个数
    if (1 != fread(file, &mode_num, 1)) {
        ret = -7;
        goto err_exit;
    }

    u16 off0 = 0;	//组索引区域的偏移
    u16 off1 = 0;	//模式索引区域的偏移
    for (int mode_cnt = 0; mode_cnt < mode_num; mode_cnt++) {
        u16 mode_seq  = 0;//模式标识
        if (2 != fread(file, &mode_seq, 2)) {
            ret = -8;
            goto err_exit;
        }
        if (2 != fread(file, &off0, 2)) {
            ret = -9;
            goto err_exit;
        }
        if (2 != fread(file, &off1, 2)) {
            ret = -10;
            goto err_exit;
        }
        if (mode_seq != mode_id) { //查询到相应的模式
            continue;
        } else {
            break;
        }
    }
    fseek(file, off0, SEEK_SET);//偏移到组索引区域

    u8 group_num = 0;
    if (1 != fread(file, &group_num, 1)) {
        ret = -11;
        goto err_exit;
    }
    u16 g_id = 0;//模块id
    u16 group_id_addr = 0;//模块id的内容地址
    int group_cnt = 0;
    for (group_cnt = 0; group_cnt < group_num; group_cnt++) {
        if (2 != fread(file, &g_id, 2)) {
            ret = -12;
            goto err_exit;
        }
        if (2 != fread(file, &group_id_addr, 2)) {
            ret = -13;
            goto err_exit;
        }
        if (g_id != group_id) {
            continue;
        } else {
            break;
        }
    }
    if (group_cnt == group_num) {
        ret = -14;
#if PARM_DEBUG
        log_error("seek group id addr err\n");
#endif
        goto err_exit;
    }
    fseek(file, group_id_addr, SEEK_SET);//偏移到模式id的具体内容

    u16 crc16 = 0;
    u16 verify_group_id = 0;
    u16 group_len;
    if (2 != fread(file, &crc16, 2)) {
        ret = -15;
        goto err_exit;
    }
    int cur_addr = fpos(file);
    if (2 != fread(file, &verify_group_id, 2)) {
        ret = -16;
        goto err_exit;
    }
    if (2 != fread(file, &group_len, 2)) {
        ret = -17;
        goto err_exit;
    }
    if (!group_len) {
        ret = -18;
        goto err_exit;
    }
    if (verify_group_id != group_id) {
        ret = -19;
        log_error("verify_group_id %x != group_id %x\n", verify_group_id, group_id);
        log_error("verify groud id err\n");
        goto err_exit;
    }

    fseek(file, cur_addr, SEEK_SET);
    u16 size = sizeof(crc16) + sizeof(group_len);
    crc_buf = zalloc(group_len  + size);
    u8 *group_buf = &crc_buf[size];
    if ((group_len + size) != fread(file, crc_buf, group_len + size)) {
        ret = -20;
        goto err_exit;
    }
    u16 calc_crc = CRC16(crc_buf, group_len + size); //id + len +playload
    if (crc16 != calc_crc) {
        ret = -21;
        log_error("crc16 %x, calc_crc %x\n", crc16, calc_crc);
        goto err_exit;
    }
    if (!data_buf) {
        ret = -22;
#if PARM_DEBUG
        log_error("input data buf NULL\n");
#endif
        goto err_exit;
    }
    if (group_len <= buf_len) {
        memcpy(data_buf, group_buf, group_len);
    } else {
        ret = -23;
        log_error("input data buf len : %d < group_len:%d\n", buf_len, group_len);
        goto err_exit;
    }

err_exit:
    if (crc_buf) {
        free(crc_buf);
        crc_buf = NULL;
    }
    fclose(file);

#if PARM_DEBUG
    if (ret) {
        log_error("ret :%d, analyze eff file err, please check it\n", ret);
    }
#endif
    return ret;
}



void music_eq_printf(void *_parm)
{
#if PARM_DEBUG
    puts("----------------------- music eq ----------------------\n");
    struct music_eq_tool *parm =  _parm;
    log_debug("global_gain:0x%x\n", *(int *)&parm->global_gain);
    log_debug("seg_num:%d\n", parm->seg_num);
    struct eq_seg_info *seg;
    for (int i = 0; i < parm->seg_num; i++) {
        seg = &parm->seg[i];
        log_debug("idx:%d, iir:%d, frq:%d, gain:0x%x, q:0x%x \n", seg->index, seg->iir_type, seg->freq, *(int *)&seg->gain, *(int *)&seg->q);
    }
#endif
}
void music_eq2_printf(void *_parm)
{
#if PARM_DEBUG
    puts("----------------------- music eq2 ----------------------\n");
    struct music_eq2_tool *parm =  _parm;
    log_debug("global_gain:0x%x\n", *(int *)&parm->global_gain);
    log_debug("seg_num:%d\n", parm->seg_num);
    struct eq_seg_info *seg;
    for (int i = 0; i < parm->seg_num; i++) {
        seg = &parm->seg[i];
        log_debug("idx:%d, iir:%d, frq:%d, gain:0x%x, q:0x%x \n", seg->index, seg->iir_type, seg->freq, *(int *)&seg->gain, *(int *)&seg->q);
    }
#endif
}

void wdrc_printf(void *_wdrc)
{
#if PARM_DEBUG
    puts("---------------------- drc -----------------------\n");
    wdrc_struct_TOOL_SET *twdrc = _wdrc;
    struct wdrc_struct *wdrc = &twdrc->parm;
    printf("wdrc->is_bypass %d\n", twdrc->is_bypass);
    printf("wdrc attacktime %d, releasetime %d threshold_num %d rms_time %d algorithm%d, mode%d ,0x%x,0x%x\n",  wdrc->attacktime, wdrc->releasetime, wdrc->threshold_num, wdrc->rms_time, wdrc->algorithm, wdrc->mode, *(int *)&wdrc->inputgain, *(int *)&wdrc->outputgain);
    struct threshold_group  *tt = (struct threshold_group *)wdrc->threshold;
    for (int i = 0; i < wdrc->threshold_num; i++) {
        printf("in_threshold  0x%x, out_threshold 0x%x\n", *(int *)&tt[i].in_threshold, *(int *)&tt[i].out_threshold);
    }
#endif
}

void eq_printf(void *_parm)
{
#if PARM_DEBUG
    puts("----------------------- eq ----------------------\n");
    struct eq_tool *parm =  _parm;
    printf("global_gain:0x%x\n", *(int *)&parm->global_gain);
    printf("seg_num:%d\n", parm->seg_num);
    struct eq_seg_info *seg;// = parm->seg;
    for (int i = 0; i < parm->seg_num; i++) {
        seg = &parm->seg[i];
        printf("idx:%d, iir:%d, frq:%d, gain:0x%x, q:0x%x \n", seg->index, seg->iir_type, seg->freq, *(int *)&seg->gain, *(int *)&seg->q);
    }
#endif
}


/**
 *注册相应模块的解析函数cmd_deal,返回值0成功，返回小于0失败
 * */
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
REGISTER_CMD_TARGET(mic_g) = {//
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_GAIN,
    .cmd_deal = mic_gain_parm_analyze,
#endif
    .eff_default_parm_init = mic_gain_parm_default_init,
    .eff_file_analyze_init = mic_gain_file_analyze_init,
};
#endif
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
REGISTER_CMD_TARGET(music_g) = {
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_GAIN,
    .cmd_deal = gain_process_parm_analyze,
#endif
    .eff_default_parm_init = music_gain_parm_default_init,
    .eff_file_analyze_init = music_gain_file_analyze_init,

};
#endif
#if (TCFG_AUDIO_DAC_CONNECT_MODE == DAC_OUTPUT_FRONT_LR_REAR_LR)
REGISTER_CMD_TARGET(music_rl_g) = {//
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_RL_GAIN,
    .cmd_deal = rl_gain_process_parm_analyze,
#endif
    .eff_default_parm_init = rl_music_gain_parm_default_init,
    .eff_file_analyze_init = rl_music_gain_file_analyze_init,

};
#endif

#if TCFG_DYNAMIC_EQ_ENABLE
REGISTER_CMD_TARGET(dyeq) = {//动态eq
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_DYNAMIC_EQ,
    .cmd_deal = dynamic_eq_parm_analyze,
#endif
    .eff_default_parm_init = dynamic_eq_parm_default_init,
    .eff_file_analyze_init = dynamic_eq_file_analyze_init,
};
#endif
#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
REGISTER_CMD_TARGET(echo) = {//混响echo
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_ECHO,
    .cmd_deal = echo_parm_analyze,
#endif
    .eff_default_parm_init = echo_parm_default_init,
    .eff_file_analyze_init = echo_file_analyze_init,
};
REGISTER_CMD_TARGET(p_reverb) = {//混响plate_reverb
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_PLATE_REVERB,
    .cmd_deal = plate_reverb_parm_analyze,
#endif
    .eff_default_parm_init = plate_reverb_parm_default_init,
    .eff_file_analyze_init = plate_reverb_file_analyze_init,

};
REGISTER_CMD_TARGET(notch_howling) = {//混响弦波啸叫抑制
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_NOTCH_HOWLING,
    .cmd_deal = notchhowling_parm_analyze,
#endif
    .eff_default_parm_init = notchhowling_parm_default_init,
    .eff_file_analyze_init = notchhowling_file_analyze_init,
};
REGISTER_CMD_TARGET(howling_ps) = {//混响移频啸叫抑制
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_HOWLINE_PS,
    .cmd_deal = howling_pitch_shift_parm_analyze,
#endif
    .eff_default_parm_init = howling_ps_parm_default_init,
    .eff_file_analyze_init = howling_ps_file_analyze_init,

};
REGISTER_CMD_TARGET(noisegate) = {//混响门限噪声
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_NOISEGATE,
    .cmd_deal = noise_gate_parm_analyze,
#endif
    .eff_default_parm_init = noisegate_parm_default_init,
    .eff_file_analyze_init = noisegate_file_analyze_init,

};
#endif
#if defined(TCFG_DRC_ENABLE)&&TCFG_DRC_ENABLE && TCFG_PHONE_DRC_ENABLE
REGISTER_CMD_TARGET(pw_drc) = {//下行宽频drc
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_PHONE_WIDEBAND_DRC,
    .cmd_deal = phone_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = downlink_wideband_wdrc_parm_default_init,
    .eff_file_analyze_init = downlink_wideband_wdrc_file_analyze_init,
};
REGISTER_CMD_TARGET(ph_drc) = {//下行窄频drc
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_PHONE_NARROWBAND_DRC,
    .cmd_deal = phone_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = downlink_narrowband_wdrc_parm_default_init,
    .eff_file_analyze_init = downlink_narrowband_wdrc_file_analyze_init,

};
REGISTER_CMD_TARGET(aw_drc) = {//上行宽频drc
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_AEC_WIDEBAND_DRC,
    .cmd_deal = phone_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = uplink_wideband_wdrc_parm_default_init,
    .eff_file_analyze_init = uplink_wideband_wdrc_file_analyze_init,
};
REGISTER_CMD_TARGET(an_drc) = {//上行窄频drc
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_AEC_NARROWBAND_DRC,
    .cmd_deal = phone_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = uplink_narrowband_wdrc_parm_default_init,
    .eff_file_analyze_init = uplink_narrowband_wdrc_file_analyze_init,

};
#endif
#if defined(TCFG_DRC_ENABLE)&&TCFG_DRC_ENABLE
REGISTER_CMD_TARGET(ml_drc) = {//music_drc
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_LOW_DRC,
    .cmd_deal = music_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = music_low_wdrc_parm_default_init,
    .eff_file_analyze_init = music_low_wdrc_file_analyze_init,

};
#if defined(MULTI_BAND_DRC) && MULTI_BAND_DRC
REGISTER_CMD_TARGET(mm_drc) = {//music_drc
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_MID_DRC,
    .cmd_deal = music_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = music_mid_wdrc_parm_default_init,
    .eff_file_analyze_init = music_mid_wdrc_file_analyze_init,

};
REGISTER_CMD_TARGET(mh_drc) = {//music_drc
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_HIGH_DRC,
    .cmd_deal = music_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = music_high_wdrc_parm_default_init,
    .eff_file_analyze_init = music_high_wdrc_file_analyze_init,

};
REGISTER_CMD_TARGET(m_whole_drc) = {//music_drc
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_WHOLE_DRC,
    .cmd_deal = music_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = music_whole_wdrc_parm_default_init,
    .eff_file_analyze_init = music_whole_wdrc_file_analyze_init,

};
REGISTER_CMD_TARGET(m_cross) = {//music_crossover
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_CROSSOVER,
    .cmd_deal = music_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = music_crossover_wdrc_parm_default_init,
    .eff_file_analyze_init = music_crossover_wdrc_file_analyze_init,
};
#endif
#if (TCFG_AUDIO_DAC_CONNECT_MODE == DAC_OUTPUT_FRONT_LR_REAR_LR)
REGISTER_CMD_TARGET(rl_drc_p) = {//music_rl_drc
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_RL_LOW_DRC,
    .cmd_deal = music_rl_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = music_rl_wdrc_parm_default_init,
    .eff_file_analyze_init = music_rl_wdrc_file_analyze_init,

};
#endif

#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
REGISTER_CMD_TARGET(micDrc0) = {//mic_drc0
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_DRC0,
    .cmd_deal = mic_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = mic_wdrc0_parm_default_init,
    .eff_file_analyze_init = mic_wdrc0_file_analyze_init,

};
REGISTER_CMD_TARGET(micDrc1) = {//mic_drc1
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_DRC1,
    .cmd_deal = mic_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = mic_wdrc1_parm_default_init,
    .eff_file_analyze_init = mic_wdrc1_file_analyze_init,

};
REGISTER_CMD_TARGET(micDrc2) = {//mic_drc2
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_DRC2,
    .cmd_deal = mic_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = mic_wdrc2_parm_default_init,
    .eff_file_analyze_init = mic_wdrc2_file_analyze_init,

};
REGISTER_CMD_TARGET(micDrc3) = {//mic_drc3
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_DRC3,
    .cmd_deal = mic_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = mic_wdrc3_parm_default_init,
    .eff_file_analyze_init = mic_wdrc3_file_analyze_init,

};
REGISTER_CMD_TARGET(micDrc4) = {//mic_drc4
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_DRC4,
    .cmd_deal = mic_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = mic_wdrc4_parm_default_init,
    .eff_file_analyze_init = mic_wdrc4_file_analyze_init,

};
#endif
#endif
#if TCFG_DYNAMIC_EQ_ENABLE
REGISTER_CMD_TARGET(music_eq2) = {//music eq2
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_EQ2,
    .cmd_deal = music_eq_parm_analyze,
#endif
    .eff_default_parm_init = music_eq2_parm_default_init,
    .eff_file_analyze_init = music_eq2_file_analyze_init,
};
#endif

#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
REGISTER_CMD_TARGET(music_ext_eq) = {//music ext eq
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_EXT_EQ,
    .cmd_deal = music_eq_parm_analyze,
#endif
    .eff_default_parm_init = music_ext_eq_parm_default_init,
    .eff_file_analyze_init = music_ext_eq_file_analyze_init,
};
#endif

#if defined(CH_SWAP_OLD) && CH_SWAP_OLD
REGISTER_CMD_TARGET(ch_swap_adj) = {//music ch swap
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_CH_SWAP,
    .cmd_deal = music_ch_swap_process_parm_analyze,
#endif
    .eff_default_parm_init = music_ch_swap_parm_default_init,
    .eff_file_analyze_init = music_ch_swap_file_analyze_init,
};
#endif
REGISTER_CMD_TARGET(music_eq) = {//music eq
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_EQ,
    .cmd_deal = music_eq_parm_analyze,
#endif
    .eff_default_parm_init = music_eq_parm_default_init,
    .eff_file_analyze_init = music_eq_file_analyze_init,

};
#if defined(TCFG_EQ_ENABLE) && TCFG_EQ_ENABLE && TCFG_AUDIO_OUT_EQ_ENABLE
REGISTER_CMD_TARGET(music_hbass_eq) = {//high bass eq
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_HIGH_BASS_EQ,
    .cmd_deal = music_eq_parm_analyze,
#endif
    .eff_default_parm_init = high_bass_eq_parm_default_init,
    .eff_file_analyze_init = high_bass_eq_file_analyze_init,

};
#endif

#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
REGISTER_CMD_TARGET(micEq0) = {//mic eq0
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_EQ0,
    .cmd_deal = mic_eq_parm_analyze,
#endif
    .eff_default_parm_init = mic_eq0_parm_default_init,
    .eff_file_analyze_init = mic_eq0_file_analyze_init,

};
REGISTER_CMD_TARGET(micEq1) = {//mic eq1
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_EQ1,
    .cmd_deal = mic_eq_parm_analyze,
#endif
    .eff_default_parm_init = mic_eq1_parm_default_init,
    .eff_file_analyze_init = mic_eq1_file_analyze_init,

};
REGISTER_CMD_TARGET(micEq2) = {//mic eq2
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_EQ2,
    .cmd_deal = mic_eq_parm_analyze,
#endif
    .eff_default_parm_init = mic_eq2_parm_default_init,
    .eff_file_analyze_init = mic_eq2_file_analyze_init,

};
REGISTER_CMD_TARGET(micEq3) = {//mic eq3
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_EQ3,
    .cmd_deal = mic_eq_parm_analyze,
#endif
    .eff_default_parm_init = mic_eq3_parm_default_init,
    .eff_file_analyze_init = mic_eq3_file_analyze_init,

};
REGISTER_CMD_TARGET(micEq4) = {//mic eq4
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_EQ4,
    .cmd_deal = mic_eq_parm_analyze,
#endif
    .eff_default_parm_init = mic_eq4_parm_default_init,
    .eff_file_analyze_init = mic_eq4_file_analyze_init,

};
#endif

#if defined(TCFG_EQ_ENABLE) && TCFG_EQ_ENABLE && TCFG_PHONE_EQ_ENABLE
REGISTER_CMD_TARGET(ph_Eq) = {//通话下行宽频(16k sr)
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_PHONE_WIDEBAND_EQ,
    .cmd_deal = phone_eq_parm_analyze,
#endif
    .eff_default_parm_init = downlink_wideband_eq_parm_default_init,
    .eff_file_analyze_init = downlink_wideband_eq_file_analyze_init,

};
REGISTER_CMD_TARGET(pn_Eq) = {//通话下行窄频(8k sr)
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_PHONE_NARROWBAND_EQ,
    .cmd_deal = phone_eq_parm_analyze,
#endif
    .eff_default_parm_init = downlink_narrowband_eq_parm_default_init,
    .eff_file_analyze_init = downlink_narrowband_eq_file_analyze_init,

};
#endif
#if defined(TCFG_EQ_ENABLE) && TCFG_EQ_ENABLE && TCFG_AEC_UL_EQ_ENABLE
REGISTER_CMD_TARGET(aw_Eq) = {//通话上行宽频(16k sr)
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_AEC_WIDEBAND_EQ,
    .cmd_deal = phone_eq_parm_analyze,
#endif
    .eff_default_parm_init = uplink_wideband_eq_parm_default_init,
    .eff_file_analyze_init = uplink_wideband_eq_file_analyze_init,

};
REGISTER_CMD_TARGET(an_Eq) = {//通话上行窄频(8k sr)
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_AEC_NARROWBAND_EQ,
    .cmd_deal = phone_eq_parm_analyze,
#endif
    .eff_default_parm_init = uplink_narrowband_eq_parm_default_init,
    .eff_file_analyze_init = uplink_narrowband_eq_file_analyze_init,

};
#endif
#if (TCFG_AUDIO_DAC_CONNECT_MODE == DAC_OUTPUT_FRONT_LR_REAR_LR)
REGISTER_CMD_TARGET(lowpass_p) = {//2.1/2.2声道低通滤波器
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_RL_RR_LOW_PASS,
    .cmd_deal = low_pass_parm_analyze,
#endif
    .eff_default_parm_init = low_pass_parm_default_init,
    .eff_file_analyze_init = low_pass_file_analyze_init,

};

REGISTER_CMD_TARGET(music_rl_eq) = {//2.1/2.2声道低频通道eq
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_RL_EQ,
    .cmd_deal = music_eq_parm_analyze,
#endif
    .eff_default_parm_init = rl_eq_parm_default_init,
    .eff_file_analyze_init = rl_eq_file_analyze_init,

};

REGISTER_CMD_TARGET(music_rr_eq) = {//RReq
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_RR_EQ,
    .cmd_deal = music_eq_parm_analyze,
#endif
    .eff_default_parm_init = rr_eq_parm_default_init,
    .eff_file_analyze_init = rr_eq_file_analyze_init,

};
REGISTER_CMD_TARGET(music_fr_eq) = {//FReq
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_FR_EQ,
    .cmd_deal = music_eq_parm_analyze,
#endif
    .eff_default_parm_init = fr_eq_parm_default_init,
    .eff_file_analyze_init = fr_eq_file_analyze_init,

};
#endif

#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
REGISTER_CMD_TARGET(linein_eq) = {//linein eq
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_AUX_EQ,
    .cmd_deal = music_eq_parm_analyze,
#endif
    .eff_default_parm_init = aux_music_eq_parm_default_init,
    .eff_file_analyze_init = aux_music_eq_file_analyze_init,

};
REGISTER_CMD_TARGET(linein_drc) = {//linein drc
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_AUX_DRC,
    .cmd_deal = music_wdrc_parm_analyze,
#endif
    .eff_default_parm_init =  aux_music_low_wdrc_parm_default_init,
    .eff_file_analyze_init =  aux_music_low_wdrc_file_analyze_init,
};
#endif

#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE && TCFG_AUDIO_OUT_DRC_ENABLE
REGISTER_CMD_TARGET(music_hbass_drc) = {//high_bass_drc无多带
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_HIGH_BASS_DRC,
    .cmd_deal = high_bass_wdrc_parm_analyze,
#endif
    .eff_default_parm_init = high_bass_wdrc_parm_default_init,
    .eff_file_analyze_init = high_bass_wdrc_file_analyze_init,

};
#endif


#if defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE
REGISTER_CMD_TARGET(mic_voice_changer) = {
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_VOICE_CHANGER,
    .cmd_deal = mic_voice_changer_parm_ananlyze,
#endif
    .eff_default_parm_init = mic_voice_changer_parm_default_init,
    .eff_file_analyze_init = mic_voice_changer_file_analyze_init,

};


REGISTER_CMD_TARGET(mic_mix_gain) = {
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MIC_MIX_GAIN,
    .cmd_deal = mic_mix_gain_parm_ananlyze,
#endif
    .eff_default_parm_init = mix_gain_parm_default_init,
    .eff_file_analyze_init = mix_gain_file_analyze_init,

};
#endif


#if AUDIO_VBASS_CONFIG
REGISTER_CMD_TARGET(vbass_h) = {
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_VBASS,
    .cmd_deal = music_vbass_parm_ananlyze,
#endif
    .eff_default_parm_init = music_vbass_parm_default_init,
    .eff_file_analyze_init = music_vbass_file_analyze_init,

};

REGISTER_CMD_TARGET(vbass_prev_g) = {
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_VBASS_PREV_GAIN,
    .cmd_deal = vbass_prev_gain_process_parm_analyze,
#endif
    .eff_default_parm_init = vbass_prev_gain_parm_default_init,
    .eff_file_analyze_init = vbass_prev_gain_file_analyze_init,

};

#if MUSIC_NOISE_GATE_EN
REGISTER_CMD_TARGET(music_ns_gate) = {
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_NOISEGATE,
    .cmd_deal = music_noise_gate_parm_analyze,
#endif
    .eff_default_parm_init = music_noise_gate_parm_default_init,
    .eff_file_analyze_init = music_noise_gate_file_analyze_init,

};
#endif
#endif

REGISTER_CMD_TARGET(tunning_eq_p) = {
    .cmd = EFF_MUSIC_TUNNING_EQ,
    .cmd_deal = tunning_eq_parm_analyze,
    .eff_default_parm_init = tunning_parm_default_init,
    .eff_file_analyze_init = tunning_file_analyze_init,
};


REGISTER_CMD_TARGET(mic_tunning_eq_p) = {
    .cmd = EFF_MIC_TUNNING_EQ,
    .cmd_deal = mic_tunning_eq_parm_analyze,
    .eff_default_parm_init = mic_tunning_parm_default_init,
    .eff_file_analyze_init = mic_tunning_file_analyze_init,
};

#if AUDIO_SURROUND_CONFIG
REGISTER_CMD_TARGET(music_sur) = {
#if TCFG_EFFECT_TOOL_ENABLE
    .cmd = EFF_MUSIC_SURRROUND_EFF,
    .cmd_deal = music_surround_effect_parm_analyze,
#endif
    .eff_default_parm_init = music_surround_effect_parm_default_init,
    .eff_file_analyze_init = music_surround_effect_file_analyze_init,

};
#endif

/**
 *注册个别查询命令解析函数,返回值1成功，返回小于0失败
 * */
#if TCFG_EFFECT_TOOL_ENABLE
REGISTER_CMD_TARGET(version) = {
    .cmd = EFF_CMD_GETVER,
    .cmd_deal = eff_tool_get_version,
};
REGISTER_CMD_TARGET(file_s) = {
    .cmd = EFF_CMD_FILE_SIZE,
    .cmd_deal = eff_tool_get_cfg_file_size,
};
REGISTER_CMD_TARGET(file_p) = {
    .cmd = EFF_CMD_FILE,
    .cmd_deal = eff_tool_get_cfg_file_data,
};
REGISTER_CMD_TARGET(change_mode) = {
    .cmd = EFF_CMD_CHANGE_MODE,
    .cmd_deal = eff_tool_set_channge_mode,
};
REGISTER_CMD_TARGET(inquire) = {
    .cmd = EFF_CMD_INQUIRE,
    .cmd_deal = eff_tool_set_inquire,
};
REGISTER_CMD_TARGET(resync_begin) = {
    .cmd = EFF_CMD_RESYNC_PARM_BEGIN,
    .cmd_deal = eff_tool_resync_parm_begin,
};

REGISTER_CMD_TARGET(resync_end) = {
    .cmd = EFF_CMD_RESYNC_PARM_END,
    .cmd_deal = eff_tool_resync_parm_end,
};
#endif

void eff_default_parm_init()
{
    struct cmd_interface *p = NULL;
    list_for_each_cmd_interface(p) {
        if (p->eff_default_parm_init) {
            p->eff_default_parm_init();
        }
    }
}

void eff_file_parm_init()
{
    struct cmd_interface *p = NULL;
    list_for_each_cmd_interface(p) {
        if (p->eff_file_analyze_init) {
            p->eff_file_analyze_init();
        }
    }
}
/*
 *效果文件切换
 *path:效果文件路径
 * */
void eff_file_switch(u8 *path)
{
    //更换效果文件名
    memcpy((void *)EFF_CFG_FILE_NAME, (const void *)path, strlen(path));
    //重新解析效果文件
    eff_file_parm_init();

    //设置需要立即更新的框图模块
#if defined(TCFG_EQ_ENABLE) && TCFG_EQ_ENABLE
    cur_eq_set_update(AEID_MUSIC_EQ, NULL, 1, 0);//音乐EQ left
    cur_eq_right_ch_set_update(AEID_MUSIC_EQ, NULL, 1, 0);//音乐EQ right
    cur_eq_set_update(AEID_MUSIC_EXTEQ, NULL, 1, 0);
    cur_eq_set_update(AEID_MUSIC_EQ2, NULL, 1, 0);
    cur_eq_set_update(AEID_MUSIC_RL_EQ, NULL, 1, 0);
    cur_eq_set_update(AEID_HIGH_BASS_EQ, NULL, 1, 0);
    cur_eq_set_update(AEID_ESCO_DL_EQ, NULL, 1, 0);//通话eq
    cur_eq_set_update(AEID_ESCO_UL_EQ, NULL, 1, 0);//通话eq
#endif

#if defined(TCFG_DRC_ENABLE)&&TCFG_DRC_ENABLE
    cur_drc_set_update(AEID_MUSIC_DRC, 0, NULL);//wdrc
    cur_drc_set_update(AEID_MUSIC_RL_DRC, 0, NULL);//wdrc
    cur_drc_set_update(AEID_HIGH_BASS_DRC, 0, NULL);//wdrc
    cur_drc_set_update(AEID_ESCO_DL_DRC, 0, NULL);//通话wdrc
    cur_drc_set_update(AEID_ESCO_UL_DRC, 0, NULL);//通话wdrc
#endif

#if TCFG_DYNAMIC_EQ_ENABLE
    dynamic_eq_update_parm(AEID_MUSIC_DYNAMIC_EQ, 0);
#endif

#if AUDIO_SURROUND_CONFIG
    SurroundEffect_TOOL_SET *parmt = &sur_parm[0];
    audio_surround_effect_update_demo(AEID_MUSIC_SURROUND, &parmt->parm, parmt->is_bypass);
#endif
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN

#if defined(TCFG_EQ_ENABLE) && TCFG_EQ_ENABLE
    cur_eq_set_update(AEID_AUX_EQ, NULL, 1, 0);//音乐EQ left
    cur_eq_right_ch_set_update(AEID_AUX_EQ, NULL, 1, 0);//音乐EQ right
    cur_eq_set_update(AEID_AUX_EXTEQ, NULL, 1, 0);
    cur_eq_set_update(AEID_AUX_EQ2, NULL, 1, 0);
    cur_eq_set_update(AEID_AUX_RL_EQ, NULL, 1, 0);
    cur_eq_set_update(AEID_AUX_HIGH_BASS_EQ, NULL, 1, 0);
#endif

#if defined(TCFG_DRC_ENABLE)&&TCFG_DRC_ENABLE
    cur_drc_set_update(AEID_AUX_DRC, 0, NULL);//wdrc
    cur_drc_set_update(AEID_AUX_RL_DRC, 0, NULL);//wdrc
    cur_drc_set_update(AEID_AUX_HIGH_BASS_DRC, 0, NULL);//wdrc

#endif
#if TCFG_DYNAMIC_EQ_ENABLE
    dynamic_eq_update_parm(AEID_AUX_DYNAMIC_EQ, 1);
#endif

#if AUDIO_SURROUND_CONFIG
    parmt = &sur_parm[1];
    audio_surround_effect_update_demo(AEID_AUX_SURROUND, &parmt->parm, parmt->is_bypass);
#endif

#endif/*LINEIN_MODE_SOLE_EQ_EN*/



}

int eff_init(void)
{
    eff_default_parm_init();
    eff_file_parm_init();

#if (TCFG_EQ_ENABLE != 0)
    cp_eq_file_seg_to_custom_tab();
#endif
    return 0;
}
__initcall(eff_init);
#endif
