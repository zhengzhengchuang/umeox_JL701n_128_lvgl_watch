/*************************************************************/
/** @file:		audio_way.c
    @brief:		音频输出方式管理
    @details:
    @author:	Huxi
    @date: 		2021-06-18
    @note:
		基本调用流程：
		1、调用audio_way_init()初始化audio输出方式
		 	根据宏定义AUDIO_OUT_WAY_TYPE配置所有输出方式
		2、调用audio_way_open()打开输出方式
		3、调用audio_way_set_sample_rate()等函数配置参数
		4、调用audio_way_start()启动输出方式
		5、调用audio_way_output_write()输出音频数据
		6、调用audio_way_stop()停止输出方式
		7、调用audio_way_close()关闭输出方式

		链表排序规则：
				调用audio_way_open()时，按照audio_way_prio[]数组中的顺序挂载到链表中。
		 	当数组中没有包含某个输出方式时，该输出方式挂载到链表最后面
		 	如果想要更改顺序，可以在audio_way_open()函数调用之前更改audio_way_prio[]数组

		输出规则：
		 	调用audio_way_output_write()输出时，按照链表里的输出方式依次输出。
		 	链表里的第一个活动输出方式为主输出方式，实际输出长度以该输出方式为准，
		 		后续的输出方式out长度为主输出方式的实际输出长度，且不使用该输出方式的实际输出长度
		 	示例：链表里有两个活动输出方式，依次是dac,iis，伪代码如下
		 		audio_way_output_write(buf, len) // 总共有len
		 			dac_olen = dac_out(buf, len);	// dac实际输出了dac_olen
		 			iis_out(buf, dac_olen);	// iis只输出dac_olen的长度，且不管是否输出完
		 			return dac_olen;		// 实际输出长度就是主输出方式dac的实际输出长度
 */
/*************************************************************/

#include "app_config.h"
#include "media/includes.h"
#include "audio_way.h"
#include "audio_way_dac.h"
#include "audio_way_bt_emitter.h"
#include "audio_link.h"


#ifndef AUDIO_OUT_WAY_TYPE
#error "no defined AUDIO_OUT_WAY_TYPE"
#endif

#define AUDIO_WAY_CH_SWITCH_BUF_POINTS			(128*2)

#define DIGITAL_VOL_MAX		31
extern const u16 dig_vol_table[DIGITAL_VOL_MAX + 1];

/*
 * 声道转换
 */
struct audio_way_ch_switch {
    s16 buf[AUDIO_WAY_CH_SWITCH_BUF_POINTS];
    u16 total_len;
    u16 remain_len;
};

/*
 * audio各输出方式优先级
 * 排序越靠前优先级越高
 */
u32 audio_way_prio[AUDIO_WAY_TYPE_MAX] = {
    AUDIO_WAY_TYPE_DAC	,
    AUDIO_WAY_TYPE_IIS,
    AUDIO_WAY_TYPE_FM,
    AUDIO_WAY_TYPE_HDMI,
    AUDIO_WAY_TYPE_SPDIF,
    AUDIO_WAY_TYPE_BT,
    AUDIO_WAY_TYPE_DONGLE,
};

static const int pcm_rates[] = {
    8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000,
    64000, 88200, 96000, 128000, 176400, 192000,
};



struct audio_way_head 	audio_way_head;
#define __this			(&audio_way_head)


#define LIST_FOR_EACH_ENTRY_HEAD() \
	list_for_each_entry(p, &__this->head, entry) { \
		for (int i=0; i<AUDIO_WAY_TYPE_MAX ; i++) { \
			if ((way_type & BIT(i)) && (p->way_type == BIT(i))) \

#define LIST_FOR_EACH_ENTRY_END() \
		} \
	} \

#define LIST_FOR_EACH_SOUND_DEV(type) \
	list_for_each_entry(p, &__this->head, entry) { \
		for (int i=0; i<AUDIO_WAY_TYPE_MAX ; i++) { \
			if ((type & BIT(i)) && (p->way_type == BIT(i))) \

#define LIST_FOR_EACH_SOUND_END() \
		} \
	} \

extern int digital_vol_level_to_gain(int level);
extern int digital_vol_gain_to_level(int gain);


/*
*********************************************************************
*                  Audio App Resume
* Description: audio激活
* Arguments  : None.
* Return	 : None.
* Note(s)    : None.
*********************************************************************
*/
__attribute__((weak))
void audio_way_resume(void)
{
}

extern ALINK_PARM alink0_platform_data;
static void audio_iis_platform_init(void)
{
#if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_IIS)
    struct sound_pcm_platform_data data;
    data.private_data = (void *)&alink0_platform_data;
    sound_platform_load("iis", &data);
#endif
}

static void audio_iis_wakeup_irq_handler(void *priv)
{
    audio_way_resume();
}


static struct audio_way *audio_iis_platform_pcm_open(void)
{
    struct audio_way *audio_hdl;
    struct sound_pcm_stream *iis = NULL;

    int err = sound_pcm_create(&iis, "iis", 0);
    if (err) {
        return NULL;
    }
    sound_pcm_set_irq_handler(iis, NULL, audio_iis_wakeup_irq_handler);

    audio_hdl = zalloc(sizeof(struct audio_way));
    audio_hdl->way_type = AUDIO_WAY_TYPE_IIS;
    audio_hdl->stream = iis;
    audio_hdl->state = AUDIO_WAY_STATE_IDLE;
    audio_hdl->out_ch = 2;

    return audio_hdl;
}

