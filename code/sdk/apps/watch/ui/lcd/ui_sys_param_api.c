#include "app_config.h"
#include "system/includes.h"
#include "ui/ui_sys_param.h"
#include "ui/watch_syscfg_manage.h"
#include "asm/mcpwm.h"
#include "audio_config.h"
#include "ui/ui_style.h"
#include "ui/ui_api.h"

#if TCFG_UI_ENABLE

#include "message_vm_cfg.h"

struct set_info set = {0};
int UIInfo_w_vm_timer = 0;
static u16 moto_time_id = 0;
struct sys_vm_param ui_sys_param;
struct pwm_platform_data moto_pwm_p_data;
struct sys_param sys_defalut_param[SYS_PARAM_NUM] = {
    {CardSetNum,          0},
    {LightLevel,          5},
    {DarkTime,            0},
    {ShortcutKey,         0},
    {LastSysVol,          0},
    {LightAlwayEn,        0},
    {LightTime,           0},
    {SysVoiceMute,        0},
    {AllDayUndisturbEn,   0},
    {TimeUndisturbEn,     0},
    {UndisturbStimeH,     0},
    {UndisturbStimeM,     0},
    {UndisturbEtimeH,     0},
    {UndisturbEtimeM,     0},
    {TrainAutoEn,         0},
    {ConnNewPhone,        0},
    {Language,            0},
    {MenuStyle,           0},
    {MotoMode,           80},
};

const static int short_key_tab[] = {
    ID_WINDOW_TRAIN,                //锻炼
    ID_WINDOW_SPORT_RECORD,         //锻炼记录
    ID_WINDOW_TRAIN_STATUS,         //训练状态
    ID_WINDOW_HEART,                //心率
    ID_WINDOW_BLOOD_OXYGEN,         //血氧饱和度
    ID_WINDOW_ACTIVERECORD,         //活动记录
    ID_WINDOW_SLEEP,                //睡眠
    ID_WINDOW_PRESSURE,             //压力
    ID_WINDOW_BREATH_TRAIN,         //呼吸训练
    ID_WINDOW_CALLRECORD,           //通话记录
    ID_WINDOW_PHONEBOOK,            //常用联系人
    ID_WINDOW_MUSIC,                //音乐
    ID_WINDOW_BARO,                 //海拔气压计
    ID_WINDOW_COMPASS,                              //指南针
    ID_WINDOW_MESS,                              //信息
    ID_WINDOW_WEATHER,              //天气
    0,                              //卡包
    0,                              //支付宝
    ID_WINDOW_STOPWATCH,            //秒表
    ID_WINDOW_CALCULAGRAPH,         //计时器
    ID_WINDOW_ALARM,                //闹钟
    ID_WINDOW_FLASHLIGHT,           //手电筒
    ID_WINDOW_FINDPHONE,            //找手机
    ID_WINDOW_PC,                   //PC模式
    ID_WINDOW_SET,                  //设置
};

// 应用列表类型
const static int app_list_type_tab[] = {
    ID_WINDOW_VMENU,
    ID_WINDOW_MAIN,
    ID_WINDOW_STAR_MENU,
};

