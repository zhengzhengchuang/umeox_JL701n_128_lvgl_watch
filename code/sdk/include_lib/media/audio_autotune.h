
#ifndef _AUDIO_PITCH_API_H_
#define _AUDIO_PITCH_API_H_

#include "media/voiceChanger_api.h"
#include "media/audio_stream.h"
typedef struct _autotune_hdl {
    AUTOTUNE_FUNC_API *ops;
    void *workbuf;
    AUTOTUNE_PARM parm;
    struct audio_stream_entry entry;	// 音频流入口
    struct list_head hentry;                        //
    u32 sample_rate;
    u32 autotune_name;
    u8 update;
    u8 status;
} autotune_hdl;

extern AUTOTUNE_FUNC_API *get_autotune_func_api();
/*
 * 电子音打开
 */
autotune_hdl *audio_autotune_open(u32 autotune_name, AUTOTUNE_PARM *param, u32 sample_rate);
/*
 *  电子音关闭
 */
void audio_autotune_close(autotune_hdl *hdl);
/*
 *  电子音参数更新
 */
void audio_autotune_update_parm(u32 autotune_name, AUTOTUNE_PARM *parm);
/*
 *  电子音数据处理
 */
int audio_autotune_run(autotune_hdl *hdl, s16 *indata, s16 *outdata, int len, u8 ch_num);
/*
 *  电子音暂停处理
 */

void audio_autotune_bypass(u32 autotune_name, u8 bypass);

autotune_hdl *get_cur_autotune_hdl_by_name(u32 autotune_name);
#endif

#ifndef RUN_NORMAL
#define RUN_NORMAL  0
#endif

#ifndef RUN_BYPASS
#define RUN_BYPASS  1
#endif

