#include "system/includes.h"
#include "app_config.h"
#include "sport_data/watch_common.h"
#include "sport_data/watch_time_common.h"
#include "sport_data/sport.h"
#include "gSensor/SC7A20.h"
#include "gSensor/gSensor_manage.h"
#include "math.h"
#include "ui/ui.h"
#include "ui/ui_style.h"
#include "dev_manager.h"
#include "message_vm_cfg.h"
#include "ui_vm/ui_vm.h"
#include "sport_data/p33_algo_motion.h"
#include "motion_module.h"
#include "timestamp/timestamp.h"

struct watch_run_info {
    int run_status;//0 无记录 1进行 2暂停 3继续 4结束
    struct motion_info start_info;
    struct motion_info result_info;
    struct motion_info tmp_info;
    struct sys_time timestart;
    struct sys_time timestop;
} run_info ;

struct watch_execise *execise_hd = NULL;
static struct motion_info __execise_info;
static u8 sport_mode;//0非运动 1户外跑 2室内跑
int sport_info_init_status = 0;

extern void sport_start_algo_deal(void);
extern void sport_end_algo_deal(void);

/**************************************************

***************************************************/
int  set_sport_mode(u8 mode)
{
    printf("%s,%d", __func__, mode);
    if (mode >= 0) {
        sport_mode = mode;
    } else {
        sport_mode = 0;
    }
    return 0;
}
u8 execise_mode_get(void)
{
    u8 mode = sport_mode;
    if (mode > 2) { //类型上限，室外跑、室内跑步以外的其他运动用室内跑步代替
        mode = 2;
    }
    printf("%s,%d", __func__, mode);
    return mode;
}

u8 execise_ctrl_status_get(void)
{
    printf("%s %d", __func__, run_info.run_status);
    return run_info.run_status;
}
int execise_ctrl_status_clr(void)
{
    printf("%s %d", __func__, run_info.run_status);
    if (run_info.run_status == 4) {
        run_info.run_status = 0;
        /* set_sport_mode(0); */
        return SUCCESS;
    } else {
        return EXECISE_STATUS_NOT_STOP;
    }
}
/**************************************************
                  运动控制接口
		用于记录运动开始和结束的时间\步数
***************************************************/
static u8 get_sport_info(struct motion_info *info)
{
    if (execise_hd == NULL) {
        execise_hd = zalloc(sizeof(struct watch_execise));
        watch_execise_handle_get(execise_hd);
    }
    return execise_hd->daily_motion_info_get(info);
}

