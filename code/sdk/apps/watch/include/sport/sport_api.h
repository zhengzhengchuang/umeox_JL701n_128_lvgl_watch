#ifndef __SPORT__API__
#define __SPORT__API__


#include "ui/ui.h"


#include "sport_data/sport.h"

extern int watch_sport_start(u8 type); //开始运动
extern int watch_sport_stop();
extern int watch_sport_update(void *p);
extern int watch_sport_restart();
extern int watch_sport_pause();




extern int sportrecord_get_start_time(struct sys_time *time);
extern int sportrecord_get_end_time(struct sys_time *time);
extern int sportrecord_get_run_time(struct sys_time *time);
extern int sportrecord_get_heat();
extern int sportrecord_get_distance(u8 *significan);
extern int sportrecord_get_recovertime(struct sys_time *time);
extern int sportrecord_get_Hheart();
extern int sportrecord_get_Lheart();
extern int sportrecord_get_meanheart();

extern int sportrecord_get_aerobic_time(struct sys_time *time);
extern int sportrecord_get_anaerobic_time(struct sys_time *time);
extern int sportrecord_get_fat_time(struct sys_time *time);
extern int sportrecord_get_limit_time(struct sys_time *time);
extern int sportrecord_get_warm_time(struct sys_time *time);


extern int watch_sport_analyze();

extern int sport_file_scan_init();
extern int sport_file_scan_release();



struct MANAGE_SOURCE {
    int       id;
    const u8 *source;
    const u8  type;
    void *func;
};


typedef struct {
    u8 type;//
    struct sys_time stime;
    struct sys_time etime;
    int sport_time;
    u16  distance;//距离
    u16  calorie;//卡路里
    u32  step;
    u16  recovery_time;
    u8 heart_rate_mode;
    u32 Intensity_time[5];
} SPORT_INFO;//



extern int sport_file_get_info_by_index(int index, int count, SPORT_INFO *info);
extern void ui_update_source_by_elm(struct element *elm, u8 redraw);
extern u16 get_sport_recode_id(void);
extern u16 get_sport_recode_size(void);
#endif
