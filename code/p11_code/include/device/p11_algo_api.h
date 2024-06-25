#pragma once

#define algo_log(fmt,...)  printf("[algo] " fmt "\n",##__VA_ARGS__)

typedef union {
    struct {
        short wrist_up: 1;        //抬腕动作
        short wrist_down: 1;      //落腕动作
        short double_click: 1;    //双击屏幕
        short hitting: 1;         //击球动作
        short on_desk: 1;         //放置桌面
    };
    short all;
} algo_type;

typedef struct {
    short x;               //gsensor x轴数据
    short y;               //gsensor y轴数据
    short z;               //gsensor z轴数据
} accel_data;

typedef struct {
    short lsb_g;           //gsensor参数 1g对应的数值
    short sps;             //gsensor参数 每秒采样次数
} accel_config;


typedef enum {
    ALGO_NOTHING,           //没有相关动作
    ALGO_WRIST_UP,          //抬腕动作
    ALGO_WRIST_DOWN,        //落腕动作
    ALGO_DOUBLE_CLICK,      //双击屏幕
    ALGO_HITTING,           //击球动作
    ALGO_ON_DESK,           //放置桌面
} algo_out;


/* --------------------------------------------------------------------------*/
/**
 * @brief g sensor算法初始化
 *
 * @param [in] select 选择需要开启的算法
 * @param [in] lsb_g  gsensor参数 1g对应的数值
 * @param [in] sps    gsensor参数 每秒采样次数
 *
 */
void  algo_gsensor_init(algo_type open, accel_config accel);

/* --------------------------------------------------------------------------*/
/**
 * @brief g sensor算法运行函数
 *
 * @param [in] data 参考结构体algo_data
 * @param [in] point gsensor 数据点数
 *
 */
algo_out algo_gsensor_run(accel_data *data, short point);

