#include "audio_sound_track_2_p_x.h"
#include "app_config.h"
#include "audio_eff_default_parm.h"


#if defined(SOUND_TRACK_2_P_X_CH_CONFIG) &&SOUND_TRACK_2_P_X_CH_CONFIG && HIGH_GRADE_LOW_PASS_FILTER_EN
LowPassParam_TOOL_SET  low_pass_parm[mode_add];//rl rr通道低通滤波器

#define LOW_BASS_GLOBAL_GAIN_STEP   1//(0.2f)//低音总增益调节淡入淡出步进,范围(0.1f~1)
#define LOW_BASS_NSOS               4  //多阶低通滤波器使能,4阶级IIR(最大配8)
#define LOW_BASS_FREQ               (100)//低通滤波器的中心截止频率

#define LOW_BASS_THRESHOLD          (-0.5f)//低通滤波后的限幅器阈值,范围(-60~0)db
#define LOW_BASS_DRC_PRE_EN         1  //(低通滤波前是否需要对数据先做一次衰减)

static float cur_L_global_gain = 0;
static float cur_R_global_gain = 0;
static u16 fade_tmr = 0;

static const struct eq_seg_info low_bass_eq_tab[] = {//一段低通滤波器
    {0, EQ_IIR_TYPE_LOW_PASS, LOW_BASS_FREQ,   0, 0.7f}
};
static int your_eq_coeff_tab[2][5];//2段eq系数表的长度
static float low_bass_L_global_gain = 0;
static float low_bass_R_global_gain = 0;
int low_bass_eq_get_filter_info_demo(void *_eq, int sr, struct audio_eq_filter_info *info)
{
    if (!sr) {
        sr = 44100;
    }
#if TCFG_EQ_ENABLE
#if LOW_BASS_NSOS
    struct audio_eq *eq = (struct audio_eq *)_eq;
    /* local_irq_disable(); */
    u16 is_bypass = low_pass_parm.is_bypass;
    u16 center_freq = low_pass_parm.low_pass.fc;//中心截止频率
    u16 nsos = low_pass_parm.low_pass.order;//阶数
    u16 nsection = 0;
    if (!is_bypass) {
        if (!center_freq) {
            center_freq = 100;
        }
        if (!nsos) {
            nsos = 1;
        }
        nsection = ((nsos & 1) + (nsos >> 1));//LOW_BASS_NSOS;

        if (eq->eq_coeff_tab) {
            free(eq->eq_coeff_tab);
            eq->eq_coeff_tab = NULL;
        }
        if (!eq->eq_coeff_tab) {
            eq->eq_coeff_tab = zalloc(sizeof(int) * 5 * nsection);
        }
        butterworth_lp_design(center_freq, sr, nsos, eq->eq_coeff_tab);//多阶低通滤波器运算,得出系数
    } else {
        nsection = 1;
        eq_get_AllpassCoeff(eq->eq_coeff_tab);
    }
    /*
        for (int i = 0; i < nsection; i++) {
            printf("cal coeff:%d, %d, %d, %d, %d ",
                   eq->eq_coeff_tab[5 * i]
                   , eq->eq_coeff_tab[5 * i + 1]
                   , eq->eq_coeff_tab[5 * i + 2]
                   , eq->eq_coeff_tab[5 * i + 3]
                   , eq->eq_coeff_tab[5 * i + 4]
                  );

        }
    */
    /* local_irq_enable(); */
    info->L_coeff = info->R_coeff = (void *)eq->eq_coeff_tab;
#else
    local_irq_disable();
    u8 nsection = ARRAY_SIZE(low_bass_eq_tab);
    for (int i = 0; i < nsection; i++) {
        eq_seg_design(&low_bass_eq_tab[i], sr, your_eq_coeff_tab[i]);//根据采样率对eq系数表，重计算，得出适用的系数表
    }
    local_irq_enable();
    info->L_coeff = info->R_coeff = (void *)your_eq_coeff_tab;//系数指针赋值
#endif
    info->L_gain = cur_L_global_gain;
    info->R_gain = cur_R_global_gain;//总增益填写，用户可修改（-20~20db）,注意大于0db存在失真风险
    info->nsection = nsection;//eq段数，根据提供给的系数表来填写，例子是2
#endif//TCFG_EQ_ENABLE
    return 0;
}

static void global_gain_fade_run(void *p)
{
    struct audio_eq *eq = (struct audio_eq *)p;
    u8 update = 0;
    if (cur_L_global_gain > low_bass_L_global_gain) {
        cur_L_global_gain  -= LOW_BASS_GLOBAL_GAIN_STEP;
        if (cur_L_global_gain  < low_bass_L_global_gain) {
            cur_L_global_gain  = low_bass_L_global_gain;
        }
        update = 1;
    }
    if (cur_L_global_gain  < low_bass_L_global_gain) {
        cur_L_global_gain  += LOW_BASS_GLOBAL_GAIN_STEP;
        if (cur_L_global_gain  > low_bass_L_global_gain) {
            cur_L_global_gain  = low_bass_L_global_gain;
        }
        update = 1;
    }

    if (cur_R_global_gain  > low_bass_R_global_gain) {
        cur_R_global_gain -= LOW_BASS_GLOBAL_GAIN_STEP;
        if (cur_R_global_gain < low_bass_R_global_gain) {
            cur_R_global_gain = low_bass_R_global_gain;
        }
        update = 1;
    }
    if (cur_R_global_gain < low_bass_R_global_gain) {
        cur_R_global_gain += LOW_BASS_GLOBAL_GAIN_STEP;
        if (cur_R_global_gain > low_bass_R_global_gain) {
            cur_R_global_gain = low_bass_R_global_gain;
        }
        update = 1;
    }
    if (update) {
        update = 0;
        //printf("cur_L_global_gain %d, cur_R_global_gain %d\n", (int)cur_L_global_gain, (int)cur_R_global_gain);
        //printf("low_bass_L_global_gain %d, low_bass_R_global_gain %d\n", (int)low_bass_L_global_gain, (int)low_bass_R_global_gain);

        local_irq_disable();
        if (eq) {
            eq->updata = 1;
        }
        local_irq_enable();
    } else {
        if (fade_tmr) {
            sys_hi_timer_del(fade_tmr);
            fade_tmr = 0;
        }
    }
}

