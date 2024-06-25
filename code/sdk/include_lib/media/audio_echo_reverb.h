#ifndef CONFIG_EFFECT_CORE_V2_ENABLE
#include "application/audio_echo_reverb.h"
#else


#ifndef reverb_echo_h__
#define reverb_echo_h__

#include "media/reverb_api.h"
/*
 * 打开声卡混响模块
 */
REVERBN_API_STRUCT *open_reverb(REVERBN_PARM_SET *reverb_seting, u16 sample_rate);
/*
 * 声卡混响处理
 */
void run_reverb(REVERBN_API_STRUCT *reverb_api_obj, short *in, short *out, int len);
/*
 * 关闭声卡混响模块
 */
void  close_reverb(REVERBN_API_STRUCT *reverb_api_obj);
/*
 * 暂停声卡混响处理
 */
void pause_reverb(REVERBN_API_STRUCT *reverb_api_obj, u8 run_mark);
/*
 * 声卡混响参数更新
 */
void update_reverb_parm(REVERBN_API_STRUCT *reverb_api_obj, REVERBN_PARM_SET *reverb_seting);

void reverb_update_bypass(REVERBN_API_STRUCT *hdl, int bypass);


/*
 * 打开快速声卡混响模块
 */
REVERB0_API_STRUCT *open_fast_reverb(EF_REVERB0_PARM *reverb_seting, u16 sample_rate);
/*
 * 快速声卡混响处理
 */
void run_fast_reverb(REVERB0_API_STRUCT *reverb_api_obj, short *in, short *out, int len);
/*
 * 关闭快速声卡混响模块
 */
void  close_fast_reverb(REVERB0_API_STRUCT *reverb_api_obj);
/*
 * 暂停快速声卡混响处理
 */
void pause_fast_reverb(REVERB0_API_STRUCT *reverb_api_obj, u8 run_mark);
/*
 *  快速声卡混响参数更新
 */
void update_fast_reverb_parm(REVERB0_API_STRUCT *reverb_api_obj, EF_REVERB0_PARM *reverb_seting);

void fast_reverb_update_bypass(REVERB0_API_STRUCT *hdl, int bypass);



/*
 * 打开plate声卡混响模块
 */
PLATE_REVERB_API_STRUCT *open_plate_reverb(Plate_reverb_parm *reverb_seting, u16 sample_rate);
/*
 * plate声卡混响处理
 */
void run_plate_reverb(PLATE_REVERB_API_STRUCT *reverb_api_obj, short *in, short *out, int len);
/*
 * 关闭plate声卡混响模块
 */
void  close_plate_reverb(PLATE_REVERB_API_STRUCT *reverb_api_obj);
/*
 * 暂停plate声卡混响处理
 */
void pause_plate_reverb(PLATE_REVERB_API_STRUCT *reverb_api_obj, u8 run_mark);
/*
 *  plate声卡混响参数更新
 */
void update_plate_reverb_parm(PLATE_REVERB_API_STRUCT *reverb_api_obj, Plate_reverb_parm *reverb_seting);

void plate_reverb_update_bypass(PLATE_REVERB_API_STRUCT *hdl, int bypass);




/*
 * 打开echo混响模块
 */
ECHO_API_STRUCT *open_echo(ECHO_PARM_SET *echo_seting, EF_REVERB_FIX_PARM *fix_parm);
/*
 * echo混响处理
 */
void run_echo(ECHO_API_STRUCT *p_echo_obj, short *in, short *out, int len);
/*
 * 关闭echo混响模块
 */
void  close_echo(ECHO_API_STRUCT *echo_api_obj);
/*
 * 暂停echo混响处理
 */
void  pause_echo(ECHO_API_STRUCT *echo_api_obj, u8 run_mark);
/*
 * 更新echo混响通用参数
 */
void update_echo_parm(ECHO_API_STRUCT *echo_api_obj, ECHO_PARM_SET *echo_seting);
/*
 * 更新echo混响gain参数
 */
void update_echo_gain(ECHO_API_STRUCT *echo_api_obj, int wetgain, int drygain);


void echo_update_bypass(ECHO_API_STRUCT *hdl, int bypass);
#endif // reverb_echo_h__
#endif /*CONFIG_EFFECT_CORE_V2_ENABLE*/
