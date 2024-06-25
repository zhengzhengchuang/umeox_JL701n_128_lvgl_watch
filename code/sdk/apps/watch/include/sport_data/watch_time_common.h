#ifndef __WATCH_TIME_COMMON_H__
#define __WATCH_TIME_COMMON_H__

#include "sport_data/watch_common.h"
//common
//日期接口
void watch_file_get_sys_time(struct sys_time *time);
void watch_file_prev_day(struct sys_time *ptime, struct sys_time *ntime);
void watch_file_prev_x_day(struct sys_time *ptime, struct sys_time *ntime, int x);
void watch_file_next_day(struct sys_time *ptime, struct sys_time *ntime);
int watch_file_get_day_num(struct sys_time *time);
void watch_time_add_min(struct sys_time *ptime, u8 min);
void watch_time_sub_min(struct sys_time *ptime, u8 min);
int watch_time_len_min(struct sys_time *ptime, struct sys_time *ntime);

u8 watch_time_age(u16 year, u8 month, u8 day);

int watch_file_time_to_id(int type, struct sys_time *file_time);
int health_file_index_to_id(struct health_file_info *__info);
int health_file_open_file_get_index(struct health_file_info *__info);
int health_file_open_new_file(struct health_file_info *__info);
void watch_common_swapX(const uint8_t *src, uint8_t *dst, int32_t len);

#endif