static void low_bass_eq_filter_info_update_demo(u32 eq_name)
{
#if TCFG_EQ_ENABLE
    struct audio_eq *eq = get_cur_eq_hdl_by_name(eq_name);
    if (!eq) {
        return;
    }
#if 1
    if (!fade_tmr) {
        fade_tmr = sys_hi_timer_add(eq, global_gain_fade_run, 20);
    }
#else
    // 参数直接更新，dB 跳跃大时，会有哒哒音
    cur_L_global_gain = low_bass_L_global_gain;
    cur_R_global_gain = low_bass_R_global_gain;
    local_irq_disable();
    if (eq) {
        eq->updata = 1;
    }
    local_irq_enable();
#endif
#endif//TCFG_EQ_ENABLE
}


void low_bass_set_global_gain(float left_global_gain, float right_global_gain)
{

    if (left_global_gain > 20) {
        left_global_gain = 20;
    } else if (left_global_gain < -90) {
        left_global_gain = -90;
    }

    if (right_global_gain > 20) {
        right_global_gain = 20;
    } else if (right_global_gain < -90) {
        right_global_gain = -90;
    }

    /* log_i("global_gain "); */
    /* put_buf(&left_global_gain, 4); */
    /* put_buf(&right_global_gain, 4); */
    low_bass_L_global_gain = left_global_gain;
    low_bass_R_global_gain = right_global_gain;
    low_bass_eq_filter_info_update_demo(EFF_MUSIC_RL_EQ);
}

void low_bass_set_left_global_gain(float left_global_gain)
{
    if (left_global_gain > 12) {
        left_global_gain = 12;
    } else if (left_global_gain < -12) {
        left_global_gain = -12;
    }

    log_i("left global_gain ");
    put_buf(&left_global_gain, 4);
    low_bass_L_global_gain = left_global_gain;
    low_bass_eq_filter_info_update_demo(EFF_MUSIC_RL_EQ);
}

void low_bass_set_right_global_gain(float right_global_gain)
{

    if (right_global_gain > 12) {
        right_global_gain = 12;
    } else if (right_global_gain < -12) {
        right_global_gain = -12;
    }

    log_i("right global_gain ");
    put_buf(&right_global_gain, 4);
    low_bass_R_global_gain = right_global_gain;
    low_bass_eq_filter_info_update_demo(EFF_MUSIC_RL_EQ);
}

void global_gain_test(void *p)
{
    struct sound_track_2_p_x *hdl = p;
    static char g_gain = -12;
    printf("g_gain %d\n", g_gain);
    low_bass_set_global_gain(g_gain, g_gain);
    g_gain++;
    if (g_gain > 0) {
        g_gain = -12;
    }
}
#endif/*defined(SOUND_TRACK_2_P_X_CH_CONFIG) && SOUND_TRACK_2_P_X_CH_CONFIG && HIGH_GRADE_LOW_PASS_FILTER_EN*/


#if defined(SOUND_TRACK_2_P_X_CH_CONFIG) &&SOUND_TRACK_2_P_X_CH_CONFIG
//left_righ_diff 0:左声道反相  1:右声道反相  2:不反相位
void pcm_dual_mix_to_dual_and_diff(void *out, void *in, u16 len, u8 left_righ_diff)
{
    s16 *outbuf = out;
    s16 *inbuf = in;
    s32 tmp32;
    len >>= 2;
    while (len--) {
        tmp32 = data_sat_s16((inbuf[0] + inbuf[1]) / 2);
        if (!left_righ_diff) {
            *outbuf++ = (tmp32 == -32768) ? 32767 : -tmp32;
            *outbuf++ = tmp32;
        } else if (left_righ_diff == 1) {
            *outbuf++ = tmp32;
            *outbuf++ = (tmp32 == -32768) ? 32767 : -tmp32;
        } else {
            *outbuf++ = tmp32;
            *outbuf++ = tmp32;
        }
        inbuf += 2;
    }
}

int stream_rl_rr_mix_data_handler(struct audio_stream_entry *entry,  struct audio_data_frame *in)
{
    if (in->offset == 0) {
#if (TWO_POINT_X_CH == TWO_POINT_ONE_CH)
        pcm_dual_mix_to_dual_and_diff(in->data, in->data, in->data_len, 2);//左右叠加除2后，
#endif
    }
    return in->data_len;
}
#endif/*defined(SOUND_TRACK_2_P_X_CH_CONFIG) && SOUND_TRACK_2_P_X_CH_CONFIG*/