/*
*********************************************************************
*                  Audio App Initialize
* Description: 初始化所有audio输出方式
* Arguments  : None.
* Return	 : None.
* Note(s)    : None.
*********************************************************************
*/
void audio_way_init(void)
{
    memset(__this, 0, sizeof(struct audio_way_head));

    INIT_LIST_HEAD(&__this->head);
    os_mutex_create(&__this->mutex);

    /***********sound platform init***********/
    sound_platform_init();

    /***********DAC platform load***********/
#if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC)
    audio_way_dac_init();
    audio_way_open(AUDIO_WAY_TYPE_DAC);
#endif /*(AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC)*/

#if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_BT)
    audio_way_bt_emitter_init();
    /* audio_way_open(AUDIO_OUT_WAY_TYPE); */
#endif /*(AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_BT)*/

#if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_IIS)
#if !(AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC)
    audio_codec_irq_init(NULL, 0);
#endif
    audio_iis_platform_init();
#endif /*(AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_IIS)*/
}

/*
*********************************************************************
*                  Audio App List Add
* Description: 把audio输出方式加到链表中
* Arguments  : *audio_hdl	audio句柄
* Return	 : None.
* Note(s)    : 按audio_way_prio[]数组中的排序依次添加
*********************************************************************
*/
static void audio_way_list_add(struct audio_way *audio_hdl)
{
    struct audio_way *p;
    int i = 0;
    int index = 0;
    // 查找输出方式在audio_way_prio[]中的位置
    for (; index < AUDIO_WAY_TYPE_MAX; index++) {
        if (audio_hdl->way_type == audio_way_prio[index]) {
            break;
        }
    }
    if (index < AUDIO_WAY_TYPE_MAX) { // 输出方式在数组之中
        list_for_each_entry(p, &__this->head, entry) {
            for (; i < AUDIO_WAY_TYPE_MAX; i++) {
                if (p->way_type == audio_way_prio[i]) { // 链表中的输出方式在数组之中
                    break;
                }
            }
            if (index <= i) { // 新添加的排序靠前
                list_add(&audio_hdl->entry, p->entry.prev);
                return ;
            }
        }
    }
    // 没排序的放最后面
    list_add_tail(&audio_hdl->entry, &__this->head);
}


/*
*********************************************************************
*                  Audio App List Check
* Description: 检查链表中是否有该输出方式
* Arguments  : way_type		audio输出方式
* Return	 : 输出方式句柄
* Note(s)    : None.
*********************************************************************
*/
static struct audio_way *audio_way_list_check(u32 way_type)
{
    struct audio_way *p;
    list_for_each_entry(p, &__this->head, entry) {
        if (p->way_type == way_type) {
            return p;
        }
    }
    return NULL;
}


/*
*********************************************************************
*                  Audio App Open
* Description: 打开audio输出方式
* Arguments  : way_type		audio输出方式
* Return	 : 0			成功
* Note(s)    : None.
*********************************************************************
*/
int audio_way_open(u32 way_type)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *audio_hdl;

    AUDIO_WAY_PEND();
    for (int i = 0; i < AUDIO_WAY_TYPE_MAX ; i++) {
        if (way_type & BIT(i)) {
            if (audio_way_list_check(BIT(i))) { // 检查输出方式是否已经存在
                log_w("audio[%d] have \n", i);
                if (ret) {
                    ret = AUDIO_WAY_ERR_ALREADY_OPEN;
                }
            } else {
                audio_hdl = NULL;
#if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC)
                if (BIT(i) == AUDIO_WAY_TYPE_DAC) {
                    audio_hdl = audio_way_dac_open();
                }
#endif /*(AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC)*/
#if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_BT)
                if (BIT(i) == AUDIO_WAY_TYPE_BT) {
                    audio_hdl = audio_way_bt_emitter_open();
                }
#endif /*(AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_BT)*/
#if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_IIS)
                if (BIT(i) == AUDIO_WAY_TYPE_IIS) {
                    audio_hdl = audio_iis_platform_pcm_open();
                }
#endif
                if (audio_hdl) {
                    ret = 0;
                    audio_way_list_add(audio_hdl);
                } else {
                    log_e("audio[%d] open err \n", i);
                }
            }
        }
    }
    AUDIO_WAY_POST();

    return ret;
}

/*
*********************************************************************
*                  Audio App Close
* Description: 关闭audio输出方式
* Arguments  : way_type		audio输出方式
* Return	 : 0			成功
* Note(s)    : None.
*********************************************************************
*/
int audio_way_close(u32 way_type)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p, *n;

    // close之前先stop
    audio_way_stop(way_type);

    AUDIO_WAY_PEND();
    list_for_each_entry_safe(p, n, &__this->head, entry) {
        for (int i = 0; i < AUDIO_WAY_TYPE_MAX; i++) {
            if ((way_type & BIT(i)) && (p->way_type == BIT(i))) {
                list_del(&p->entry);
                sound_pcm_free(p->stream);
                free(p);
                ret = 0;
                break;
            }
        }
    }
    AUDIO_WAY_POST();

    return ret;
}



/*
*********************************************************************
*                  Audio App Start
* Description: 启动audio输出方式
* Arguments  : way_type		audio输出方式
* Return	 : 0			成功
* Note(s)    : None.
*********************************************************************
*/
int audio_way_start(u32 way_type)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

    AUDIO_WAY_PEND();
    LIST_FOR_EACH_ENTRY_HEAD() {
        if (p->state != AUDIO_WAY_STATE_START) {
            sound_pcm_start(p->stream);
            p->state = AUDIO_WAY_STATE_START;
        }
        ret = 0;
        break;
    }
    LIST_FOR_EACH_ENTRY_END();
    AUDIO_WAY_POST();

    return ret;
}


