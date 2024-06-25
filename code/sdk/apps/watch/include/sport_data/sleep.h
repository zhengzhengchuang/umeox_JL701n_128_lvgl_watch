#ifndef __SLEEP_H__
#define __SLEEP_H__
#include "sport_data/p33_algo_motion.h"
struct sleep_record {
    u8 flag;//睡眠类型：RSM深睡浅睡
    struct sys_time flag_start;
    struct sys_time flag_end;
    struct sleep_record *next;
};

struct sleep_data_t {
    u8 sleepcnt;
    u8 status;//0为初始化 1短睡眠 2长睡眠段开始3长睡眠段结束
    struct sleep_record *sleeprecord_head;

};
struct sleep_data_analysis {
    u32 sleep_msec_awake;
    u32 sleep_msec_shallow;
    u32 sleep_msec_deep;
    u32 sleep_msec_deep_max;
    u32 sleep_msec_rem;
    u32 sleep_msec_continue;
    u32 sleep_msec_sleep;
    u32 sleep_msec_all;
    u32 sleep_score;
    u32 sleep_continue_score;
    u8 sleep_evaluate;
    u8 deep_evalute;
    u8 shallow_evalute;
    u8 rem_evalute;
    u8 awake_times;
};

enum {
    SLEEP_EVA_NULL,
    SLEEP_EVA_LOW,
    SLEEP_EVA_NORMAL,
    SLEEP_EVA_HIGH,
};
#pragma pack(1)//不平台对齐编译
struct sleep_data_head {
    u8 hour;
    u8 min;
    u16 len;
};
struct sleep_record_block {
    u8 sleep_type;
    u8 sleep_min;

};
struct sleep_analyse_record {
    u8 score_sleep;
    u8 percent_deep;
    u8 percent_shallow;
    u8 percent_rem;
    u8 sleep_evaluate: 2;
    u8 deep_evalute: 2;
    u8 shallow_evalute: 2;
    u8 rem_evalute: 2;
    u8 continue_score;
    u8 awake_times;
};
#pragma pack()//平台对齐编译


//睡眠信息记录
u16 record_short_sleep(u8 flag);
u16 record_long_sleep(u8 flag);

int motion_module_sleep_file_write(void);
int motion_module_sleep_file_read(sleep_data *__sleep);
int motion_module_sleep_file_analysis(struct sleep_data_analysis *__analysis, sleep_data *__sleep);
// u16 record_2hour_sleep(short *buf);


//睡眠数据接口，用于绘图
// extern u8 get_sleep_record(struct sleep_record *p);//获取最近一次睡眠数据
// extern void sleep_flag_free(void);//清除最近一次睡眠数据，并释放空间

// extern int sleep_info_trans_buf(u8 *buf);
// extern int sleep_info_trans_buf_len(void);

static int get_sleep_start_time(struct sys_time *sleeptime);//睡眠开始时间
static int get_sleep_stop_time(struct sys_time *sleeptime);//睡眠结束时间
// extern int sleep_time_start_stop_time(int type,struct sys_time *sleeptime)

// extern int get_watch_sleep_target(void);

// extern int sleep_file_write(void);
#endif//__SLEEP_H__
