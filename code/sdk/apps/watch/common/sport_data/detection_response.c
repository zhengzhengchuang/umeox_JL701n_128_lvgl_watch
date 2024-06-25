
#include "detection_response.h"
#include "event.h"
#include "system/includes.h"
#include "app_config.h"
#if(CONFIG_UI_STYLE == STYLE_JL_WTACH)
#include "ui/style_JL.h"
#elif (CONFIG_UI_STYLE==STYLE_JL_WTACH_NEW)
#include "ui/style_JL_new.h"
#endif
#include "btstack/avctp_user.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "ui/ui_style.h"

#include "sport_data/watch_common.h"
#include "sport_data/watch_time_common.h"
#include "debug.h"
/* #define SPORT_DETECTION_INTERVAL 10*60//每隔10MIN提醒一次 */
#if 1//TCFG_UI_ENABLE_MOTO
#include "ui/ui_sys_param.h"
#endif /* #if TCFG_UI_ENABLE_MOTO */

struct detection_set {
    u8 s_hour;
    u8 s_min;
    u8 e_hour;
    u8 e_min;
    u8 enable;//00  01 ff
    u8 response_mode;
};
int detection_type_show_set(int type);
static struct detection_set wrist_detection, sedentary_detection, sleep_detection, hr_detection, fall_detection;
void sleep_detecion_response(void)
{
    printf("%s enable=%d mode=%d", __func__, hr_detection.enable, hr_detection.response_mode);

}
/* static u8 sport_hr_flag = 0; */
/* void clr_sport_hr_flag(void) */
/* { */
/* sport_hr_flag = 0; */
/* } */
void sport_hr_detection_response(void)
{
    /* if (sport_hr_flag == 1) { //防止频繁重入 */
    /* return; */
    /* } */
    printf("%s enable=%d mode=%d", __func__, hr_detection.enable, hr_detection.response_mode);
    if (hr_detection.response_mode == SCREEN_LIGHT) {
        detection_type_show_set(EXERCISE_HEART_RATE);
        UI_WINDOW_PREEMPTION_POSH(ID_WINDOW_DETECTION, NULL, NULL, UI_WINDOW_PREEMPTION_TYPE_DETECTION);
    }
    if (hr_detection.response_mode == SHAKE) {
#if TCFG_UI_ENABLE_MOTO
        UI_MOTO_RUN(2);//震动一次
#endif
    }
    if (hr_detection.response_mode == CALL) {
        call_emergency_contact_number();
    }
    /* sport_hr_flag = 1; */
    /* sys_timeout_add(NULL, clr_sport_hr_flag, SPORT_DETECTION_INTERVAL * 1000); */

}

//抬腕响应
void wrist_detection_response(void)
{
    printf("%s enable=%d mode=%d", __func__, wrist_detection.enable, wrist_detection.response_mode);
    if (wrist_detection.response_mode == SCREEN_LIGHT) {
        ui_screen_recover(1);//亮屏
    }
    if (wrist_detection.response_mode == SHAKE) {
#if TCFG_UI_ENABLE_MOTO
        UI_MOTO_RUN(2);//震动一次
#endif
    }
    if (wrist_detection.response_mode == CALL) {
        call_emergency_contact_number();
    }

}
static void timer_fall_stop(void *P)
{
    if (fall_detection.response_mode == (SHAKE)) {
#if TCFG_UI_ENABLE_MOTO
        UI_MOTO_RUN(0);
#endif
    } else if (fall_detection.response_mode == (SCREEN_LIGHT)) {
#if TCFG_UI_ENABLE
        screen_light_alway_switch(0);
#endif /* #if TCFG_UI_ENABLE */
    }
}
//跌倒响应
void fall_detection_response(void)
{
    printf("%s enable=%d mode=%d", __func__, fall_detection.enable, fall_detection.response_mode);
    if (fall_detection.response_mode == (SCREEN_LIGHT)) {
#if (CONFIG_UI_STYLE == STYLE_JL_WTACH_NEW)
        u16 light_time[4] = {5 * 60, 10 * 60, 15 * 60, 20 * 60};
        u8 light_time_sel = get_ui_sys_param(LightTime);
        screen_light_alway_switch(1);
        ui_screen_recover(1);
        sys_timeout_add(NULL, timer_fall_stop, 1000 * light_time[light_time_sel]);
#endif
    }
    if (fall_detection.response_mode == (SHAKE)) {
#if TCFG_UI_ENABLE_MOTO
        UI_MOTO_RUN(1);//间隔震动
#endif
        sys_timeout_add(NULL, timer_fall_stop, 10 * 1000);
    }
    if (fall_detection.response_mode == (CALL)) {
        UI_WINDOW_PREEMPTION_POSH(ID_WINDOW_FALL, NULL, NULL, UI_WINDOW_PREEMPTION_TYPE_FALL);
    }
}
//久坐响应
void sedentary_detecion_response(void)
{
    printf("%s enable=%d mode=%d", __func__, sedentary_detection.enable, sedentary_detection.response_mode);
    if (sedentary_detection.response_mode == (SCREEN_LIGHT)) {
        detection_type_show_set(SEDENTARY);
        UI_WINDOW_PREEMPTION_POSH(ID_WINDOW_DETECTION, NULL, NULL, UI_WINDOW_PREEMPTION_TYPE_DETECTION);
    }
    if (sedentary_detection.response_mode == (SHAKE)) {
#if TCFG_UI_ENABLE_MOTO
        UI_MOTO_RUN(2);//震动一次
#endif
    }
}


u8 emergency_contact_number[20] = "12345678900";


