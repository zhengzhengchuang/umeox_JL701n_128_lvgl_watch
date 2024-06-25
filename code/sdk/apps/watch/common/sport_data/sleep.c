
#include "system/includes.h"
#include "app_config.h"
#include "sport_data/watch_common.h"
#include "sport_data/sleep.h"
#include "sport_data/watch_time_common.h"
#include "ui_vm/ui_vm.h"
#include "message_vm_cfg.h"
#include "sport_data/p33_algo_motion.h"
#include "motion_module.h"
#include "timestamp/timestamp.h"

#define LOG_TAG             "[SLEEP]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_CLI_ENABLE
#include "debug.h"
/**********************************************************
  					睡眠管理
**********************************************************/
#define SLEEP_FILE_RECORD_BY_JL_EN 		(1)
#define SLEEP_DATA_SOUCRE_BY_USR_EN		(0)
#define SLEEP_FILE_RECORD_TEST     		(0)
#define SLEEP_FILE_ANALYSIS_EN			(1)

#define SLEEP_FILE_RECORD_HOUR			(18)

#define FILE_TYPE 						F_TYPE_SLEEP
#define MAX_FILE_SIZE 					(496)
#define SLEEP_MAX_BLOCK					(100)

#ifndef min
#define min(a,b) ((a)<(b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a)>(b) ? (a) : (b))
#endif
/************************************************
*用于将第三方睡眠算法数据转换成杰理睡眠数据
************************************************/
void motion_module_sleep_data_get_test(sleep_data *sleep)
{
#if SLEEP_FILE_RECORD_TEST
    if (!sleep) {
        return;
    }
    sleep_data data;
    data.blocks = 10;
    struct sys_time time;
    watch_file_get_sys_time(&time);
    watch_file_prev_day(&time, &time);
    time.hour = 23;
    time.min = 10;
    for (int i = 0; i < data.blocks; i++) {
        data.chart[i].timestamp = timestamp_mytime_2_utc_sec(&time);
        data.chart[i].stage = i % 3 + 1;

        int len = 10 + i * 10;
        len %= 60;
        log_info("%d %d %d -%d:%d", time.year, time.month, time.day, time.hour, time.min);
        log_info("%s %d %x %d %d", __func__, i, data.chart[i].timestamp, data.chart[i].stage, len);
        watch_time_add_min(&time, len);
    }
    data.chart[data.blocks].timestamp = timestamp_mytime_2_utc_sec(&time);
    data.chart[data.blocks].stage = 0;
    data.blocks++;

    memcpy(sleep, data, sizeof(sleep_data));
#endif
    if (sleep) {
        memset(sleep, 0, sizeof(sleep_data));
    }
    return ;
}
/************************************************
*用于将第三方睡眠算法数据转换成杰理睡眠数据
************************************************/
void motion_module_sleep_data_get_usr(sleep_data *sleep)
{
    if (sleep) {
        memset(sleep, 0, sizeof(sleep_data));
    }

}
/************************************************
			睡眠文件记录
************************************************/

