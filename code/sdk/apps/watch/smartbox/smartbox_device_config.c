#include "smartbox/config.h"
#include "smartbox/smartbox_device_config.h"
#include "btstack/avctp_user.h"
#include "smartbox/event.h"
#include "custom_cfg.h"
#include "JL_rcsp_packet.h"
#include "smartbox_extra_flash_opt.h"

#if (SMART_BOX_EN)

// u16
#define DEV_CFG_OTA					BIT(0) //OTA
#define DEV_CFG_RES_UPDATE			BIT(1) //资源更新
#define DEV_CFG_WATCH_ADD			BIT(2) //表盘增加、删除、查询
#define DEV_CFG_WATCH_SWITCH		BIT(3) //表盘切换
#define DEV_CFG_WATCH_SHOW			BIT(4) //表盘预览

// u16
#define DEV_CFG_SCREEN_DISP			BIT(0) //屏幕设置
#define DEV_CFG_MOTOR_SET			BIT(1) //振动强度
#define DEV_CFG_UNDISTURB			BIT(2) //勿扰模式
#define DEV_CFG_TRAIN_SET			BIT(3) //锻炼设置
#define DEV_CFG_BT_SETTING			BIT(4) //蓝牙断开设置

// u16
#define DEV_CFG_CONTACTA			BIT(0) //常用联系人
#define DEV_CFG_MUSIC_BROWSE		BIT(1) //音乐文件浏览
#define DEV_CFG_MUSIC_TRANSFER		BIT(2) //音乐文件删除、传输
#define DEV_CFG_ALARM				BIT(3) //闹钟设置
#define DEV_CFG_NOTICE				BIT(4) //信息同步
#define DEV_CFG_WEATHER				BIT(5) //天气同步
#define DEV_CFG_FINDPHONE			BIT(6) //找手机

// sport LTV
#define DEV_CFG_SPORT_TYPE_GSENSOR		0
#define DEV_CFG_GSENSOR_ONOFF				BIT(0) //传感器开关
#define DEV_CFG_GSENSOR_STEPS				BIT(1) //运动计步

#define DEV_CFG_SPORT_TYPE_HR			1
#define DEV_CFG_HR_ONOFF					BIT(0) //传感器开关
#define DEV_CFG_HR_CONTINUE					BIT(1) //连续心率

#define DEV_CFG_SPORT_TYPE_MR			2
#define DEV_CFG_MR_ONOFF					BIT(0) //传感器开关

#define DEV_CFG_SPORT_TYPE_PRESSURE		3
#define DEV_CFG_PRESSURE_ONOFF				BIT(0) //传感器开关

#define DEV_CFG_SPORT_TYPE_GPS			4
#define DEV_CFG_GPS_ONOFF					BIT(0) //传感器开关

#define DEV_CFG_SPORT_TYPE_MODE			0xFE
#define DEV_CFG_SPORT_RECORD				BIT(0) //运动记录
#define DEV_CFG_SPORT_STATISTICS			BIT(1) //运动统计
#define DEV_CFG_SPORT_OUTDOOR				BIT(2) //室外运动
#define DEV_CFG_SPORT_INDOOR				BIT(3) //室内运动


#define DEV_CFG_SPORT_TYPE_COMMON		0xFF
#define DEV_CFG_HEALTH						BIT(0) //健康监控
#define DEV_CFG_PIM							BIT(1) //个人信息
#define DEV_CFG_SLEEP						BIT(2) //睡眠监控
#define DEV_CFG_BURN_THE_PLANKS				BIT(3) //久坐提醒
#define DEV_CFG_SPORT_HR					BIT(4) //运动心率提醒
#define DEV_CFG_HEAT						BIT(5) //压力自动检测
#define DEV_CFG_FALL_DOWN					BIT(6) //跌倒检测
#define DEV_CFG_SENSOR_SET					BIT(7) //传感器设置



