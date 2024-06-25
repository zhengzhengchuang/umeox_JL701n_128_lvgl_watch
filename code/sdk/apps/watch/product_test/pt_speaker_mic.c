#include "product_test.h"
#include "pt_speaker_mic.h"
#include "audio_config.h"
#include "audio_dec/audio_dec.h"
#include "audio_enc/audio_enc.h"
#include "spectrum/spectrum_fft.h"
#include "clock_cfg.h"

#if PT_SPEAKER_MIC_ENABLE

#define PT_SPEAKER_MIC_USE_SPECTRUM			1	// 频谱判断

static const struct audio_sin_param sine_test0[] = {
    /* {200 << 9, 4000, 0, 100}, */
    {  500 << 9, 0x7fffffff, 0, 100},
};
static const struct audio_sin_param sine_test1[] = {
    { 1000 << 9, 0x7fffffff, 0, 100},
};
static const struct audio_sin_param sine_test2[] = {
    { 3000 << 9, 0x7fffffff, 0, 100},
};

struct pt_sin_info {
    const struct audio_sin_param *sin;
    u8  sin_num;
#if PT_SPEAKER_MIC_USE_SPECTRUM
    u8  spectrum_num;	// 频谱点
    u8  spectrum_num_contrast;	// 对比频谱点
    u8  spectrum_diff_val;	// 频谱差值
#else
#error "需要频谱判断"
#endif /* #if PT_SPEAKER_MIC_USE_SPECTRUM*/
};

static struct pt_sin_info pt_sin_table[] = {
#if PT_SPEAKER_MIC_USE_SPECTRUM
    {sine_test0, ARRAY_SIZE(sine_test0), 0x03, 0x05, 0x05},
    {sine_test1, ARRAY_SIZE(sine_test1), 0x07, 0x09, 0x05},
    {sine_test2, ARRAY_SIZE(sine_test2), 0x10, 0x12, 0x05},
#else /* #if PT_SPEAKER_MIC_USE_SPECTRUM*/
    {sine_test0, ARRAY_SIZE(sine_test0)},
    {sine_test1, ARRAY_SIZE(sine_test1)},
    {sine_test2, ARRAY_SIZE(sine_test2)},
#endif /* #if PT_SPEAKER_MIC_USE_SPECTRUM*/
};

struct pt_speaker_mic {
    // 解码
    struct audio_dec_sine_app_hdl *dec;
    u16 dec_idx;	// 当前index
    u16 once_ms;	// 执行一次的时间
    u16 to_id;		// 超时

    // 编码
    struct audio_adc_output_hdl adc_output;
    struct adc_mic_ch    mic_ch;
    int mic_sr;
    u8  mic_gain;

    // 频谱
#if PT_SPEAKER_MIC_USE_SPECTRUM
    u8  spectrum_num;	// 频谱点
    u8  spectrum_num_contrast;	// 对比频谱点
    u8  spectrum_diff_val;	// 频谱差值
    u16 spectrum_cnt;
    spectrum_fft_hdl *spec_hdl;
#endif

    u8  status;		// 状态标志。0-idle，1-run，2-wait close
    u8  res;
};

static struct pt_speaker_mic pt_spk_mic = {
    .once_ms = 3000,
    .mic_gain = 7,
    .mic_sr  = 44100,
};

extern int audio_dec_sine_app_init_ok(struct audio_dec_sine_app_hdl *sine_dec);
extern int audio_dec_sine_app_play_end(struct audio_dec_sine_app_hdl *sine_dec);

static void pt_speaker_mic_play_next(void *priv);

int pt_speaker_mic_init(void)
{
    return 0;
}

#if PT_SPEAKER_MIC_USE_SPECTRUM
static void spectrum_check(void)
{
    if (pt_spk_mic.spec_hdl) {
        if (pt_spk_mic.spectrum_cnt == 0) {
            return ;
        }
        u8 db_num = audio_spectrum_fft_get_num(pt_spk_mic.spec_hdl);//获取频谱个数
        short *db_data = audio_spectrum_fft_get_val(pt_spk_mic.spec_hdl);//获取存储频谱值得地址
        if (!db_data) {
            return;
        }
        local_irq_disable();
        if (pt_spk_mic.spectrum_cnt && (db_num > pt_spk_mic.spectrum_num) && (db_num > pt_spk_mic.spectrum_num_contrast)) {
            s16 diff_val = db_data[pt_spk_mic.spectrum_num] - db_data[pt_spk_mic.spectrum_num_contrast];
            if (diff_val > pt_spk_mic.spectrum_diff_val) {
                pt_spk_mic.spectrum_cnt--;
            }
        }
        local_irq_enable();

#if 0
        {
            static u8 cnt = 0;
            cnt ++;
            if (cnt < 20) {
                return ;
            }
            cnt = 0;
            putchar('\n');
            for (int i = 0; i < db_num; i++) {
                //输出db_num个 db值
                /* printf("db_data db[%d] %d\n", i, db_data[i]); */
                put_u16hex(db_data[i]);
            }
            putchar('\n');
        }
#endif
    }
}
#endif/*PT_SPEAKER_MIC_USE_SPECTRUM*/