void motion_module_sleep_data_get(sleep_data *sleep)
{
#if SLEEP_FILE_RECORD_TEST
    motion_module_sleep_data_get_test(sleep);
#elif SLEEP_DATA_SOUCRE_BY_USR_EN
    motion_module_sleep_data_get_usr(sleep);
#else
    motion_module_get_sleep(sleep);
#endif
}
/************************************************
			睡眠文件记录分析
************************************************/
int motion_module_sleep_file_analysis(struct sleep_data_analysis *__analysis, sleep_data *__sleep)
{
    if (!(__analysis && __sleep)) {
        log_error("%s param err");
        return -1;
    }
    int i = __sleep->chart[0].stage == SLEEP_STAGE_AWAKE ? 1 : 0;
    int single_time_msec = 0;
    u32 sleep_msec_awake = 0;
    u32 sleep_msec_shallow = 0;
    u32 sleep_msec_deep = 0;
    u32 sleep_msec_deep_max = 0;
    u32 sleep_msec_rem = 0;
    u32 sleep_msec_continue = 0;
    u32 sleep_msec_sleep = 0;
    u32 sleep_msec_all = 0;
    u32 awake_times = 0;
    for (; i < __sleep->blocks - 1; i++) {
        single_time_msec = __sleep->chart[i + 1].timestamp - __sleep->chart[i].timestamp;
        switch (__sleep->chart[i + 1].stage) {
        case SLEEP_STAGE_AWAKE:
            sleep_msec_awake += single_time_msec;
            awake_times ++;
            break;
        case SLEEP_STAGE_LIGHT:
            sleep_msec_shallow += single_time_msec;
            break;
        case SLEEP_STAGE_DEEP:
            sleep_msec_deep += single_time_msec;
            sleep_msec_deep_max = max(sleep_msec_deep_max, single_time_msec);
            break;
        case SLEEP_STAGE_REM:
            sleep_msec_rem += single_time_msec;
            break;
        }
        sleep_msec_continue = max(sleep_msec_continue, single_time_msec);
    }
    sleep_msec_sleep = sleep_msec_rem + sleep_msec_deep + sleep_msec_shallow;
    sleep_msec_all = sleep_msec_sleep + sleep_msec_awake;
    log_info("msec:%d %d %d %d,all:%d sleep:%d deepmax:%d contin:%d\n", \
             sleep_msec_awake, sleep_msec_shallow, sleep_msec_deep, sleep_msec_rem, sleep_msec_all, sleep_msec_sleep, sleep_msec_deep_max, sleep_msec_continue);

    float sleep_val_sleep = 1.0f;
    float sleep_val_shallow = 1.0f;
    float sleep_val_deep = 1.1f;
    float sleep_val_rem = 0.0f;
    float sleep_val_continue = 5.0f;

    float sleep_percent_sleep = 0.62f;
    float sleep_percent_shallow = 0.13f;
    float sleep_percent_deep = 0.13f;
    float sleep_percent_rem = 0.13f;
    float sleep_percent_continue = 0.1f;

    float sleep_continue_sleep = 0.13f;
    float sleep_continue_shallow = 0.13f;
    float sleep_continue_deep = 0.13f;
    float sleep_continue_rem = 0.13f;
    float sleep_continue_continue = 0.62f;

    float socre_percent_sleep = (float)sleep_val_sleep * sleep_msec_sleep / sleep_msec_all;
    socre_percent_sleep = min(socre_percent_sleep, 1.0f);
    float socre_percent_shallow = (float)sleep_val_sleep * sleep_msec_awake / sleep_msec_all;
    socre_percent_shallow = min(socre_percent_shallow, 1.0f);
    float socre_percent_deep = (float)sleep_val_sleep * sleep_msec_deep / sleep_msec_all;
    socre_percent_deep = min(socre_percent_deep, 1.0f);
    float socre_percent_rem = (float)sleep_val_sleep * sleep_msec_rem / sleep_msec_all;
    socre_percent_rem = min(socre_percent_rem, 1.0f);
    float socre_percent_continue = (float)sleep_percent_continue * sleep_msec_continue / sleep_msec_all;
    socre_percent_continue = min(socre_percent_continue, 1.0f);

    u32 sleep_score = 100 * \
                      (sleep_percent_sleep * socre_percent_sleep + \
                       sleep_percent_shallow * socre_percent_shallow + \
                       socre_percent_deep * sleep_percent_deep + \
                       sleep_percent_rem * socre_percent_rem + \
                       sleep_percent_continue * socre_percent_continue);
    sleep_score =  min(100, sleep_score);
    u32 sleep_continue_score = 100 * \
                               (sleep_continue_sleep * socre_percent_sleep + \
                                sleep_continue_shallow * socre_percent_shallow + \
                                sleep_continue_deep * sleep_percent_deep + \
                                sleep_continue_rem * socre_percent_rem + \
                                sleep_continue_continue * socre_percent_continue);
    sleep_continue_score =  min(100, sleep_continue_score);
    u8 sleep_evaluate = SLEEP_EVA_NULL;
    sleep_evaluate = sleep_msec_sleep > 2 * 60 * 60 * 1000 ? SLEEP_EVA_LOW : sleep_evaluate;
    sleep_evaluate = sleep_msec_sleep > 6 * 60 * 60 * 1000 ? SLEEP_EVA_NORMAL : sleep_evaluate;
    sleep_evaluate = sleep_msec_sleep > 10 * 60 * 60 * 1000 ? SLEEP_EVA_HIGH : sleep_evaluate;
    u8 deep_evalute = SLEEP_EVA_NULL;
    deep_evalute = (float)sleep_msec_deep / sleep_msec_all > 0.1f ? SLEEP_EVA_LOW : deep_evalute;
    deep_evalute = (float)sleep_msec_deep / sleep_msec_all > 0.2f ? SLEEP_EVA_NORMAL : deep_evalute;
    deep_evalute = (float)sleep_msec_deep / sleep_msec_all > 0.4f ? SLEEP_EVA_HIGH : deep_evalute;
    u8 shallow_evalute = SLEEP_EVA_NULL;
    shallow_evalute = (float)sleep_msec_sleep / sleep_msec_all > 0.2f ? SLEEP_EVA_LOW : shallow_evalute;
    shallow_evalute = (float)sleep_msec_sleep / sleep_msec_all > 0.4f ? SLEEP_EVA_NORMAL : shallow_evalute;
    shallow_evalute = (float)sleep_msec_sleep / sleep_msec_all > 0.7f ? SLEEP_EVA_HIGH : shallow_evalute;
    u8 rem_evalute = SLEEP_EVA_NULL;
    rem_evalute = (float)sleep_msec_rem / sleep_msec_all > 0.05f ? SLEEP_EVA_LOW : rem_evalute;
    rem_evalute = (float)sleep_msec_rem / sleep_msec_all > 0.1f ? SLEEP_EVA_NORMAL : rem_evalute;
    rem_evalute = (float)sleep_msec_rem / sleep_msec_all > 0.3f ? SLEEP_EVA_HIGH : rem_evalute;

    log_info("socre:%0.2f %0.2f eva:%d %d %d %d", \
             sleep_score, sleep_continue_score, sleep_evaluate, deep_evalute, shallow_evalute, rem_evalute);
    return 0;


}
/************************************************
			睡眠文件记录
************************************************/
#if TCFG_NOR_VM && SLEEP_FILE_RECORD_BY_JL_EN
int motion_module_sleep_file_write(void)
{
    //get sleep data
//    sleep_data *__sleep = motion_module_sleep_data_get();
    sleep_data *__sleep = zalloc(sizeof(sleep_data));
    if (!__sleep) {
        log_error("zalloc error \n");
        return -1;
    }
    motion_module_sleep_data_get(__sleep);
    if (!__sleep || !__sleep->blocks) {
        log_error("no sleep data need record");
        return -1;
    }
    if (__sleep && (__sleep->blocks == 1) && (__sleep->chart[0].stage == SLEEP_STAGE_AWAKE)) {
        log_error("sleep data is awake");
        return -2;
    }
    //get record file
    struct health_file_info sleep_record_file = {0};
    struct health_file_info *__info = &sleep_record_file;
    memset(__info, 0, sizeof(struct health_file_info));
    __info->file_type  = FILE_TYPE;
    __info->max_file_size = MAX_FILE_SIZE;
    void *file_hd = get_flash_vm_hd(__info->file_type);
    if (!file_hd) {
        log_error("open file_hd err\n");
        return -3;
    }
    if (health_file_open_file_get_index(__info) < 0) {
        log_error("open file err\n");
        return -4;
    }
    //write file head
    if (__info->cur_file_id) { //新文件
        struct sys_time cur_time;
        watch_file_get_sys_time(&cur_time);
        __info->cur_file_head.type = __info->file_type;
        __info->cur_file_head.year = cur_time.year;
        __info->cur_file_head.month = cur_time.month;
        __info->cur_file_head.day = cur_time.day;
        __info->cur_file_head.crc = BIT(0);
        __info->cur_file_head.crc = ~__info->cur_file_head.crc;
        __info->cur_file_head.version = 0;
        __info->cur_file_head.interval = 0xff;
        __info->cur_file_head.save = 0;
        watch_common_swapX(&__info->cur_file_head.year, &__info->cur_file_head.year, 2);
        flash_common_write_packet(file_hd, __info->cur_file_id, sizeof(__info->cur_file_head), &__info->cur_file_head);
    } else { //旧文件,采用追加的方式，会额外占用存储空间，如要压缩文件大小，可以考虑使用删除重新创建的方式
        __info->cur_file_head.crc = ~ __info->cur_file_head.crc;
        __info->cur_file_head.crc = __info->cur_file_head.crc << 1;
        __info->cur_file_head.crc |= BIT(0);
        __info->cur_file_head.crc = ~ __info->cur_file_head.crc;
        if (!health_file_index_to_id(__info)) {
            log_error("file_id err");
            return -5;
        }
        flash_common_update_by_id(file_hd, __info->cur_file_id, 5, sizeof(__info->cur_file_head.crc), &__info->cur_file_head.crc);
    }
    //正常情况下 第一段和最后一段的睡眠状态是清醒
    int sleep_block_start = (__sleep->chart[0].stage == SLEEP_STAGE_AWAKE) ? 1 : 0;
    //write data head
    struct sleep_data_head sleep_head;
    struct sys_time tmp_time, last_time;
    timestamp_utc_sec_2_mytime(__sleep->chart[sleep_block_start].timestamp, &tmp_time);
    sleep_head.hour  = tmp_time.hour;
    sleep_head.min  = tmp_time.min;
    sleep_head.len = 0xffff;//(__sleep->blocks - 1 - sleep_block_start) * sizeof(struct sleep_record_block);
    log_info("hour:%d min:%d len:%d", sleep_head.hour, sleep_head.min, sleep_head.len);
    watch_common_swapX(&sleep_head.len, &sleep_head.len, sizeof(sleep_head.len));
    flash_common_write_packet(file_hd, __info->cur_file_id, sizeof(struct sleep_data_head), &sleep_head);
    //write data block
    struct sleep_record_block sleep_block;
    int write_blocks = __sleep->blocks - 2;//第一包和最后一包数据是清醒，不记录
    memset(&last_time, 0, sizeof(struct sys_time));
    for (int i = sleep_block_start; i < __sleep->blocks - 1; i++) {
        timestamp_utc_sec_2_mytime(__sleep->chart[i].timestamp, &tmp_time);
        log_info("%s i:%d timestamp:%x %d %d %d-%d:%d", __func__, i, __sleep->chart[i].timestamp, tmp_time.year, tmp_time.month, tmp_time.day, tmp_time.hour, tmp_time.min);
        if (tmp_time.hour * 60 + tmp_time.min < last_time.hour * 60 + last_time.min) { //按协议 同一在跨天时需要进行分段
            write_blocks++;
            sleep_block.sleep_min = 24 * 60 - (last_time.hour * 60 + last_time.min);
            sleep_block.sleep_type = (__sleep->chart[i].stage == SLEEP_STAGE_AWAKE) ? 0xff : __sleep->chart[i].stage;
            log_info("1stage:%d min:%d", sleep_block.sleep_type, sleep_block.sleep_min);
            flash_common_write_packet(file_hd, __info->cur_file_id, sizeof(struct sleep_record_block), & sleep_block);
            sleep_block.sleep_min = tmp_time.hour * 60 + tmp_time.min;
            sleep_block.sleep_type = (__sleep->chart[i].stage == SLEEP_STAGE_AWAKE) ? 0xff : __sleep->chart[i].stage;
            log_info("2stage:%d min:%d", sleep_block.sleep_type, sleep_block.sleep_min);
            flash_common_write_packet(file_hd, __info->cur_file_id, sizeof(struct sleep_record_block), & sleep_block);
        } else { //正常写入
            sleep_block.sleep_min = (__sleep->chart[i + 1].timestamp - __sleep->chart[i].timestamp) / 60;
            sleep_block.sleep_type = (__sleep->chart[i].stage == SLEEP_STAGE_AWAKE) ? 0xff : __sleep->chart[i].stage;
            log_info("3stage:%d min:%d", sleep_block.sleep_type, sleep_block.sleep_min);
            flash_common_write_packet(file_hd, __info->cur_file_id, sizeof(struct sleep_record_block), &sleep_block);
        }
        memcpy(&last_time, &tmp_time, sizeof(struct sys_time));
    }
    write_blocks *= 2;
    int analysis_len = 0;
#if SLEEP_FILE_ANALYSIS_EN
    struct sleep_data_analysis __analysis;
    struct sleep_data_head analysis_head;
    struct sleep_analyse_record __record;
    motion_module_sleep_file_analysis(&__analysis, __sleep);
    analysis_head.hour  = 0xff;
    analysis_head.min  = 0xff;
    analysis_head.len = sizeof(struct sleep_analyse_record);
    watch_common_swapX(&analysis_head.len, &analysis_head.len, 2);
    __record.score_sleep = __analysis.sleep_score;
    __record.percent_deep = 100 * __analysis.sleep_msec_deep / __analysis.sleep_msec_all;
    __record.percent_shallow = 100 * __analysis.sleep_msec_shallow / __analysis.sleep_msec_all;
    __record.percent_rem = 100 * __analysis.sleep_msec_rem / __analysis.sleep_msec_all;
    __record.sleep_evaluate = __analysis.sleep_evaluate;
    __record.deep_evalute = __analysis.deep_evalute;;
    __record.shallow_evalute = __analysis.shallow_evalute;
    __record.rem_evalute = __analysis.rem_evalute;
    __record.continue_score = __analysis.sleep_continue_score;
    __record.awake_times = __analysis.awake_times;
    flash_common_write_packet(file_hd, __info->cur_file_id, sizeof(struct sleep_data_head), &analysis_head);
    flash_common_write_packet(file_hd, __info->cur_file_id, sizeof(struct sleep_analyse_record), &__record);
    analysis_len = sizeof(struct sleep_data_head) + sizeof(struct sleep_analyse_record);

#endif //SLEEP_FILE_ANALYSIS_EN
    flash_common_write_push(file_hd, __info->cur_file_id);
    int new_file_offset = flash_common_get_file_size_by_id(file_hd, __info->cur_file_id) - write_blocks - analysis_len - 2;
    printf("%s new_file_offset%d analysis_len:%d write_blocks:%d", __func__, new_file_offset, analysis_len, write_blocks);
    watch_common_swapX(&write_blocks, &write_blocks, 2);
    flash_common_update_by_id(file_hd, __info->cur_file_id, new_file_offset, 2, &write_blocks);
#if 1
    int file_index = flash_common_get_total(file_hd) - 1;
    int file_size  = flash_common_get_file_size_by_index(file_hd, file_index);
    u8 *rbuf = zalloc(file_size + 1);
    ASSERT(rbuf);
    flash_common_read_by_index(file_hd, file_index, 0, file_size, rbuf);
    put_buf(rbuf, file_size);
    free(rbuf);
#endif
    if (__sleep) {
        free(__sleep);
        __sleep = NULL;
    }

    return 0;
}
int motion_module_sleep_file_read(sleep_data *__sleep)
{
    //get last record file
    if (!__sleep) {
        return -1 ;
    }
    struct health_file_info sleep_record_file = {0};
    struct health_file_info *__info = &sleep_record_file;
    memset(__info, 0, sizeof(struct health_file_info));
    __info->file_type  = FILE_TYPE;
    __info->max_file_size = MAX_FILE_SIZE;
    void *file_hd = get_flash_vm_hd(__info->file_type);
    __info->file_total = flash_common_get_total(file_hd);
    if (!__info->file_total) {
        log_error("sleep file not find");
        return -1;
    }
    //get record data
    __info->cur_file_index = __info->file_total - 1;
    __info->cur_file_size = flash_common_get_file_size_by_index(file_hd, __info->cur_file_index);
    log_info("%s file_total:%d index:%d size:%d", __func__, __info->file_total, __info->cur_file_index, __info->cur_file_size);
    int sleep_file_offset = 0;
    int all_block_nums = 0;
    int is_day_span = 0;

    flash_common_read_by_index(file_hd, __info->cur_file_index, sleep_file_offset, sizeof(HEALTH_FILE_HEAD), &__info->cur_file_head);
    watch_common_swapX(&__info->cur_file_head.year, &__info->cur_file_head.year, 2);
    sleep_file_offset += sizeof(HEALTH_FILE_HEAD);

    while (sleep_file_offset != __info->cur_file_size) {
        log_info("%s sleep_file_offset:%d ", __func__, sleep_file_offset);

        struct sys_time cur_time;
        watch_file_get_sys_time(&cur_time);
        int cur_t = 0, last_t = 0;
        is_day_span = 0;
        struct sleep_data_head sleep_head;
        flash_common_read_by_index(file_hd, __info->cur_file_index, sleep_file_offset, sizeof(struct sleep_data_head), &sleep_head);
        watch_common_swapX(&sleep_head.len, &sleep_head.len, sizeof(sleep_head.len));
        log_info("%s sleephead%d %d %d ", __func__, sleep_head.hour, sleep_head.min, sleep_head.len);
        sleep_file_offset += sizeof(struct sleep_data_head);
        struct sleep_record_block *rblock = (struct sleep_record_block *) zalloc(sleep_head.len);
        ASSERT(rblock);
        flash_common_read_by_index(file_hd, __info->cur_file_index, sleep_file_offset, sleep_head.len, rblock);
        int single_block_nums = sleep_head.len / sizeof(struct sleep_record_block);
        cur_time.hour = sleep_head.hour;
        cur_time.min = sleep_head.min;
        cur_t = cur_time.hour * 60 + cur_time.min;
        if (cur_time.hour * 60 + cur_time.min > SLEEP_FILE_RECORD_HOUR * 60) {
            for (int i = 0; i < single_block_nums; i++) {
                if (last_t > cur_t) {
                    is_day_span = 1;
                    break;
                }
                last_t = cur_t;
                cur_t += rblock[i].sleep_min % (24 * 60) ;
            }
        }

        if (is_day_span) {
            watch_file_prev_day(&cur_time, &cur_time);
        }
        cur_time.hour = sleep_head.hour;
        cur_time.min = sleep_head.min;
        for (int i = 0; i < single_block_nums; i++) {
            log_info("%s block:%d %d i:%d stage%d min:%d", __func__, cur_time.hour, cur_time.min, i, rblock[i].sleep_type, rblock[i].sleep_min);
            __sleep->chart[all_block_nums + i].timestamp = timestamp_mytime_2_utc_sec(&cur_time);
            __sleep->chart[all_block_nums + i].stage = rblock[i].sleep_type;
            watch_time_add_min(&cur_time, rblock[i].sleep_min);
        }
        sleep_file_offset += sleep_head.len;
        free(rblock);
        all_block_nums += single_block_nums ;
        __sleep->chart[all_block_nums].timestamp = timestamp_mytime_2_utc_sec(&cur_time);
        __sleep->chart[all_block_nums].stage = 0;//补全两段睡眠之间的清醒状态
        all_block_nums += 1;
    }
    __sleep->blocks = all_block_nums;
    return 0;

}
#else
//不使用杰理存储方式
/* int motion_module_sleep_file_write(void) */
/* { */
/* return 0; */
/* } */
/* int motion_module_sleep_file_read(sleep_data *__sleep) */
/* { */
/* return 0; */
/* } */
#endif