/*
*********************************************************************
*                  Audio App Stop
* Description: 停止audio输出方式
* Arguments  : way_type		audio输出方式
* Return	 : 0			成功
* Note(s)    : None.
*********************************************************************
*/
int audio_way_stop(u32 way_type)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

    AUDIO_WAY_PEND();
    LIST_FOR_EACH_ENTRY_HEAD() {
        if (p->state != AUDIO_WAY_STATE_IDLE) {
            p->state = AUDIO_WAY_STATE_IDLE;
            sound_pcm_stop(p->stream);
#if (AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_SRC)
            if (p->src) {
                audio_hw_src_stop(p->src);
                audio_hw_src_close(p->src);
                free(p->src);
                p->src = NULL;
            }
#endif /*(AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_SRC)*/
#if (AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_CH_SWITCH)
            if (p->ch_switch) {
                free(p->ch_switch);
                p->ch_switch = NULL;
            }
#endif /*(AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_CH_SWITCH)*/
            p->in_sr = 0;
        }
        ret = 0;
        break;
    }
    LIST_FOR_EACH_ENTRY_END();
    AUDIO_WAY_POST();

    return ret;
}

#if (AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_CH_SWITCH)

static inline void audio_pcm_single_to_dual(s16 *out_pcm, s16 *in_pcm, int points)
{
    while (points--) {
        *out_pcm++ = *in_pcm;
        *out_pcm++ = *in_pcm;
        in_pcm++;
    }
}
static inline void audio_pcm_dual_to_single(s16 *out_pcm, s16 *in_pcm, int points)
{
    while (points--) {
#if 1
        *out_pcm++ = data_sat_s16(in_pcm[0] + in_pcm[1]);
#else
        s32 tmp32 = (in_pcm[0] + in_pcm[1]);
        if (tmp32 < -32768) {
            tmp32 = -32768;
        } else if (tmp32 > 32767) {
            tmp32 = 32767;
        }
        *out_pcm++ = tmp32;
#endif
        in_pcm += 2;
    }
}

/*
*********************************************************************
*                  Audio App Output By Channel Switch
* Description: 声道转换输出
* Arguments  : p			audio输出方式
* 			   *buf			输出数据
*              len			输出长度
* Return	 : 实际输出长度
* Note(s)    : None.
*********************************************************************
*/
static int audio_ch_switch_output_write(struct audio_way *p, s16 *buf, int len)
{
    struct audio_way_ch_switch *ch_switch = p->ch_switch;
    u32 wlen;
    s16 *obuf;

    // 输出剩余的
    if (ch_switch->remain_len) {
        obuf = &ch_switch->buf[ch_switch->total_len - ch_switch->remain_len];
        wlen = ch_switch->remain_len * 2;
#if (AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_SRC)
        if (p->src) { // 变采样
            wlen = audio_src_resample_write(p->src, obuf, wlen);
        } else
#endif /*(AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_SRC)*/
        {
            /* printf("or:%d \n", wlen); */
            /* put_buf(obuf, wlen); */
            wlen = sound_pcm_write(p->stream, obuf, wlen);
            /* printf("ord:%d \n", wlen); */
        }
        wlen >>= 1;
        if (ch_switch->remain_len >= wlen) {
            ch_switch->remain_len -= wlen;
        }
        if (ch_switch->remain_len) { // 还没输出完
            return 0;
        }
    }

    len >>= 1;
    u32 total = len;
    while (len) {
        u32 olen = len;

        // 声道转换
        if ((p->in_ch == 2) && (p->out_ch == 1)) { // 双变单
            if (olen > (AUDIO_WAY_CH_SWITCH_BUF_POINTS * 2)) {
                olen = (AUDIO_WAY_CH_SWITCH_BUF_POINTS * 2);
            }
            audio_pcm_dual_to_single(ch_switch->buf, buf, olen / 2);
            ch_switch->remain_len = ch_switch->total_len = olen / 2;
        } else if ((p->in_ch == 1) && (p->out_ch == 2)) { // 单变双
            if (olen > (AUDIO_WAY_CH_SWITCH_BUF_POINTS / 2)) {
                olen = (AUDIO_WAY_CH_SWITCH_BUF_POINTS / 2);
            }
            audio_pcm_single_to_dual(ch_switch->buf, buf, olen);
            ch_switch->remain_len = ch_switch->total_len = olen * 2;
        } else {
            log_e("ch switch not support, %d,%d \n", p->in_ch, p->out_ch);
            while (1);
        }
        len -= (olen);
        buf += (olen);

        // 输出
        obuf = ch_switch->buf;
        wlen = ch_switch->remain_len * 2;
#if (AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_SRC)
        if (p->src) { // 变采样
            wlen = audio_src_resample_write(p->src, obuf, wlen);
        } else
#endif /*(AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_SRC)*/
        {
            /* printf("ow:%d \n", wlen); */
            /* put_buf(obuf, wlen); */
            wlen = sound_pcm_write(p->stream, obuf, wlen);
            /* printf("owd:%d \n", wlen); */
        }
        wlen >>= 1;
        if (ch_switch->remain_len >= wlen) {
            ch_switch->remain_len -= wlen;
        }
        if (ch_switch->remain_len) { // 还没输出完
            break;
        }
    }

    return (total - len) * 2;
}

#endif /*(AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_CH_SWITCH)*/

