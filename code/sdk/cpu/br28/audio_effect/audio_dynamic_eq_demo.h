#ifndef __AUDIO_DYNAMEIC_EQ_H__
#define __AUDIO_DYNAMEIC_EQ_H__

#include "media/dynamic_eq.h"
#include "media/effects_adj.h"


struct dynamic_eq_hdl {
    struct dynamic_eq_detection *dy_eq_det;
    struct dynamic_eq *dy_eq;
    u32 dynamic_eq_name;
    struct list_head hentry;                         //
};

/* 先打开audio_dynamic_eq_detection_open_demo */
/* 再打开audio_dynamic_eq_open_demo */
struct dynamic_eq *audio_dynamic_eq_open_demo(u32 dynamic_eq_name, u32 sample_rate, u8 channel);
void audio_dynamic_eq_close_demo(struct dynamic_eq *hdl);

struct dynamic_eq_detection *audio_dynamic_eq_detection_open_demo(u32 dynamic_eq_name, u32 sample_rate, u8 channel);
void audio_dynamic_eq_detection_close_demo(struct dynamic_eq_detection *hdl);

struct dynamic_eq_hdl *audio_dynamic_eq_ctrl_open(u32 dynamic_eq_name, u32 sample_rate, u8 channel);
void audio_dynamic_eq_ctrl_close(struct dynamic_eq_hdl *hdl);

void audio_dynamic_eq_detection_update_parm(u32 dynamic_eq_name, void *parm, int bypass);
void audio_dynamic_eq_update_parm(u32 dynamic_eq_name, void *parm, void *parm2, int bypass);
struct dynamic_eq_hdl *get_cur_dynamic_eq_hdl_by_name(u32 dynamic_eq_name);

#endif/*__AUDIO_DYNAMEIC_EQ_H__*/
