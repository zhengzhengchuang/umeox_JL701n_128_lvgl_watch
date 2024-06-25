#ifndef WATCH_API_H
#define WATCH_API_H

#if 0
#define norm_code  __attribute__((section(".fall_norm_code")))
#define spare_const  __attribute__((section(".fall_const")))
#define spare_code __attribute__((section(".motion_spare_code")))
#define norm_code  __attribute__((section(".motion_norm_code")))
#define norm_code  __attribute__((section(".ped_norm_code")))
#define spare_code __attribute__((section(".Prep_spare_code")))
#define norm_code  __attribute__((section(".Prep_norm_code")))
#define spare_const  __attribute__((section(".Prep_const")))
#define norm_code  __attribute__((section(".sedentary_norm_code")))
#define norm_code  __attribute__((section(".sleep_norm_code")))
#define norm_code  __attribute__((section(".wrist_norm_code")))
#endif

typedef struct watch_parameter {
    int ped_enable;
    int wrist_enable;
    int sleep_enable;
    int fall_enable;
    int Sedentary_enable;
    int motion_enable;
    int fs;
    int len;
    int type;//贴片位置的类型（0~7）
} WATCH_parameter;


typedef struct ped_parameter {
    int ThreadValue1;
    int ThreadValue2;
    int ThreadValue3;
    int fast_stp;
    int runing;
} PED_parameter;

typedef struct bri_parameter {

    int ACCEL_X;//翻腕到正面的x轴阈值
    int ACCEL_Z;//翻腕到正面的z轴阈值
    int ACCEL_Z_UP2FOR;//手表正面向身体内测翻腕阈值
    int y_value1;//左手y轴抬腕检测阈值
    int y_value2;//右手y轴抬腕检测阈值
    int x_value;//左右手抬腕x轴阈值
    int run_bright;//抬腕亮屏的跑步状态阈值增量
} BRI_parameter;

typedef struct sleep_parameter {
    int sleep_deep;//进入睡眠的最低判断时间（满足连续小于腕动阈值的最短时间判定为睡眠）
    int alpha;//活动量调节因子
    int wrist_value;//腕动阈值(120)
    int activity_value;//活动量阈值

} SLEEP_parameter;

typedef struct fall_detection {
    int smv;
    int thre;
    int comp;
} FALL_detection;

typedef struct Sedentary_reminder {
    int activity_value1;
    int activity_value2;
} Sedentary_r;

typedef struct Personal_Information {
    int height;
    int weight;
    int age;
    int gender;
} Personal_inf;

typedef struct HeartRate_parameter { //人为选择输入参数，如果没有就输入0即可
    int max_HR;//最大心率
    int resting_HR;//静息心率

} HeartRate_par;

int get_watch_buf(int fs, int len);

void init_watch(void *ptr, WATCH_parameter WP, PED_parameter PP, BRI_parameter BP, SLEEP_parameter SP, FALL_detection FD, Personal_inf PI, Sedentary_r SR);
void init_motion(void *ptr);
void init_HR(void *ptr, HeartRate_par HP, int model);
void init_MotionHR(void *ptr, int model);
void run_watch(void *ptr, short *data, int len, int *flag, WATCH_parameter WP, int wear_flag, int HR);
//void run_count_steps(void*ptr);
//void preprocess(void*ptr, short *data, int len);
//int WristBright(void*ptr);
//void SleepDetection(void*ptr);

int getCurrentStep(void *ptr);
void resetSteps(void *ptr);
int getDeepSleepTime(void *ptr);
int getShallowSleepTime(void *ptr);
int getSleepTimeAll(void *ptr);
int getREMSleepTime(void *ptr);
void resetSleepTime(void *ptr);
void Sleep_quality_evaluation(void *ptr);
int get_SleepScore(void *ptr);
short get_scale_deep(void *ptr);
short get_scale_shallow(void *ptr);
short get_scale_REM(void *ptr);
char sleep_time_evaluate(void *ptr);
char scale_deep_evaluate(void *ptr);
char scale_shallow_evaluate(void *ptr);
char scale_REM_evaluate(void *ptr);
char DeepSleep_continuity_score(void *ptr);
char get_wakeup_times(void *ptr);
int getRestingHeartRate(void *ptr);
int getMaxHeartRate(void *ptr);
int getMinHeartRate(void *ptr);
char *get_2HourStatus(void *ptr);
int getMotionStatus(void *ptr);
int getStepDistance(void *ptr);
int getStepKCal(void *ptr);
int getMotion_time(void *ptr);
int getStep_Per_min(void *ptr);
int getSpeed_distribution(void *ptr);
int getStride(void *ptr);
int getStep_frequency(void *ptr);
void resetMotion(void *ptr);
void resetMotionParameter(void *ptr);
void resetSedentaryRemind(void *ptr);
int getIntensity(void *ptr);
int getIntensity1_time(void *ptr);
int getIntensity2_time(void *ptr);
int getIntensity3_time(void *ptr);
int getIntensity4_time(void *ptr);
int getIntensity5_time(void *ptr);
void getMotionSwitch(void *ptr, int flag);
int restore_time(void *ptr, int Interval_time, int last_restore_time);
#endif // !WATCH_API_H