struct sleep_data_t sleepdata;

#define FILE_WRITE_FLASH TCFG_NOR_VM
/************************************************
			记录短睡眠状态
************************************************/
u16 record_short_sleep(u8 flag)
{
    printf("%s", __func__);
    if (sleepdata.status == 0) {
        //开始记录睡眠状态
        sleepdata.sleeprecord_head = zalloc(sizeof(struct sleep_record)); //创建一个节点
        if (sleepdata.sleeprecord_head == NULL) {
            log_e("sleeprecord_zalloc_error");
            return 0;
        }
        //记录该段睡眠开始时间
        watch_file_get_sys_time(&sleepdata.sleeprecord_head->flag_start);
        sleepdata.sleeprecord_head->flag = flag;
        sleepdata.sleepcnt = 1;
        sleepdata.status = 1; //短睡眠标志
    } else if (sleepdata.status == 1) {
        //记录该段睡眠结束时间
        watch_file_get_sys_time(&sleepdata.sleeprecord_head->flag_start);
    }
    return sleepdata.sleepcnt;
}
/************************************************
			记录长睡眠状态
************************************************/
u16 record_long_sleep(u8 flag)
{
    printf("%s", __func__);

    if (sleepdata.status == 0) {
        log_e("no_short_sleep_flag\n");
        return 0;
    } else if (sleepdata.status == 1) {
        log_e("no_2hours_sleep_flag\n");
        return 0;
    }
    //记录长睡眠状态
    struct sleep_record *tmp = sleepdata.sleeprecord_head;
    printf("tmp=%x", tmp);
    struct sleep_record *psleep;
    for (int i = 1; i < sleepdata.sleepcnt; i++) {
        tmp = tmp->next; //指针移到末尾
    }
    printf("flag=%d,tmp->flag=%d", flag, tmp->flag);
    if (flag == tmp->flag) {
        return 0;
    }
    psleep = zalloc(sizeof(struct sleep_record));
    if (psleep == NULL) {
        log_e("%s psleep zalloc error", __func__);
        return 0;
    }
    watch_file_get_sys_time(&tmp->flag_end);
    watch_time_sub_min(&tmp->flag_end, 1);
    psleep->flag = flag;
    watch_file_get_sys_time(&psleep->flag_start);
    tmp->next = psleep;
    sleepdata.sleepcnt++;
    printf("cnt=%d", sleepdata.sleepcnt);

    return sleepdata.sleepcnt;
}
/************************************************
		进入长睡眠时刷新更换短睡眠状态
************************************************/