static void mic_output_data(void *priv, s16 *data, int len)
{
#if PT_SPEAKER_MIC_USE_SPECTRUM
    if (pt_spk_mic.spec_hdl) {
        audio_spectrum_fft_run(pt_spk_mic.spec_hdl, data, len);
        spectrum_check();
    }
#endif/*PT_SPEAKER_MIC_USE_SPECTRUM*/
}

static int pt_speaker_mic_enc_close(void)
{
    audio_mic_close(&pt_spk_mic.mic_ch, &pt_spk_mic.adc_output);

#if PT_SPEAKER_MIC_USE_SPECTRUM
    if (pt_spk_mic.spec_hdl) {
        audio_spectrum_fft_close(pt_spk_mic.spec_hdl);
        pt_spk_mic.spec_hdl = NULL;
    }
    clock_remove_set(SPECTRUM_CLK);
#endif/*PT_SPEAKER_MIC_USE_SPECTRUM*/

    return 0;
}

static int pt_speaker_mic_enc_open(void)
{
    pt_speaker_mic_enc_close();

    pt_spk_mic.adc_output.handler = mic_output_data;
    pt_spk_mic.adc_output.priv    = &pt_spk_mic;
    if (audio_mic_open(&pt_spk_mic.mic_ch, pt_spk_mic.mic_sr, pt_spk_mic.mic_gain) == 0) {
        audio_mic_add_output(&pt_spk_mic.adc_output);
        audio_mic_start(&pt_spk_mic.mic_ch);
    } else {
        return false;
    }

#if PT_SPEAKER_MIC_USE_SPECTRUM
    spectrum_fft_open_parm parm = {0};
    parm.sr = pt_spk_mic.mic_sr;
    parm.channel = 1;
    parm.attackFactor = 0.9;
    parm.releaseFactor = 0.9;
    parm.mode = 2;
    pt_spk_mic.spec_hdl = audio_spectrum_fft_open(&parm);

    clock_add_set(SPECTRUM_CLK);
#endif/* #if PT_SPEAKER_MIC_USE_SPECTRUM*/
    return false;
}

static int pt_dec_sine_app_evt_cb(void *priv, enum audio_dec_app_event event, int *param)
{
    /* log_i("audio_dec_sine_app_evt_cb, priv:0x%x, event:%d \n", priv, event); */
    struct audio_dec_sine_app_hdl *sine_dec = priv;
    switch (event) {
    case AUDIO_DEC_APP_EVENT_DEC_PROBE:
        if (sine_dec->sin_maker) {
            break;
        }
        // 先配置参数
        audio_dec_sine_app_probe(sine_dec);
        if (!sine_dec->sin_maker) {
            return -ENOENT;
        }
        break;
    case AUDIO_DEC_APP_EVENT_START_INIT_OK:
        log_i("sine_dec start init ok\n");
        audio_dec_sine_app_init_ok(sine_dec);
        break;
    case AUDIO_DEC_APP_EVENT_START_ERR:
        log_i("dec start err\n");
    case AUDIO_DEC_APP_EVENT_PLAY_END:
        log_i("dec play end\n");
        if (pt_spk_mic.dec == sine_dec) {
            audio_dec_sine_app_play_end(sine_dec);
            pt_spk_mic.dec = NULL;
        } else {
            log_w("sin_dec:0x%x, pt:0x%x \n", sine_dec, pt_spk_mic.dec);
        }
        break;
    default :
        break;
    }
    return 0;
}

static int pt_speaker_mic_play_close(void)
{
    if (pt_spk_mic.to_id) {
        sys_timeout_del(pt_spk_mic.to_id);
        pt_spk_mic.to_id = 0;
    }
    if (pt_spk_mic.dec) {
        audio_dec_sine_app_play_end(pt_spk_mic.dec);
        pt_spk_mic.dec = NULL;
    }
    return 0;
}
static int pt_speaker_mic_play_open(void)
{
    pt_speaker_mic_play_close();

    pt_spk_mic.dec = audio_dec_sine_app_create_by_parm(pt_sin_table[pt_spk_mic.dec_idx].sin, pt_sin_table[pt_spk_mic.dec_idx].sin_num, 1);
    if (pt_spk_mic.dec) {
#if PT_SPEAKER_MIC_USE_SPECTRUM
        pt_spk_mic.spectrum_num = pt_sin_table[pt_spk_mic.dec_idx].spectrum_num;
        pt_spk_mic.spectrum_num_contrast = pt_sin_table[pt_spk_mic.dec_idx].spectrum_num_contrast;
        pt_spk_mic.spectrum_diff_val = pt_sin_table[pt_spk_mic.dec_idx].spectrum_diff_val;
        pt_spk_mic.spectrum_cnt = 100;
#endif /* #if PT_SPEAKER_MIC_USE_SPECTRUM*/
        pt_spk_mic.dec_idx++;
        pt_spk_mic.dec->dec->evt_cb = pt_dec_sine_app_evt_cb;
        pt_spk_mic.dec->priv = pt_spk_mic.dec;
        audio_dec_sine_app_open(pt_spk_mic.dec);
        pt_spk_mic.to_id = sys_timeout_add(NULL, pt_speaker_mic_play_next, pt_spk_mic.once_ms);
        log_i("play idx:%d \n", pt_spk_mic.dec_idx);
        return true;
    }
    log_i("play err \n");
    return false;
}
static void pt_speaker_mic_play_next(void *priv)
{
    log_i("play next \n");

    u32 result = PT_E_OK;
    if (pt_spk_mic.status != 1) {
        return ;
    }
#if PT_SPEAKER_MIC_USE_SPECTRUM
    if (pt_spk_mic.spectrum_cnt) {
        log_i("spectrum err\n");
        result = PT_E_MOD_TEST_ERROR;
        goto __end;
    }
#endif/*PT_SPEAKER_MIC_USE_SPECTRUM*/
    if (pt_spk_mic.dec_idx >= ARRAY_SIZE(pt_sin_table)) {
        if (pt_spk_mic.dec) {
            // test end
            log_i("play end \n");
            goto __end;
        }
        return ;
    }
    int ret = pt_speaker_mic_play_open();
    if (ret == false) {
        // test err
        result = PT_E_MOD_TEST_ERROR;
        goto __end;
    }
    return ;

__end:
    pt_speaker_mic_play_close();
    pt_speaker_mic_enc_close();
    pt_spk_mic.res = result;
}