int sport_run_start(void)
{
    printf("%s", __func__);
    if (run_info.run_status == 1) {
        log_e("run_record_now\n");
        return 1;
    }
    run_info.run_status = 0;
    watch_file_get_sys_time(&run_info.timestart);
    memset(&run_info.start_info, 0, sizeof(struct motion_info));
    memset(&run_info.tmp_info, 0, sizeof(struct motion_info));
    memset(&run_info.result_info, 0, sizeof(struct motion_info));
    memset(&__execise_info, 0, sizeof(struct motion_info));
    get_sport_info(&run_info.start_info);
    algo_deal_sport_start();
    run_info.run_status = 1;
    return run_info.run_status;
}
int sport_run_pause(void)
{
    printf("%s", __func__);
    if ((run_info.run_status == 0) | (run_info.run_status == 2)) {
        log_e("run_record_not_start\n");
        return 0;
    }
    get_sport_info(&run_info.tmp_info);
    run_info.result_info.step += (run_info.tmp_info.step - run_info.start_info.step);
    run_info.result_info.step_distance += (run_info.tmp_info.step_distance - run_info.start_info.step_distance);
    run_info.result_info.caloric += (run_info.tmp_info.caloric - run_info.start_info.caloric);
    run_info.result_info.motion_time += (run_info.tmp_info.motion_time - run_info.start_info.motion_time);
    if (run_info.result_info.step_distance != 0) {
        run_info.result_info.motion_pace = (run_info.result_info.motion_time * 1000 / run_info.result_info.step_distance) ; //平均配速 单位秒/公里
    } else {
        run_info.result_info.motion_pace = 0;
    }
    run_info.run_status = 2;
    return run_info.run_status;
}
int sport_run_continue(void)
{
    printf("%s", __func__);
    if (run_info.run_status == 1) {
        log_e("run_record_now\n");
        return 1;
    }
    memset(&run_info.start_info, 0, sizeof(struct motion_info));
    memset(&run_info.tmp_info, 0, sizeof(struct motion_info));
    get_sport_info(&run_info.start_info);
    run_info.run_status = 3;
    return run_info.run_status;
}
int sport_run_stop(void)
{
    printf("%s", __func__);
    if (run_info.run_status == 0) {
        log_e("run_record_not_start\n");
        return 0;
    }
    if (run_info.run_status == 1) {
        watch_file_get_sys_time(&run_info.timestop);
        run_info.run_status = 4;
        return run_info.run_status;
    }
    algo_deal_sport_end();//关闭运动算法,关闭连续心率，刷新一下运动恢复时间和运动强度区间时间
    get_sport_info(&run_info.tmp_info);
    /* run_info.result_info.step += (run_info.tmp_info.step - run_info.start_info.step); */
    /* run_info.result_info.step_distance += (run_info.tmp_info.step_distance - run_info.start_info.step_distance); */
    /* run_info.result_info.caloric += (run_info.tmp_info.caloric - run_info.start_info.caloric); */
    /* run_info.result_info.motion_time += (run_info.tmp_info.motion_time - run_info.start_info.motion_time); */
    /* if (run_info.result_info.step_distance != 0) { */
    /* run_info.result_info.motion_pace = (run_info.result_info.motion_time * 1000 / run_info.result_info.step_distance) ; //平均配速 单位秒/公里 */
    /* } else { */
    /* run_info.result_info.motion_pace = 0; */
    /* } */
    run_info.result_info.recoverytime_hour = run_info.tmp_info.recoverytime_hour;
    run_info.result_info.recoverytime_min = run_info.tmp_info.recoverytime_min;
    run_info.result_info.intensity = run_info.tmp_info.intensity;
    run_info.result_info.intensity_time[0] = run_info.tmp_info.intensity_time[0];
    run_info.result_info.intensity_time[1] = run_info.tmp_info.intensity_time[1];
    run_info.result_info.intensity_time[2] = run_info.tmp_info.intensity_time[2];
    run_info.result_info.intensity_time[3] = run_info.tmp_info.intensity_time[3];
    run_info.result_info.intensity_time[4] = run_info.tmp_info.intensity_time[45];

    watch_file_get_sys_time(&run_info.timestop);

    run_info.run_status = 4;
    return run_info.run_status;
}

int execise_info_get(struct motion_info *info)
{

    if (run_info.run_status == 0) { //无运动记录
        /* printf("%s %d", __func__, __LINE__); */
        memset(info, 0, sizeof(struct motion_info));
        return NOT_SPORT ;
    } else if (run_info.run_status == 1) { //开始，无暂停记录
        /* printf("%s %d", __func__, __LINE__); */
        get_sport_info(&__execise_info);//增量
        __execise_info.step 			-=	run_info.start_info.step;
        __execise_info.step_distance	-=	run_info.start_info.step_distance;
        __execise_info.caloric			-= 	run_info.start_info.caloric;
        __execise_info.motion_time		-=  run_info.start_info.motion_time;
        memcpy(info, &__execise_info, sizeof(struct motion_info));
    } else if (run_info.run_status == 2) { //暂停
        /* printf("%s %d", __func__, __LINE__); */
        memcpy(info, &run_info.result_info, sizeof(struct motion_info));
    } else if (run_info.run_status == 3) { //继续，暂停过
        /* printf("%s %d", __func__, __LINE__); */
        get_sport_info(&__execise_info);//已记录值+未记录值
        __execise_info.step				+= (run_info.result_info.step	-	run_info.start_info.step);
        __execise_info.step_distance	+= (run_info.result_info.step_distance	-	run_info.start_info.step_distance);
        __execise_info.caloric			+= (run_info.result_info.caloric		-	run_info.start_info.caloric);
        __execise_info.motion_time		+= (run_info.result_info.motion_time	-	run_info.start_info.motion_time);
        memcpy(info, &__execise_info, sizeof(struct motion_info));
    } else if (run_info.run_status == 4) { //已结束
        /* printf("%s %d", __func__, __LINE__); */
        memcpy(info, &run_info.result_info, sizeof(struct motion_info));
    }
    return SUCCESS;
}
void execise_info_clr(void)
{
    printf("%s %d", __func__, __LINE__);
    memset(&__execise_info, 0, sizeof(struct motion_info));
}

int execise_ctrl_status_set(unsigned char sport_mode, unsigned char status)
{
    printf("sport_mode=%dstatus= %d", sport_mode, status);
    if (sport_mode != 0xff) {
        set_sport_mode(sport_mode);
    }
    switch (status) {
    case SPORT_STATUS_START:
        sport_run_start();
        break;
    case SPORT_STATUS_PAUSE:
        sport_run_pause();
        break;
    case SPORT_STATUS_CONTINNUE:
        sport_run_continue();
        break;
    case SPORT_STATUS_STOP:
        sport_run_stop();
        break;
    }
    return SUCCESS;
}

