
#ifndef _AUDIO_WAY_H_
#define _AUDIO_WAY_H_

#include "system/includes.h"
#include "sound/sound.h"

// 支持的功能
#define AUDIO_WAY_SUPPORT_SRC		BIT(0)
#define AUDIO_WAY_SUPPORT_CH_SWITCH	BIT(1)
#define AUDIO_WAY_SUPPORT			(AUDIO_WAY_SUPPORT_SRC | AUDIO_WAY_SUPPORT_CH_SWITCH)


#define AUDIO_WAY_ERR_NO_WAY			(-ENODEV) // 查找出错
#define AUDIO_WAY_ERR_ALREADY_OPEN		(-100) // 已经打开

/*
 * 状态
 */
enum audio_way_state {
    AUDIO_WAY_STATE_IDLE = 0,	// 空闲
    AUDIO_WAY_STATE_START,		// 已经启动
};

/*
 * 主结构体
 */
struct audio_way {
    u32 way_type;	// 当前audio输出方式
    struct list_head entry;	// 链表
    struct sound_pcm_stream *stream;// stream
    enum audio_way_state state;		// 状态
    u32 in_sr;		// 输入采样率
#if (AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_SRC)
    u32 out_sr;		// 输出采样率
    void *src;		// 变采样
#endif /*(AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_SRC)*/
    u8  out_ch;		// 输出声道
#if (AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_CH_SWITCH)
    u8  in_ch;		// 输入声道
    void *ch_switch;// 声道转换
#endif /*(AUDIO_WAY_SUPPORT & AUDIO_WAY_SUPPORT_CH_SWITCH)*/
};

/*
 * 头部结构体
 */
struct audio_way_head {
    struct list_head head;	// 链表。用于链接多个 struct audio_way
    OS_MUTEX mutex;	// 互斥。避免多任务操作出现异常
};
extern struct audio_way_head 	audio_way_head;

#define AUDIO_WAY_PEND()		os_mutex_pend(&audio_way_head.mutex, 0)
#define AUDIO_WAY_POST()		os_mutex_post(&audio_way_head.mutex)

/*
*********************************************************************
*                  Audio App Resume
* Description: audio激活
* Arguments  : None.
* Return	 : None.
* Note(s)    : None.
*********************************************************************
*/
void audio_way_resume(void);

/*
*********************************************************************
*                  Audio App Initialize
* Description: 初始化所有audio输出方式
* Arguments  : None.
* Return	 : None.
* Note(s)    : None.
*********************************************************************
*/
void audio_way_init(void);


/*
*********************************************************************
*                  Audio App Open
* Description: 打开audio输出方式
* Arguments  : way_type		audio输出方式
* Return	 : 0			成功
* Note(s)    : None.
*********************************************************************
*/
int audio_way_open(u32 way_type);


/*
*********************************************************************
*                  Audio App Close
* Description: 关闭audio输出方式
* Arguments  : way_type		audio输出方式
* Return	 : 0			成功
* Note(s)    : None.
*********************************************************************
*/
int audio_way_close(u32 way_type);



/*
*********************************************************************
*                  Audio App Start
* Description: 启动audio输出方式
* Arguments  : way_type		audio输出方式
* Return	 : 0			成功
* Note(s)    : None.
*********************************************************************
*/
int audio_way_start(u32 way_type);


/*
*********************************************************************
*                  Audio App Stop
* Description: 停止audio输出方式
* Arguments  : way_type		audio输出方式
* Return	 : 0			成功
* Note(s)    : None.
*********************************************************************
*/
int audio_way_stop(u32 way_type);


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
int audio_way_output_write(s16 *buf, int len);

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
int audio_way_set_channel_num(u32 way_type, u8 channel_num);


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
int audio_way_get_channel_num(u32 way_type);


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
int audio_way_check_sample_rate(u32 way_type, u32 sample_rate, u8 high);


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
int audio_way_set_sample_rate(u32 way_type, u32 sample_rate);


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
int audio_way_get_sample_rate(u32 way_type);


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
int audio_way_get_hw_hrp(u32 way_type);

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
int audio_way_set_gain(u32 way_type, int gain);


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
int audio_way_get_gain(u32 way_type);
int audio_way_set_analog_gain(u32 way_type, u32 chmap, int gain);
int audio_way_set_digital_gain(u32 way_type, u32 chmap, int gain);


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
int audio_way_ioctrl(u32 way_type, u32 cmd, void *parm);


int sound_pcm_device_buffered_len(void *priv);

int sound_pcm_device_mount_syncts(u32 snd_type, void *syncts);

void sound_pcm_device_unmount_syncts(u32 snd_type, void *syncts);
#endif /*_AUDIO_WAY_H_*/