extern void sys_enter_soft_poweroff(void *priv);
extern u8 get_ui_page_list_total_num();
extern u8 ui_style_vm_init(void);
static void get_sys_time(struct sys_time *time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        printf("[ERROR] open rtc error ");
        memset(time, 0, sizeof(*time));
        return;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
    printf(">>>>>>>>>>>>>>>>>Get systime : %d-%d-%d,%d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec);
    dev_close(fd);
}

int ui_check_list_tyep(int page_id)
{
    for (int i = 0; i < ARRAY_SIZE(app_list_type_tab); i++) {
        if (app_list_type_tab[i] == page_id) {
            return true;
        }
    }
    return false;
}

void set_ui_sys_param(u8 label, int value)
{
    u8 i;
    for (i = 0; i < SYS_PARAM_NUM; i++) {
        if (label == ui_sys_param.sys_param_table[i].label) {
            ui_sys_param.sys_param_table[i].value = value;
            return;
        }
    }

    printf("not found sys param\n");
}

int get_ui_sys_param(u8 label)
{
    u8 i;
    for (i = 0; i < SYS_PARAM_NUM; i++) {
        if (label == ui_sys_param.sys_param_table[i].label) {
            return ui_sys_param.sys_param_table[i].value;
        }
    }

    printf("not found sys param\n");
    return -1;
}

int write_UIInfo_to_vm(void *info)
{
    int ret = 0;
    g_printf("write_UIInfo_to_vm");
    if (UIInfo_w_vm_timer != 0) {
        sys_timer_del(UIInfo_w_vm_timer);
        UIInfo_w_vm_timer = 0;
    }
    ui_sys_param.valid = 1;
    if ((int)info == (int)SYSCFG_WRITE_ERASE_STATUS) {
        ui_sys_param.valid = 0;
    }
    ret = syscfg_write(VM_UI_SYS_INFO, &ui_sys_param, sizeof(ui_sys_param));
    if (ret != sizeof(ui_sys_param)) {
        printf("write ui_sysinfo VM err\n");
        return -1;
    }

    return ret;
}

int read_UIInfo_from_vm()
{
    int ret = 0;
    u8 ui_sys_param_vaild = 0;
    g_printf("read_UIInfo_from_vm");
    ret = syscfg_read(VM_UI_SYS_INFO, &ui_sys_param_vaild, sizeof(ui_sys_param_vaild));
    if ((ret != sizeof(ui_sys_param_vaild)) || (ui_sys_param_vaild == 0)) {
        printf("ui_sysinfo invalid\n");
        memcpy(&ui_sys_param.sys_param_table, &sys_defalut_param, sizeof(sys_defalut_param));
        set_ui_sys_param(LastSysVol, app_audio_get_volume(APP_AUDIO_STATE_WTONE));
        ui_style_vm_init();
        return -1;
    }
    ret = syscfg_read(VM_UI_SYS_INFO, &ui_sys_param, sizeof(ui_sys_param));
    if (ret != sizeof(ui_sys_param)) {
        printf("read ui_sysinfo VM err\n");
        memcpy(&ui_sys_param.sys_param_table, &sys_defalut_param, sizeof(sys_defalut_param));
        set_ui_sys_param(LastSysVol, app_audio_get_volume(APP_AUDIO_STATE_WTONE));
        ui_style_vm_init();
        return -2;
    }
    set_ui_sys_param(LastSysVol, app_audio_get_volume(APP_AUDIO_STATE_WTONE));
    ui_style_vm_init();
    return ret;
}

void erase_UIInfo_in_vm()
{
    int ret = 0;
    ui_sys_param.valid = 0;
    ret = syscfg_write(VM_UI_SYS_INFO, &ui_sys_param, sizeof(ui_sys_param));
    if (ret != sizeof(ui_sys_param)) {
        printf("erase ui_sysinfo VM err\n");
    }
}

void restore_sys_settings()
{
    flash_message_cfg_reset();//清外挂flash 存储
    /* erase_UIInfo_in_vm(); */
    watch_reboot_or_shutdown(1, 1);
}

void ui_set_dark_time(u8 sel)
{
#if TCFG_UI_SHUT_DOWN_TIME
    u8 dark_time[4] = {10, 15, 20, 20};

    if (get_ui_sys_param(LightAlwayEn) == 0) 
    {
        if (sel >= sizeof(dark_time)) {
            return;
        }
        ui_set_shut_down_time(dark_time[sel]);
        ui_auto_shut_down_modify();
    }
#endif
}

#if TCFG_UI_SHUT_DOWN_TIME

static u32 ui_get_dark_time(void)
{
    u8 dark_time[4] = {10, 15, 20, 20};
    u8 sel = get_ui_sys_param(DarkTime);

    if (get_ui_sys_param(LightAlwayEn) == 0) {
        if (sel >= sizeof(dark_time)) {
            return 0;
        }
        return dark_time[sel];
    }
    return 0;
}


static u16 alway_light_reset_to_id = 0;
static void alway_light_reset_to(void *priv)
{
    if (alway_light_reset_to_id == 0) {
        return ;
    }
    // 恢复
    set_ui_sys_param(LightAlwayEn, 0);
    ui_set_dark_time(get_ui_sys_param(DarkTime));
}
#endif /* #if TCFG_UI_SHUT_DOWN_TIME */

void screen_light_alway_switch(u8 on)
{
#if TCFG_UI_SHUT_DOWN_TIME
    //5min 10min 15min 20min
    u16 light_time[4] = {5 * 60, 10 * 60, 15 * 60, 20 * 60};
    u8 light_time_sel = get_ui_sys_param(LightTime);

    if (alway_light_reset_to_id) {
        sys_timeout_del(alway_light_reset_to_id);
        alway_light_reset_to_id = 0;
    }
    if (on) {
        if (light_time_sel >= sizeof(light_time)) {
            g_printf("light_time_sel >= sizeof(light_time)");
            return;
        }
        ui_set_shut_down_time(light_time[light_time_sel]);
        ui_auto_shut_down_modify();
        set_ui_sys_param(LightAlwayEn, 1);
        u32 to = (light_time[light_time_sel] - ui_get_dark_time()) * 1000;
        alway_light_reset_to_id = sys_timeout_add(NULL, alway_light_reset_to, to);
    } else {
        set_ui_sys_param(LightAlwayEn, 0);
        ui_set_dark_time(get_ui_sys_param(DarkTime));
    }
#endif
}

int get_light_level()
{
    return get_ui_sys_param(LightLevel);
}

void ui_ajust_light(u8 level)
{
    if (level > 10) {
        level = 10;
    }
    if (level == 0) {
        level = 1;
    }
    extern int lcd_drv_backlight_ctrl(u8 percent);
    lcd_drv_backlight_ctrl(level * 10);
}

void ui_set_voice(int precent)
{
    s8 volume;

    volume = get_tone_vol() * precent / 100;
    if (volume > get_tone_vol()) {
        volume = get_tone_vol();
    }

    /* printf("%s %d--volume:%d, max_volume:%d",__FUNCTION__,__LINE__,volume,get_tone_vol()); */
    app_audio_set_volume(APP_AUDIO_STATE_WTONE, volume, 1);
    if (volume > 0) {
        set_ui_sys_param(LastSysVol, volume);
    }
}

static int ui_voice_to_percent(s8 volume)
{
    return volume * 100 / get_tone_vol();
}

static u8 moto_mode = 0;
extern void mcpwm_set_duty(pwm_ch_num_type pwm_ch, u16 duty);
void ui_moto_init(u8 gpio)
{
    extern void mcpwm_init(struct pwm_platform_data * arg);
    
    moto_pwm_p_data.pwm_aligned_mode = pwm_edge_aligned;         //边沿对齐
    moto_pwm_p_data.pwm_ch_num = pwm_ch1;                        //通道
    moto_pwm_p_data.frequency = 10000;                           //Hz
    moto_pwm_p_data.duty = 10000;                                //占空比
    moto_pwm_p_data.h_pin = gpio;                                //任意引脚
    moto_pwm_p_data.l_pin = -1;                                  //任意引脚,不需要就填-1
    moto_pwm_p_data.complementary_en = 1;                        //两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&moto_pwm_p_data);
    mcpwm_set_duty(moto_pwm_p_data.pwm_ch_num, 0);
    moto_mode = get_ui_sys_param(MotoMode);
}

void ui_moto_set_H_L(u8 mode)
{
    set_ui_sys_param(MotoMode, mode);

    if (mode == TCFG_MOTO_PWM_NULL) {
        mcpwm_set_duty(moto_pwm_p_data.pwm_ch_num, TCFG_MOTO_PWM_NULL * 100);
    }
}

static void ui_moto_out_sleep(void)
{
    mcpwm_open(pwm_ch1);
    JL_MCPWM->CH1_CON0 = 0x24;
}

static void ui_moto_in_sleep(void)
{
    mcpwm_close(pwm_ch1);
    JL_MCPWM->CH1_CON0 = 0x00;
}

static void moto_play(void *mode)
{
    if (moto_time_id == 0) {
        return ;
    }
    static u8 time = 0;
    time++;
    printf("moto_run ");
    if (time < 2) {
        if (moto_mode == TCFG_MOTO_PWM_H) {
            ui_moto_out_sleep();
            mcpwm_set_duty(moto_pwm_p_data.pwm_ch_num, TCFG_MOTO_PWM_H * 100);
        } else if (moto_mode == TCFG_MOTO_PWM_L) {
            ui_moto_out_sleep();
            mcpwm_set_duty(moto_pwm_p_data.pwm_ch_num, TCFG_MOTO_PWM_L * 100);
        }
    } else {
        time = 0 ;
        mcpwm_set_duty(moto_pwm_p_data.pwm_ch_num, TCFG_MOTO_PWM_NULL * 100);
        ui_moto_in_sleep();
    }
}

void ui_moto_run(u8 run_mode)
{
    u32 rets;//
    __asm__ volatile("%0 = rets":"=r"(rets));
    printf("__func__ %s %x\n", __func__, rets);


    static u8 run_key = 0;
    static u8 moto_key = 0;
    struct sys_time new_time;
    u8 old_start_hour;
    u8 old_start_min;
    u8 old_end_hour;
    u8 old_end_min;
    u32 old_start_all_time;
    u32 old_end_all_time;
    u32 new_all_time;

    if (run_mode == 3) { //全天勿扰加锁
        moto_key = 1;
        mcpwm_set_duty(moto_pwm_p_data.pwm_ch_num, TCFG_MOTO_PWM_NULL * 100);
    } else if (run_mode  == 4) {
        moto_key = 0;
    }

    if (moto_key == 0) { //非全天勿扰
        if (run_mode == 5) { //定时勿扰判处理
            printf("run_mode == 5");
            run_key = 5;
            return ;
        } else if (run_mode  == 6) {
            printf("run_mode == 6");
            run_key = 0;
            moto_key = 0;
        }
    }

    if (run_key == 5) {
        printf("run_mode == 5");
        old_start_hour = get_ui_sys_param(UndisturbStimeH);
        old_start_min  = get_ui_sys_param(UndisturbStimeM);
        old_end_hour   = get_ui_sys_param(UndisturbEtimeH);
        old_end_min    = get_ui_sys_param(UndisturbEtimeM);
        old_start_all_time = old_start_hour * 60 + old_start_min;
        old_end_all_time = old_end_hour * 60 + old_end_min;
        get_sys_time(&new_time);
        new_all_time = new_time.hour * 60 + new_time.min;

        printf(">>>>>>old_start_hour >>>>>>>>>= %d", old_start_hour);
        printf(">>>>>>old_start_min  >>>>>>>>>= %d", old_start_min);
        printf(">>>>>>old_end_hour   >>>>>>>>>= %d", old_end_hour);
        printf(">>>>>>old_end_min    >>>>>>>>>= %d", old_end_min);
        printf(">>>>>>>>>>>>>>>old_start_all_time = %d", old_start_all_time);
        printf(">>>>>>>>>>>>>>>old_end_all_time = %d", old_end_all_time);
        printf(">>>>>>>>>>>>>>>new_all_time = %d", new_all_time);

        if (old_start_all_time == old_end_all_time) {
            moto_key = 0;
        }

        if (old_start_all_time < old_end_all_time) { //首先判断是开始时间大还是结束时间大确认是否超24小时
            if ((old_start_all_time <= new_all_time) && (new_all_time < old_end_all_time)) { //判断为在勿扰时间内
                printf("moto_run_disable");
                moto_key = 1;
            } else {
                printf("moto_run_enable");
                moto_key = 0;
            }
        } else { //这种情况不做处理 //开始时间 大于 结束时间

        }
    }

    printf("moto_key  == %d", moto_key);

    if (moto_key == 0) 
    {
        moto_mode = get_ui_sys_param(MotoMode);
        if (run_mode == 1) { //间隔震动
            printf("moto_run_mode = 1 ");
            if (moto_time_id == 0) {
                moto_time_id = sys_timer_add(NULL, moto_play, 1000);
            }
        } else if (run_mode == 2) { //震动一次
            printf("moto_run_mode = 2 ");
            if (moto_time_id == 0) {
                moto_time_id = sys_timeout_add(NULL, moto_play, 10);
            }
        } else { //为静音模式
            printf("moto_run_mode = 0 ");
            if (moto_time_id) {
                sys_timer_del(moto_time_id);
                moto_time_id = 0;
            }
            mcpwm_set_duty(moto_pwm_p_data.pwm_ch_num, TCFG_MOTO_PWM_NULL * 100);
        }
    }
}

void ui_moto_run_test(u8 percent)
{
    if (0 == percent) {
        mcpwm_set_duty(moto_pwm_p_data.pwm_ch_num, 0);
        ui_moto_in_sleep();
    } else {
        ui_moto_out_sleep();
        mcpwm_set_duty(moto_pwm_p_data.pwm_ch_num, percent * 100);
    }
}

static void ui_volume_up(u8 step)
{
    s8 volume = app_audio_get_volume(APP_AUDIO_STATE_WTONE);
    volume += step;
    if (volume > get_tone_vol()) {
        volume = get_tone_vol();
    }
    /* printf("%s %d--volume:%d, max_volume:%d",__FUNCTION__,__LINE__,volume,get_tone_vol()); */
    app_audio_set_volume(APP_AUDIO_STATE_WTONE, volume, 1);
    if (volume > 0) {
        set_ui_sys_param(LastSysVol, volume);
    }
}

static void ui_volume_down(u8 step)
{
    s8 volume = app_audio_get_volume(APP_AUDIO_STATE_WTONE);
    volume -= step;
    if (volume < 0) {
        volume = 0;
    }
    /* printf("%s %d--volume:%d, max_volume:%d",__FUNCTION__,__LINE__,volume,get_tone_vol()); */
    app_audio_set_volume(APP_AUDIO_STATE_WTONE, volume, 1);
    if (volume > 0) {
        set_ui_sys_param(LastSysVol, volume);
    }
}

void ui_voice_mute(u8 en)
{
    if (en) {
        app_audio_set_volume(APP_AUDIO_STATE_WTONE, 0, 1);
    } else {
        app_audio_set_volume(APP_AUDIO_STATE_WTONE, get_ui_sys_param(LastSysVol), 1);
    }
}

u8 ui_show_shortcut_key()
{
    u8 shortcutkey_sel = get_ui_sys_param(ShortcutKey);
    if (!UI_WINDOW_BACK_INDEX()) {
        if (short_key_tab[shortcutkey_sel]) {
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(short_key_tab[shortcutkey_sel]);
            return true;
        }
    }
    return false;
}
extern u32 ui_show_page_list[9];
u8 ui_style_vm_init(void)
{
    u8 menu_style = get_ui_sys_param(MenuStyle);
    printf("%s style=%d", __func__, menu_style);
    if (menu_style < ARRAY_SIZE(app_list_type_tab)) {
        /* ui_show_page_list[1] = ID_WINDOW_STAR_MENU; */
        /* ui_show_page_list[1] = app_list_type_tab[menu_style]; */
    } else {
        return -1;
    }
    /* ui_page_list_id_modify(1, ui_show_page_list[1]); */
    return 0;
}

u8 ui_show_menu_page()
{
    u8 menu_style = get_ui_sys_param(MenuStyle);
    if (menu_style < ARRAY_SIZE(app_list_type_tab)) {
        if (ui_page_get_auto_sw_effect() == 0) {
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(app_list_type_tab[menu_style]);
        } else {
            static struct effect_auto_pass pass = {0};
            pass.pl = 0;
            pass.direction = 0;
            ui_auto_sw_page_effect_post(UI_GET_WINDOW_ID(), app_list_type_tab[menu_style], EFFECT_MODE_AUTO_PASS, NULL, &pass);
        }
    } else {
        return false;
    }

    return true;
}

void ui_page_list_update(u32 *id_list, u8 num)
{
    u8 list_total_num = get_ui_page_list_total_num();
    u8 i;
    if (list_total_num < num) {
        for (i = 0; i < list_total_num; i++) {
            ui_page_list_id_modify(i, id_list[i]);
        }
        for (i = list_total_num; i < num; i++) {
            ui_page_add(id_list[i]);
        }
    } else if (list_total_num > num) {
        for (i = 0; i < num; i++) {
            ui_page_list_id_modify(i, id_list[i]);
        }

        for (i = num; i < list_total_num; i++) {
            //ui_page_del_by_num(i);
            ui_page_del_by_num(num);
        }
    } else {
        for (i = 0; i < num; i++) {
            ui_page_list_id_modify(i, id_list[i]);
        }
    }
}

void watch_reboot_or_shutdown(u8 flag, u8 erase)
{
    void *p = NULL;
    if (erase) {
        p = SYSCFG_WRITE_ERASE_STATUS;
    }
    extern void watch_sensor_test(void);
    extern int watch_sensor_close(void);

    watch_sensor_close();
    watch_syscfg_write_all(p);
    if (flag) {
        cpu_reset();
    } else {
        sys_enter_soft_poweroff(NULL);
    }
}

REGISTER_WATCH_SYSCFG(sys_param_ops) = {
    .name = "sys_param",
    .read = read_UIInfo_from_vm,
    .write = write_UIInfo_to_vm,
};
#endif/* #if TCFG_UI_ENABLE */

