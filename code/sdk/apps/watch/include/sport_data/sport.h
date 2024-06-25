#ifndef __SPORT_H__
#define __SPORT_H__

//运动模式的启停
// extern u8 execise_ctrl_status_set(unsigned char sport_mode,unsigned char status);
// extern u8 set_sport_mode(u8 mode);
// extern u8 execise_mode_get(void);
// extern u8 execise_ctrl_status_get(void);
// extern void execise_info_clr(void);
//运动对外接口
// extern unsigned char execise_info_get(struct motion_info *info);
// extern u32 get_sport_start_time(struct sys_time *t);
// extern u32 get_sport_end_time(struct sys_time *t);
// extern int get_sport_pace_sec(void);
struct daily_active_data {
    u32 step;
    u32 distance;
    u32 kcal;
};
#pragma pack(1)//不平台对齐编译
struct daily_active_block {
    u16 step;
    u16 distance_ten;
    u16 kcal;
};
#pragma pack(0)//平台对齐编译

struct daily_ctrl {
    u16 timer_id ;
    u8 first_flag;
};

int	daily_active_record_start(void);
int daily_active_record_stop(void);
int get_sport_active_time(void);



#endif


