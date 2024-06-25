#pragma once
#include "p33_algo_motion.h"

typedef struct {
    int total_steps;
    int distance;
    int calories;        //卡路里 = 基础代谢率（BMR） + 活动代谢率（AMR）
    int calories_amr;    //活动代谢率（AMR）：活动代谢率是指在运动和日常活动中所消耗的能量
    int sedentary;
    int step_frequency;
} motion_data;

typedef struct {
    u8  wear_status;
    u8  hr_value;
    u8  rri_point;
    u8  gps_online;
} motion_envir;

/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 初始化
 */
void motion_module_init(void);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 运行
 *
 * @param [in] data   accelerate 数据
 * @param [in] point  accelerate 数据点数，一个点包含xyz三个轴
 */
void motion_module_run(short *data, short point);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 设置佩戴状态
 *
 * @param [in] wear  1：佩戴 0：未佩戴
 * @note  充电时应设置为“未佩戴”
 */
void motion_module_set_wear(u8 wear);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 设置心率值
 *
 * @param [in] hr_value  心率值
 * @note  心率监测结束时需要设置为0
 * @note  心率模块没有输出rri则调用该接口，否则调用 motion_module_set_rri 接口
 */
void motion_module_set_hr(u8 hr_value);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 设置 R-R interval
 *
 * @param [in] rri        R-R interval
 * @param [in] rri_point  R-R interval 数据笔数
 * @note  心率监测结束时需要设置rri_point为0
 * @note  心率模块有输出rri则调用该接口，否则调用 motion_module_set_hr 接口
 */
void motion_module_set_rri(u16 *rri, u8 rri_point);
/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 设置gps距离
 *
 * @param [in] distance  距离，单位cm ，应每秒更新，gps断开时设置为 -1
 */
void motion_module_set_gps_distance(s32 distance);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 获取运动距离
 *
 * @param [out] distance  距离，单位cm
 */
int motion_module_get_distance(void);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 清除运动距离
 *
 */
void motion_module_clear_distance(void);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 获取步数
 * @param [out] 步数
 */
int motion_module_get_steps(void);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 清除步数
 *
 */
void motion_module_clear_steps(void);


/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 获取步频
 * @param [out] 步/分钟
 */
int motion_module_get_step_frequency(void);


/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 获取卡路里
 * @param [out] 卡
 */
int motion_module_get_calories(void);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 清除卡路里
 *
 */
void motion_module_clear_calories(void);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 获取久坐时间
 * @param [out] 分钟
 */
int motion_module_get_sedentary(void);


/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 清除久坐时间
 *
 */
void motion_module_clear_sedentary(void);


/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 获取睡眠数据
 *
 */
void motion_module_get_sleep(sleep_data *sleep);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动模块 清除睡眠数据
 *
 */
void motion_module_clean_sleep(void);
