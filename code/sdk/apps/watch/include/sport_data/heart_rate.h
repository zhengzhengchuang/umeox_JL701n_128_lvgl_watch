#ifndef __HEART_RATE_H__
#define __HEART_RATE_H__
//	功能开关
// extern int heart_rate_start(void);
// extern int heart_rate_stop(void);
//int heart_rate_day_buf_disable(void);
//int heart_rate_day_buf_enable(void)
//	数据获取
// extern int get_day_heart_rate_data(u8 *buf,int buf_len,struct sys_time *time);
// extern int get_day_heart_rate_data_len(struct sys_time *time);
//	清除数据
// extern void clr_heart_rate(void);
//	私有函数
static int heart_rate_file_write(void);
static int heart_rate_file_write_break(void);
static int heart_rate_file_write_stop(void);

static int heart_rate_file_time_get(struct sys_time *ntime);
static int heart_rate_file_read(struct sys_time *p);

static void heart_rate_refresh_data_handle(void);
static void heart_rate_refresh_data(void *priv);

#endif

