/****************************************************************/
/* Copyright:(c)JIELI  2011-2022 All Rights Reserved.           */
/****************************************************************/
/*
>file name : broadcast_audio_sync.h
>create time : Sat 11 Jun 2022 03:44:33 PM CST
*****************************************************************/
#ifndef _BROADCAST_AUDIO_SYNC_H_
#define _BROADCAST_AUDIO_SYNC_H_
#include "typedef.h"

#define BROADCAST_SOURCE_DMA                0   /*周期采样输入源*/
#define BROADCAST_SOURCE_BT_BLOCK           1   /*蓝牙阻塞式输入源*/
#define BROADCAST_SOURCE_FILE_BLOCK         2   /*文件阻塞式输入源*/

struct broadcast_audio_stream {
    u8 source;                                          /*输入源选择*/
    u8 nch;                                             /*声道数*/
    short delay_time;                                   /*目标延时*/
    int sample_rate;                                    /*输入采样率*/
    int broadcast_sample_rate;                          /*广播目标采样率*/

    /*output handle*/
    void *priv;                                         /*输出私有数据*/
    int (*output)(void *priv, void *data, int len);     /*输出接口*/

    /*clock callback*/
    void *clock_data;                                   /*参考时钟数据*/
    u32(*clock_time)(void *clock);                      /*参考时钟的当前时钟获取*/
    u32(*tick_clock)(void *clock);                      /*参考时钟的节奏时钟获取 - 广播音箱为pdu时钟*/

    /*private_input*/
    void *private_input;                                /*输入的私有数据 - 阻塞输入需要配置*/
    int (*buffered_time)(void *private_input);           /*蓝牙2.1 A2DP需查询缓冲延时*/
    void (*kick_input)(void *private_input);            /*被本模块阻塞后进行kick操作*/

    const char *task;                                   /*运行的task - 一般DMA采样需要用到*/
};

/***********************************************************
 *      broadcast audio sync open
 * description  : 广播音频流同步的打开
 * arguments    : stream        -   广播的broadcast audio stream结构参数
 * return       : NULL      -   failed,
 *                非NULL    -   private_data
 * notes        : None.
 ***********************************************************/
void *broadcast_audio_sync_open(struct broadcast_audio_stream *stream);

/***********************************************************
 *      broadcast audio sync dma input
 * description  : 广播音频流同步的dma周期采样输入
 * arguments    : bcsync    -   音频同步的私有结构
 *                data      -   音频采样数据
 *                len       -   音频采样数据长度(bytes)
 * return       : 实际的处理长度.
 * notes        : None.
 ***********************************************************/
int broadcast_audio_sync_dma_input(void *basync, void *data, int len);

/***********************************************************
 *      broadcast audio update tx num
 * description  : 广播音频流蓝牙端发送(采样)的pcm采样数
 * arguments    : bcsync    -   音频同步的私有结构
 *                frames    -   发生的采样数
 * return       : None.
 * notes        : None.
 ***********************************************************/
void broadcast_audio_update_tx_num(void *basync, int frames);

/***********************************************************
 *      broadcast audio sync block input
 * description  : 广播音频流同步的阻塞数据输入
 * arguments    : bcsync    -   音频同步的私有结构
 *                data      -   音频采样数据
 *                len       -   音频采样数据长度(bytes)
 * return       : 实际的处理长度.
 * notes        : None.
 ***********************************************************/
int broadcast_audio_sync_block_input(void *basync, void *data, int len);

/***********************************************************
 *      broadcast audio sync close
 * description  : 广播音频流同步关闭
 * arguments    : priv      -   音频同步的私有结构
 * return       : None.
 * notes        : None.
 ***********************************************************/
void broadcast_audio_sync_close(void *priv);

/***********************************************************
 *      broadcast audio sync update source fmt
 * description  : 广播音频流同步的输入格式更新
 * arguments    : priv          -   音频同步的私有结构
 *                sample_rate   -   采样率
 *                channel_num   -   通道数
 * return       : None.
 * notes        : 主要用于开始时格式未知，在确认格式后更新
 *                格式信息到音频流同步.
 ***********************************************************/
void broadcast_audio_sync_update_source_fmt(void *priv, int sample_rate, u8 channel_num);

#endif