int get_sport_strength_time(void)
{
    static u8 strength = 0;
    strength ++ ;
    if (strength > 30) {
        strength = 0;
    }
    return strength;
}

int get_sport_active_time(void)
{
    static u8 time = 0;
    time ++ ;
    if (time > 12) {
        time = 0;
    }
    return time;
}


static const u8 time_table[12] = {1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1};
int get_sport_active_time_table()//活动时间表
{
    int table = 0;
    for (int i = 0; i < sizeof(time_table); i++) {
        if (time_table[i]) {
            table |= BIT(i);
        }
    }
    return table;
}


u32 get_sport_start_time(struct sys_time *t)
{

    u32 time;
    time = ((run_info.timestart.sec	& 0x3F) | \
            ((run_info.timestart.min	& 0x3F) << 6) | \
            ((run_info.timestart.hour	& 0x1F) << 12) | \
            ((run_info.timestart.day	& 0x1F) << 17) | \
            ((run_info.timestart.month	& 0x0F) << 22) | \
            (((run_info.timestart.year - 2010)	& 0x3F) << 26));
    printf("%s=%l", __func__, time);
    if (t) {
        memcpy(t, &run_info.timestart, sizeof(struct sys_time));
    }
    return time;
}

static u16 pace_sec = 0;
int get_sport_pace_sec(void) //每公里调用一次
{
    struct motion_info info;
    execise_info_get(&info);
    u16 pace_now_sec = info.motion_time;
    u16 pace_prev_sec = pace_sec;
    pace_sec = pace_now_sec; //计入当前时长
    printf("pace_sec=%d", pace_now_sec - pace_prev_sec);
    return pace_now_sec - pace_prev_sec; //当前一公里的时长
}

u32 get_sport_end_time(struct sys_time *t)
{

    u32 time;
    time = ((run_info.timestop.sec	& 0x3F) | \
            ((run_info.timestop.min	& 0x3F) << 6) | \
            ((run_info.timestop.hour	& 0x1F) << 12) | \
            ((run_info.timestop.day	& 0x1F) << 17) | \
            ((run_info.timestop.month & 0x0F) << 22) | \
            (((run_info.timestop.year - 2010)	& 0x3F) << 26));
    printf("%s=%l", __func__, time);
    if (t) {
        memcpy(t, &run_info.timestop, sizeof(struct sys_time));
    }
    return time;
}



//******************************
//全天步数、距离、热量记录
//******************************
#define LOG_TAG             "[DAYLY_SPORT]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_CLI_ENABLE
#include "debug.h"
/**********************************************************
  					睡眠管理
**********************************************************/
#define FILE_RECORD_BY_JL_EN 		(1)
#define DATA_SOUCRE_BY_USR_EN		(0)
#define FILE_RECORD_TEST     		(0)