/*
*********************************************************************
*                  Audio App Output Data
* Description: 数据输出
* Arguments  : *buf			输出数据
*              len			输出长度
* Return	 : 实际输出长度
* Note(s)    : None.
*********************************************************************
*/
int audio_way_output_write(s16 *buf, int len)
{
    struct audio_way *p;
    int wlen;
    int olen = -1;

    AUDIO_WAY_PEND();
    list_for_each_entry(p, &__this->head, entry) {
        if (p->state == AUDIO_WAY_STATE_START) {
#if TCFG_AUDIO_OUTPUT_IIS
            extern void audio_aec_ref_start(u8 en);
            extern u8 bt_phone_dec_is_running();
            if (bt_phone_dec_is_running()) {
                audio_aec_ref_start(1);
            }
#endif /*TCFG_AUDIO_OUTPUT_IIS*/
#if (AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_CH_SWITCH)
            if (p->ch_switch) { // 声道转换
                /* printf("w:%d \n", len); */
                /* put_buf(buf, len); */
                wlen = audio_ch_switch_output_write(p, buf, len);
                /* printf("wend:%d \n", wlen); */
            } else
#endif /*(AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_CH_SWITCH)*/
#if (AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_SRC)
                if (p->src) { // 变采样
                    wlen = audio_src_resample_write(p->src, buf, len);
                } else
#endif /*(AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_SRC)*/
                {
                    wlen = sound_pcm_write(p->stream, buf, len);
                }
            if (olen == -1) { // 主输出方式
                if (wlen < len) { // 输出不了解码会被挂起，设置激活时间
                    /* printf("set interrupt \n"); */
                    /* putchar('$'); */
                    sound_pcm_trigger_interrupt(p->stream, SOUND_TIME_US, 2500);
                }
                if (wlen <= 0) {
                    break;
                }
                olen = len = wlen;
            } else { // 普通输出方式
                if (wlen != len) {
                    log_w("write err:%d,%d \n", len, wlen);
                }
            }
        }
    }
    AUDIO_WAY_POST();

    if (olen < 0) {
        return 0;
    }

    return olen;
}

/*
*********************************************************************
*                  Audio App Set Channel Number
* Description: 设置声道数
* Arguments  : way_type		audio输出方式
*              channel_num	声道数
* Return	 : >0			设置成功，返回设置的声道数
* 			   <0			出错（比如找不到audio输出方式等）
* Note(s)    : None.
*********************************************************************
*/
int audio_way_set_channel_num(u32 way_type, u8 channel_num)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

#if (AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_CH_SWITCH)
    AUDIO_WAY_PEND();
    LIST_FOR_EACH_ENTRY_HEAD() {
        if (p->in_ch != channel_num) {
            p->in_ch = channel_num;
            log_i("audio[%d], set ch:%d,%d \n", i, p->in_ch, p->out_ch);
            if (p->in_ch != p->out_ch) { // 输入输出不同，声道转换
                if (p->ch_switch == NULL) {
                    p->ch_switch = malloc(sizeof(struct audio_way_ch_switch));
                    ASSERT(p->ch_switch);
                }
                memset(p->ch_switch, 0, sizeof(struct audio_way_ch_switch));
            } else { // 输入输出相同
                if (p->ch_switch) {
                    free(p->ch_switch);
                    p->ch_switch = NULL;
                }
            }
            ret = channel_num;
        }
    }
    LIST_FOR_EACH_ENTRY_END();
    AUDIO_WAY_POST();
#endif /*(AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_CH_SWITCH)*/

    return ret;
}


/*
*********************************************************************
*                  Audio App Get Channel Number
* Description: 获取audio声道数
* Arguments  : way_type		audio输出方式
* Return	 : >0			声道数
* 			   <0			出错（比如找不到audio输出方式等）
* Note(s)    : audio声道数由硬件环境配置决定
*              如果有多个输出方式，返回第一个输出方式的声道数
*********************************************************************
*/
int audio_way_get_channel_num(u32 way_type)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

    /* AUDIO_WAY_PEND(); */
    LIST_FOR_EACH_ENTRY_HEAD() {
        struct sound_pcm_hw_params params = {0};
        if (p->stream->ops && p->stream->ops->ioctl) {
            p->stream->ops->ioctl(p->stream->substream, SOUND_PCM_GET_HW_PARAMS, &params);
        } else {
            log_e("check channel audio[i] error \n", i);
        }
        return params.channels;
    }
    LIST_FOR_EACH_ENTRY_END();
    /* AUDIO_WAY_POST(); */

    return ret;
}


