/*****************************************************************
>file name : audio_bt_emitter_hw.c
*****************************************************************/
#define LOG_TAG     "[BT_EMITTER-hw]"
#define LOG_ERROR_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DEBUG_ENABLE
/* #define LOG_DUMP_ENABLE */
#include "debug.h"
#include "audio_bt_emitter.h"
/* #include "sound/pcm.h" */
#include "audio_sbc_codec.h"
#include "application/audio_dig_vol.h"
#include "system/includes.h"


#define AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME		1	// 使用系统时钟来做传输时间戳参考
#define AUDIO_BT_EMITTER_USE_BT_TIME_ADJUST			1	// 使用蓝牙时钟校准

struct audio_bt_emitter_timestamp {
    u16 id;
    u16 run_once;
    u32 run_points;
#if AUDIO_BT_EMITTER_USE_BT_TIME_ADJUST
    u32 bt_time;
    int bt_cnt;
#endif /* #if AUDIO_BT_EMITTER_USE_BT_TIME_ADJUST */
};

struct audio_bt_emitter_volume {
    s16 volume[BT_EMITTER_CH_MAX];
};

struct audio_bt_emitter_hw {
    volatile u32 ptr;
    volatile u32 points;
    volatile u8 irq_en;
    volatile u8 output_flag;
    u8  channels;
    int sample_rate;
    s16 *addr;
    int frame_len;
    struct audio_bt_emitter_volume  		vol; 			/* BT_EMITTER 音量 */
#if AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME
    struct audio_bt_emitter_timestamp 		ts; 			/* BT_EMITTER 时间戳*/
#endif /*#if AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME*/
    struct audio_bt_emitter_context *bt_emitter;
};

static struct audio_bt_emitter_hw 	bt_emitter_hw = {0};
#define __this		(&bt_emitter_hw)

extern void local_bt_us_time(u32 *_clkn, u32 *finecnt);

#if AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME
static void audio_bt_emitter_time_func(void *priv)
{
    if (!__this) {
        return ;
    }
    u32 run_once = __this->ts.run_once;
#if AUDIO_BT_EMITTER_USE_BT_TIME_ADJUST
    u32 _clkn, finecnt;
    local_bt_us_time(&_clkn, &finecnt);
    int offset_time = ((_clkn - __this->ts.bt_time) & 0x7ffffff) * 625 / 1000;
    __this->ts.bt_cnt += 10;
    if (offset_time >= (__this->ts.bt_cnt + 10)) {
        __this->ts.bt_time = _clkn;
        run_once += __this->ts.run_once;
        __this->ts.bt_cnt = (__this->ts.bt_cnt + 10) - offset_time;
    } else if (offset_time <= (__this->ts.bt_cnt - 10)) {
        __this->ts.bt_time = _clkn;
        run_once -= __this->ts.run_once;
        __this->ts.bt_cnt = (__this->ts.bt_cnt - 10) - offset_time;
    }
    /* printf("t:%d, %d, o:%d \n", __this->ts.bt_cnt, offset_time, run_once); */
#endif /* #if AUDIO_BT_EMITTER_USE_BT_TIME_ADJUST */
    local_irq_disable();
    __this->ts.run_points += run_once;
    local_irq_enable();
    if (__this->irq_en) {
        __this->irq_en = 0;
        struct sound_pcm_substream *substream;
        list_for_each_entry(substream, &__this->bt_emitter->stream_list, entry) {
            sound_pcm_substream_irq_handler(substream);
        }
    }
}
#endif /*#if AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME*/


void audio_bt_emitter_hw_output(void)
{
    int ret, len, wlen;
    s16 *obuf = NULL;
    s16 *waddr;
    s32 t_value;
    long long tmp64;
    local_irq_disable();
    if (__this->output_flag) {
        local_irq_enable();
        return ; // 防重入
    }
    __this->output_flag = 1;
    local_irq_enable();

    while (1) {
        local_irq_disable();
        if (__this->points == 0) {
            local_irq_enable();
            break;
        }
        int points = __this->frame_len - __this->ptr;
        if (points > __this->points) {
            points = __this->points;
        }
#if AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME
        if (points > __this->ts.run_points) {
            points = __this->ts.run_points;
        }
        if (points == 0) {
            local_irq_enable();
            break;
        }
#endif /*#if AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME*/
        local_irq_enable();
        ret = audio_sbc_enc_write_alloc(&obuf, &len);
        if (ret == false) {
            putchar('L');
            /* break; */
            goto __out_tail; // 没启动，按节奏丢数
        }
        if (len == 0) {
            break;
        }
        waddr = (s16 *)__this->addr + (__this->ptr * __this->channels);
        wlen = points * 2 * __this->channels;
        if (wlen > len) {
            wlen = len;
            points = wlen / 2 / __this->channels;
        }
        if (__this->channels == 2) {	// 双声道
            audio_digital_vol_dual_data_run(obuf, waddr, wlen >> 2, __this->vol.volume[0], __this->vol.volume[1]);
        } else { // 单声道
            audio_digital_vol_single_data_run(obuf, waddr, wlen >> 1, __this->vol.volume[0]);
        }
__out_tail:
        local_irq_disable();
        __this->ptr += points;
        if (__this->ptr >= __this->frame_len) {
            __this->ptr -= __this->frame_len;
        }
        if (__this->points >= points) {
            __this->points -= points;
        } else {
            local_irq_enable();
            ASSERT((__this->points >= points), "hwpt:%d, pt:%d", __this->points, points);
        }
#if AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME
        if (__this->ts.run_points >= points) {
            __this->ts.run_points -= points;
        } else {
            local_irq_enable();
            ASSERT((__this->ts.run_points >= points), "tspt:%d, pt:%d", __this->ts.run_points, points);
        }
#endif /*#if AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME*/
        local_irq_enable();

        if (obuf) {
            audio_sbc_enc_write_updata(wlen);
        }
    }

    local_irq_disable();
    __this->output_flag = 0;
    local_irq_enable();
}

