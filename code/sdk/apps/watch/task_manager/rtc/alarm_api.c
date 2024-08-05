#include "system/includes.h"
#include "rtc/alarm.h"
#include "common/app_common.h"
#include "system/timer.h"
#include "app_main.h"
#include "tone_player.h"
#include "app_task.h"
#include "app_config.h"

#if TCFG_PAY_ALIOS_ENABLE
#if (TCFG_PAY_ALIOS_WAY_SEL == TCFG_PAY_ALIOS_WAY_ALIYUN)
#include "upay.h"
#define ALIOS_SYNC_TIME_DONE()				upay_vendor_sync_time_done()
#else
#include "alipay.h"
#define ALIOS_SYNC_TIME_DONE()				alipay_vendor_sync_time_done()
#endif
#endif

#if TCFG_APP_RTC_EN
#ifdef  RTC_ALM_EN
/* #define ALARM_DEBUG_EN */
#ifdef ALARM_DEBUG_EN
#define alarm_printf        printf
#define alarm_putchar       putchar
#define alarm_printf_buf    put_buf
#else
#define alarm_printf(...)
#define alarm_putchar(...)
#define alarm_printf_buf(...)
#endif

#define PRINT_FUN()              alarm_printf("func : %s\n", __FUNCTION__)
#define PRINT_FUN_RETURN_INFO()  alarm_printf("func : %s, line : %d.\n", __FUNCTION__, __LINE__)

#define MAX_YEAR          2099
#define MIN_YEAR          2000

/*************************************************************
   此文件函数主要是rtc闹钟的实现代码
   用户可以不主要关心实现，专心留意api调用
   常用的api 有:

u8 alarm_add(PT_ALARM p, u8 index);
增加闹钟

void alarm_delete(u8 index);
删除闹钟

void rtc_update_time_api(struct sys_time *time)
更新时间api(更新了时间必须调用该接口)

u8 alarm_active_flag_get(void);
闹钟到达的标志

u8 alarm_get_info(PT_ALARM p, u8 index)
获取闹钟信息

u8 alarm_get_active_index(void);
获取当前激活使能的闹钟(可以理解为下一个会响的闹钟)


u8 alarm_get_total(void)
获取当前已经设备的闹钟数(包括关闭的闹钟)


void rtc_calculate_next_few_day(struct sys_time *data_time,u8 days)
获取今天星期几


void alarm_name_set(u8 *p, u8 index, u8 len)
设置闹钟名字


u8 alarm_name_get(u8 *p, u8 index)
获取闹钟名字
**************************************************************/

struct p11_sys_time {
    u32 mask;
    struct sys_time ram_time;
};


static volatile u8  g_alarm_active_flag = 0;
static volatile u8  alarm_cur_active = 0;//当前在响的闹钟
static volatile u8  snooze_time = 10;//单位：分钟
static struct p11_sys_time *p11_rtc_time = 0;


typedef struct __alarm_map__ {
    u32 mask: 16;
u32 map :
    ((M_MAX_ALARM_NUMS + M_MAX_SNOOZE_ALARM_NUM + 7) / 8 * 8); //存储了闹钟数目信息（闹钟数）, 第BIT(M_MAX_ALARM_NUMS + 1)为贪睡闹钟
u32 map_sw  :
    ((M_MAX_ALARM_NUMS + M_MAX_SNOOZE_ALARM_NUM + 7) / 8 * 8); //存储闹钟的使能开关(闹钟开关),第BIT(M_MAX_ALARM_NUMS + 1)为贪睡闹钟
u32 active_map :
    ((M_MAX_ALARM_NUMS + M_MAX_SNOOZE_ALARM_NUM + 7) / 8 * 8); //存储最后激活闹钟,第BIT(M_MAX_ALARM_NUMS + 1)为贪睡闹钟
    u8  table[M_MAX_ALARM_NUMS];//闹钟排序
} T_ALARM_MAP, *PT_ALARM_MAP;


/* volatile u8  g_alarm_ring_max_cnt = 100; */

static T_ALARM alarm_tab[M_MAX_ALARM_NUMS + M_MAX_SNOOZE_ALARM_NUM] = {0};//数组最后一个为贪睡闹钟
static u8 alarm_name[M_MAX_ALARM_NAME_LEN] = {0};

#define RTC_MASK (0x55aa+M_MAX_ALARM_NAME_LEN)

static void *dev_handle;

T_ALARM_MAP alarm_map = {0};



/*----------------------------------------------------------------------------*/
/* debug 函数代码 */
/*----------------------------------------------------------------------------*/


void alarm_vm_puts_info(PT_ALARM p)
{
    alarm_printf("index : %d\n", p->index);
    alarm_printf("mode: %d\n", p->mode);
    alarm_printf("sw: %d\n", p->sw);
    /* alarm_puts_time(&(p->time)); */
}

