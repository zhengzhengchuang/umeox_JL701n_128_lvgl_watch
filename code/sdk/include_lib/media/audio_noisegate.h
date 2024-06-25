#ifndef CONFIG_EFFECT_CORE_V2_ENABLE
#include "application/audio_noisegate.h"
#else

#ifndef _AUDIO_NOISEGATE_API_H
#define _AUDIO_NOISEGATE_API_H

#include "media/audio_stream.h"
#include "media/noisegate_api.h"

typedef struct _NOISEGATE_API_STRUCT_ {
    void				*workbuf;    //运算buf指针
    NoiseGateParam 	parm;  //参数

    struct audio_stream_entry entry;	// 音频流入口
    struct list_head hentry;                         //
    u32 ns_name;
    u8 status;
    u8 update;
} NOISEGATE_API_STRUCT;


NOISEGATE_API_STRUCT *open_noisegate(NoiseGateParam *noisegate_parm);
int run_noisegate(NOISEGATE_API_STRUCT *noisegate_hdl, short *in, short *out, int len);
void close_noisegate(NOISEGATE_API_STRUCT *hdl);
void update_noisegate(NOISEGATE_API_STRUCT *hdl, noisegate_update_param *parm);
void noisegate_bypass(NOISEGATE_API_STRUCT *hdl, u8 bypass);



NOISEGATE_API_STRUCT *get_cur_noisegate_hdl_by_name(u32 ns_name);
NOISEGATE_API_STRUCT *audio_noisegate_open(u32 ns_name, NoiseGateParam *noisegate_para);
void audio_noisegate_close(NOISEGATE_API_STRUCT *hdl);
void audio_noisegate_update(u32 ns_name, noisegate_update_param *parm);
void audio_noisegate_bypass(u32 ns_name, u8 bypass);
void audio_noisegate_run(NOISEGATE_API_STRUCT *hdl, s16 *data, u16 len);

#ifndef RUN_NORMAL
#define RUN_NORMAL  0
#endif

#ifndef RUN_BYPASS
#define RUN_BYPASS  1
#endif
#endif

#endif /*CONFIG_EFFECT_CORE_V2_ENABLE*/