int set_emergency_contact_number(u8 *number, u8 len)
{
    if (len == 0) {
        return -1;
    }
    printf_buf(number, len);
    memcpy(emergency_contact_number, number, len);
    return 0;
}


int call_emergency_contact_number(void)
{
    user_send_cmd_prepare(USER_CTRL_DIAL_NUMBER, strlen(emergency_contact_number), (u8 *)&emergency_contact_number);
    return 0;
}

//时段使能刷新
int refresh_detection_time(void)
{
    struct sys_time n_time;
    watch_file_get_sys_time(&n_time);
    u8 save_time[4];
    u8 enable = 0;
    u8 old_enable = 0;
    for (int type = WRIST; type <= SLEEP; type++) {
        memset(save_time, 0, 4);
        enable = 0;
        get_detection_time(type, &enable, save_time);
        old_enable = get_watch_motion_switch(type);
        /* printf("%s type=%d,enable%d=%d,Stime=%d:%d,Etime=%d:%d,ntime=%d:%d", __func__, type, old_enable, enable, save_time[0], save_time[1], save_time[2], save_time[3], n_time.hour, n_time.min); */
        if (enable == SWITCH_OFF) {
            if (old_enable == SWITCH_OFF) {
                continue;
            } else {
                set_watch_motion_switch(type, 0);
                continue;
            }
        } else if (enable == SWITCH_ON) {
            if (old_enable == SWITCH_OFF) {
                set_watch_motion_switch(type, 1);
                continue;
            } else {
                continue;
            }
        } else if (enable == SWITCH_TIMING) {
            if ((save_time[0] * 60 + save_time[1]) < (save_time[2] * 60 + save_time[3])) { // -0--s-1-e--0-开始时间＜结束时间，按自然时间处理
                if ((n_time.hour * 60 + n_time.min) >= (save_time[2] * 60 + save_time[3])) {
                    if (old_enable == 1) {
                        set_watch_motion_switch(type, 0);
                        continue;
                    }
                } else {
                    if ((n_time.hour * 60 + n_time.min) >= (save_time[0] * 60 + save_time[1])) {
                        if (old_enable == 0) {
                            set_watch_motion_switch(type, 1);
                            continue;
                        }
                    } else {
                        if (old_enable == 1) {
                            set_watch_motion_switch(type, 0);
                        }
                    }
                }
            } else {	// -1--e-0-s--1-结束时间小于开始时间，按+1day处理
                if ((n_time.hour * 60 + n_time.min) >= (save_time[0] * 60 + save_time[1])) {
                    if (old_enable == 0) {
                        set_watch_motion_switch(type, 1);
                        continue;
                    }
                } else {
                    if ((n_time.hour * 60 + n_time.min) >= (save_time[2] * 60 + save_time[3])) {
                        if (old_enable == 1) {
                            set_watch_motion_switch(type, 0);
                            continue;
                        }
                    } else {
                        if (old_enable == 0) {
                            set_watch_motion_switch(type, 1);
                        }
                    }
                }
            }
        }
    }
    return 0;
}



//保存时段设置

int save_detection_set(u8 type, u8 enable, u8 response_mode, u8 *time)
{
    if (time != NULL) {
        printf("%s type=%d,enable=%d,mode=%d,stime=%02d:%02d,etime=%02d:%02d", __func__, type, enable, response_mode, time[0], time[1], time[2], time[3]);
    }
    switch (type) {
    case WRIST:
        wrist_detection.enable = enable;
        wrist_detection.response_mode = response_mode;
        if (enable == 0x02) {
            wrist_detection.s_hour = time[0];
            wrist_detection.s_min = time[1];
            wrist_detection.e_hour = time[2];
            wrist_detection.e_min = time[3];
        }
        break;

    case SEDENTARY:
        sedentary_detection.enable = enable;
        sedentary_detection.response_mode = response_mode;
        if (enable == 0x02) {
            sedentary_detection.s_hour = time[0];
            sedentary_detection.s_min = time[1];
            sedentary_detection.e_hour = time[2];
            sedentary_detection.e_min = time[3];
        }
        break;
    case SLEEP:
        sleep_detection.enable = enable;
        if (enable == 0x02) {
            sleep_detection.s_hour = time[0];
            sleep_detection.s_min = time[1];
            sleep_detection.e_hour = time[2];
            sleep_detection.e_min = time[3];
        }
        break;
    case FALL:
        fall_detection.enable = enable;
        fall_detection.response_mode = response_mode;
        break;
    case EXERCISE_HEART_RATE:
    case CONTINUE_HEART_RATE:
        hr_detection.response_mode = response_mode;
        break;
    }
    return 0 ;
}
int get_detection_time(u8 type, u8 *enable, u8 *time)
{
    switch (type) {
    case WRIST:
        *enable = wrist_detection.enable;
        if (*enable == 0x02) {
            time[0] = wrist_detection.s_hour;
            time[1] = wrist_detection.s_min;
            time[2] = wrist_detection.e_hour;
            time[3] = wrist_detection.e_min;
        }
        break;
    case SEDENTARY:
        *enable = sedentary_detection.enable;
        if (*enable == 0x02) {
            time[0] = sedentary_detection.s_hour = time[0];
            time[1] = sedentary_detection.s_min = time[1];
            time[2] = sedentary_detection.e_hour = time[2];
            time[3] = sedentary_detection.e_min = time[3];
        }
        break;
    case SLEEP:
        *enable = sleep_detection.enable;
        if (*enable == 0x02) {
            time[0] = sleep_detection.s_hour;
            time[1] = sleep_detection.s_min;
            time[2] = sleep_detection.e_hour;
            time[3] = sleep_detection.e_min;
        }
        break;
    case FALL:
        *enable = fall_detection.enable;
        break;
    }
    return 0 ;

}