u16 record_2hour_sleep(u8 *buf)
{
    printf("%s", __func__);
    if (sleepdata.status == 0) {
        log_e("no_short_sleep_flag\n");
        return 0;
    }
    if (sleepdata.status == 2) {
        log_e("repetition_2hour_sleep\n");
        return 0;
    }
    u8 flag_status = buf[0];
    u8 flag_count = 0;

    struct sleep_record *psleep;
    struct sleep_record *tmp;
    tmp = sleepdata.sleeprecord_head;

    tmp->flag = buf[0];
    printf("buf[0]=%d", buf[0]);
    sleepdata.status = 2;
    for (int i = 1; i < 120; i++) {

        if (flag_status != buf[i])	{ //flag change
            //记录上一段睡眠
            psleep = zalloc(sizeof(struct sleep_record));
            tmp->next = psleep;
            struct sys_time ptime;
            ptime = tmp->flag_start;
            printf("ptime1 %d-%d", ptime.min, ptime.sec);
            printf("cnt=%d", flag_count);
            watch_time_add_min(&ptime, flag_count);
            printf("ptime2 %d-%d", ptime.min, ptime.sec);
            tmp->flag_end = ptime;
            printf("ptime3 %d-%d", ptime.min, ptime.sec);
            printf("timeend %d-%d", tmp->flag_end.min, tmp->flag_end.sec);
            tmp = tmp->next;
            tmp->flag = buf[i];
            sleepdata.sleepcnt++;

            watch_time_add_min(&ptime, 1); //获得下一段睡眠开始时间
            tmp->flag_start = ptime;

            flag_status = buf[i]; //更新新的睡眠标志
            printf("status%d count%d sleepcnt=%d", flag_status, flag_count, sleepdata.sleepcnt);
            printf("time %d-%d", tmp->flag_start.min, tmp->flag_start.sec);

            flag_count = 0; //计算新一段睡眠的长度
        }
        flag_count++;
    }


    return sleepdata.sleepcnt;
}
/************************************************
			获取最近一次睡眠状态
************************************************/

