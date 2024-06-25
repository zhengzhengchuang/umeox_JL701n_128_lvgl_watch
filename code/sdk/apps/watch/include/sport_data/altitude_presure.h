#ifndef __ALTITUDE_PRESURE_H__
#define __ALTITUDE_PRESURE_H__


//获取海拔气压功能开关
int altitude_airpresure_start(void);
int altitude_airpresure_stop(void);
int altitude_airpresure_day_buf_enable(void);
int altitude_airpresure_day_buf_disable(void);
//更新气压数据
void altitude_airpresure_refresh_data(void);

//获取实时气压
u16 get_data_air_presure(void);
//获取最小气压
u16 get_min_air_presure(void);
//获取最大气压
u16  get_max_air_presure(void);
//获取日气压数据
u16 get_day_air_presure_data(u8 *buf);


//获取实时海拔
u32 get_data_altitude(void);
//获取最小海拔
u32 get_min_altitude(void);
//获取最大海拔
u32  get_max_altitude(void);
//获取日海拔数据
u32 get_day_altitude_data(u8 *buf);
//清除海拔气压记录，每日凌晨调用
void clr_air_presure_altitude(void);
//文件操作

//type=0 海拔
//type=1 气压

//读写海拔-气压数据文件
int altitude_presure_write_file(int type);
int altitude_presure_read_file(struct sys_time *ptime, int type);

//获取任意时段气压/海拔数据
int altitude_presure_get_anyhour_data(struct sys_time *ptime, struct sys_time *ntime, u8 *buf, int type);
int altitude_presure_get_3day_data(struct sys_time *ntime, u8 *databuf, int type);
int altitude_presure_get_week_data(struct sys_time *ntime, u8 *databuf, int type);
//清除数据
int altitude_presure_file_del(struct sys_time *ptime, struct sys_time *ntime, int type);
int altitude_presure_file_del_all(int type);

#endif
