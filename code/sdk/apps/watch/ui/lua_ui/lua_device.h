#ifndef __LUA_DEVICE_H__
#define __LUA_DEVICE_H__


// 逐个获取下一个数据，从0开始一直到最后一个
#define DEVICE_GET_NEXT_DATA	0x01

// 逐个获取上一个数据，从最后一个开始一直到0
#define DEVICE_GET_PREV_DATA	0x02

// 一次性获取全部数据，返回数据table
// table = {{data0, time0}, {data1, time1}, {data2, time2}, ...}
#define DEVICE_GET_ALL_DATA		0x04

// 获取指定索引的数据
#define DEVICE_GET_IND_DATA		0x08

// 获取最大值
#define DEVICE_GET_MAX_DATA		0x10

// 获取最小值
#define DEVICE_GET_MIN_DATA		0x20

// 获取数据有多少个
#define DEVICE_GET_DATA_NUM		0x40

// 获取实时数据
#define DEVICE_GET_REAL_DATA    0X80


enum {
    DEVICE_SPORT_INFO_ALL,
    DEVICE_SPORT_INFO_STEP,
    DEVICE_SPORT_INFO_DISTANCE,
    DEVICE_SPORT_INFO_KCAL,
    DEVICE_SPORT_INFO_MOTIONSTATUS,//运动状态
    DEVICE_SPORT_INFO_MOTIONTIME,//运动时长
    DEVICE_SPORT_INFO_SPEED,//速度
    DEVICE_SPORT_INFO_PACE,//配速
    DEVICE_SPORT_INFO_STEPFREQ,//步频
    DEVICE_SPORT_INFO_STEPSTRIDE,//步幅
    DEVICE_SPORT_INFO_INTENSITY,//强度区间
    DEVICE_SPORT_INFO_RECOVERYTIME,
    DEVICE_SPORT_INFO_INTENSITYTIME,
    DEVICE_SPORT_DAILY_STEP,
    DEVICE_SPORT_DAILY_KCAL,
    DEVICE_SPORT_DAILY_DISTANCE

};
enum {
    DEVICE_SPORT_CTRL_CLR,//清空状态
    DEVICE_SPORT_CTRL_START,//运动开始
    DEVICE_SPORT_CTRL_PAUSE,//运动暂停
    DEVICE_SPORT_CTRL_KEEP,//运动继续
    DEVICE_SPORT_CTRL_STOP,//运动结束
    DEVICE_SPORT_CTRL_STATUS//查询状态5
};

// 获取数组长度
#define get_array_num(array)	sizeof(array) / sizeof(array[0])

#endif

