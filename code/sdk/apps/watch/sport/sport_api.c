#include "ui/ui.h"
#include "app_config.h"
#include "ui/ui_api.h"
#include "system/timer.h"
#include "key_event_deal.h"
#include "audio_config.h"
#include "jiffies.h"
#include "app_power_manage.h"
#include "asm/charge.h"
#include "audio_dec_file.h"
#include "device/device.h"
#include "sport/sport_api.h"
#include "sport_data/watch_common.h"



#if TCFG_NOR_VM 

#include "ui_vm/ui_vm.h"



extern int flash_common_write_file(void *handle, int id, int len, u8 *data);
extern int flash_common_read_by_id(void *handle, int id, int offset, int len, u8 *data);
extern int flash_common_read_by_index(void *handle, int index, int offset, int len, u8 *data);
extern int flash_common_get_id_table(void *handle, int len, u8 *data);
extern int flash_common_init(void **handle, const u8 *name, u16 type, u16 max, int start, int end);
extern int flash_common_get_total(void *handle);
extern int flash_common_delete_by_id(void *handle, int id);
extern int flash_common_delete_by_index(void *handle, int index);
extern int flash_common_open_id(void *handle, int id, int len);
extern int flash_common_write_packet(void *handle, int id, int len, u8 *data);
extern int flash_common_write_push(void *priv, int id);
extern int flash_common_update_by_id(void *handle, int id, int offset, int len, u8 *data);
extern void *get_flash_vm_hd(u8 type);
u8 get_flash_vm_number_max(u8 type);



struct watch_execise *__execise_hd = NULL;
struct motion_info *__execise_info = NULL;

#pragma pack(1)//不平台对齐编译
typedef struct {
    u8 sport_mode;//运动模式
    u8 version;
    u8  interval;
    u8  mask;//0xee//检查位完整 0xe0 数据被破坏
    u32 block: 15;
    u32 size: 17;
    u8  reserved[5];//5
} SPORT_Head;//16字节
#pragma pack()//平台对齐编译

#define SPORT_START_FLAG  (0x0)
#define SPORT_DATA_FLAG   (0x1)
#define SPORT_STOP_FLAG   (0x2)
#define SPORT_END_FLAG    (0xff)

#define SPORT_MIN_DISTANCE 10
#define SPORT_MAX_TIME 3*60*60
#define SPORT_FILE_SIZE 16*1024

#pragma pack(1)//不平台对齐编译
typedef struct {
    u8 flag;//1有后续 0没有后续
    u8 len;
    u8 data[0];//接后续结尾
} SPORT_COMMON;//4
#pragma pack()//平台对齐编译


#pragma pack(1)//不平台对齐编译
typedef struct {
    u8 flag;//1有后续 0没有后续
    u8 len;
    u8 heart;
    u16 step_freq;
    u16 speed;
    u8 tag[0];//接后续结尾
} SPORT_Data;//4
#pragma pack()//平台对齐编译


#pragma pack(1)//不平台对齐编译
typedef struct {
    u16  sport_sec;//运动时长
    u32  end_time;//结束时间
    u16  distance;//距离
    u16  calorie;//卡路里
    u32  step;
    u16  recovery_time;
    u8   heartrate_mode;
    u8   exercise_intensity_buf[20];
} SPORT_Tag;//16
#pragma pack()//平台对齐编译

#pragma pack(1)//不平台对齐编译
typedef struct {
    u8 	flag;
    u8 len;
    u16 pace_sec;//公里时长
    u8 	pace_km;//公里数

} SPORT_PACE;//16
#pragma pack()//平台对齐编译



#pragma pack(1)//不平台对齐编译
typedef struct {
    SPORT_Head head;
    SPORT_Data data[0];
} SPORT_Record;
#pragma pack()//平台对齐编译

#define SIZEOF_START_DATA  (4)
#define SIZEOF_STOP_DATA   (4)
#define SIZEOF_END_DATA    (4)
#define SIZEOF_DATA        (5)




#define MUTEX_DEBUG (1)
#define TASK_DEBUG "ui"

static void __sport_debug(int line)
{
    if (strcmp("ui", os_current_task())) {
        printf("__FUNCTION__ = %s __LINE__ = %d \n", __FUNCTION__, line);
        /* ASSERT(0); */
    }
}