u32 smartbox_get_device_config(void *priv, u8 *buf, u16 buf_size)
{
    u16 rlen, wlen = 0;
    u8  u8val;
    u16 u16val;

    if ((wlen + 2) <= buf_size) {
        buf[0] = 0; //产品标识
        buf[1] = 0; //版本号
        wlen += 2;
    }

    u16val = DEV_CFG_OTA | DEV_CFG_RES_UPDATE | DEV_CFG_WATCH_ADD | DEV_CFG_WATCH_SWITCH | DEV_CFG_WATCH_SHOW;
    if ((wlen + 2) <= buf_size) {
        memcpy(&buf[wlen], &u16val, 2);
        wlen += 2;
    }

    u16val = DEV_CFG_SCREEN_DISP | DEV_CFG_MOTOR_SET | DEV_CFG_UNDISTURB
#if TCFG_UI_ENABLE_TRAIN
             | DEV_CFG_TRAIN_SET
#endif /* #if TCFG_UI_ENABLE_TRAIN */
             | DEV_CFG_BT_SETTING;
    if ((wlen + 2) <= buf_size) {
        memcpy(&buf[wlen], &u16val, 2);
        wlen += 2;
    }

    u16val = DEV_CFG_CONTACTA
#if TCFG_UI_ENABLE_MUSIC
             | DEV_CFG_MUSIC_BROWSE | DEV_CFG_MUSIC_TRANSFER
#endif /* #if TCFG_UI_ENABLE_MUSIC */
#if TCFG_UI_ENABLE_ALARM
             | DEV_CFG_ALARM
#endif /* #if TCFG_UI_ENABLE_ALARM */
#if TCFG_UI_ENABLE_NOTICE
             | DEV_CFG_NOTICE
#endif /* #if TCFG_UI_ENABLE_NOTICE */
#if TCFG_UI_ENABLE_WEATHER
             | DEV_CFG_WEATHER
#endif /* #if TCFG_UI_ENABLE_WEATHER */
#if TCFG_UI_ENABLE_FINDPHONE
             | DEV_CFG_FINDPHONE
#endif /* #if TCFG_UI_ENABLE_FINDPHONE */
             ;
    if ((wlen + 2) <= buf_size) {
        memcpy(&buf[wlen], &u16val, 2);
        wlen += 2;
    }

#if TCFG_GSENSOR_ENABLE
    u8val = DEV_CFG_GSENSOR_ONOFF | DEV_CFG_GSENSOR_STEPS;
    rlen = add_one_attr(buf, buf_size, wlen, DEV_CFG_SPORT_TYPE_GSENSOR, &u8val, 1);
    wlen += rlen;
#endif /* #if TCFG_GSENSOR_ENABLE */

#if TCFG_HR_SENSOR_ENABLE
    u8val = DEV_CFG_HR_ONOFF | DEV_CFG_HR_CONTINUE;
    rlen = add_one_attr(buf, buf_size, wlen, DEV_CFG_SPORT_TYPE_HR, &u8val, 1);
    wlen += rlen;

    u8val = DEV_CFG_MR_ONOFF;
    rlen = add_one_attr(buf, buf_size, wlen, DEV_CFG_SPORT_TYPE_MR, &u8val, 1);
    wlen += rlen;
#endif /* #if TCFG_HR_SENSOR_ENABLE */

#if TCFG_BMP280_ENABLE
    u8val = DEV_CFG_PRESSURE_ONOFF;
    rlen = add_one_attr(buf, buf_size, wlen, DEV_CFG_SPORT_TYPE_PRESSURE, &u8val, 1);
    wlen += rlen;
#endif /* #if TCFG_BMP280_ENABLE */

#if 0
    u8val = DEV_CFG_GPS_ONOFF;
    rlen = add_one_attr(buf, buf_size, wlen, DEV_CFG_SPORT_TYPE_GPS, &u8val, 1);
    wlen += rlen;
#endif

    u8val = 0
#if TCFG_UI_ENABLE_SPORTRECORD
            | DEV_CFG_SPORT_RECORD | DEV_CFG_SPORT_STATISTICS
#endif /* #if TCFG_UI_ENABLE_SPORTRECORD */
            | DEV_CFG_SPORT_OUTDOOR | DEV_CFG_SPORT_INDOOR;
    rlen = add_one_attr(buf, buf_size, wlen, DEV_CFG_SPORT_TYPE_MODE, &u8val, 1);
    wlen += rlen;

    u8val = DEV_CFG_HEALTH | DEV_CFG_PIM
#if TCFG_UI_ENABLE_SLEEP
            | DEV_CFG_SLEEP
#endif /* #if TCFG_UI_ENABLE_SLEEP */
            | DEV_CFG_BURN_THE_PLANKS | DEV_CFG_SPORT_HR
#if TCFG_UI_ENABLE_HEAT
            | DEV_CFG_HEAT
#endif /* #if TCFG_UI_ENABLE_HEAT */
            | DEV_CFG_FALL_DOWN
            | DEV_CFG_SENSOR_SET
            ;
    rlen = add_one_attr(buf, buf_size, wlen, DEV_CFG_SPORT_TYPE_COMMON, &u8val, 1);
    wlen += rlen;

    return wlen;
}
#endif//SMART_BOX_EN