/*************************************************************************
 * BT_EMITTER 模拟硬件初始化
 *
 * Input    :  bt_emitter  - audio_bt_emitter_context指针
 * Output   :  无
 * Notes    :  初始化，仅BT_EMITTER内部使用.
 *=======================================================================*/
void audio_bt_emitter_hw_init(struct audio_bt_emitter_context *bt_emitter)
{
    __this->irq_en = 0;
    __this->points = 0;
    __this->ptr = 0;
    __this->bt_emitter = bt_emitter;
    __this->addr = bt_emitter->fifo.addr;
    __this->frame_len = bt_emitter->fifo.frame_len;
    __this->channels = bt_emitter->hw_params.channels;
    __this->sample_rate = bt_emitter->hw_params.sample_rate;
}

/*************************************************************************
 * BT_EMITTER 模拟硬件关闭dma使能
 *
 * Input    :  无
 * Output   :  无
 * Notes    :  关闭dma使能，仅BT_EMITTER内部使用.
 *=======================================================================*/
void audio_bt_emitter_hw_dma_disable(void)
{
#if AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME
    if (__this->ts.id) {
        sys_s_hi_timer_del(__this->ts.id);
        __this->ts.id = 0;
    }
#endif /*#if AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME*/
    audio_sbc_enc_data_stop_pre();
}

/*************************************************************************
 * BT_EMITTER 模拟硬件dma使能
 *
 * Input    :  无
 * Output   :  无
 * Notes    :  dma使能，仅BT_EMITTER内部使用.
 *=======================================================================*/
void audio_bt_emitter_hw_dma_enable(void)
{
#if AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME
    if (__this->ts.id == 0) {
        __this->ts.run_points = 0;
        __this->ts.run_once = __this->sample_rate * 10 / 1000;
#if AUDIO_BT_EMITTER_USE_BT_TIME_ADJUST
        u32 _clkn, finecnt;
        local_bt_us_time(&_clkn, &finecnt);
        __this->ts.bt_time = _clkn;
        __this->ts.bt_cnt = 0;
#endif /* #if AUDIO_BT_EMITTER_USE_BT_TIME_ADJUST */
        __this->ts.id = sys_s_hi_timer_add(__this, audio_bt_emitter_time_func, 10);
        // y_printf("run_once:%d \n", __this->ts.run_once);
        // y_printf("sr:%d, ch:%d \n", __this->sample_rate, __this->channels);
    }
#endif /*#if AUDIO_BT_EMITTER_TIMESTAMP_USE_SYS_TIME*/
    __this->irq_en = 1;
}

/*************************************************************************
 * BT_EMITTER 模拟硬件更新fifo
 *
 * Input    :  len - 更新长度
 * Output   :  无
 * Notes    :  更新fifo，仅BT_EMITTER内部使用.
 *=======================================================================*/
void audio_bt_emitter_hw_fifo_write_update(u32 len)
{
    local_irq_disable();
    __this->points += len;
    local_irq_enable();

    if (audio_sbc_enc_is_work()) {
        audio_sbc_enc_resume();
    } else {
        audio_bt_emitter_hw_output(); // 没启动，按节奏丢数
    }
}

/*************************************************************************
 * BT_EMITTER 模拟硬件获取fifo数据长度
 *
 * Input    :  无
 * Output   :  fifo数据长度
 * Notes    :  获取fifo数据长度，仅BT_EMITTER内部使用.
 *=======================================================================*/
int audio_bt_emitter_hw_fifo_get_buff_len(void)
{
    return __this->points;
}


/*************************************************************************
 * BT_EMITTER 模拟硬件设置音量
 *
 * Input    :  ch - 声道
 *             vol - 音量
 * Output   :  无
 * Notes    :  设置音量
 *=======================================================================*/
void audio_bt_emitter_hw_set_digital_gain(u8 ch, s16 vol)
{
    __this->vol.volume[ch] = vol;
}

/*************************************************************************
 * BT_EMITTER 模拟硬件获取音量
 *
 * Input    :  无
 * Output   :  音量值
 * Notes    :  获取音量，仅BT_EMITTER内部使用.
 *=======================================================================*/
s16 audio_bt_emitter_hw_get_digital_gain(u8 ch)
{
    return __this->vol.volume[ch];
}