static int pt_speaker_mic_test_start(int priv)
{
    log_i("play test \n");
    tone_play_stop();

    pt_spk_mic.dec_idx = 0;

    int ret;
    u32 result = PT_E_OK;
    ret = pt_speaker_mic_enc_open();
    if (ret == false) {
    }
    ret = pt_speaker_mic_play_open();
    if (ret == false) {
        pt_speaker_mic_enc_close();
        pt_speaker_mic_play_close();
        result = PT_E_MOD_TEST_ERROR;
        pt_spk_mic.res = result;
    }
    return 0;
}
static int pt_speaker_mic_test_stop(int priv)
{
    log_i("play test stop \n");
    pt_speaker_mic_play_close();
    pt_speaker_mic_enc_close();
    pt_spk_mic.status = 0;
    return 0;
}

int pt_speaker_mic_start(void)
{
    if (pt_spk_mic.status) {
        return PT_E_MOD_RUN;
    }
    pt_spk_mic.res = PT_E_MOD_RUN;
    int msg[3] = {0};
    msg[0] = (int)pt_speaker_mic_test_start;
    msg[1] = 1;
    msg[2] = (int)0;
    do {
        int os_err = os_taskq_post_type("app_core", Q_CALLBACK, 3, msg);
        if (os_err == OS_ERR_NONE) {
            break;
        }
        if (os_err != OS_Q_FULL) {
            pt_spk_mic.res = PT_E_SYS_ERROR;
            return PT_E_SYS_ERROR;
        }
        os_time_dly(1);
    } while (1);

    pt_spk_mic.status = 1;

    return 0;
}

int pt_speaker_mic_stop(void)
{
    if (pt_spk_mic.status) {
        pt_spk_mic.status = 2;
        int msg[3] = {0};
        msg[0] = (int)pt_speaker_mic_test_stop;
        msg[1] = 1;
        msg[2] = (int)0;
        do {
            int os_err = os_taskq_post_type("app_core", Q_CALLBACK, 3, msg);
            if (os_err == OS_ERR_NONE) {
                break;
            }
            if (os_err != OS_Q_FULL) {
                if (pt_spk_mic.res == PT_E_MOD_RUN) {
                    pt_spk_mic.res = PT_E_MOD_STOP_NO_END;
                }
                return PT_E_MOD_CANT_STOP;//PT_E_SYS_ERROR;
            }
            os_time_dly(1);
        } while (1);
    }
    if (pt_spk_mic.res == PT_E_MOD_RUN) {
        pt_spk_mic.res = PT_E_MOD_STOP_NO_END;
    }
    return 0;
}

int pt_speaker_mic_ioctrl(u32 order, int len, void *param)
{
    u32 result = 0;
    switch (PT_ORDER_C_GET(order)) {
    case PT_N_C_START:
        result = pt_speaker_mic_start();
        break;
    case PT_N_C_STOP:
        result = pt_speaker_mic_stop();
        break;
    case PT_N_C_GET_RESULT:
        result = pt_spk_mic.res;
        break;
    default:
        result = PT_E_PARAM;
        break;
    }
    product_test_push_data(order, 4, &result);
    return result;
}


REGISTER_PT_MODULE(speaker_mic) = {
    .module = PT_M_SPEAKER_MIC,
    .attr	= PT_ATTR_SELF | PT_ATTR_ENV_OUT | PT_ATTR_ENV_IN,
    .init	= pt_speaker_mic_init,
    .ioctrl	= pt_speaker_mic_ioctrl,
};


void pt_speaker_mic_test(void)
{
    pt_speaker_mic_init();
    pt_speaker_mic_start();
}

#endif /* #if PT_SPEAKER_MIC_ENABLE */

