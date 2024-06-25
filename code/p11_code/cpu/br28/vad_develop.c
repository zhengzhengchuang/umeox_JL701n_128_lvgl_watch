/*****************************************************************
>file name : vad_develop.c
>create time : Thu 28 Apr 2022 07:11:24 PM CST
*****************************************************************/
#include "includes.h"
/*******************************************************
 * DVAD算法模块开发说明
 * 1、默认使用杰理api，命名为jl_dvad_xxx
 * 2、第三方算法开发，需将对应接口替换为自身算法模块
 *
 *******************************************************/
extern const int CONFIG_DVAD_DEVELOP_ENABLE;

extern void *jl_dvad_open(void *cfg);
extern int jl_dvad_get_mic_gain(void *handle);
extern int jl_dvad_detect(void *handle, short *data, int samples);
extern void jl_dvad_close(void *handle);

void *dvad_open(void *cfg)
{
    if (CONFIG_DVAD_DEVELOP_ENABLE == 0) {
        return jl_dvad_open(cfg);
    }

    /*其他算法开发*/
    if (CONFIG_DVAD_DEVELOP_ENABLE == 1) {
        return NULL;
    }

    return NULL;
}

int dvad_get_mic_gain(void *handle)
{
    if (CONFIG_DVAD_DEVELOP_ENABLE == 0) {
        return jl_dvad_get_mic_gain(handle);
    }

    if (CONFIG_DVAD_DEVELOP_ENABLE == 1) {
        return 0;
    }

    return 0;
}

/*
 * 参数说明 : handle    - 算法私有数据
 *            data      - PCM采样数据
 *            samples   - 音频采样点数
 * 返回值 : 0 - 保持原有状态不变
 *          1 - DVAD检测中
 *          2 - DVAD检测到语音
 *          3 - DVAD进入待机
 *
 */
int dvad_detect(void *handle, short *data, int samples)
{
    if (CONFIG_DVAD_DEVELOP_ENABLE == 0) {
        return jl_dvad_detect(handle, data, samples);
    }

    if (CONFIG_DVAD_DEVELOP_ENABLE == 1) {
        return 0;
    }

    return 0;
}

void dvad_close(void *handle)
{
    if (CONFIG_DVAD_DEVELOP_ENABLE == 0) {
        return jl_dvad_close(handle);
    }

    if (CONFIG_DVAD_DEVELOP_ENABLE == 1) {

    }
}