/*
*********************************************************************
*                  Audio App Check Sample Rate
* Description: 检测采样率
* Arguments  : way_type		audio输出方式
*              sample_rate	需要检测的采样率
*              high			0-低一级采样率，1-高一级采样率
* Return	 : >0			匹配到的采样率
* 			   <0			出错（比如找不到audio输出方式等）
* Note(s)    : 示例：输出方式支持采样率有 8000,16000,32000
*              sample_rate=12000,high=0，返回8000
*              sample_rate=12000,high=1，返回16000
*              sample_rate=16000,high=任意值，返回16000
*              * 如果有多个输出方式，返回第一个输出方式的采样率
*********************************************************************
*/
int audio_way_check_sample_rate(u32 way_type, u32 sample_rate, u8 high)
{
    int rate_num = ARRAY_SIZE(pcm_rates);

    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

    /* AUDIO_WAY_PEND(); */
    LIST_FOR_EACH_ENTRY_HEAD() {
        struct sound_pcm_hw_params params;
        memset(&params, 0xff, sizeof(struct sound_pcm_hw_params));
        if (p->stream->ops && p->stream->ops->ioctl) {
            p->stream->ops->ioctl(p->stream->substream, SOUND_PCM_GET_HW_PARAMS, &params);
        } else {
            log_e("check sr audio[i] error \n", i);
        }

        int normal_sr = 44100;
        for (int j = 0; j < rate_num; j++) {
            if (!(BIT(j) & params.rates)) {
                continue; // sound不支持该采样率
            }
            if (sample_rate == pcm_rates[j]) {
                return sample_rate;
            }

            if (sample_rate < pcm_rates[j]) {
                if (high) {
                    return pcm_rates[j];
                } else {
                    return pcm_rates[j > 0 ? (j - 1) : 0];
                }
            }
            normal_sr = pcm_rates[j];
        }
        return normal_sr;
    }
    LIST_FOR_EACH_ENTRY_END();
    /* AUDIO_WAY_POST(); */

    return ret;
}

static int audio_src_output_handler(void *hdl, void *buf, int len)
{
    struct audio_way *p = hdl;
    return sound_pcm_write(p->stream, buf, len);
}

/*
*********************************************************************
*                  Audio App Set Sample Rate
* Description: 设置采样率
* Arguments  : way_type		audio输出方式
*              sample_rate	采样率
* Return	 : >0			设置成功，返回设置的采样率
* 			   <0			出错（比如找不到audio输出方式等）
* Note(s)    : None.
*********************************************************************
*/
int audio_way_set_sample_rate(u32 way_type, u32 sample_rate)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;
    int out_sr = 0;

    AUDIO_WAY_PEND();
    LIST_FOR_EACH_ENTRY_HEAD() {
        if (p->in_sr != sample_rate) {
            p->in_sr = sample_rate;
            out_sr = audio_way_get_sample_rate(BIT(i)); // 获取当前正在使用的采样;
            if (out_sr <= 0) { // 还没有设置采样率 // 如果有设置采样率，中途不让改变
                out_sr = audio_way_check_sample_rate(BIT(i), sample_rate, 0); // 检查设置的采样是否支持
                if (out_sr != sample_rate) {
                    log_w("audio[%d], check sr out:%d, %d \n", i, sample_rate, out_sr);
                    if (out_sr <= 0) {
                        out_sr = sample_rate;
                    }
                }

            }
#if (AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_SRC)
            p->out_sr = out_sr;
            log_i("audio[%d], set sr:%d,%d \n", i, p->in_sr, p->out_sr);
            if (p->in_sr != p->out_sr) { // 输入输出不同，变采样
                if (p->src == NULL) {
                    p->src = zalloc(sizeof(struct audio_src_handle));
                    ASSERT(p->src);
                    audio_hw_src_open(p->src, p->out_ch, SRC_TYPE_RESAMPLE);
                    audio_src_set_output_handler(p->src, p, audio_src_output_handler);
                }
                audio_hw_src_set_rate(p->src, p->in_sr, p->out_sr);
            } else { // 输入输出相同
                if (p->src) {
                    audio_hw_src_stop(p->src);
                    audio_hw_src_close(p->src);
                    free(p->src);
                    p->src = NULL;
                }
            }
#endif /*(AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_SRC)*/
        } else {
            out_sr = sample_rate;
        }

        int err = sound_pcm_prepare(p->stream, out_sr, 50, WRITE_MODE_BLOCK);

        if (err) {
            log_w("audio pcm pre err:%d \n", err);
        } else {
            ret = out_sr;
        }

    }
    LIST_FOR_EACH_ENTRY_END();
    AUDIO_WAY_POST();

    return ret;
}


/*
*********************************************************************
*                  Audio App Get Sample Rate
* Description: 获取采样率
* Arguments  : way_type		audio输出方式
* Return	 : >0			获取到的采样率
* 			   <0			出错（比如找不到audio输出方式等）
* Note(s)    : 如果有多个输出方式，返回第一个输出方式的采样率
*********************************************************************
*/
int audio_way_get_sample_rate(u32 way_type)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

    /* AUDIO_WAY_PEND(); */
    LIST_FOR_EACH_ENTRY_HEAD() {
        struct sound_pcm_hw_params params;
        memset(&params, 0xff, sizeof(struct sound_pcm_hw_params));
        if (p->stream->ops && p->stream->ops->ioctl) {
            p->stream->ops->ioctl(p->stream->substream, SOUND_PCM_GET_HW_PARAMS, &params);
        } else {
            log_e("get sr audio[i] error \n", i);
        }
        return params.sample_rate;
    }
    LIST_FOR_EACH_ENTRY_END();
    /* AUDIO_WAY_POST(); */

    return ret;
}


/*
*********************************************************************
*                  Audio App Get Hw Hrp
* Description: 获取硬件的读指针
* Arguments  : way_type		audio输出方式
* Return	 : >=0			获取到的硬件读指针
* 			   <0			出错（比如找不到audio输出方式等）
* Note(s)    : 如果有多个输出方式，返回第一个输出方式的硬件读指针
*********************************************************************
*/
int audio_way_get_hw_hrp(u32 way_type)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

    /* AUDIO_WAY_PEND(); */
    LIST_FOR_EACH_ENTRY_HEAD() {
        if (p->stream->ops && p->stream->ops->ioctl) {
            return p->stream->ops->ioctl(p->stream->substream, SOUND_PCM_GET_HW_HRP, NULL);
        } else {
            log_e("get hrp audio[i] error \n", i);
        }
    }
    LIST_FOR_EACH_ENTRY_END();
    /* AUDIO_WAY_POST(); */
    return ret;
}


