#ifndef __BLOOD_OXYGEN_H__
#define __BLOOD_OXYGEN_H__

//	功能开关
// extern int  blood_oxygen_start(void);
// extern int  blood_oxygen_stop(void);
//int blood_oxygen_day_buf_enable(void);
//int blood_oxygen_day_buf_disable(void);
//	数据获取
// extern int get_day_blood_oxygen_data(u8 *buf,int buf_len,struct sys_time *time);
// extern int get_day_blood_oxygen_data_len(struct sys_time *time);
//	清除数据
// extern void clr_blood_oxygen(void)


//	私有函数
static int blood_oxygen_file_write(void);
static int blood_oxygen_file_write_break(void);
static int blood_oxygen_file_write_stop(void);

static int blood_oxygen_file_time_get(struct sys_time *ntime);
static int blood_oxygen_file_read(struct sys_time *p);

static void blood_oxygen_refresh_data(void *priv);
static void blood_oxygen_refresh_data_handle(void);

#endif