#define FILE_TYPE 					F_TYPE_STEP
#define MAX_FILE_SIZE 				(496)
#define ST_TIME_INTERVAL 60
struct daily_ctrl __daily_ctrl;
static int motion_module_get_daily_active_data(struct daily_active_data *__data)
{
    if (!__data) {
        return -1;
    }

    __data->step = motion_module_get_steps();
    __data->distance = motion_module_get_distance();
    __data->kcal = motion_module_get_calories();
    return 0;
};
static int motion_module_clean_daily_active_data()
{
    motion_module_clear_distance();
    motion_module_clear_steps();
    motion_module_clear_calories();
    clr_sport_info();
    return 0;
}
static int daily_active_file_write(void)
{
    struct health_file_info daily_record_file = {0};
    struct health_file_info *__info = (&daily_record_file);

    struct daily_active_data daily_data = {0};
    struct daily_active_data last_data = {0};
    struct sys_time cur_time;
    watch_file_get_sys_time(&cur_time);

    memset(__info, 0, sizeof(struct health_file_info));
    __info->file_type = FILE_TYPE;
    __info->max_file_size = MAX_FILE_SIZE;

    void *file_hd = get_flash_vm_hd(__info->file_type);
    if (!file_hd) {
        log_error("open file_hd err\n");
        return -1;
    }

    if (health_file_open_file_get_index(__info) < 0) {
        log_error("open file err\n");
        return -2;
    }
    motion_module_get_daily_active_data(&daily_data);
    if (!__info->cur_file_id) {
        int read_len = sizeof(HEALTH_FILE_HEAD);
        int read_offset = read_len + sizeof(struct health_file_data_head);
        int blocks_len = sizeof(struct health_file_data_head) + sizeof(struct daily_active_block);
        struct daily_active_block single_block;
        while (read_len != __info->cur_file_size) {
            flash_common_read_by_index(file_hd,
                                       __info->cur_file_index,
                                       read_offset,
                                       sizeof(struct daily_active_block),
                                       &single_block);
            read_offset += blocks_len;
            read_len += blocks_len;
            last_data.step += single_block.step;
            last_data.distance += single_block.distance_ten;
            last_data.kcal += single_block.kcal;
        }
        if (health_file_open_new_file(__info) < 0) {
            log_error("open file id err\n");
            return -3;
        }
    } else {
        __info->cur_file_head.type = __info->file_type;
        __info->cur_file_head.year = cur_time.year;
        __info->cur_file_head.month = cur_time.month;
        __info->cur_file_head.day = cur_time.day;
        __info->cur_file_head.crc = 0xffff;
        __info->cur_file_head.version = 0;
        __info->cur_file_head.interval = ST_TIME_INTERVAL;
        __info->cur_file_head.save = 0x0000;
        watch_common_swapX(&__info->cur_file_head.year, &__info->cur_file_head.year, 2);
        flash_common_write_packet(file_hd, __info->cur_file_id, sizeof(__info->cur_file_head), &__info->cur_file_head);
    }
    struct health_file_data_head data_head;
    data_head.hour = cur_time.hour;
    data_head.min = cur_time.min;
    data_head.len = sizeof(struct daily_active_block);
    watch_common_swapX(&data_head.len, &data_head.len, 2);
    struct daily_active_block block;
    block.step = daily_data.step - last_data.step;
    block.distance_ten = daily_data.distance - last_data.distance;
    block.kcal = daily_data.kcal - last_data.kcal;
    flash_common_write_packet(file_hd, __info->cur_file_id, sizeof(struct health_file_data_head), &data_head);
    flash_common_write_packet(file_hd, __info->cur_file_id, sizeof(struct daily_active_block), &block);
    flash_common_write_push(file_hd, __info->cur_file_id);
    int rlen = flash_common_get_file_size_by_id(file_hd, __info->cur_file_id) - sizeof(HEALTH_FILE_HEAD);
    u8 *rbuf = zalloc(rlen + 1);
    if (rbuf) {
        flash_common_read_by_id(file_hd, __info->cur_file_id, sizeof(HEALTH_FILE_HEAD), rlen, rbuf);
        u16 crc_value = CRC16(rbuf, rlen);
        watch_common_swapX(&crc_value, &crc_value, 2);
        free(rbuf);
        rbuf = NULL;
        flash_common_update_by_id(file_hd, __info->cur_file_id, 5, 2, &crc_value);
    }
#if 0
    rlen = flash_common_get_file_size_by_id(file_hd, __info->cur_file_id);
    rbuf = zalloc(rlen + 1);
    flash_common_read_by_id(file_hd, __info->cur_file_id, 0, rlen, rbuf);
    put_buf(rbuf, rlen);
    free(rbuf);
#endif // 1
    return 0;
}

static void daily_active_file_update(void *priv)
{
    struct sys_time cur_time;
    watch_file_get_sys_time(&cur_time);
    if (cur_time.hour == 0) {
        motion_module_clean_daily_active_data();
    }
    daily_active_file_write();
    if (__daily_ctrl.first_flag) {
        __daily_ctrl.first_flag = 0;
        if (__daily_ctrl.timer_id) {
            sys_timer_del(__daily_ctrl.timer_id);
            __daily_ctrl.timer_id = sys_timer_add(NULL, daily_active_file_update, ST_TIME_INTERVAL * 60 * 1000);
        }
    }
}

int	daily_active_record_start(void)
{
    struct sys_time cur_time;
    watch_file_get_sys_time(&cur_time);
    int first_interval = 3600 - cur_time.min * 60 + cur_time.sec;
    if (!__daily_ctrl.timer_id) {
        __daily_ctrl.first_flag = 1;
        __daily_ctrl.timer_id = sys_timer_add(NULL, daily_active_file_update, first_interval * 1000);
    }

    return 0;
}
int daily_active_record_stop(void)
{
    if (__daily_ctrl.timer_id) {
        sys_timer_del(__daily_ctrl.timer_id);
        __daily_ctrl.timer_id = 0;
    }
    return 0;
}
void daily_active_record_test(void)
{
    daily_active_file_write();
}


