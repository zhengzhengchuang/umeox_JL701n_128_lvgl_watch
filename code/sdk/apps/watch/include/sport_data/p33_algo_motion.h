#pragma once

#define algo_log(fmt,...)  printf("[algo] %s " fmt "\n",__func__, ##__VA_ARGS__)

#define  SLEEP_MAX_BLOCK          (100)

#pragma pack(1)

typedef union {
    struct {
        short step_counter: 1;   //计步器
        short distance: 1;       //距离
        short step_frequency: 1; //步频
        short calories: 1;       //卡路里 = 基础代谢率（BMR） + 活动代谢率（AMR）
        short calories_amr: 1;   //活动代谢率（AMR）：活动代谢率是指在运动和日常活动中所消耗的能量
        short sedentary: 1;      //久坐提醒
        short activity_type: 1;  //活动类型
        short sleep: 1;          //睡眠
    };
    short all;
} algo_type;

typedef struct {
    short height;          //身高 cm
    short weight;          //体重 kg
    short gender;          //性别 0 man 1 female
    short ages;            //年龄
    short step_factor;     //步长系数 一般40~60
} user_info;


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
    ACTIVITY_UNKNOWN,      //活动：未知
    ACTIVITY_STILL,        //活动：静止
    ACTIVITY_WALK,         //活动：走路
    ACTIVITY_RUN,          //活动：跑步
    ACTIVITY_CYCLING,      //活动：骑行
    ACTIVITY_BASKETBALLL,  //活动：篮球
    ACTIVITY_BADMINTON,    //活动：羽毛球
} activity_type;

typedef enum {
    SLEEP_STAGE_AWAKE,
    SLEEP_STAGE_LIGHT,
    SLEEP_STAGE_DEEP,
    SLEEP_STAGE_REM,
} sleep_type;

typedef struct {
    unsigned int  timestamp;
    unsigned char stage;
} sleep_chart;

typedef struct {
    sleep_chart   chart[SLEEP_MAX_BLOCK];
    unsigned char blocks;
} sleep_data;

typedef struct {
    algo_type update;          //算法更新标志
    int       steps;           //步数
    int       sedentary;       //久坐时间，单位 秒
    int       calories;        //卡路里 = 基础代谢率（BMR） + 活动代谢率（AMR） 单位 卡
    int       calories_amr;    //活动代谢率（AMR） 单位 卡
    int       activity;        //活动类型，参考 activity_type
    int       distance;        //距离, 单位cm
    int       step_frequency;  //步频，单位spm (步/分钟)
    int       sleep;           //睡眠, 参考sleep_type
} algo_out;

#pragma pack()
/* --------------------------------------------------------------------------/
/**
 * @brief 运动算法初始化
 *
 * @param [in] open   选择需要开启的算法
 * @param [in] accel  gsensor配置
 * @param [in] user   用户信息
 *
 */
void algo_motion_init(algo_type open, accel_config accel, user_info user);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动算法运行函数
 *
 * @param [in] *accel       gsensor 3轴数据
 * @param [in] accel_point  gsensor 数据点数
 * @param [in] *rri         R-R间隔
 * @param [in] *rri         R-R间隔点数
 * @param [out]             算法输出，见 algo_out 定义
 *
 */
algo_out algo_motion_run(unsigned int utc, accel_data *accel, short accel_point,  short *rri, short rri_point);
/* --------------------------------------------------------------------------/
/**
 * @brief 运动算法设置佩戴状态
 *
 * @param [in]    wear  0:未佩戴 1：佩戴
 * @note  充电时应设置为“未佩戴”
 *
 */
void algo_motion_wear_set(unsigned char wear);
/* --------------------------------------------------------------------------/
/**
 * @brief 运动算法获取睡眠数据
 *
 * @param [out]             睡眠数据，见 sleep_data 定义
 *
 */
void algo_motion_sleep_get(sleep_data *out);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动算法清除睡眠数据
 */
void algo_motion_sleep_clean(void);

/* --------------------------------------------------------------------------/
/**
 * @brief 运动算法获取睡眠特征数据
 *
 * @param [out] unsigned char*   1440笔睡眠特征数据
 * @note  如果睡眠结果不准确，请保存睡眠特征数据及记录正确的入睡和出睡时间，提供给杰理
 */
unsigned char *algo_motion_sleep_get_feature(void);

