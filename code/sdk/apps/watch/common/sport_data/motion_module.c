
#include "app_config.h"
#include "includes.h"
#include "motion_module.h"
#include "system/timer.h"
#include "sport_info_personal_info.h"
#include "sport_data/watch_time_common.h"
#include "timestamp/timestamp.h"

#define LOG(fmt,...)  printf("[algo] %s " fmt "\n",__func__, ##__VA_ARGS__)

extern void log_save(u8 send, const s8 *format, ...);


static motion_data   data = {0};
static motion_envir  envir = {0};
static u16           buf_rri[5];
static u32           timestamp_steps = 0;

static void get_sys_time(struct sys_time *time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        memset(time, 0, sizeof(*time));
        return;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
    dev_close(fd);
}

void motion_module_init(void)
{
    accel_config   accel;
    algo_type      open_algo;
    user_info      user = {.ages = 28, .gender = 1, .height = 170, .weight = 60, .step_factor = 80};

    personal_information info = {0};
    int  info_valid = sport_personal_info_get(&info);
    u8 age = watch_time_age(info.birth_y, info.birth_m, info.birth_d);
    LOG("gender=%d ages=%d height=%d weight=%d", info.gender, age, info.height, info.weight);

    accel.sps   = 25;
    accel.lsb_g = 1024;

    open_algo.all = 0xffff;
    // open_algo.all = 0;
    // open_algo.step_counter   = 1;
    // open_algo.step_frequency = 1;
    // open_algo.distance       = 1;
    // open_algo.calories       = 1;
    // open_algo.calories_amr   = 1;
    // open_algo.sleep          = 1;

    if (info_valid) {
        user.gender      = info.gender;
        user.ages        = age;
        user.height      = info.height;
        user.weight      = info.weight;
        user.step_factor = 45;  //用于估算步行距离
    }

    /* algo_motion_set_debug(1); */
    algo_motion_init(open_algo, accel, user);

    memset(&data,  0, sizeof(motion_data));
    memset(&envir, 0, sizeof(motion_envir));

#if TCFG_GSENSOR_ALGO_SILAN
    void silan_motion_module_init(void);
    silan_motion_module_init();
#endif

}

void print_accel(accel_data *accel, short point)
{
    for (short i = 0; i < point; i++) {
        algo_log("\t%d\t%d\t%d", accel[i].x, accel[i].y, accel[i].z);
    }
}

static void sleep_data_spp_send(void);
void motion_module_run(short *gsensor, short point)
{
    struct sys_time time;
    get_sys_time(&time);

    uint32_t timestamp = timestamp_mytime_2_utc_sec(&time);
    algo_out algo  = algo_motion_run(timestamp, gsensor, point, buf_rri, envir.rri_point);

    // sleep_data_spp_send(); //采集睡眠特征数据：通过spp发送睡眠特征数据到手机

    if (algo.update.step_counter) {
        data.total_steps += algo.steps;
        timestamp_steps   = timestamp;
        algo_log("steps=%d", data.total_steps);
    }

    if (algo.update.distance) {
        if (!envir.gps_online) {
            data.distance += algo.distance;
        }
    }

    if (algo.update.calories) {
        data.calories += algo.calories;
    }

    if (algo.update.calories_amr) {
        data.calories_amr += algo.calories_amr;
    }

    if (algo.update.step_frequency) {
        data.step_frequency = algo.step_frequency;
    }

    if (algo.update.sedentary) {
        if (timestamp_steps == 0) {
            timestamp_steps   = timestamp;
        }

        if (timestamp > timestamp_steps) {
            data.sedentary = (timestamp - timestamp_steps) / 60;
        }
    }

    /* log_save(1,"%05d step %05d m %04d spm \n%04d cal %04d cal",data.total_steps, data.distance/100, */
    /* data.step_frequency, data.calories/1000, data.calories_amr/1000); */
#if TCFG_GSENSOR_ALGO_SILAN
    void silan_motion_module_handle(short * data, short point);
    silan_motion_module_handle(gsensor, point);
#endif
}

void motion_module_set_wear(u8 wear)
{
    envir.wear_status = wear;
    algo_motion_wear_set(wear);
}

void motion_module_set_hr(u8 hr_value)
{
    if (hr_value == 0) {
        envir.rri_point = 0;
    } else {
        envir.rri_point = 1;
        buf_rri[0] = (1000 * 60) / hr_value;
    }
}

void motion_module_set_rri(u16 *rri, u8 rri_point)
{
    envir.rri_point = rri_point <= 5 ? rri_point : 5;

    if (envir.rri_point > 0) {
        memcpy(buf_rri, rri, sizeof(u16)*envir.rri_point);
    }
}


void motion_module_set_gps_distance(s32 distance)
{
    envir.gps_online = (distance > 0);

    if (envir.gps_online) {
        data.distance += distance;
    }
}

int motion_module_get_distance(void)
{
    return data.distance;
}

void motion_module_clear_distance(void)
{
    data.distance = 0;
}

int motion_module_get_steps(void)
{
    return data.total_steps;
}

void motion_module_clear_steps(void)
{
    data.total_steps = 0;
}


int motion_module_get_step_frequency(void)
{
    return data.step_frequency;
}

int motion_module_get_calories(void)
{
    return data.calories;
}

void motion_module_clear_calories(void)
{
    data.calories = 0;
}

int motion_module_get_sedentary(void)
{
    return data.sedentary;
}

void motion_module_clear_sedentary(void)
{
    data.sedentary = 0;
}

void motion_module_get_sleep_demo_data(sleep_data *sleep)
{
    ASSERT(sleep != NULL);

    struct sys_time time;
    get_sys_time(&time);

    u32 timestamp   = timestamp_mytime_2_utc_sec(&time);
    u32 today_start = timestamp - (timestamp % (24 * 60 * 60));

    u8  blocks = 10;
    sleep->chart[0].stage          = 0;
    sleep->chart[0].timestamp      = today_start - (30 * 60);

    for (u8 i = 1; i < blocks; i++) {
        sleep->chart[i].stage      = 2 - i % 2;
        sleep->chart[i].timestamp  = today_start + i * (30 * 60);
    }

    sleep->chart[blocks].stage     = 0;
    sleep->chart[blocks].timestamp = today_start + blocks * (30 * 60);
    sleep->blocks = blocks + 1;
}

void motion_module_get_sleep(sleep_data *sleep)
{
    ASSERT(sleep != NULL);
//    motion_module_get_sleep_demo_data(sleep);
    algo_motion_sleep_get(sleep);
}

void motion_module_clean_sleep(void)
{
    algo_motion_sleep_clean();
}
