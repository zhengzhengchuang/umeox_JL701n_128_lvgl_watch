
#ifndef _AUDIO_VOICE_CHANGER_API_H_
#define _AUDIO_VOICE_CHANGER_API_H_

#include "media/voiceChanger_api.h"
#include "media/audio_stream.h"
typedef struct _voice_changer_hdl {
    VOICECHANGER_FUNC_API *ops;
    void *workbuf;
    VOICECHANGER_PARM parm;

    struct audio_stream_entry entry;	// 音频流入口
    struct list_head hentry;                        //
    u32 sample_rate;
    u32 voice_changer_name;
    u8 update;
    u8 status;
} voice_changer_hdl;
/*
 * 获取变声模块默认参数；open时不传默认参数会使用内部默认参数
 * 仅用于获取初值。实时参数存放在open的返回句柄parm中
 */
extern VOICECHANGER_FUNC_API *get_voiceChanger_func_api();
/*
 * 变声模块打开
 */
voice_changer_hdl *audio_voice_changer_open(VOICECHANGER_PARM *param, u32 sample_rate, u32 voice_changer_name);
/*
 * 变声模块关闭
 */
void audio_voice_changer_close(voice_changer_hdl *hdl);
/*
 * 变声模块参数更新
 */
void audio_voice_changer_update_parm(u32 voice_changer_name, VOICECHANGER_PARM *parm);
/*
 * 变声模块数据处理
 */
int audio_voice_changer_run(voice_changer_hdl *hdl, s16 *indata, s16 *outdata, int len, u8 ch_num);
/*
 * 变声模块暂停处理
 */
void audio_voice_changer_bypass(u32 voice_changer_name, u32 bypass);
#endif

#ifndef RUN_NORMAL
#define RUN_NORMAL  0
#endif

#ifndef RUN_BYPASS
#define RUN_BYPASS  1
#endif