void alarm_puts_time(struct sys_time *pTime)
{
    /* u32 rets_addr; */
    /* __asm__ volatile("%0 = rets ;" : "=r"(rets_addr)); */
    /* g_printf("%s:0x%x", __func__, rets_addr); */
    u8 week = 0;

#if 1
    alarm_printf("alarm_time : %d-%d-%d,%d:%d:%d\n", pTime->year, pTime->month, pTime->day, pTime->hour, pTime->min, pTime->sec);
    week = rtc_calculate_week_val(pTime);
#endif
    alarm_printf("alarm week : %d\n", week);
}

/*----------------------------------------------------------------------------*/
/* rtc 硬化相关代码*/
/*----------------------------------------------------------------------------*/
void *is_sys_time_online()
{
    return dev_handle;
}


static void get_sys_time(struct sys_time *time)//获取时间
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        printf("[ERROR] open rtc error \n");
        return;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
    //printf(">>>>>>>>>>>>>>>>>Get systime : %d-%d-%d,%d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec);
    dev_close(fd);

    return;
    // if (!dev_handle) {
    //     return ;
    // }
    // dev_ioctl(dev_handle, IOCTL_GET_SYS_TIME, (u32)time);
}

static void set_sys_time(struct sys_time *time)//设置时间
{
    if (!dev_handle) {
        return ;
    }
    dev_ioctl(dev_handle, IOCTL_SET_SYS_TIME, (u32)time);
#if TCFG_PAY_ALIOS_ENABLE
    ALIOS_SYNC_TIME_DONE(); //更新时间
#endif /* #if TCFG_PAY_ALIOS_ENABLE */
}

void alarm_hw_set_sw(u8 sw)//闹钟开关
{
    /* printf("alarm sw : %d\n", sw); */
    if (!dev_handle) {
        return ;
    }
    dev_ioctl(dev_handle, IOCTL_SET_ALARM_ENABLE, !!sw);
}