u8 get_sleep_record(struct sleep_record *p)
{
    if (sleepdata.sleeprecord_head != NULL) {
        memcpy(p, sleepdata.sleeprecord_head, sizeof(struct sleep_record));
        return sleepdata.sleepcnt;
    } else {
        return 0;
    }
}

/************************************************
		清空最近一次睡眠数据
*************************************************/

void sleep_flag_free(void)
{
    if (sleepdata.sleepcnt != 0) {
        int cnt = sleepdata.sleepcnt ;
        for (int i = 0; i < cnt; i++) {
            struct sleep_record *tmp = sleepdata.sleeprecord_head;
            for (int n = 1; n < sleepdata.sleepcnt; n++) {
                tmp = tmp->next; //指针移到末尾
            }
            printf("flag=%d", tmp->flag);
            free(tmp);
            sleepdata.sleepcnt--;
            printf("sleepdata.sleepcnt=%d", sleepdata.sleepcnt);

        }
    }
}
/************************************************
  		记录转buf绘图
buf大小通过get_sleep_time_all获取总分钟数申请
************************************************/
int sleep_info_trans_buf(u8 *buf)
{
    if (sleepdata.sleeprecord_head == NULL) {
        log_e("sleepdata.sleeprecord_head==NULL");
        return 0;
    }
    int buf_pos = 0;
    struct sleep_record	*tmp = sleepdata.sleeprecord_head;
    for (int i = 0; i < sleepdata.sleepcnt; i++) {
        int len = watch_time_len_min(&tmp->flag_start, &tmp->flag_end);
        for (int n = 0; n < len; n++) {
            buf[buf_pos] = tmp->flag;
            buf_pos++;
        }
        tmp = tmp->next;
    }
    return buf_pos;
}
int sleep_info_trans_buf_len(void)
{
    if (sleepdata.sleeprecord_head == NULL) {
        log_e("sleepdata.sleeprecord_head==NULL");
        return 0;
    }
    int buf_pos = 0;
    struct sleep_record	*tmp = sleepdata.sleeprecord_head;
    for (int i = 0; i < sleepdata.sleepcnt; i++) {
        int len = watch_time_len_min(&tmp->flag_start, &tmp->flag_end);
        buf_pos += len;
        tmp = tmp->next;
    }
    return buf_pos;
}
//最近一次睡眠的开始时间
int get_sleep_start_time(struct sys_time *sleeptime)
{
    if (sleepdata.sleeprecord_head != NULL) {
        struct sys_time ntime;
        ntime = sleepdata.sleeprecord_head->flag_start;
        printf("yyyy=%d,mm=%d,dd=%d,hh=%d,mm=%d", ntime.year, ntime.month, ntime.day, ntime.hour, ntime.min);
        memcpy(sleeptime, &sleepdata.sleeprecord_head->flag_start, sizeof(struct sys_time));
        return 1;
    }
    return 0;
}
//睡觉结束时间
int get_sleep_stop_time(struct sys_time *sleeptime)
{
    if (sleepdata.sleepcnt != 0) {
        struct sleep_record *tmp = sleepdata.sleeprecord_head;
        for (int n = 1; n < sleepdata.sleepcnt; n++) {
            tmp = tmp->next; //指针移到末尾
        }
        printf("%s month=%d day=%d hour=%d min=%d", __func__, tmp->flag_end.month, tmp->flag_end.day, tmp->flag_end.hour, tmp->flag_end.min);
        memcpy(sleeptime, &tmp->flag_end, sizeof(struct sys_time));
        return 1;
    }
    return 0;
}
int sleep_time_start_stop_time(int type, struct sys_time *sleeptime)
{
    if (type == SLEEP_START_TIME) {
        return get_sleep_start_time(sleeptime);
    } else if (type == SLEEP_STOP_TIME) {
        return get_sleep_stop_time(sleeptime);
    }
    return 0;
}
int get_watch_sleep_target(void)
{
    return 8;
}