/*
*********************************************************************
*                  Audio App Set Gain
* Description: 设置增益
* Arguments  : way_type		audio输出方式
*              gain			增益
* Return	 : >0			设置成功，返回设置的增益
* 			   <0			出错（比如找不到audio输出方式等）
* Note(s)    : None.
*********************************************************************
*/
int audio_way_set_gain(u32 way_type, int gain)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

    /* AUDIO_WAY_PEND(); */
    LIST_FOR_EACH_ENTRY_HEAD() {
        struct sound_volume volume = {
            .chmap = SOUND_CHMAP_FL | SOUND_CHMAP_FR,
        };
#if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC)
        if (BIT(i) == AUDIO_WAY_TYPE_DAC) {
            // 模拟音量
            volume.volume[0] = gain;
            volume.volume[1] = gain;
            sound_pcm_ctl_ioctl(p->stream, SNDCTL_IOCTL_SET_ANA_GAIN, &volume);
            // 数字音量
            volume.volume[0] = 16384;
            volume.volume[1] = 16384;
            sound_pcm_ctl_ioctl(p->stream, SNDCTL_IOCTL_SET_DIG_GAIN, &volume);
            /* printf("JL_AUDIO->DAC_VL0 : 0x%x, JL_ANA->DAA_CON1 : 0x%x\n", JL_AUDIO->DAC_VL0, JL_ANA->DAA_CON1); */
        } else
#endif /* #if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC) */
        {
            // 数字音量
            volume.volume[0] = digital_vol_level_to_gain(gain);
            volume.volume[1] = digital_vol_level_to_gain(gain);
            sound_pcm_ctl_ioctl(p->stream, SNDCTL_IOCTL_SET_DIG_GAIN, &volume);
        }
        ret = gain;
    }
    LIST_FOR_EACH_ENTRY_END();
    /* AUDIO_WAY_POST(); */

    return ret;
}

int audio_way_set_analog_gain(u32 way_type, u32 chmap, int gain)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

    /* AUDIO_WAY_PEND(); */
    LIST_FOR_EACH_ENTRY_HEAD() {
        struct sound_volume volume = {
            /* .chmap = SOUND_CHMAP_FL | SOUND_CHMAP_FR, */
            .chmap = chmap,
        };
        if (BIT(i) == AUDIO_WAY_TYPE_DAC) {
            // 模拟音量
            volume.volume[0] = gain;
            volume.volume[1] = gain;
            sound_pcm_ctl_ioctl(p->stream, SNDCTL_IOCTL_SET_ANA_GAIN, &volume);
        }
        ret = gain;
    }
    LIST_FOR_EACH_ENTRY_END();
    /* AUDIO_WAY_POST(); */

    return ret;
}


int audio_way_set_digital_gain(u32 way_type, u32 chmap, int gain)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

    /* AUDIO_WAY_PEND(); */
    LIST_FOR_EACH_ENTRY_HEAD() {
        struct sound_volume volume = {
            /* .chmap = SOUND_CHMAP_FL | SOUND_CHMAP_FR, */
            .chmap = chmap,
        };
        /* if (BIT(i) == AUDIO_WAY_TYPE_DAC) { */
        // 数字音量
        volume.volume[0] = gain;
        volume.volume[1] = gain;
        sound_pcm_ctl_ioctl(p->stream, SNDCTL_IOCTL_SET_DIG_GAIN, &volume);
        /* } */
        ret = gain;
    }
    LIST_FOR_EACH_ENTRY_END();
    /* AUDIO_WAY_POST(); */

    return ret;
}




/*
*********************************************************************
*                  Audio App Get Gain
* Description: 获取增益
* Arguments  : way_type		audio输出方式
* Return	 : >0			获取到的增益
* 			   <0			出错（比如找不到audio输出方式等）
* Note(s)    : 如果有多个输出方式，返回第一个输出方式的增益
*********************************************************************
*/
int audio_way_get_gain(u32 way_type)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

    /* AUDIO_WAY_PEND(); */
    LIST_FOR_EACH_ENTRY_HEAD() {
        struct sound_volume volume = {
            .chmap = SOUND_CHMAP_FL | SOUND_CHMAP_FR,
        };
#if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC)
        if (BIT(i) == AUDIO_WAY_TYPE_DAC) {
            // 模拟音量
            sound_pcm_ctl_ioctl(p->stream, SNDCTL_IOCTL_GET_ANA_GAIN, &volume);
        } else
#endif /* #if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC) */
        {
            // 数字音量
            sound_pcm_ctl_ioctl(p->stream, SNDCTL_IOCTL_GET_DIG_GAIN, &volume);
            volume.volume[0] = digital_vol_gain_to_level(volume.volume[0]);
        }
        return volume.volume[0];
    }
    LIST_FOR_EACH_ENTRY_END();
    /* AUDIO_WAY_POST(); */

    return ret;
}


/*
*********************************************************************
*                  Audio App Ioctrl
* Description: IO控制
* Arguments  : way_type		audio输出方式
*              cmd			命令
*              *parm		参数
* Return	 : <0			出错（比如找不到audio输出方式等）
* Note(s)    : None.
*********************************************************************
*/
int audio_way_ioctrl(u32 way_type, u32 cmd, void *parm)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

    /* AUDIO_WAY_PEND(); */
    LIST_FOR_EACH_ENTRY_HEAD() {
        ret = sound_pcm_ctl_ioctl(p->stream, cmd, parm);
    }
    LIST_FOR_EACH_ENTRY_END();
    /* AUDIO_WAY_POST(); */

    return ret;
}