static int __int2time(int t, struct sys_time *time)
{
    time->sec = t & 0x3f;
    time->min = (t >> 6) & 0x3f;
    time->hour = (t >> 12) & 0x1f;
    time->day = (t >> 17) & 0x1f;
    time->month = (t >> 22) & 0xf;
    time->year = ((t >> 26) & 0x3f) + 2010;
    printf("time : %d-%d-%d,%d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec);
    return 0;
}



static int __time2int(struct sys_time *time)
{
    int t;
    printf("time : %d-%d-%d,%d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec);
    ASSERT(time->year >= 2010 && time->year <= 2010 + 0x3f, "input year need >= 2010 &&  <= 2073 \n");
    t = (time->sec & 0x3f) | ((time->min & 0x3f) << 6) | ((time->hour & 0x1f) << 12) | ((time->day & 0x1f) << 17) | ((time->month & 0xf) << 22) | (((time->year - 2010) & 0x3f) << 26);
    __int2time(t, time);
    return t;
}
// 暂时模拟时间
static int get_elapse_time(struct sys_time *time)
{
    static struct sys_time cur_time = {0};
    static u32 jiffies_offset = 0;
    static u32 jiffies_base = 0;
    int current_msec = jiffies_msec();

    int msec = current_msec + jiffies_offset - jiffies_base;

    if (msec < 0) {  //越界，清0处理
        jiffies_base = 0;
        cur_time.hour = 0;
        cur_time.min = 0;
        cur_time.sec = 0;
        msec = current_msec + jiffies_offset - jiffies_base;
    }

    if (msec >= 1000) {
        do {
            msec -= 1000;
            if (++cur_time.sec >= 60) {
                cur_time.sec = 0;
                if (++cur_time.min >= 60) {
                    cur_time.min = 0;
                    if (++cur_time.hour >= 99) {
                        cur_time.hour = 0;
                    }
                }
            }
        } while (msec >= 1000);

        jiffies_base = current_msec + jiffies_offset - msec;
    }

    memcpy(time, &cur_time, sizeof(struct sys_time));
    /* sprintf(time, "[%02d:%02d:%02d.%03d]", cur_time.hour, cur_time.min, */
    /* cur_time.sec, msec); */

    return 14;
}



static int __get_rtc_time(struct sys_time *time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        get_elapse_time(time);
        time->year = 2021;
        time->month = 11;
        time->day = 18;
        return 0;
        /* memset(time, 0x00, sizeof(struct sys_time)); */
        /* return -1; */
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
    printf("get_sys_time : %d-%d-%d,%d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec);
    dev_close(fd);
    return 0;
}

static int __cur_time2int(struct sys_time *t)
{
    struct sys_time time;
    if (!t) {
        t = &time;
    }
    __get_rtc_time(t);
    return __time2int(t);
}



#define BLOCK_SIZE (512)
static SPORT_Head s_head = {0};
static SPORT_Tag  s_tag  = {0};
static SPORT_Record *record_w = NULL;
static SPORT_Record *record_r = NULL;

#define __this_head (&s_head)
#define __this_tag  (&s_tag)

static u8 block_data_write[BLOCK_SIZE] __attribute__((aligned(4)));
#define SPORT_REC_PATH      "storage/sd1/C/SPORT"
static void *w_file = 0;//
static void *r_file = 0;//
static int   w_seek = 0;
static  u16  w_timer = 0;
static  u32  w_sport_time = 0;
static  u8   stop = 0;
static  u8   *w_temp_buf = 0;
static void *last_file = 0;

u8 *sport_write_buf_alloc(int len)
{

    int offset  = w_seek % BLOCK_SIZE;
    int free_size = BLOCK_SIZE - offset;
    if (free_size >= len) {
        return (u8 *)block_data_write + offset;
    } else {
        w_temp_buf =  malloc(len);
        return w_temp_buf;
    }
}


int sport_buf_write_updata(u8 *buf, int len)
{
#if TCFG_NOR_VM
    int l_len = 0;
    if (w_temp_buf) { //跨cache
        if ((w_seek / BLOCK_SIZE) != (w_seek + len) / BLOCK_SIZE) {
            l_len = (w_seek + BLOCK_SIZE - 1) / BLOCK_SIZE * BLOCK_SIZE - w_seek;
            memcpy((u8 *)block_data_write + w_seek % BLOCK_SIZE, buf, l_len);
            /* fseek(w_file, w_seek / BLOCK_SIZE * BLOCK_SIZE, SEEK_SET); */
            /* int ret = fwrite(w_file, block_data_write, BLOCK_SIZE); */
            int ret = flash_common_write_packet(get_flash_vm_hd(F_TYPE_SPORTRECORD), (int)w_file, BLOCK_SIZE, block_data_write);
            put_buf(block_data_write, BLOCK_SIZE);


            if (ret != BLOCK_SIZE) {
                printf("%s %dwrite err\n", __FUNCTION__, __LINE__);
                ASSERT(0);
            }
            w_seek += l_len;

            if (len - l_len > 0) {
                if (len - l_len > BLOCK_SIZE) {
                    printf("%s %dwrite err\n", __FUNCTION__, __LINE__);
                    ASSERT(0);
                }
                memcpy((u8 *)block_data_write, buf + l_len, len - l_len);
                w_seek += (len - l_len);
                free(w_temp_buf);
                w_temp_buf = NULL;
            }
        } else {
            memcpy((u8 *)block_data_write + w_seek % BLOCK_SIZE, buf, len);
            w_seek += len;
            free(w_temp_buf);
            w_temp_buf = NULL;
        }
    } else {
        w_seek += len;
        if (!(w_seek % BLOCK_SIZE)) {
            /* fseek(w_file, w_seek - BLOCK_SIZE, SEEK_SET); */
            int ret = flash_common_write_packet(get_flash_vm_hd(F_TYPE_SPORTRECORD), (int)w_file, BLOCK_SIZE, block_data_write);

            /* int ret = fwrite(w_file, block_data_write, BLOCK_SIZE); */
            if (ret != BLOCK_SIZE) {
                printf("%s %dwrite err\n", __FUNCTION__, __LINE__);
                ASSERT(0);
            }
            put_buf(block_data_write, BLOCK_SIZE);
        }
    }
#else

    int l_len = 0;
    if (w_temp_buf) { //跨cache
        if ((w_seek / BLOCK_SIZE) != (w_seek + len) / BLOCK_SIZE) {
            l_len = (w_seek + BLOCK_SIZE - 1) / BLOCK_SIZE * BLOCK_SIZE - w_seek;
            memcpy((u8 *)block_data_write + w_seek % BLOCK_SIZE, buf, l_len);
            fseek(w_file, w_seek / BLOCK_SIZE * BLOCK_SIZE, SEEK_SET);
            int ret = fwrite(w_file, block_data_write, BLOCK_SIZE);
            put_buf(block_data_write, BLOCK_SIZE);
            if (ret != BLOCK_SIZE) {
                printf("%s %dwrite err\n", __FUNCTION__, __LINE__);
                ASSERT(0);
            }
            w_seek += l_len;

            if (len - l_len > 0) {
                if (len - l_len > BLOCK_SIZE) {
                    printf("%s %dwrite err\n", __FUNCTION__, __LINE__);
                    ASSERT(0);
                }
                memcpy((u8 *)block_data_write, buf + l_len, len - l_len);
                w_seek += (len - l_len);
                free(w_temp_buf);
                w_temp_buf = NULL;
            }
        } else {
            memcpy((u8 *)block_data_write + w_seek % BLOCK_SIZE, buf, len);
            w_seek += len;
            free(w_temp_buf);
            w_temp_buf = NULL;
        }
    } else {
        w_seek += len;
        if (!(w_seek % BLOCK_SIZE)) {
            fseek(w_file, w_seek - BLOCK_SIZE, SEEK_SET);
            int ret = fwrite(w_file, block_data_write, BLOCK_SIZE);
            if (ret != BLOCK_SIZE) {
                printf("%s %dwrite err\n", __FUNCTION__, __LINE__);
                ASSERT(0);
            }
            put_buf(block_data_write, BLOCK_SIZE);
        }
    }

#endif
    return 0;
}


void watch_sport_file_collating(void *priv)
{
    printf("%s %d %d ", __func__, flash_common_get_total(get_flash_vm_hd(F_TYPE_SPORTRECORD)), get_flash_vm_number_max(F_TYPE_SPORTRECORD));
    if (flash_common_get_total(get_flash_vm_hd(F_TYPE_SPORTRECORD)) == get_flash_vm_number_max(F_TYPE_SPORTRECORD)) {
        printf("total==%d", flash_common_get_total(get_flash_vm_hd(F_TYPE_SPORTRECORD)));
        int file_size = flash_common_get_file_size_by_index(get_flash_vm_hd(F_TYPE_SPORTRECORD), 0);
        flash_common_delete_by_index(get_flash_vm_hd(F_TYPE_SPORTRECORD), 0);
        flash_common_file_defrag(get_flash_vm_hd(F_TYPE_SPORTRECORD),  SPORT_FILE_SIZE);
    }
}

static void watch_sport_update_by_timer(void *priv)
{
    watch_sport_update(priv);
}


int watch_sport_start(u8 type) //开始运动
{
    __sport_debug(__LINE__);

    if (w_file) {
        return -1;
    }

    struct sys_time time;
    __get_rtc_time(&time);
    if (__execise_hd == NULL) {
        __execise_hd = zalloc(sizeof(struct watch_execise));
        watch_execise_handle_get(__execise_hd);
    }
    if (__execise_info == NULL) {
        __execise_info = zalloc(sizeof(struct motion_info));
    }
    __execise_hd->execise_info_get(__execise_info);
#if TCFG_NOR_VM
    int i_time = __cur_time2int(&time);
    printf("%s %d", __func__, __LINE__);
    watch_sport_file_collating(NULL);
    printf("%s %d", __func__, __LINE__);
    w_file = (void *)flash_common_open_id(get_flash_vm_hd(F_TYPE_SPORTRECORD), 0, SPORT_FILE_SIZE);
    if (!(int)w_file) {
        printf("file open fail \n");
        return -1;
    }
    printf("first record...\n");
    SPORT_Head *head;
    head = (SPORT_Head *)block_data_write;
    head->version = 0;
    head->sport_mode = type;
    head->interval = 5;
    head->mask  = 0xff;
    head->block  = 0xffffffff;
    head->size   = 0xffffffff;

    memcpy(__this_head, head, sizeof(SPORT_Head));

    __this_head->block = 0;
    __this_head->size = 0;

    w_seek = sizeof(SPORT_Record);

    SPORT_COMMON *buf = (SPORT_COMMON *)sport_write_buf_alloc(SIZEOF_START_DATA + sizeof(SPORT_COMMON));
    SPORT_COMMON temp;
    temp.flag = 0;
    temp.len = SIZEOF_START_DATA;
    memcpy(buf, &temp, sizeof(SPORT_COMMON));
    memcpy(buf->data, &i_time, sizeof(int));
    sport_buf_write_updata((u8 *)buf, SIZEOF_START_DATA + sizeof(SPORT_COMMON));
    w_timer = sys_timer_add(NULL, watch_sport_update_by_timer, head->interval * 1000);
    stop = 0;


#else
    u8 *path = zalloc(128);
    snprintf(path, 128, "/%s%02d%02d%02d%02d.spt", SPORT_REC_PATH, time.month, time.day, time.hour, time.min);
    printf("%s\n", path);
    w_file = fopen(path, "w+");
    if (!w_file) {
        free(path);
        return -1;
    }
    free(path);
    path = NULL;
    struct vfs_attr attr = {0};
    fget_attrs(w_file, &attr);
    printf("file size:%d", attr.fsize);
    printf("line:%d", __LINE__);
    int fsize = attr.fsize;
    /* if(!fsize) */
    {
        printf("first record...\n");
        SPORT_Head *head;
        head = (SPORT_Head *)block_data_write;
        head->sport_mode = type;
        head->interval = 5;
        head->mask  = 0xe0;
        head->block  = 0;
        head->size   = 8 * 1024;
        fseek(w_file, head->size, SEEK_SET);
        memcpy(__this_head, head, sizeof(SPORT_Head));
        w_seek = sizeof(SPORT_Record);

        SPORT_COMMON *buf = sport_write_buf_alloc(SIZEOF_START_DATA + sizeof(SPORT_COMMON));
        SPORT_COMMON temp;
        temp.flag = 0;
        temp.len = SIZEOF_START_DATA;
        int i_time = __cur_time2int(&time);
        memcpy(buf, &temp, sizeof(SPORT_COMMON));
        memcpy(buf->data, &i_time, sizeof(int));
        sport_buf_write_updata(buf, SIZEOF_START_DATA + sizeof(SPORT_COMMON));

        w_timer = sys_timer_add(NULL, watch_sport_update_by_timer, head->interval * 1000);

        stop = 0;
    }
#endif
    return 0;
}



#define L_INTENSITY_TIME_LEN 20
#define L_INTENSITY_MODE_LEN 1
#define L_RECOVERY_TIME_LEN 2
#define L_STEP_LEN 4
#define L_CALORIE_LEN 2
#define L_DISTANCE_LEN 2
#define L_SAVE_BIT_LEN 4
#define L_SPORT_TIME_LEN 2
#define L_SPORT_ENDTIME_LEN 4

static SPORT_INFO *s_info = NULL;

int watch_sport_analyze()
{
    printf("%s", __func__);
#if TCFG_NOR_VM
    //打开文件
    int offset = 0;
    int id_index = flash_common_get_total(get_flash_vm_hd(F_TYPE_SPORTRECORD));//id
    printf("file_id=%d", id_index);
    if (id_index == 0) {
        return -1;
    }
//创建结构体
//解析内容
    if (s_info) {
        printf("%s", __func__);
        memset(s_info, 0, sizeof(SPORT_INFO));
    } else {
        s_info = zalloc(sizeof(SPORT_INFO));
    }
    int __stime = 0;
    int __etime = 0;
    int file_len = flash_common_get_file_size_by_id(get_flash_vm_hd(F_TYPE_SPORTRECORD), id_index);
    u8 *buf = zalloc(file_len + 1);
    flash_common_read_by_id(get_flash_vm_hd(F_TYPE_SPORTRECORD), id_index, 0, file_len, buf);
    extern void printf_buf(u8 * buf, u32 len);
    printf_buf(buf, file_len);
    memcpy(&__stime, buf + 15, 4);
    __int2time(__stime, &s_info->stime);
    printf("__stime=%d s_time=%d,%d,%d", __stime, s_info->stime.hour, s_info->stime.min, s_info->stime.sec);
    printf("file_len=%d", file_len);
    offset += L_INTENSITY_TIME_LEN;
    memcpy(s_info->Intensity_time, buf + (file_len - offset), 20);
    offset += L_INTENSITY_MODE_LEN;
    memcpy(&s_info->heart_rate_mode, buf + (file_len - offset), 1);
    offset += L_RECOVERY_TIME_LEN;
    memcpy(&s_info->recovery_time, buf + (file_len - offset), 2);
    offset += L_STEP_LEN;
    memcpy(&s_info->step, buf + file_len - offset, 4); //4
    //printf("STEPlen=%d file_len=%d L_STEP=%d", (file_len- offset),file_len,L_STEP_LEN);
    offset += L_CALORIE_LEN;
    memcpy(&s_info->calorie, buf + (file_len - offset), 2); //2
    offset += L_DISTANCE_LEN;
    memcpy(&s_info->distance, buf + (file_len - offset), 2); //2
    offset += (L_SAVE_BIT_LEN + L_SPORT_TIME_LEN);
    memcpy(&s_info->sport_time, buf + (file_len - offset), 2); //2
    offset += L_SPORT_ENDTIME_LEN;
    memcpy(&__etime, buf + (file_len - offset), 4); //4
    __int2time(__etime, &s_info->etime);
    printf("file-offs=%d", file_len - offset);
    printf("step%d kcal%d distance%d sportT%d ", s_info->step, s_info->calorie, s_info->distance, s_info->sport_time);







#else
    SPORT_Head head = {0};
    SPORT_Tag  s_tag  = {0};

    if (!r_file) {
        r_file = fopen(SPORT_REC_PATH"/*****.SPT", "r");
    }

    if (!r_file) {
        return -1;
    }

    if (s_info) {
        memset(s_info, 0, sizeof(SPORT_INFO));
    } else {
        s_info = zalloc(sizeof(SPORT_INFO));
    }

    fseek(r_file, 0, SEEK_SET);
    fread(r_file, &head, sizeof(SPORT_Head));

    if (head.mask != 0xee) {
        printf("file wrong \n");
        return 0;
    }

    s_info->type = head.sport_mode;
    SPORT_COMMON *start_data = malloc(SIZEOF_START_DATA + sizeof(SPORT_COMMON));
    fread(r_file, start_data, SIZEOF_START_DATA + sizeof(SPORT_COMMON));
    __int2time(*((int *)start_data->data), &s_info->stime);
    free(start_data);

    int size = head.size;
    int offset = size - sizeof(SPORT_Tag);
    fseek(r_file, offset, SEEK_SET);
    fread(r_file, &s_tag, sizeof(SPORT_Tag));

    s_info->sport_time  = s_tag.sport_sec;
    __int2time(s_tag.end_time, &s_info->etime);

    s_info->distance = s_tag.distance;
    s_info->calorie = s_tag.calorie;
    s_info->step = s_tag.step;
    s_info->recovery_time = s_tag.recovery_time;

    if (r_file) {
        fclose(r_file);
        r_file = NULL;
    }
#endif
    return 0;
}


static int __watch_sport_read(SPORT_Data *data)
{
    if (__execise_hd == NULL) {
        __execise_hd = zalloc(sizeof(struct watch_execise));
        watch_execise_handle_get(__execise_hd);
    }
    if (__execise_info == NULL) {
        __execise_info = zalloc(sizeof(struct motion_info));
    }
    __execise_hd->execise_info_get(__execise_info);
    struct watch_heart_rate __heartrate_hd;
    watch_heart_rate_handle_get(&__heartrate_hd);
    data->heart = __heartrate_hd.heart_rate_data_get(LAST_DATA, NULL);

    data->step_freq = __execise_info->motion_step_freq;

    data->speed = __execise_info->motion_speed;
    data->flag = 1;
    data->len = SIZEOF_DATA;
    return sizeof(SPORT_Data);
}



int watch_sport_update(void *p)
{
    if (!w_file) {
        return -1;
    }

    if (!w_timer) {
        return -1;
    }

    if (stop) {
        return -1;
    }
    if (__execise_hd == NULL) {
        __execise_hd = zalloc(sizeof(struct watch_execise));
        watch_execise_handle_get(__execise_hd);
    }
    if (__execise_info == NULL) {
        __execise_info = zalloc(sizeof(struct motion_info));
    }
    __execise_hd->execise_info_get(__execise_info);
    if (w_sport_time >= SPORT_MAX_TIME) {
        printf("%s", "sport_time over max");
        __execise_hd->execise_ctrl_status_set(NO_SPORT_MODE, SPORT_STATUS_PAUSE);
        __execise_hd->execise_ctrl_status_set(NO_SPORT_MODE, SPORT_STATUS_STOP);
        watch_sport_stop();
        extern int sport_info_sync_end_exercise_by_fw(void);
        sport_info_sync_end_exercise_by_fw();
        extern int execise_ctrl_status_clr(void);
        execise_ctrl_status_clr();
#if TCFG_UI_ENABLE
        ui_auto_goto_dial_enable();
        UI_HIDE_WINDOW(UI_GET_WINDOW_ID());
        UI_SHOW_WINDOW(ID_WINDOW_SPORT_RECORD);
#endif
    }
    stop = 0;
    SPORT_Data temp = {0};
    printf("%s %d\n", __FUNCTION__, __LINE__);
    u8 *buf = sport_write_buf_alloc(sizeof(SPORT_Data));
    int len =  __watch_sport_read(&temp);
    memcpy(buf, &temp, sizeof(SPORT_Data));
    sport_buf_write_updata(buf, len);
    __this_head->block++;

    static u8 pace_km;
    u8 distance_p = 10;
    u8 distance_new = __execise_info->step_distance;
    if (distance_new == 0) {
        pace_km = 0;
    }
    if ((distance_new  >= distance_p) & (distance_new % distance_p == 0) & (pace_km != distance_new)) {
        pace_km = distance_new;
        buf = sport_write_buf_alloc(sizeof(SPORT_PACE));
        SPORT_PACE temp_pace;
        temp_pace.flag = 0x03;
        temp_pace.len = 03;
        temp_pace.pace_km = pace_km / distance_p;
        extern int get_sport_pace_sec(void);
        temp_pace.pace_sec = get_sport_pace_sec();
        memcpy(buf, &temp_pace, sizeof(SPORT_PACE));
        sport_buf_write_updata(buf, sizeof(SPORT_PACE));
        __this_head->block++;
    }

    /* if (__this_head->block % 500 == 0)  */
    if (0) {
        int t_test = __cur_time2int(NULL);
        SPORT_COMMON *t_data = malloc(SIZEOF_END_DATA + sizeof(SPORT_COMMON));
        t_data->flag = 0x2;
        t_data->len  = SIZEOF_END_DATA;
        memcpy(t_data->data, &t_test, sizeof(int));
        buf = sport_write_buf_alloc(sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        memcpy(buf, t_data, sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        sport_buf_write_updata(buf, sizeof(SPORT_COMMON) + SIZEOF_END_DATA);

        t_data->flag = 0x0;
        t_data->len  = SIZEOF_END_DATA;
        buf = sport_write_buf_alloc(sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        memcpy(buf, t_data, sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        sport_buf_write_updata(buf, sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        free(t_data);
    } else {
        w_sport_time += __this_head->interval;
    }
__err:
    return -1;
}


int watch_sport_pause()
{
    printf("%s", __func__);
    __sport_debug(__LINE__);
    if (!w_file) {
        return -1;
    }

    if (!stop) {
        stop = 1;
        u8 *buf;
        int t_test = __cur_time2int(NULL);
        SPORT_COMMON *t_data = malloc(SIZEOF_END_DATA + sizeof(SPORT_COMMON));
        t_data->flag = 0x2;
        t_data->len  = SIZEOF_END_DATA;
        memcpy(t_data->data, &t_test, sizeof(int));
        buf = sport_write_buf_alloc(sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        memcpy(buf, t_data, sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        sport_buf_write_updata(buf, sizeof(SPORT_COMMON) + SIZEOF_END_DATA);

        t_data->flag = 0x0;
        t_data->len  = SIZEOF_END_DATA;
        buf = sport_write_buf_alloc(sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        memcpy(buf, t_data, sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        sport_buf_write_updata(buf, sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        free(t_data);
    }

    return stop;
}


int watch_sport_restart()
{
    __sport_debug(__LINE__);
    if (!w_file) {
        return -1;
    }
    if (stop) {
        stop = 0;

        u8 *buf;
        int t_test = __cur_time2int(NULL);
        SPORT_COMMON *t_data = malloc(SIZEOF_END_DATA + sizeof(SPORT_COMMON));

        t_data->flag = 0x0;//开始时间包
        t_data->len  = SIZEOF_END_DATA;
        buf = sport_write_buf_alloc(sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        memcpy(buf, t_data, sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        sport_buf_write_updata(buf, sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
        free(t_data);
        __this_head->block++;
    }
    return stop;
}

int get_watch_sport_status()
{
    __sport_debug(__LINE__);
    if (!w_file) {
        return 0;
    }

    if (stop) {
        return 1;
    }

    return 2;
}

int watch_sport_stop()
{

    __sport_debug(__LINE__);
    if (!w_file) {
        return -1;
    }
    stop = 1;
    if (w_timer) {
        sys_timer_del(w_timer);
        w_timer = 0;
    }
    if (__execise_hd == NULL) {
        __execise_hd = zalloc(sizeof(struct watch_execise));
        watch_execise_handle_get(__execise_hd);
    }
    if (__execise_info == NULL) {
        __execise_info = zalloc(sizeof(struct motion_info));
    }
    __execise_hd->execise_info_get(__execise_info);

#if TCFG_NOR_VM

    if ((w_seek % BLOCK_SIZE)) {
        flash_common_write_packet(get_flash_vm_hd(F_TYPE_SPORTRECORD), (int)w_file, w_seek % BLOCK_SIZE, block_data_write);
        put_buf(block_data_write, w_seek % BLOCK_SIZE);
        printf(">>>>>>>>>>>>>>>>>>>%s %d %d\n", __FUNCTION__, __LINE__, w_seek);
        /* fseek(w_file, w_seek / BLOCK_SIZE * BLOCK_SIZE, SEEK_SET); */
        /* fwrite(w_file, block_data_write, BLOCK_SIZE); */
    }

    __this_tag->sport_sec = w_sport_time;
    __this_tag->end_time = __cur_time2int(NULL);
    __this_tag->distance = (u16)__execise_info->step_distance;
    __this_tag->calorie  = (u16)__execise_info->caloric;
    __this_tag->step     = (u32)__execise_info->step;
    __this_tag->recovery_time = (__execise_info->recoverytime_hour << 8 | __execise_info->recoverytime_min) & 0xff;
    __this_tag->heartrate_mode = (u8)__execise_info->intensity_mode;
    memcpy(__this_tag->exercise_intensity_buf, __execise_info->intensity_time, 20);

    printf("time_len-2=%x,time_end-4=%x,distance-2=%x,kcal-2=%x,step-4=%x,revovery_time-2=%x", \
           __this_tag->sport_sec, __this_tag->end_time, __this_tag->distance, __this_tag->calorie, __this_tag->step, __this_tag->recovery_time);
    __this_head->size = w_seek;


    SPORT_COMMON *end_data = malloc(SIZEOF_END_DATA + sizeof(SPORT_COMMON));
    end_data->flag = 0xff;
    end_data->len  = SIZEOF_END_DATA;
    memcpy(end_data->data, &__this_tag->end_time, sizeof(int));
    memcpy(block_data_write, end_data, sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
    memcpy(((u8 *)block_data_write) + sizeof(SPORT_COMMON) + SIZEOF_END_DATA, __this_tag, sizeof(SPORT_Tag));
    free(end_data);
    end_data = NULL;
    if (__execise_hd != NULL) {
        free(__execise_hd);
        __execise_hd = NULL;
    }
    if (__execise_info != NULL) {
        free(__execise_info);
        __execise_info = NULL;
    }
    /* fseek(w_file, w_seek, SEEK_SET); */

    flash_common_write_packet(get_flash_vm_hd(F_TYPE_SPORTRECORD), (int)w_file, sizeof(SPORT_COMMON) + SIZEOF_END_DATA + sizeof(SPORT_Tag), block_data_write);
    put_buf(block_data_write, sizeof(SPORT_COMMON) + SIZEOF_END_DATA + sizeof(SPORT_Tag));
    printf(">>>>>>>>>>>>>>>>>>>%s %d %d\n", __FUNCTION__, __LINE__, w_seek);

    flash_common_write_push(get_flash_vm_hd(F_TYPE_SPORTRECORD), (int)w_file);
    printf("__sport_distance%d", __this_tag->distance);
    if (__this_tag->distance < SPORT_MIN_DISTANCE) {
        flash_common_delete_by_id(get_flash_vm_hd(F_TYPE_SPORTRECORD), (int)w_file);
        printf(">>>>>>>sport_distance too small ");
        last_file = 0;
        return -1;
    }
    printf("%s", __func__);
    /* fwrite(w_file, block_data_write, sizeof(SPORT_COMMON) + SIZEOF_END_DATA + sizeof(SPORT_Tag)); */
    __this_head->block++;
    w_seek +=  sizeof(SPORT_COMMON) + SIZEOF_END_DATA ;
    w_seek +=  sizeof(SPORT_Tag);
    /* printf(">>>>>>>>>>>>>>>%s %d %x\n",__FUNCTION__,__LINE__,w_seek); */
    /* printf(">>>>>>>>>>>>>>>%s %d %x %x\n",__FUNCTION__,__LINE__,__this_head->block,fpos(w_file)); */
    __this_head->size = w_seek;
    __this_head->mask = 0xee;
    printf(">>>>>>>>>>>>>>>>>>>%s %d %d\n", __FUNCTION__, __LINE__, w_seek);
    flash_common_update_by_id(get_flash_vm_hd(F_TYPE_SPORTRECORD), (int)w_file, 0, sizeof(SPORT_Head), (u8 *)__this_head);
    /* fseek(w_file, 0, SEEK_SET); */
    /* fwrite(w_file, __this_head, sizeof(SPORT_Head)); */
    put_buf((u8 *)__this_head, sizeof(SPORT_Head));
    /* fseek(w_file, w_seek, SEEK_SET); */
    /* fclose(w_file); */
    last_file = w_file;
    w_file = NULL;
    w_sport_time = 0;
    sys_timeout_add(NULL, watch_sport_file_collating, 1000);
#else
    if ((w_seek % BLOCK_SIZE)) {
        fseek(w_file, w_seek / BLOCK_SIZE * BLOCK_SIZE, SEEK_SET);
        fwrite(w_file, block_data_write, BLOCK_SIZE);
    }
    __this_tag->sport_sec = w_sport_time;
    __this_tag->end_time = __cur_time2int(NULL);
    __this_tag->distance = get_sport_distance();
    __this_tag->calorie  = get_sport_kcal();
    __this_tag->step     = get_sport_step() ;
    __this_tag->recovery_time = get_recovery_time();
    __this_head->size = w_seek;


    SPORT_COMMON *end_data = malloc(SIZEOF_END_DATA + sizeof(SPORT_COMMON));
    end_data->flag = 0xff;
    end_data->len  = SIZEOF_END_DATA;
    memcpy(end_data->data, &__this_tag->end_time, sizeof(int));
    memcpy(block_data_write, end_data, sizeof(SPORT_COMMON) + SIZEOF_END_DATA);
    memcpy(((u8 *)block_data_write) + sizeof(SPORT_COMMON) + SIZEOF_END_DATA, __this_tag, sizeof(SPORT_Tag));
    free(end_data);
    end_data = NULL;

    fseek(w_file, w_seek, SEEK_SET);
    fwrite(w_file, block_data_write, sizeof(SPORT_COMMON) + SIZEOF_END_DATA + sizeof(SPORT_Tag));
    __this_head->block++;
    w_seek +=  sizeof(SPORT_COMMON) + SIZEOF_END_DATA ;
    w_seek +=  sizeof(SPORT_Tag);
    /* printf(">>>>>>>>>>>>>>>%s %d %x\n",__FUNCTION__,__LINE__,w_seek); */
    /* printf(">>>>>>>>>>>>>>>%s %d %x %x\n",__FUNCTION__,__LINE__,__this_head->block,fpos(w_file)); */
    __this_head->size = w_seek;
    __this_head->mask = 0xee;
    fseek(w_file, 0, SEEK_SET);
    fwrite(w_file, __this_head, sizeof(SPORT_Head));
    put_buf(__this_head, sizeof(SPORT_Head));
    fseek(w_file, w_seek, SEEK_SET);
    fclose(w_file);
    w_file = NULL;

#endif
    return 0;
}

int sportrecord_get_type()
{
    if (s_info) {
        return s_info->type;
    }
    return 0;
}

int sportrecord_get_start_time(struct sys_time *time)
{
    if (s_info) {
        memcpy(time, &s_info->stime, sizeof(struct sys_time));
        return 0;
    }

    time->sec = 30;
    time->min = 20;
    time->hour = 10;
    time->day = 9;
    time->month = 8;
    time->year = 2017;

    return 0;
}

int sportrecord_get_end_time(struct sys_time *time)
{
    if (s_info) {
        memcpy(time, &s_info->etime, sizeof(struct sys_time));
        return 0;
    }
    time->sec = 30;
    time->min = 20;
    time->hour = 20;
    time->day = 9;
    time->month = 8;
    time->year = 2017;
    return 0;
}

int sportrecord_get_run_time(struct sys_time *time)
{
    if (s_info) {
        time->day = s_info->sport_time / (60 * 60 * 24);
        time->hour = s_info->sport_time % (60 * 60 * 24) / (60 * 60);
        time->min = s_info->sport_time % (60 * 60 * 24) % (60 * 60) / 60;
        time->sec = s_info->sport_time % (60 * 60 * 24) % (60 * 60) % 60;
        return 0;
    }
    time->sec = 30;
    time->min = 10;
    time->hour = 3;
    time->day = 0;
    time->month = 0;
    time->year = 0;
    return 0;
}


int sportrecord_get_aerobic_time(struct sys_time *time)
{
    if (s_info) {
        time->hour = (u8) s_info->Intensity_time[4] / 60;
        time->min = (u8) s_info->Intensity_time[4] % 60;
        time->sec = 0;
        return 0;
    }
    time->sec = 30;
    time->min = 10;
    time->hour = 1;
    time->day = 0;
    time->month = 0;
    time->year = 0;
    return 0;
}

int sportrecord_get_anaerobic_time(struct sys_time *time)
{
    if (s_info) {
        time->hour = (u8) s_info->Intensity_time[3] / 60;
        time->min = (u8) s_info->Intensity_time[3] % 60;
        time->sec = 0;
        return 0;
    }

    time->sec = 30;
    time->min = 30;
    time->hour = 1;
    time->day = 0;
    time->month = 0;
    time->year = 0;
    return 0;
}


int sportrecord_get_fat_time(struct sys_time *time)
{
    if (s_info) {
        time->hour = (u8) s_info->Intensity_time[2] / 60;
        time->min = (u8) s_info->Intensity_time[2] % 60;
        time->sec = 0;
        return 0;
    }

    time->sec = 30;
    time->min = 20;
    time->hour = 1;
    time->day = 0;
    time->month = 0;
    time->year = 0;

    return 0;
}

int sportrecord_get_limit_time(struct sys_time *time)
{
    if (s_info) {
        time->hour = (u8) s_info->Intensity_time[1] / 60;
        time->min = (u8) s_info->Intensity_time[1] % 60;
        time->sec = 0;
        return 0;
    }

    time->sec = 30;
    time->min = 15;
    time->hour = 1;
    time->day = 0;
    time->month = 0;
    time->year = 0;
    return 0;
}

int sportrecord_get_warm_time(struct sys_time *time)
{

    if (s_info) {
        time->hour = (u8) s_info->Intensity_time[0] / 60;
        time->min = (u8) s_info->Intensity_time[0] % 60;
        time->sec = 0;
        return 0;
    }

    time->sec = 30;
    time->min = 5;
    time->hour = 1;
    time->day = 0;
    time->month = 0;
    time->year = 0;
    return 0;
}



int sportrecord_get_Hheart()
{
    return 95;
}

int sportrecord_get_Lheart()
{
    return 75;
}

int sportrecord_get_heat()
{
    if (s_info) {
        return s_info->calorie;
    }
    return 55;
}

int sportrecord_get_meanheart()
{
    return 85;
}

int sportrecord_get_distance(u8 *significan)
{
    if (s_info) {
        return s_info->distance;
    }
    return 1234;
}

int sportrecord_get_recovertime(struct sys_time *time)
{
    if (s_info) {
        time->sec = 0;
        time->min = 0;
        time->hour = s_info->recovery_time >> 2;
        time->day = 0;
        time->month = 0;
        time->year = 0;
        return 0;
    }

    time->sec = 00;
    time->min = 00;
    time->hour = 44;
    time->day = 0;
    time->month = 0;
    time->year = 0;
    return 0;
}



static const u8 FILE_SCAN_PARAM[] = "-t"
                                    "SPT"
                                    " -sn -r"
                                    ;

static struct vfscan *fs;

int sport_file_scan_init()
{
    FILE *file;
    int total = 0;
    u8 name_buf[16];
    int len = 0;
    fs = fscan(SPORT_REC_PATH, FILE_SCAN_PARAM, 9);
    if (!fs) {
        return 0;
    }

    /* printf(">>> file number=%d \n",fs->file_number); */
    /* total = fs->file_number;  */
    /*  */
    /* for(int i = 1;i <= fs->file_number; i++){ */
    /*     file = fselect(fs, FSEL_BY_NUMBER, i); */
    /*     if(!file){ */
    /*         break; */
    /*     } */
    /*     len = fget_name(file, name_buf, sizeof(name_buf)); */
    /*     if (name_buf[0] == '\\' && name_buf[1] == 'U') { */
    /*         printf(">>>>>>>>>>>>>>>>>%s %d\n",__FUNCTION__,__LINE__); */
    /*     } else { */
    /*         printf("%s\n",name_buf); */
    /*     } */
    /*     fclose(file); */
    /*     file = NULL; */
    /* } */
    /* fscan_release(fs); */
    return total;
}

int sport_file_scan_release()
{
    if (!fs) {
        return -1;
    }
    fscan_release(fs);
    fs = NULL;
    return 0;
}



int sport_file_get_info_by_index(int index, int count, SPORT_INFO *info)
{
    u8 buf[SIZEOF_START_DATA + sizeof(SPORT_COMMON)] __attribute__((aligned(4)));
    SPORT_Head head = {0};
    SPORT_Tag  s_tag  = {0};
    FILE *file;
    if (!fs) {
        memset(info, 0, sizeof(SPORT_INFO));
        return -1;
    }
    if (!index) {
        memset(info, 0, sizeof(SPORT_INFO));
        return -1;
    }

    for (int i = 0; i < count ; i++) {
        info = info + i;
        file = fselect(fs, FSEL_BY_NUMBER, index + i);
        if (!file) {
            memset(info, 0, sizeof(SPORT_INFO));
            return -1;
        }
        info->type = head.sport_mode;
        SPORT_COMMON *start_data = (SPORT_COMMON *)buf;
        fread(file, start_data, SIZEOF_START_DATA + sizeof(SPORT_COMMON));
        __int2time(*((int *)start_data->data), &s_info->stime);
        int size = head.size;
        int offset = size - sizeof(SPORT_Tag);
        fseek(file, offset, SEEK_SET);
        fread(file, &s_tag, sizeof(SPORT_Tag));
        info->sport_time  = s_tag.sport_sec;
        __int2time(s_tag.end_time, &s_info->etime);
        info->distance = s_tag.distance;
        info->calorie = s_tag.calorie;
        info->step = s_tag.step;
        info->recovery_time = s_tag.recovery_time;
        if (file) {
            fclose(file);
            file = NULL;
        }
    }

    return 0;
}


u16 get_sport_recode_id(void)
{
    return (u16)last_file;
}
u16 get_sport_recode_size(void)
{
    if (last_file == 0) {
        return (u16)0;
    } else {
        return (u16)flash_common_get_file_size_by_id(get_flash_vm_hd(F_TYPE_SPORTRECORD), (int)last_file);
    }
}
#endif