/************************************************
				写文件
************************************************/
struct sleep_file {
    u16 file_id;
    u16 w_file_offset;
    struct sys_time file_time;
} sleepfiledata;
int sleep_file_write(void)
{
    printf("%s", __func__);
#if TCFG_NOR_VM
    struct sys_time ntime;
    if (get_sleep_stop_time(&ntime) != 1) {
        log_e("sleep_end_time_error\n");
        return 0;
    }
    if (ntime.day != sleepfiledata.file_time.day) { //当前日期与上一次保存日期不一致
        sleepfiledata.file_time = ntime;
        u8 fbuf[11];//写文件头
        fbuf[0] = 0x05;
        fbuf[1] = (ntime.year >> 8) & 0xff;
        fbuf[2] = (ntime.year & 0xff);
        fbuf[3] = ntime.month;
        fbuf[4] = ntime.day;
        fbuf[5] = 0xff; //CRC
        fbuf[6] = 0xff; //CRC
        fbuf[7] = 0x00; //版本号
        fbuf[8] = 0xff; //存储时间间隔
        fbuf[9] = 0xff; //保留位1
        fbuf[10] = 0xff; //保留位2
        /*printf("filetype=%d,year=%d,month=%d,day=%d",fbuf[0],ntime.year,ntime.month,ntime.day);*/
        if (flash_common_get_total(get_flash_vm_hd(F_TYPE_SLEEP)) == get_flash_vm_number_max(F_TYPE_SLEEP)) {
            printf("total==%d", flash_common_get_total(get_flash_vm_hd(F_TYPE_SLEEP)));
            flash_common_delete_by_index(get_flash_vm_hd(F_TYPE_SLEEP), 0);
        }//清理旧文件
        sleepfiledata.file_id = flash_common_open_id(get_flash_vm_hd(F_TYPE_SLEEP), 0, 512); // 创建新文件
        if (sleepfiledata.file_id == 0) {
            log_e("file_id_error\n");
            return 0;
        }
        printf("file_head id=%d", sleepfiledata.file_id);
        flash_common_write_packet(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id, 11, fbuf); //写入文件头
        sleepfiledata.w_file_offset = 11;
        u8 data_head[4];//数据段首信息
        get_sleep_start_time(&ntime);
        data_head[0] = ntime.hour;
        data_head[1] = ntime.min;
        data_head[2] = ((sleepdata.sleepcnt * 2) >> 8) & 0xff; //len_h
        data_head[3] = (sleepdata.sleepcnt * 2) & 0xff; //len_l
        printf("hour=%d,min=%d", data_head[0], data_head[1]);
        flash_common_write_packet(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id, 4, data_head);
        sleepfiledata.w_file_offset += 4;
        printf("file_data0_head");
        //依次写入本次所有睡眠数据
        struct sleep_record	*tmp = sleepdata.sleeprecord_head;
        for (int i = 0; i < sleepdata.sleepcnt; i++) {
            u8 sleep_point[2];
            sleep_point[0] = tmp->flag;
            sleep_point[1] = (u8)watch_time_len_min(&tmp->flag_start, &tmp->flag_end);
            flash_common_write_packet(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id, 2, sleep_point);
            tmp = tmp->next;
        }

        sleepfiledata.w_file_offset += sleepdata.sleepcnt * 2;
        //写入睡眠评估情况
        data_head[0] = 0xff;
        data_head[1] = 0xff;
        data_head[2] = 0x00; //len_h
        data_head[3] = 0x07; //len_l
        flash_common_write_packet(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id, 4, data_head);
        sleepfiledata.w_file_offset += 4;
        extern void sleep_quality_evaluation_get(void);
        sleep_quality_evaluation_get();//评估
        printf("sleep_quality_evaluation___end_");
        flash_common_write_push(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id);
        printf("offset=%d", sleepfiledata.w_file_offset);

        sleep_flag_free();//清除睡眠数据
    } else {//多段睡眠
        u8 *tbuf = zalloc(sleepfiledata.w_file_offset + 1);
        flash_common_read_by_id(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id, 0, sleepfiledata.w_file_offset, tbuf);
        flash_common_delete_by_id(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id);
        sleepfiledata.file_id = flash_common_open_id(get_flash_vm_hd(F_TYPE_SLEEP), 0, 512);
        flash_common_write_packet(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id, sleepfiledata.w_file_offset, tbuf);
        free(tbuf);
        tbuf = NULL;
        get_sleep_start_time(&ntime);

        u8 data_head[4];//数据段首信息
        data_head[0] = ntime.hour;
        data_head[1] = ntime.min;
        data_head[2] = ((sleepdata.sleepcnt * 2) >> 8) & 0xff; //len_h
        data_head[3] = (sleepdata.sleepcnt * 2) & 0xff; //len_l
        printf("hour=%d,min=%d", data_head[0], data_head[1]);
        flash_common_write_packet(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id, 4, data_head);
        sleepfiledata.w_file_offset += 4;

        printf("offset=%d", sleepfiledata.w_file_offset);

        printf("file_data1_head");

        //依次写入本次所有睡眠数据
        struct sleep_record	*tmp = sleepdata.sleeprecord_head;
        for (int i = 0; i < sleepdata.sleepcnt; i++) {
            u8 sleep_point[2];
            sleep_point[0] = tmp->flag;
            sleep_point[1] = (u8)watch_time_len_min(&tmp->flag_start, &tmp->flag_end);
            flash_common_write_packet(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id, 2, sleep_point);
            tmp = tmp->next;
        }
        sleepfiledata.w_file_offset += sleepdata.sleepcnt * 2;
        //写入睡眠评估情况
        data_head[0] = 0xff;
        data_head[1] = 0xff;
        data_head[2] = 0x00; //len_h
        data_head[3] = 0x07; //len_l
        flash_common_write_packet(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id, 4, data_head);
        sleepfiledata.w_file_offset += 4;
        extern void sleep_quality_evaluation_get(void);
        sleep_quality_evaluation_get();//评估测试
        printf("sleep_quality_evaluation___end_");
        flash_common_write_push(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id);


        printf("offset=%d", sleepfiledata.w_file_offset);
        sleep_flag_free();//清除睡眠数据
    }
//校验文件，这里不适用CRC加密校验，采用按次改位加密的方式,理论上单日睡眠次数不会超过16,超过16次的数据将无法记录
//如需记录16段以上记录可使用协议保留位fbuf[10]fbuf[11]，最高支持32段
    /* u8 CRC[2];	 */
    /* flash_common_read_by_id(get_flash_vm_hd(F_TYPE_SLEEP),sleepfiledata.file_id,5,2,CRC); */
    /* if(CRC!=0){ */
    /* for(int i=0;i<16;i++){ */
    /* if(i<8){ */
    /* if(CRC[0]&BIT(i)==1){ */
    /* CRC[0] &= ~BIT(i); */
    /* break; */
    /* } */
    /* }else{ */
    /* if(CRC[1]&BIT(i-8)==1){ */
    /* CRC[1] &= ~BIT(i-8); */
    /* break; */
    /* } */
    /* } */
    /* } */


    /* }else{ */
    /* flash_common_read_by_id(get_flash_vm_hd(F_TYPE_SLEEP),sleepfiledata.file_id,10,2,CRC);//利用保留位拓展睡眠记录demo */
    /* for(int i=0;i<16;i++){ */
    /* if(i<8){ */
    /* if(CRC[0]&BIT(i)==1){ */
    /* CRC[0] &= ~BIT(i); */
    /* break; */
    /* } */
    /* }else{ */
    /* if(CRC[1]&BIT(i-8)==1){ */
    /* CRC[1] &= ~BIT(i-8); */
    /* break; */
    /* } */
    /* } */
    /* }	 */
    /* } */
#endif
    return 0;
}
/************************************************
  //睡眠模拟

 ************************************************/