int sound_pcm_device_buffered_len(void *priv)
{
    u32 snd_type = (u32)priv;
    struct audio_way *p;
    LIST_FOR_EACH_SOUND_DEV(snd_type) {
        return sound_pcm_iotcl(p->stream, SOUND_PCM_GET_HW_BUFFERED_LEN, NULL);
    }
    LIST_FOR_EACH_SOUND_END();
    return 0;
}


int sound_pcm_device_mount_syncts(u32 snd_type, void *syncts)
{
    int ret = AUDIO_WAY_ERR_NO_WAY;
    struct audio_way *p;

    AUDIO_WAY_PEND();
    LIST_FOR_EACH_SOUND_DEV(snd_type) {
        sound_pcm_set_syncts(p->stream, syncts);
        ret = 0;
        break;
    }
    LIST_FOR_EACH_SOUND_END();
    AUDIO_WAY_POST();
    return ret;
}

void sound_pcm_device_unmount_syncts(u32 snd_type, void *syncts)
{
    struct audio_way *p;

    AUDIO_WAY_PEND();
    LIST_FOR_EACH_SOUND_DEV(snd_type) {
        sound_pcm_unmount_syncts(p->stream, syncts);
        break;
    }
    LIST_FOR_EACH_SOUND_END();
    AUDIO_WAY_POST();
}

/*************************************************************/
// test

static const unsigned char sin44K[88 * 4] ALIGNED(4) = {
    0x00, 0x00, 0x45, 0x0E, 0x41, 0x1C, 0xAA, 0x29, 0x3B, 0x36, 0xB2, 0x41, 0xD5, 0x4B, 0x6E, 0x54,
    0x51, 0x5B, 0x5A, 0x60, 0x70, 0x63, 0x82, 0x64, 0x8A, 0x63, 0x8E, 0x60, 0x9D, 0x5B, 0xD1, 0x54,
    0x4D, 0x4C, 0x3D, 0x42, 0xD5, 0x36, 0x50, 0x2A, 0xF1, 0x1C, 0xFB, 0x0E, 0xB7, 0x00, 0x70, 0xF2,
    0x6E, 0xE4, 0xFD, 0xD6, 0x60, 0xCA, 0xD9, 0xBE, 0xA5, 0xB4, 0xF7, 0xAB, 0xFC, 0xA4, 0xDA, 0x9F,
    0xAB, 0x9C, 0x7F, 0x9B, 0x5E, 0x9C, 0x3F, 0x9F, 0x19, 0xA4, 0xCE, 0xAA, 0x3D, 0xB3, 0x3A, 0xBD,
    0x92, 0xC8, 0x0A, 0xD5, 0x60, 0xE2, 0x50, 0xF0,
    0x00, 0x00, 0x45, 0x0E, 0x41, 0x1C, 0xAA, 0x29, 0x3B, 0x36, 0xB2, 0x41, 0xD5, 0x4B, 0x6E, 0x54,
    0x51, 0x5B, 0x5A, 0x60, 0x70, 0x63, 0x82, 0x64, 0x8A, 0x63, 0x8E, 0x60, 0x9D, 0x5B, 0xD1, 0x54,
    0x4D, 0x4C, 0x3D, 0x42, 0xD5, 0x36, 0x50, 0x2A, 0xF1, 0x1C, 0xFB, 0x0E, 0xB7, 0x00, 0x70, 0xF2,
    0x6E, 0xE4, 0xFD, 0xD6, 0x60, 0xCA, 0xD9, 0xBE, 0xA5, 0xB4, 0xF7, 0xAB, 0xFC, 0xA4, 0xDA, 0x9F,
    0xAB, 0x9C, 0x7F, 0x9B, 0x5E, 0x9C, 0x3F, 0x9F, 0x19, 0xA4, 0xCE, 0xAA, 0x3D, 0xB3, 0x3A, 0xBD,
    0x92, 0xC8, 0x0A, 0xD5, 0x60, 0xE2, 0x50, 0xF0,
    0x00, 0x00, 0x45, 0x0E, 0x41, 0x1C, 0xAA, 0x29, 0x3B, 0x36, 0xB2, 0x41, 0xD5, 0x4B, 0x6E, 0x54,
    0x51, 0x5B, 0x5A, 0x60, 0x70, 0x63, 0x82, 0x64, 0x8A, 0x63, 0x8E, 0x60, 0x9D, 0x5B, 0xD1, 0x54,
    0x4D, 0x4C, 0x3D, 0x42, 0xD5, 0x36, 0x50, 0x2A, 0xF1, 0x1C, 0xFB, 0x0E, 0xB7, 0x00, 0x70, 0xF2,
    0x6E, 0xE4, 0xFD, 0xD6, 0x60, 0xCA, 0xD9, 0xBE, 0xA5, 0xB4, 0xF7, 0xAB, 0xFC, 0xA4, 0xDA, 0x9F,
    0xAB, 0x9C, 0x7F, 0x9B, 0x5E, 0x9C, 0x3F, 0x9F, 0x19, 0xA4, 0xCE, 0xAA, 0x3D, 0xB3, 0x3A, 0xBD,
    0x92, 0xC8, 0x0A, 0xD5, 0x60, 0xE2, 0x50, 0xF0,
    0x00, 0x00, 0x45, 0x0E, 0x41, 0x1C, 0xAA, 0x29, 0x3B, 0x36, 0xB2, 0x41, 0xD5, 0x4B, 0x6E, 0x54,
    0x51, 0x5B, 0x5A, 0x60, 0x70, 0x63, 0x82, 0x64, 0x8A, 0x63, 0x8E, 0x60, 0x9D, 0x5B, 0xD1, 0x54,
    0x4D, 0x4C, 0x3D, 0x42, 0xD5, 0x36, 0x50, 0x2A, 0xF1, 0x1C, 0xFB, 0x0E, 0xB7, 0x00, 0x70, 0xF2,
    0x6E, 0xE4, 0xFD, 0xD6, 0x60, 0xCA, 0xD9, 0xBE, 0xA5, 0xB4, 0xF7, 0xAB, 0xFC, 0xA4, 0xDA, 0x9F,
    0xAB, 0x9C, 0x7F, 0x9B, 0x5E, 0x9C, 0x3F, 0x9F, 0x19, 0xA4, 0xCE, 0xAA, 0x3D, 0xB3, 0x3A, 0xBD,
    0x92, 0xC8, 0x0A, 0xD5, 0x60, 0xE2, 0x50, 0xF0,
};


