/*****************************************************************
>file name : audio_bt_emitter.h
>decription : 蓝牙发射模块
*****************************************************************/
#ifndef _AUDIO_BT_EMITTER_H_
#define _AUDIO_BT_EMITTER_H_
#include "sound/sound.h"


#define AUDIO_BT_EMITTER_STREAM_RUNNING(ctx)       (ctx->status.ref_count > 0)

#define BT_EMITTER_CH_MAX                          2

struct audio_bt_emitter_dma {
    u32 sw_ptr;
    u32 hw_ptr;
};

/*
 * BT_EMITTER结构体
 */
struct audio_bt_emitter_context {
    struct sound_pcm_hw_params 				hw_params;		/* BT_EMITTER 硬件参数 */
    struct sound_pcm_map_status 			status;  		/* BT_EMITTER 状态映射 */
    struct sound_pcm_map_fifo   			fifo;    		/* BT_EMITTER fifo映射 */
    struct audio_bt_emitter_dma  			dma; 			/* BT_EMITTER DMA */
    struct list_head 						stream_list;    /* BT_EMITTER 关联sound pcm数据流链表 */
};


/*
 * Audio bt_emitter dma
 */
/*************************************************************************
 * BT_EMITTER DMA注册接口(由BT_EMITTER驱动调用)
 *
 * Input    :  bt_emitter  - audio_bt_emitter_context指针
 *             data - 设备平台配置数据
 * Output   :  0 - 成功，非0 - 失败.
 * Notes    :  非外部使用接口，仅BT_EMITTER内部使用.
 *=======================================================================*/
int audio_bt_emitter_dma_register(struct audio_bt_emitter_context *bt_emitter, struct sound_pcm_platform_data *data);

/*************************************************************************
 * BT_EMITTER DMA注销接口(由BT_EMITTER驱动调用)
 *
 * Input    :  bt_emitter  - audio_bt_emitter_context指针
 * Output   :  无.
 * Notes    :  非外部使用接口，仅BT_EMITTER内部使用.
 *=======================================================================*/
void audio_bt_emitter_dma_unregister(struct audio_bt_emitter_context *bt_emitter);

/*
 * Audio bt_emitter controller
 */
/*************************************************************************
 * BT_EMITTER 控制器(codec/power/analog/digital)注册接口
 *
 * Input    :  bt_emitter  - audio_bt_emitter_context指针
 *             data - 设备平台配置数据
 * Output   :  0 - 成功，非0 - 失败.
 * Notes    :  非外部使用接口，仅BT_EMITTER内部使用.
 *=======================================================================*/
int audio_bt_emitter_controller_register(struct audio_bt_emitter_context *bt_emitter, struct sound_pcm_platform_data *data);

/*************************************************************************
 * BT_EMITTER 控制器(codec/power/analog/digital)注销接口
 *
 * Input    :  bt_emitter  - audio_bt_emitter_context指针
 * Output   :  无.
 * Notes    :  非外部使用接口，仅BT_EMITTER内部使用.
 *=======================================================================*/
void audio_bt_emitter_controller_unregister(struct audio_bt_emitter_context *bt_emitter);


/*
 * Audio bt_emitter hw
 */
/*************************************************************************
 * BT_EMITTER 模拟硬件初始化
 *
 * Input    :  bt_emitter  - audio_bt_emitter_context指针
 * Output   :  无
 * Notes    :  硬件初始化，仅BT_EMITTER内部使用.
 *=======================================================================*/
void audio_bt_emitter_hw_init(struct audio_bt_emitter_context *bt_emitter);

/*************************************************************************
 * BT_EMITTER 模拟硬件关闭dma使能
 *
 * Input    :  无
 * Output   :  无
 * Notes    :  关闭dma使能，仅BT_EMITTER内部使用.
 *=======================================================================*/
void audio_bt_emitter_hw_dma_disable(void);

/*************************************************************************
 * BT_EMITTER 模拟硬件dma使能
 *
 * Input    :  无
 * Output   :  无
 * Notes    :  dma使能，仅BT_EMITTER内部使用.
 *=======================================================================*/
void audio_bt_emitter_hw_dma_enable(void);

/*************************************************************************
 * BT_EMITTER 模拟硬件更新fifo
 *
 * Input    :  len - 更新长度
 * Output   :  无
 * Notes    :  更新fifo
 *=======================================================================*/
void audio_bt_emitter_hw_fifo_write_update(u32 len);

/*************************************************************************
 * BT_EMITTER 模拟硬件获取fifo数据长度
 *
 * Input    :  无
 * Output   :  fifo数据长度
 * Notes    :  获取fifo数据长度
 *=======================================================================*/
int audio_bt_emitter_hw_fifo_get_buff_len(void);

/*************************************************************************
 * BT_EMITTER 模拟硬件设置音量
 *
 * Input    :  ch - 声道
 *             vol - 音量
 * Output   :  无
 * Notes    :  设置音量
 *=======================================================================*/
void audio_bt_emitter_hw_set_digital_gain(u8 ch, s16 vol);

/*************************************************************************
 * BT_EMITTER 模拟硬件获取音量
 *
 * Input    :  无
 * Output   :  音量值
 * Notes    :  获取音量，仅BT_EMITTER内部使用.
 *=======================================================================*/
s16 audio_bt_emitter_hw_get_digital_gain(u8 ch);


#endif /*_AUDIO_BT_EMITTER_H_*/

