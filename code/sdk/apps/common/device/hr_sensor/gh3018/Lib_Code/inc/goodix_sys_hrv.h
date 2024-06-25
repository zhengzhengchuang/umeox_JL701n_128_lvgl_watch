/*
* HRVLib.h
*/

#ifndef _GOODIX_SYS_HRV_LIB_H_
#define _GOODIX_SYS_HRV_LIB_H_

#include <stdint.h>
#include "iot_sys_def.h"
#if DEBUG_HRV
#include "..\..\HRVTest\goodix_debug.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 返回错误码 */
typedef enum {
    GX_HRV_ALGO_OK              = 0x00000000, // 成功返回，或无出值刷新
    GX_HRV_ALGO_FAILED          = 0x10000001, // 失败返回
    GX_HRV_ALGO_NULL_PTR        = 0x10000002, // 外部传入的指针为空
    GX_HRV_ALGO_INVALID_PARAM   = 0x10000003, // 参数范围无效
    GX_HRV_ALGO_OUT_OF_MEM      = 0x10000004, // 内存分配失败
    GX_HRV_ALGO_DEINIT_ABORT    = 0x10000005, // 释放失败
    GX_HRV_ALGO_UPDATE          = 0x10000006, // 有出值刷新
}goodix_hrv_ret;

#define PPG_CHANNEL_NUM		            4
#define RRI_NUM                         4
#define ACC_THR_NUM                     4

typedef struct {
    int32_t need_ipl;                                   // 是否需要插值到1KHz
    int32_t fs;                                         // 采样率
    int32_t acc_thr[ACC_THR_NUM];                       // acc 相关阈值，默认值为：20/10/3/3
}goodix_hrv_config;

typedef struct {
	uint8_t bit_num;                                //数据位数
    int32_t ppg_rawdata[PPG_CHANNEL_NUM];                 // PPG原始信号，最多四通道
    int32_t rawdata_cur_adj_flag[PPG_CHANNEL_NUM];        // 电流调光标志位
    int32_t rawdata_gain_adj_flag[PPG_CHANNEL_NUM];       // 增益调光标志位
	int32_t acc_x;                                      // 加速计 x 轴
    int32_t acc_y;                                      // 加速计 y 轴
    int32_t acc_z;                                      // 加速计 z 轴
    int32_t frame_id;                                   // 帧序号
    int32_t hr;                                         // 当前心率值
}goodix_hrv_input_rawdata;

typedef struct {
    int32_t rri[RRI_NUM];                               // RRI结果数组
    int32_t rri_confidence;                             // 结果置信度
	int32_t rri_valid_num;                              // RRI有效结果数量
}goodix_hrv_result;

IOT_SYS_DLL_API uint8_t *goodix_hrv_version(void);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_init(goodix_hrv_config *config);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_calc(goodix_hrv_input_rawdata *input_data, goodix_hrv_result *output_data);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_deinit(void);

#if DEBUG_HRV
IOT_SYS_DLL_API goodix_hrv_debug *copy_debug_addr();
#endif
#ifdef __cplusplus
}
#endif

#endif // _GOODIX_SYS_HRV_LIB_H_