extern void wdt_clear(void);
static void audio_way_write_test(void)
{
    u16 cnt = 1000;
    int total_len = 100 * 44100 * 2 * 2;
    u32 use_len = 0;
    int ret;
    u8 flag = 0;

    /* audio_way_set_sample_rate(BIT(0), 16000); */
    /* audio_way_set_channel_num(BIT(0), 1); */

    while (total_len > 0) {
        wdt_clear();

        s16 *obuf = (s16 *)sin44K;
        int olen = sizeof(sin44K);
        if (use_len) {
            obuf = (s16 *)&sin44K[use_len];
            olen = sizeof(sin44K) - use_len;
        }
        int wlen = audio_way_output_write(obuf, olen);
        if (wlen != olen) {
            os_time_dly(1);
            if (flag == 0) {
                if (cnt > 2) {
                    cnt /= 2;
                }
            }
            flag = 0;
        } else {
            flag = 1;
        }
        total_len -= wlen;
        if (wlen > 0) {
            use_len += wlen;
            if (use_len >= sizeof(sin44K)) {
                use_len = 0;
            }
        }

        // 随机测试
        if (--cnt) {
            continue;
        }
        cnt = rand32() % 300 + 1000;
        printf("\n\n %s,%d \n", __func__, __LINE__);
        /* u8 idx = rand32() % AUDIO_WAY_TYPE_MAX; */
        u8 idx = rand32() % 2;
        /* printf("idx:%d \n", idx); */
        u8 test_id = rand32() % 11;
        /* printf("test_id :%d \n", test_id); */
        if (test_id < 4) {
            y_printf("audio open:%d \n", idx);
            ret = audio_way_open(BIT(idx));
            if (ret == 0) {
                audio_way_ioctrl(BIT(idx), SNDCTL_IOCTL_POWER_ON, NULL);
                audio_way_set_sample_rate(BIT(idx), 44100);
                audio_way_set_gain(BIT(idx), 10);
                audio_way_start(BIT(idx));
            }
        } else if (test_id < 5) {
            y_printf("audio close:%d \n", idx);
            audio_way_stop(BIT(idx));
            audio_way_ioctrl(BIT(idx), SNDCTL_IOCTL_POWER_OFF, NULL);
            audio_way_close(BIT(idx));
        } else if (test_id < 7) {
            int sr = pcm_rates[rand32() % 6 + 1];
            y_printf("audio setsr:%d, %d \n", idx, sr);
            audio_way_set_sample_rate(BIT(idx), sr);
        } else if (test_id < 9) {
            int ch = rand32() % 2 + 1;
            y_printf("audio setch:%d, %d \n", idx, ch);
            audio_way_set_channel_num(BIT(idx), ch);
        } else {
            int gain = rand32() % 30;
            y_printf("audio setgain:%d, %d \n", idx, gain);
            audio_way_set_gain(BIT(idx), gain);
        }

        printf("%s,%d \n\n\n", __func__, __LINE__);
    }
}

void audio_way_test(void)
{
    printf("%s,%d \n", __func__, __LINE__);
    audio_way_init();
    audio_way_open(AUDIO_WAY_TYPE_ALL);
    audio_way_ioctrl(AUDIO_WAY_TYPE_ALL, SNDCTL_IOCTL_POWER_ON, NULL);
    audio_way_set_sample_rate(AUDIO_WAY_TYPE_ALL, 44100);
    audio_way_set_gain(AUDIO_WAY_TYPE_ALL, 30);
    audio_way_start(AUDIO_WAY_TYPE_ALL);
    printf("%s,%d \n", __func__, __LINE__);

    audio_way_write_test();

    printf("%s,%d \n", __func__, __LINE__);
    audio_way_stop(AUDIO_WAY_TYPE_ALL);
    audio_way_ioctrl(AUDIO_WAY_TYPE_ALL, SNDCTL_IOCTL_POWER_OFF, NULL);
    audio_way_close(AUDIO_WAY_TYPE_ALL);
    printf("%s,%d \n", __func__, __LINE__);
}

struct sound_pcm_substream *audio_way_get_substream_hdl(void)
{
    struct audio_way *p;
    list_for_each_entry(p, &__this->head, entry) {
        break;
    }
    if (p == NULL) {
        return NULL;
    } else {
        return p->stream->substream;
    }
}