#define SL_TEST 0
#define TEST_DAY 1

u16 record_long_sleep_test(u8 flag, u8 len, struct sys_time *ntime)
{
    printf("%s flag=%d len=%d", __func__, flag, len);
    if (flag == 0) {//睡醒
        struct sys_time testtime;
        struct sleep_record *tmp = sleepdata.sleeprecord_head;
        for (int i = 1; i < sleepdata.sleepcnt; i++) {
            tmp = tmp->next;
        }
        printf("flag=%d,tmp->flag=%d", flag, tmp->flag);
        testtime = tmp->flag_start;
        if (len > 60) {//结束时，把最后一段时长补上
            watch_time_add_min(&testtime, 60);
            watch_time_add_min(&testtime, len - 60);
        } else {
            watch_time_add_min(&testtime, len);
        }

        tmp->flag_end = testtime;
        return sleepdata.sleepcnt;
    }
    if (sleepdata.status == 0) {
        //开始记录睡眠状态
        sleepdata.sleeprecord_head = zalloc(sizeof(struct sleep_record)); //创建一个节点
        if (sleepdata.sleeprecord_head == NULL) {
            log_e("sleeprecord_zalloc_error");
            return 0;
        }

        memcpy(&sleepdata.sleeprecord_head->flag_start, ntime, sizeof(struct sys_time)); //赋予开始时间
        sleepdata.sleeprecord_head->flag = flag;
        sleepdata.sleepcnt = 1;
        sleepdata.status = 1;
    } else {
        struct sys_time testtime;
        struct sleep_record *tmp = sleepdata.sleeprecord_head;
        struct sleep_record *psleep;
        for (int i = 1; i < sleepdata.sleepcnt; i++) {
            tmp = tmp->next;
        }
        printf("flag=%d,tmp->flag=%d", flag, tmp->flag);
        psleep = zalloc(sizeof(struct sleep_record));
        if (psleep == NULL) {
            log_e("%s psleep zalloc error", __func__);
            return 0;
        }
        testtime = tmp->flag_start;
        if (len > 60) {
            watch_time_add_min(&testtime, 60);
            watch_time_add_min(&testtime, len - 60);
        } else {
            watch_time_add_min(&testtime, len);
        }
        tmp->flag_end = testtime;
        psleep->flag = flag;
        watch_time_add_min(&testtime, 1);
        psleep->flag_start = testtime;
        tmp->next = psleep;
        sleepdata.sleepcnt++;
        printf("cnt=%d", sleepdata.sleepcnt);

    }
    return sleepdata.sleepcnt;
}