void alarm_hw_write_time(struct sys_time *time, u8 sw)//写闹钟寄存器
{
    if (!dev_handle) {
        return ;
    }
    alarm_printf("alarm_time : %d-%d-%d,%d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec);

    alarm_hw_set_sw(0);
    dev_ioctl(dev_handle, IOCTL_SET_ALARM, (u32)time);
    alarm_hw_set_sw(sw);
}




/*----------------------------------------------------------------------------*/
/* vm读写操作部分代码 */
/*----------------------------------------------------------------------------*/

static void alarm_vm_reset()
{
    int i = 0;
    int ret = 0;
    T_ALARM_MAP map_temp = {0};
    memset(&map_temp, 0x0, sizeof(T_ALARM_MAP));
    memset(&map_temp.table, 0xff, M_MAX_ALARM_NUMS);
    map_temp.mask = RTC_MASK;
    ret = syscfg_write(VM_ALARM_MASK, (u8 *)&map_temp, sizeof(T_ALARM_MAP));
    if (ret != sizeof(T_ALARM_MAP)) {
        PRINT_FUN_RETURN_INFO();
        return;
    }

    for (i = 0; i < (M_MAX_ALARM_NUMS + M_MAX_SNOOZE_ALARM_NUM); i++) {
        T_ALARM_VM tmp = {0};
        tmp.head = RTC_MASK;
        tmp.alarm.index =  i;
        ret = syscfg_write(VM_ALARM_0 + i, &tmp, sizeof(T_ALARM_VM));
        if (ret != sizeof(T_ALARM_VM)) {
            alarm_printf("The %d alarm write vm err!\n", index);
            return;
        }
    }

}

//写闹钟map表
static void alarm_vm_write_mask(PT_ALARM_MAP map)
{
    int ret = 0;
    PRINT_FUN();
    T_ALARM_MAP map_temp = {0};
    memcpy(&map_temp, map, sizeof(T_ALARM_MAP));
    if (map_temp.mask != RTC_MASK) {
        memset(&map_temp, 0x0, sizeof(T_ALARM_MAP));
        map_temp.mask = RTC_MASK;
    }

    ret = syscfg_write(VM_ALARM_MASK, (u8 *)&map_temp, sizeof(T_ALARM_MAP));
    if (ret != sizeof(T_ALARM_MAP)) {
        PRINT_FUN_RETURN_INFO();
        return;
    }
}

//获取闹钟所有信息
static void alarm_vm_read_info(PT_ALARM_MAP map)
{
    PRINT_FUN();
    T_ALARM_VM tmp;
    int ret = 0;
    u8 i;

    ret = syscfg_read(VM_ALARM_MASK, (u8 *)map, sizeof(T_ALARM_MAP));
    if (ret != sizeof(T_ALARM_MAP) || map->mask != RTC_MASK) 
    {
        PRINT_FUN_RETURN_INFO();
        memset(map, 0, sizeof(T_ALARM_MAP));
        map->mask = RTC_MASK;
        memset(map->table, 0xff, M_MAX_ALARM_NUMS);
        alarm_vm_reset();
        return;
    }

    //贪睡闹钟不记忆
    for (i = 0; i < (M_MAX_ALARM_NUMS); i++) 
    {
        if((map->map) & BIT(i)) 
        {
            ret = syscfg_read(VM_ALARM_0 + i, &tmp, sizeof(T_ALARM_VM));
            if (ret != sizeof(T_ALARM_VM) || tmp.head != RTC_MASK) {
                alarm_printf("can't find the %d alarm from vm.\n", i);
                memset(&(alarm_tab[i]), 0x00, sizeof(T_ALARM));
                continue;
            }

            printf("vm info : index=%d, sw=%d, mode=%d, h=%d, m=%d, name_len=%d\n", tmp.alarm.index, tmp.alarm.sw, tmp.alarm.mode, \
                   tmp.alarm.time.hour, tmp.alarm.time.min, tmp.alarm.name_len);
            memcpy(&alarm_tab[i], &(tmp.alarm), sizeof(T_ALARM));
            if (alarm_tab[i].sw) {
                map->map_sw |= BIT(i);
            } else {
                map->map_sw &= ~(BIT(i));
            }
        }
    }
}


//更新闹钟信息
static void alarm_vm_write_info_by_index(PT_ALARM_MAP map, u8 index)
{

    PRINT_FUN();
    s32 ret = 0;
    T_ALARM_VM tmp;
    tmp.head = RTC_MASK;
    memcpy(&(tmp.alarm), &alarm_tab[index], sizeof(T_ALARM));
    tmp.alarm.index =  index;
    ret = syscfg_write(VM_ALARM_0 + index, &tmp, sizeof(T_ALARM_VM));
    if (ret != sizeof(T_ALARM_VM)) {
        alarm_printf("The %d alarm write vm err!\n", index);
        return;
    }

    alarm_printf("vm info : index=%d, sw=%d, mode=%d, h=%d, m=%d, name_len=%d\n", tmp.alarm.index, tmp.alarm.sw, tmp.alarm.mode, \
                 tmp.alarm.time.hour, tmp.alarm.time.min, tmp.alarm.name_len);
    alarm_vm_write_mask(map);
    return;
}


/*----------------------------------------------------------------------------*/
/*闹钟名字部分代码 */
/*----------------------------------------------------------------------------*/
static void alarm_vm_write_name(u8 *p, u8 index)
{
    PRINT_FUN();

    s32 ret = 0;

    ret = syscfg_write(VM_ALARM_NAME_0 + index, p, sizeof(alarm_name));
    if (ret < 0) {
        PRINT_FUN_RETURN_INFO();
        return;
    }

    return;
}

static void alarm_vm_read_name(u8 *p, u8 index)
{
    PRINT_FUN();

    s32 ret = 0;

    ret = syscfg_read(VM_ALARM_NAME_0 + index, p, sizeof(alarm_name));
    if (ret < 0) {
        PRINT_FUN_RETURN_INFO();
        return;
    }

    return;
}

void alarm_name_clear(void)
{
    PRINT_FUN();
    memset(alarm_name, 0x00, sizeof(alarm_name));
    return;
}


void alarm_name_set(u8 *p, u8 index, u8 len)
{
    PRINT_FUN();

    if (index > M_MAX_ALARM_INDEX) {
        PRINT_FUN_RETURN_INFO();
        alarm_printf("alarm is full!\n");
        return;
    }


    if ((len == 0) || (len > sizeof(alarm_name))) {
        PRINT_FUN_RETURN_INFO();
        return;
    }

    alarm_name_clear();

    alarm_printf("alarm name len : %d\n", len);
    alarm_printf_buf(p, len);

    memcpy(alarm_name, p, len);
    alarm_vm_write_name(alarm_name, index);

    return;
}

u8 alarm_name_get(u8 *p, u8 index)
{
    PRINT_FUN();

    u8 name_len = 0;

    if (index > M_MAX_ALARM_INDEX) {
        PRINT_FUN_RETURN_INFO();
        return 0;
    }

    alarm_vm_read_name(alarm_name, index);

    name_len = alarm_tab[index].name_len;
    memcpy(p, alarm_name, name_len);

    alarm_printf("alarm name len : %d\n", name_len);
    alarm_printf_buf(alarm_name, name_len);

    return name_len;
}



/*----------------------------------------------------------------------------*/
/*工具函数部分代码 */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**@brief	月份换算为月天数
   @param 	month，year
   @return  月份天数
   @note
*/
/*----------------------------------------------------------------------------*/
u16 month_for_day(u8 month, u16 year)
{
    extern u16 month_to_day(u16 year, u8 month);
    return month_to_day(year, month);
}


void rtc_calculate_next_few_min(struct sys_time *cur_time, struct sys_time *p_time, u8 min)
{
    if (!min || min >= 60) {
        return;
    }
    memcpy(p_time, cur_time, sizeof(struct sys_time));
    p_time->min = cur_time->min + min;
    if (p_time->min >= 60) {
        p_time->min -= 60;
        if (++p_time->hour >= 24) {
            p_time->hour = 0;
            if (++p_time->day > month_for_day(p_time->month, p_time->year)) {
                p_time->day = 1;
                if (++p_time->month > 12) {
                    p_time->month = 1;
                    if (++p_time->year > MAX_YEAR) {
                        p_time->year = MIN_YEAR;
                    }
                }
            }
        }
    }
    p_time->sec = 0;
    alarm_puts_time(p_time);
}


/*----------------------------------------------------------------------------*/
/**@brief  计算未来几天的日期 days要小于29,防止跨两个月
   @param  data_time--计算日期
   @return none
   @note
*/
/*----------------------------------------------------------------------------*/
void rtc_calculate_next_few_day(struct sys_time *data_time, u8 days)
{
    if (!days || days >= 29) {
        return;
    }
    u16 tmp16 = month_for_day(data_time->month, data_time->year);
    data_time->day += days;
    if (data_time->day > tmp16) {
        data_time->month++;
        data_time->day -= tmp16;
        if (data_time->month > 12) {
            data_time->month = 1;
            data_time->year++;
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  日期转换为星期
   @param  data_time--日期
   @return 星期
   @note
*/
/*----------------------------------------------------------------------------*/
//蔡勒（Zeller）公式：w=y+[y/4]+[c/4]-2c+[26x(m+1)/10]+d-1
u8 rtc_calculate_week_val(struct sys_time *data_time)
{
    struct sys_time t_time;
    u32 century, val, year;

    memcpy(&t_time, data_time, sizeof(struct sys_time));
    if (t_time.month < 3) {
        t_time.month = t_time.month + 12;
        t_time.year--;
    }
    year = t_time.year % 100;
    century = t_time.year / 100;
    val = year + (year / 4) + (century / 4) + (26 * (t_time.month + 1) / 10) + t_time.day;
    val = val - century * 2 - 1;

    return (u8)(val % 7);
}


static int __alarm_cmp_time_num(u32 num1, u32 num2)
{
    int ret = -2;

    if (num1 > num2) {
        ret = 1;
    } else if (num1 == num2) {
        ret = 0;
    } else if (num1 < num2) {
        ret = -1;
    }

    return ret;
}

/*
 *  函数功能 ：比较两个闹钟的时间
 *  函数形参 ：time1 - 闹钟1；time2 - 闹钟2
 *  返回值   ：0-两闹钟相等；1-闹钟1时间比对比闹钟2要晚；-1-闹钟1比对比闹钟2早 -2-比较出错
 *  备注     ：无
 *
 * */
static int alarm_cmp_time_member(struct sys_time *time1, struct sys_time *time2, TIME_MEMBER_ENUM type)
{
    switch (type) {
    case TIME_MEMBER_YEAR:
        return __alarm_cmp_time_num(time1->year,  time2->year);
    case TIME_MEMBER_MONTH:
        return __alarm_cmp_time_num(time1->month, time2->month);
    case TIME_MEMBER_DAY:
        return __alarm_cmp_time_num(time1->day,   time2->day);
    case TIME_MEMBER_HOUR:
        return __alarm_cmp_time_num(time1->hour,  time2->hour);
    case TIME_MEMBER_MIN:
        return __alarm_cmp_time_num(time1->min,   time2->min);
    case TIME_MEMBER_SEC:
        return __alarm_cmp_time_num(time1->sec,   time2->sec);

    default:
        return -2;
    }

    return -2;
}

/*
 *  函数功能 ：比较两个闹钟的时间
 *  函数形参 ：time1 - 闹钟1；time2 - 闹钟2
 *  返回值   ：0-两闹钟相等；1-闹钟1时间比对比闹钟2要晚；-1-闹钟1比对比闹钟2早 -2-比较出错
 *  备注     ：无
 *
 * */
static int alarm_cmp_time(struct sys_time *time1, struct sys_time *time2)
{
    u8 i;
    int ret = 0;

    for (i = 0; i < TIME_MEMBER_MAX; i++) {
        ret = alarm_cmp_time_member(time1, time2, i);
        if (ret != 0) {
            break;
        }
    }

    return ret;
}




/*----------------------------------------------------------------------------*/
/*核心部分代码 */
/*----------------------------------------------------------------------------*/



/*
**  函数功能 ：根据闹钟的模式计算出实际时间
**  函数形参 ：pTime-闹钟时间结构体；week-当下星期； mode-闹钟模式
**  返回值   ：void
**  备注     ：无
*/

static void __alarm_calc_time_by_week_mode(struct sys_time *pTime, u8 mode)
{
    PRINT_FUN();
    u8 i = 0;
    u8 alarm_week = 0;
    u8 tmp_mode = 0;
    alarm_week = rtc_calculate_week_val(pTime);//alarm_week 可以理解为最近可以设置的闹钟（忽略week）
    if (alarm_week == 0) {
        alarm_week = 7;       //星期天写成7，方便对比计算
    }

    //查找当前可以设置闹钟日期最近的日期
    for (i = 1; i < 8; i++) {
        if (mode & BIT(i)) {
            if (i >= alarm_week) {
                tmp_mode = i;
                break;
            }
        }
    }

    if (i >= 8) {//翻越了星期的
        for (i = 1; i < 8; i++) {
            if (mode & BIT(i)) {
                tmp_mode = i;
                break;
            }
        }
    }

    if ((tmp_mode >= 1) && (tmp_mode < 8)) {
        if (tmp_mode > alarm_week) {
            alarm_printf("***a***\n");
            //没有翻越星期
            rtc_calculate_next_few_day(pTime, tmp_mode - alarm_week);
        } else if (tmp_mode < alarm_week) {
            //翻越了星期
            alarm_printf("***b***\n");
            rtc_calculate_next_few_day(pTime, 7 - (alarm_week - tmp_mode));
        }
    }
    return;
}



/*
**  函数功能 ：根据闹钟的时、分计算出它的贪睡时间（年、月、日、时、分、秒）
**  函数形参 ：带有时、分和闹钟模式的时间结构体
**  返回值   ：void
**  备注     ：无
*/
static void alarm_calc_snooze_time_by_index(struct sys_time *cTime, u8 index)
{
    PT_ALARM pAlarm_tab;
    pAlarm_tab = &(alarm_tab[index]);
    if (index == (M_MAX_ALARM_INDEX + M_MAX_SNOOZE_ALARM_NUM)) {//如果是贪睡闹钟
        rtc_calculate_next_few_min(cTime, &pAlarm_tab->time, snooze_time);
    }
}



/*
**  函数功能 ：根据闹钟的时、分计算出它具体的时间（年、月、日、时、分、秒）
**  函数形参 ：带有时、分和闹钟模式的时间结构体
**  返回值   ：void
**  备注     ：无
*/
static void alarm_calc_real_time_by_index(struct sys_time *cTime, u8 index)
{
    struct sys_time tmp = {0};
    PT_ALARM pAlarm_tab;

    if (index > M_MAX_ALARM_INDEX + M_MAX_SNOOZE_ALARM_NUM) {
        PRINT_FUN_RETURN_INFO();
        return;
    }

    pAlarm_tab = &(alarm_tab[index]);
    struct sys_time *pTime = &(pAlarm_tab->time);

    if (pAlarm_tab->mode > M_MAX_ALARM_MODE) {
        PRINT_FUN_RETURN_INFO();
        return;
    }
    u32 c_tmp = ((cTime->hour & 0x1f) << 12) | ((cTime->min & 0x3f) << 6) | (cTime->sec & 0x3f);
    u32 p_tmp = ((pTime->hour & 0x1f) << 12) | ((pTime->min & 0x3f) << 6) | (pTime->sec & 0x3f);

    if (p_tmp > c_tmp) { //时间未到

        PRINT_FUN_RETURN_INFO();
        pTime->year  = cTime->year;
        pTime->month = cTime->month;
        pTime->day   = cTime->day;
        pTime->sec   = 0;
    } else {
        PRINT_FUN_RETURN_INFO();
        memcpy(&tmp, cTime, sizeof(struct sys_time));
        rtc_calculate_next_few_day(&tmp, 1);
        pTime->year  = tmp.year;
        pTime->month = tmp.month;
        pTime->day   = tmp.day;
        pTime->sec   = 0;
    }

    if ((pAlarm_tab->mode != E_ALARM_MODE_ONCE) && (pAlarm_tab->mode != E_ALARM_MODE_EVERY_DAY)) {
        __alarm_calc_time_by_week_mode(pTime, pAlarm_tab->mode);
    }
    alarm_puts_time(pTime);
}

static void __alarm_get_the_earliest(void)
{
    PRINT_FUN();
    int ret = 0;
    u8 index = 0;
    u8 i = 0;

    struct sys_time *pTmp = NULL;
    alarm_map.active_map = 0 ;
    for (i = 0; i < (M_MAX_ALARM_NUMS + M_MAX_SNOOZE_ALARM_NUM); i++) {
        if (alarm_map.map_sw & BIT(i)) {
            alarm_map.active_map |= BIT(i);
            pTmp = &(alarm_tab[i].time);
            index = i;
            break;
        }
    }


    if (i >= (M_MAX_ALARM_NUMS + M_MAX_SNOOZE_ALARM_NUM)) {
        alarm_printf("***no alarm***\n");
        alarm_hw_set_sw(0);
        return;
    }

    for (i = index + 1; i < (M_MAX_ALARM_NUMS + M_MAX_SNOOZE_ALARM_NUM); i++) 
    {
        if (alarm_map.map_sw & BIT(i)) {
            ret = alarm_cmp_time(pTmp, &(alarm_tab[i].time));
            if (0 == ret) {
                alarm_map.active_map |= BIT(i);
                alarm_printf("***A***\n");
            } else if (1 == ret) {
                alarm_printf("***B***\n");
                pTmp = &(alarm_tab[i].time);
                index = i;
                alarm_map.active_map = 0;
                alarm_map.active_map |= BIT(i);
            }
        }
    }

    /* alarm_puts_time(pTmp); */
    /* printf("find the %dth alarm, the save alarm : %x\n", index, alarm_map.active_map); */
    alarm_hw_write_time(pTmp, alarm_tab[index].sw);
    alarm_puts_time(pTmp);
    return;
}


static void __alarm_update_all_time(struct sys_time *cTIME)
{
    PRINT_FUN();
    u8 i = 0;
    for (i = 0; i < M_MAX_ALARM_NUMS + M_MAX_SNOOZE_ALARM_NUM; i++) {
        if (alarm_map.map_sw & BIT(i)) {
            alarm_calc_real_time_by_index(cTIME, i);
        }
    }

    return;
}


static void alarm_update()
{
    PRINT_FUN();
    struct sys_time current_time = {0};

    if (!is_sys_time_online()) {
        return ;
    }

    get_sys_time(&current_time);
    local_irq_disable();
    __alarm_update_all_time(&current_time);
    local_irq_enable();
    __alarm_get_the_earliest();
}


u8 alarm_get_active_index(void)
{
    return alarm_cur_active;
}

u8 alarm_get_info(PT_ALARM p, u8 index)
{
    u8 ret = E_SUCCESS;

    local_irq_disable();
    if (alarm_map.map & BIT(index)) {
        p->index = alarm_tab[index].index;
        p->sw = alarm_tab[index].sw;
        p->mode = alarm_tab[index].mode;
        p->time.hour = alarm_tab[index].time.hour;
        p->time.min = alarm_tab[index].time.min;
        p->name_len = alarm_tab[index].name_len;
    } else {
        memset(p, 0x0, sizeof(T_ALARM));
        ret = E_FAILURE;
    }
    local_irq_enable();
    return ret;
}

u8 alarm_get_total(void)
{
    PRINT_FUN();

    u8 total = 0;
    u8 i = 0;

    local_irq_disable();
    for (i = 0; i < M_MAX_ALARM_NUMS; i++) {
        if (alarm_map.map & BIT(i)) {
            total++;
        }
    }
    local_irq_enable();
    alarm_printf("total %d alarm\n", total);

    return total;
}

void rtc_update_time_api(struct sys_time *time)
{
    if (!is_sys_time_online()) {
        return ;
    }
    set_sys_time(time);
    local_irq_disable();
    __alarm_update_all_time(time);
    local_irq_enable();
    __alarm_get_the_earliest();
    alarm_vm_write_mask(&alarm_map);

    printf("%s\n", __func__);
}


void alarm_update_info_after_isr(void)
{
    PRINT_FUN();
    struct sys_time time = {0};

    if (!is_sys_time_online()) {
        return ;
    }

    get_sys_time(&time);
    u8 i = 0;
    printf("alarm_map.active_map =%x\n", alarm_map.active_map);
    local_irq_disable();
    alarm_cur_active = alarm_map.active_map;
    alarm_calc_snooze_time_by_index(&time, M_MAX_ALARM_NUMS);
    for (i = 0; i < (M_MAX_ALARM_NUMS + M_MAX_SNOOZE_ALARM_NUM); i++) {
        if (alarm_map.active_map & BIT(i)) {
            if (alarm_tab[i].mode != 0) {
                //闹钟不只响一次
                //计算一次闹钟的时间
                alarm_calc_real_time_by_index(&time, i);
            } else {
                //闹钟只响一次
                alarm_map.map_sw &= ~BIT(i);
                alarm_tab[i].sw = 0;
                local_irq_enable();
                alarm_vm_write_info_by_index(&alarm_map, i);
                local_irq_disable();
            }
        }
    }
    local_irq_enable();
    __alarm_get_the_earliest();
    alarm_vm_write_mask(&alarm_map);
}


u8 alarm_add(PT_ALARM p, u8 index)
{

    struct sys_time current_time = {0};
    PRINT_FUN();
    u8 ret = E_SUCCESS;

    if (!is_sys_time_online()) {
        alarm_printf("alarm is not open!\n");
        return E_FAILURE;
    }

    if (index > M_MAX_ALARM_INDEX) {
        PRINT_FUN_RETURN_INFO();
        alarm_printf("alarm is full!\n");
        return E_FAILURE;
    }

    if (p->mode > M_MAX_ALARM_MODE) {
        PRINT_FUN_RETURN_INFO();
        alarm_printf("alarm's mode is error");
        return E_FAILURE;
    }


    local_irq_disable();
    if (alarm_map.table[index] == 0xff) {
        int max = -1;
        for (int i = 0; i < M_MAX_ALARM_NUMS; i++) {
            if (alarm_map.table[i] != 0xff && alarm_map.table[i] > max) {
                max = alarm_map.table[i];
            }
        }
        max++;
        alarm_map.table[index] = max;
    }

    alarm_map.map |= BIT(index);
    if (0 == p->sw) {
        alarm_printf("close the %dth alarm!\n", p->index);
        alarm_map.map_sw &= ~BIT(p->index);
    } else if (1 == p->sw) {
        alarm_printf("set the %dth alarm!\n", p->index);
        alarm_map.map_sw |= BIT(p->index);
    }
    
    alarm_tab[index].index      = p->index;
    alarm_tab[index].sw         = p->sw;
    alarm_tab[index].mode       = p->mode;
    alarm_tab[index].time.hour = p->time.hour;
    alarm_tab[index].time.min  = p->time.min;
    alarm_tab[index].name_len   = p->name_len;

    get_sys_time(&current_time);
    alarm_calc_real_time_by_index(&current_time, index);//根据当前时间和闹钟模式计算出最新闹钟时间
    local_irq_enable();
    __alarm_get_the_earliest();
    alarm_vm_write_info_by_index(&alarm_map, index);
    return ret;
}


void alarm_delete(u8 index)
{
    PRINT_FUN();
    u8 number;
    if (index > M_MAX_ALARM_INDEX) {
        PRINT_FUN_RETURN_INFO();
        alarm_printf("alarm is full!\n");
        return;
    }
    alarm_printf("delete the %dth alarm!\n", index);

    local_irq_disable();

    if (alarm_map.table[index] == 0xff) {
        local_irq_enable();
        printf("alarm delete manage err \n");
        return;
    }
    number =  alarm_map.table[index];
    alarm_map.table[index] = 0xff;
    for (int i = 0; i < M_MAX_ALARM_NUMS; i++) {
        if (alarm_map.table[i] != 0xff && alarm_map.table[i] >  number) {
            alarm_map.table[i]--;
        }
    }

    alarm_map.map &= ~BIT(index);
    alarm_map.map_sw &= ~BIT(index);
    alarm_tab[index].sw  = 0;
    local_irq_enable();
    __alarm_get_the_earliest();
    alarm_vm_write_info_by_index(&alarm_map, index);
    return;
}

void alarm_delete_all(void)
{
    u8 flag[M_MAX_ALARM_NUMS] = {0};
    PRINT_FUN();

    local_irq_disable();

    for (int i = 0; i < M_MAX_ALARM_NUMS; i++) {
        if (alarm_map.table[i] != 0xff) {
            flag[i] = 1;
        }
        alarm_map.table[i] = 0xff;
        alarm_map.map &= ~BIT(i);
        alarm_map.map_sw &= ~BIT(i);
        alarm_tab[i].sw  = 0;
    }

    local_irq_enable();
    __alarm_get_the_earliest();
    for (int i = 0; i < M_MAX_ALARM_NUMS; i++) {
        if (flag[i]) {
            alarm_vm_write_info_by_index(&alarm_map, i);
        }
    }
    return;
}

void alarm_snooze()
{
    local_irq_disable();
    alarm_map.map |= BIT(M_MAX_ALARM_NUMS);
    alarm_map.map_sw |= BIT(M_MAX_ALARM_NUMS);
    alarm_tab[M_MAX_ALARM_NUMS].sw  = 1;
    local_irq_enable();
    __alarm_get_the_earliest();
    alarm_vm_write_info_by_index(&alarm_map, M_MAX_ALARM_NUMS);
}

u8 get_alarm_number2table(u8 num, u8 *table)
{
    u8 idle = 0xff;
    local_irq_disable();
    for (int i = 0; i < M_MAX_ALARM_NUMS; i++) {
        //printf(">>>>>%s %d\n",__FUNCTION__,alarm_map.table[i]);
        if (alarm_map.table[i] != 0xff && alarm_map.table[i] ==  num) {
            local_irq_enable();
            *table = i;
            return 1;
        } else if (alarm_map.table[i] == 0xff && idle == 0xff) {
            idle = i;
        }
    }
    local_irq_enable();
    *table = idle;
    return 0;
}

void alarm_active_flag_set(u8 flag)
{
    g_alarm_active_flag = flag;

    return;
}

u8 alarm_active_flag_get(void)
{
    return g_alarm_active_flag;
}

static void alarm_check(void *priv)
{
    extern APP_VAR app_var;
    extern u32 timer_get_ms(void);
    if ((timer_get_ms() - app_var.start_time) > 3000) {
        struct sys_event e;
        e.type = SYS_DEVICE_EVENT;
        e.arg  = (void *)DEVICE_EVENT_FROM_ALM;
        e.u.dev.event = DEVICE_EVENT_IN;
        e.u.dev.value = 0;
        sys_event_notify(&e);
    } else {
        sys_timeout_add(NULL, alarm_check, 100);
    }

    printf("********%s\n", __func__);
}

#if (defined(CONFIG_CPU_BR28))
static int write_p11_sys_time(int param)
{
    struct sys_time cur_time;
    if (p11_rtc_time) {
        get_sys_time(&cur_time);
        memcpy(&p11_rtc_time->ram_time, &cur_time, sizeof(struct sys_time));
        p11_rtc_time->mask = 0x12345678;
    }
    //printf("write_p11_sys_time \n");
    return 0;
}

extern const struct sys_time def_sys_time;
int user_write_p11_sys_time(int param)
{
    struct sys_time cur_time;
    if (p11_rtc_time) 
    {
        printf("%s:param = %d\n", __func__, param);
        if(param == 0)
            get_sys_time(&cur_time);
        else if(param == 1)
            memcpy(&cur_time, &def_sys_time, sizeof(struct sys_time));
        memcpy(&p11_rtc_time->ram_time, &cur_time, sizeof(struct sys_time));
        p11_rtc_time->mask = 0x12345678;
    }
    cpu_reset();
    /* printf("write_p11_sys_time \n"); */
    return 0;
}

static void write_p11_sys_time_by_timer(void *priv)
{
    int argv[3];
    argv[0] = (int)write_p11_sys_time;
    argv[1] = 1;
    os_taskq_post_type("app_core", Q_CALLBACK, 3, argv);
}

void user_dev_restart_handle(void *priv)
{
    int argv[3];
    argv[0] = (int)user_write_p11_sys_time;
    argv[1] = 1;
    argv[2] = 0;
    os_taskq_post_type("app_core", Q_CALLBACK, 3, argv);
}

void user_dev_reset_handle(void *priv)
{
    int argv[3];
    argv[0] = (int)user_write_p11_sys_time;
    argv[1] = 1;
    argv[2] = 1;
    os_taskq_post_type("app_core", Q_CALLBACK, 3, argv);
}

bool read_p11_sys_time(struct sys_time *t)
{
    if (p11_rtc_time && (p11_rtc_time->mask == 0x12345678)) {
        memcpy(t, &p11_rtc_time->ram_time, sizeof(struct sys_time));
        return true;
    }
    return false;
}

void p11_sys_time_init()
{
    ASSERT(P11_HEAP_SIZE >= sizeof(struct sys_time));
    //在p11开辟一片空间暂存时间变量
    p11_rtc_time = (struct p11_sys_time *)(P11_HEAP_BEGIN + P11_HEAP_SIZE - sizeof(struct p11_sys_time));
    sys_s_hi_timer_add(NULL, write_p11_sys_time_by_timer, 2000);
}
#endif

void alarm_init()
{
    u8 i = 0;
    if (dev_handle) {
        return ;
    }

    dev_handle = dev_open("rtc", NULL);
    if (!dev_handle) {
        ASSERT(0, "%s %d \n", __func__, __LINE__);
    }

    alarm_vm_read_info(&alarm_map);

    if (!alarm_active_flag_get()) { //判断是否闹钟在响
        alarm_update();//开机重新写入闹钟寄存器信息
    } else {
        sys_timeout_add(NULL, alarm_check, 100);
    }


    //贪睡闹钟部分参数初始化
    alarm_tab[M_MAX_ALARM_NUMS].index = M_MAX_ALARM_NUMS;
    alarm_tab[M_MAX_ALARM_NUMS].sw = 0;
    alarm_tab[M_MAX_ALARM_NUMS].mode = 0;

    /* register_sys_event_handler(SYS_ALL_EVENT, 1, NULL, alarm_event_handler); */
}

#endif  //end of RTC_ALM_EN
#endif


