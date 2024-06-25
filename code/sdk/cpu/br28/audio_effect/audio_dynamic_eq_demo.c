#include "audio_dynamic_eq_demo.h"
#include "audio_eff_default_parm.h"
/* 动态eq 精度是precision的话，两段，peak 44100 是30M(不包含信号检测) */

#if defined(TCFG_DYNAMIC_EQ_ENABLE) && TCFG_DYNAMIC_EQ_ENABLE
struct dynamic_eq_list {
    struct list_head head;            //链表头
};
static struct dynamic_eq_list dy_eq_hdl = {0};


DynamicEQParam_TOOL_SET  dynamic_eq[mode_add];

struct dynamic_eq_hdl *get_cur_dynamic_eq_hdl_by_name(u32 dynamic_eq_name)
{
    local_irq_disable();
    struct dynamic_eq_hdl *hdl;
    list_for_each_entry(hdl, &dy_eq_hdl.head, hentry) {
        log_d("dynamic_eq_name %d, name %d\n", dynamic_eq_name, hdl->dynamic_eq_name);
        if (dynamic_eq_name == hdl->dynamic_eq_name) {
            local_irq_enable();
            return hdl;
        }
    }
    local_irq_enable();
    log_e("cur dynamic_eq NULL\n");
    return NULL;
}



/* 先打开audio_dynamic_eq_detection_open_demo */
/* 再打开audio_dynamic_eq_open_demo */
struct dynamic_eq *audio_dynamic_eq_open_demo(u32 dynamic_eq_name, u32 sample_rate, u8 channel)
{
    DynamicEQEffectParam effectParam[2];
    DynamicEQParam param = {0};
    u8 tar = 0;
    if (dynamic_eq_name == AEID_AUX_DYNAMIC_EQ) {
        tar = 1;
    }
    param.detect_mode = dynamic_eq[tar].detect_mode;//TWOPOINT;
    param.channel = channel;
    param.nSection = dynamic_eq[tar].nSection;//EQ_NSECTION;
    param.SampleRate = sample_rate;
    param.DetectdataInc = (channel == 1) ? 1 : 2;
    param.IndataInc = (channel == 1) ? 1 : 2;
    param.OutdataInc = (channel == 1) ? 1 : 2;
    memcpy(effectParam, &dynamic_eq[tar].effect_param, sizeof(DynamicEQEffectParam)*param.nSection);
    struct dynamic_eq *hdl = dynamic_eq_open((DynamicEQEffectParam *)effectParam, (DynamicEQParam *)&param);
    return hdl;
}

void audio_dynamic_eq_close_demo(struct dynamic_eq *hdl)
{
    dynamic_eq_close(hdl);
}

void audio_dynamic_eq_update_parm(u32 dynamic_eq_name, void *parm, void *parm2, int bypass)
{
    struct dynamic_eq_hdl *_hdl = get_cur_dynamic_eq_hdl_by_name(dynamic_eq_name);
    if (!_hdl) {
        return ;
    }
    struct dynamic_eq *hdl = _hdl->dy_eq;
    if (hdl) {
        dynamic_eq_update(hdl, parm, parm2);

        dynamic_eq_bypass(hdl, bypass);
    }
}


struct dynamic_eq_detection *audio_dynamic_eq_detection_open_demo(u32 dynamic_eq_name, u32 sample_rate, u8 channel)
{
    u8 tar = 0;
    if (dynamic_eq_name == AEID_AUX_DYNAMIC_EQ) {
        tar = 1;
    }

    DynamicEQDetectionParam detectParm[2];
    detectParm[0].fc = dynamic_eq[tar].effect_param[0].fc;//CENTER_FREQ_0;
    detectParm[1].fc = dynamic_eq[tar].effect_param[1].fc;//CENTER_FREQ_1;

    struct dynamic_eq_detection *hdl = dynamic_eq_detection_open((DynamicEQDetectionParam *)&detectParm, dynamic_eq[tar].nSection, channel, sample_rate);
    return hdl;
}


void audio_dynamic_eq_detection_close_demo(struct dynamic_eq_detection *hdl)
{
    dynamic_eq_detection_close(hdl);
}


void audio_dynamic_eq_detection_update_parm(u32 dynamic_eq_name, void *parm, int bypass)
{
    struct dynamic_eq_hdl *_hdl = get_cur_dynamic_eq_hdl_by_name(dynamic_eq_name);
    if (!_hdl) {
        return ;
    }
    struct dynamic_eq_detection *hdl = _hdl->dy_eq_det;
    if (hdl) {
        dynamic_eq_detection_update(hdl, parm);
        dynamic_eq_detection_bypass(hdl, bypass);
    }
}


int dy_eq_prob_handler(struct audio_stream_entry *entry,  struct audio_data_frame *in)	// 预处理
{
    struct dynamic_eq *_hdl = container_of(entry, struct dynamic_eq, entry);
    if (!_hdl) {
        return 0;
    }

    struct dynamic_eq_detection *hdl = _hdl->det_hdl;
    if (in->data_len - in->offset > 0) {
        if (hdl) {
            hdl->in_32bit = 1;
            dynamic_eq_detection_run(hdl, (short *)((int)in->data + in->offset), in->data_len - in->offset);
        }
    }
    return 0;
}


struct dynamic_eq_hdl *audio_dynamic_eq_ctrl_open(u32 dynamic_eq_name, u32 sample_rate, u8 channel)
{
    struct dynamic_eq_hdl *hdl = zalloc(sizeof(struct dynamic_eq_hdl));
    if (!hdl) {
        return NULL;
    }
    struct dynamic_eq_detection *dy_eq_det = audio_dynamic_eq_detection_open_demo(dynamic_eq_name, sample_rate, channel);
    if (!dy_eq_det) {
        return NULL;
    }
    struct dynamic_eq *dy_eq = audio_dynamic_eq_open_demo(dynamic_eq_name, sample_rate, channel);
    if (!dy_eq) {
        log_e("dy_eq NULL\n");
        return NULL;
    }
    if (dy_eq_det) {
        dynamic_eq_set_detection_callback(dy_eq, dy_eq_det, get_dynamic_eq_detection_parm);
    }
    hdl->dynamic_eq_name = dynamic_eq_name;
    hdl->dy_eq_det = dy_eq_det;
    hdl->dy_eq = dy_eq;
    hdl->dy_eq->entry.prob_handler = dy_eq_prob_handler;
    hdl->dy_eq->det_hdl = dy_eq_det;
    local_irq_disable();
    list_add(&hdl->hentry, &dy_eq_hdl.head);
    local_irq_enable();
    return hdl;
}

void audio_dynamic_eq_ctrl_close(struct dynamic_eq_hdl *hdl)
{
    if (!hdl) {
        return;
    }

    if (hdl->dy_eq) {
        audio_dynamic_eq_close_demo(hdl->dy_eq);
        hdl->dy_eq = NULL;
    }
    if (hdl->dy_eq_det) {
        audio_dynamic_eq_detection_close_demo(hdl->dy_eq_det);
        hdl->dy_eq_det = NULL;
    }
    local_irq_disable();
    list_del(&hdl->hentry);
    local_irq_enable();

    if (hdl) {
        free(hdl);
        hdl = NULL;
    }
}

static int audio_dynamic_eq_init(void)
{
    INIT_LIST_HEAD(&dy_eq_hdl.head);

    return 0;
}
__initcall(audio_dynamic_eq_init);
#endif