int sleepdeeptime = 0;
int sleepshallowtime = 0;
int sleepremtime = 0;

struct sleep_quality_evaluation {
    char all_sleep_score;
    char deepsleep_scale;
    char shallowsleep_scale;
    char remsleep_scale;
    char sleeptime_evaluation;//4*2bits
    char deepsleep_continue_score;
    char wakeup_times;
};
struct sleep_quality_evaluation *sleep_evaluation;
void sleep_quality_evaluation_get(void)
{
    printf("%s", __func__);
    sleep_evaluation = zalloc(sizeof(struct sleep_quality_evaluation));
    struct sleep_quality_evaluation *__this;
    __this = sleep_evaluation;
#if SL_TEST
    int timeall = sleepdeeptime + sleepshallowtime + sleepremtime; //分钟
    int sleep_time_tmp = (timeall >= 9 * 60) ? (timeall - 9 * 60) : (9 * 60 - timeall);
    __this->all_sleep_score = (100 - (int)(sleep_time_tmp / 10));
    __this->deepsleep_scale = 70;
    __this->shallowsleep_scale = 85;
    __this->remsleep_scale = 60;
    __this->sleeptime_evaluation = 0xfe; //4*2bits
    __this->deepsleep_continue_score = 80;
    __this->wakeup_times = 0;
    sleepdeeptime = 0;
    sleepshallowtime = 0;
    sleepremtime = 0;
#else
    struct watch_sleep __sleep_hd;
    watch_sleep_handle_get(&__sleep_hd);
    int arg[3] = {0};
    __sleep_hd.sleep_time_analyse(arg, SLEEP_ALL);
    __this->all_sleep_score = (char)arg[1];
    __this->sleeptime_evaluation |= ((char)arg[2] << 6);

    __sleep_hd.sleep_time_analyse(arg, SLEEP_DEEP);
    __this->deepsleep_scale = (char)arg[1];
    __this->sleeptime_evaluation |= ((char)arg[2] << 4);

    __sleep_hd.sleep_time_analyse(arg, SLEEP_SHALLOW);
    __this->shallowsleep_scale = (char)arg[1];
    __this->sleeptime_evaluation |= ((char)arg[2] << 2);

    __sleep_hd.sleep_time_analyse(arg, SLEEP_SHALLOW);
    __this->remsleep_scale = (char)arg[1];
    __this->sleeptime_evaluation |= ((char)arg[2] << 0);

    __sleep_hd.sleep_time_analyse(arg, SLEEP_CONTINUE);
    __this->deepsleep_continue_score = (char)arg[1];
    __this->wakeup_times = (char)arg[0];

#endif

    flash_common_write_packet(get_flash_vm_hd(F_TYPE_SLEEP), sleepfiledata.file_id, 7, (u8 *)__this) ;
    sleepfiledata.w_file_offset += 7;
    printf("%s file_id=%d", __func__, sleepfiledata.file_id);
    free(__this);
}
void sleep_file_test(void)
{
#if SL_TEST

    struct sys_time ntime, ptime;
    watch_file_get_sys_time(&ptime);
    printf("%d %d %d ", ptime.year, ptime.month, ptime.day);
    watch_file_prev_x_day(ntime, ptime, TEST_DAY); //前一天
    printf("%d %d %d ", ntime.year, ntime.month, ptime.day);
//day1
    record_long_sleep_test(1, 0, &ntime); //创建一个睡眠记录节点
    record_long_sleep_test(1, 10, NULL); //写入前一天的数据
    u32	timecnt = 0;
    u8 flag_old = 1;
    while (1) {
        u8 flag = rand32() % 3 + 1;
        if (flag == flag_old) {
            continue;
        } else {
            flag_old = flag;
        }
        u8 time_len = rand32() % 110 + 10;
        timecnt += time_len;

        if (timecnt > (7 * 60)) {
            record_long_sleep_test(0, time_len, NULL); //结束记录
            break;
        }
        record_long_sleep_test(flag, time_len, NULL); //记录flag
        switch (flag) {
        case 1:
            sleepshallowtime += time_len;
            break;
        case 2:
            sleepdeeptime += time_len;
            break;
        case 3:
            sleepremtime += time_len;
            break;
        }
    }
    printf("%s moni ok", __func__);
    sleep_file_write();
    sleepdata.status = 0;
    printf("%s record ok!!!", __func__);


#endif
}
