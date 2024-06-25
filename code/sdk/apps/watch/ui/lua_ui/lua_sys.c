// 凡与操作系统相关内容均在sys库实现
// 系统时间获取、设置，RTC
// 系统设置

#include "system/includes.h"
#include "app_config.h"
#include "list.h"
#include "event.h"
#include "ui/ui_sys_param.h"
#include "audio_config.h"
#include "ioctl_cmds.h"
#include "sys_time.h"
#include "ui/ui_time.h"

/* lua解释器头文件 */
#include "lua/lauxlib.h"
#include "lua/lualib.h"

/* lua_ui模块头文件 */
#include "lua_module.h"
#include "lua_sys.h"
#include "lua_gui.h"

#include "message_vm_cfg.h"

#include "rtc/alarm.h"

#if (TCFG_LUA_ENABLE)


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ system steup
static int sys_setLightLevel(lua_State *L)
{
    u8 light_ajust_sel;
    bool update = lua_toboolean(L, -1);
    light_ajust_sel = lua_tointeger(L, -2);

    if (update) {
        set_ui_sys_param(LightLevel, light_ajust_sel);
    }
    g_printf("%s:%d %d", __FUNCTION__, light_ajust_sel, update);
    ui_ajust_light(light_ajust_sel * 2);

    return 0;
}


static int sys_setDarkTime(lua_State *L)
{
    u8 screen_dark_time_sel = lua_tointeger(L, -1);

    g_printf("%s:%d", __FUNCTION__, screen_dark_time_sel);
    ui_set_dark_time(screen_dark_time_sel);
    set_ui_sys_param(DarkTime, screen_dark_time_sel);

    return 0;
}


// 自动息屏功能开关
// 参数:
// (onoff) --> true 打开自动息屏功能
// 		--> false 关闭自动息屏功能
static int sys_setAutoDark(lua_State *L)
{
    int onoff = lua_toboolean(L, -1);

    if (onoff) {
        extern void ui_auto_shut_down_enable(void);
        ui_auto_shut_down_enable();
    } else {
        extern u8 ui_auto_shut_down_disable(void);
        ui_auto_shut_down_disable();
    }
    return 0;
}



static int sys_setSysVolByPercent(lua_State *L)
{
    int percent;
    percent = lua_tointeger(L, -1);//获取lua解析器栈顶数据

    g_printf("%s:%d", __FUNCTION__, percent);
    ui_set_voice(percent);

    return 0;
}


static int sys_getSysVolPercent(lua_State *L)
{
    u8 vslider_percent = ui_voice_to_percent(app_audio_get_volume(APP_AUDIO_STATE_MUSIC));
    lua_pushinteger(L, vslider_percent);

    return 1;
}


static int sys_setSysVolUp(lua_State *L)
{
    u8 step;

    step = lua_tointeger(L, -1);//获取lua解析器栈顶数据
    g_printf("%s:%d", __FUNCTION__, step);
    ui_volume_up(step);

    return 0;
}


static int sys_setSysVolDown(lua_State *L)
{
    u8 step;

    step = lua_tointeger(L, -1);//获取lua解析器栈顶数据
    g_printf("%s:%d", __FUNCTION__, step);
    ui_volume_down(step);

    return 0;
}


static int sys_setSysVoiceMute(lua_State *L)
{
    u8 voice_mute_sel;
    voice_mute_sel = lua_tointeger(L, -1);//获取lua解析器栈顶数据

    g_printf("%s:%d", __FUNCTION__, voice_mute_sel);
    ui_voice_mute(voice_mute_sel);
    set_ui_sys_param(SysVoiceMute, voice_mute_sel);

    return 0;
}


static int sys_getWatchName(lua_State *L)
{
    const char *watch_name;
    char *ext_name;
    char name[13] = {0};
    int action = lua_tointeger(L, -1);

    if (action) {
        extern int watch_get_style();
        extern char *watch_get_item(int style);
        ext_name = strrchr(watch_get_item(watch_get_style()), '.');
        watch_name = strrchr(watch_get_item(watch_get_style()), '/') + 1;
        memcpy(name, watch_name, strlen(watch_name) - strlen(ext_name));
        g_printf("ext_name:%s watch_name:%s, name:%s", ext_name, watch_name, name);
        lua_pushstring(L, name);
    } else {
        extern int standby_watch_get_style();
        if (standby_watch_get_style() != -1) {
            ext_name = strrchr(watch_get_item(standby_watch_get_style()), '.');
            watch_name = strrchr(watch_get_item(standby_watch_get_style()), '/') + 1;
            memcpy(name, watch_name, strlen(watch_name) - strlen(ext_name));
            r_printf("ext_name:%s watch_name:%s name:%s", ext_name, watch_name, name);
            lua_pushstring(L, name);
        } else {
            lua_pushstring(L, "无");
        }
    }

    return 1;
}


static int sys_getCardSelTab(lua_State *L)
{
    lua_newtable(L);
    set_integer_array_to_table(L, ui_sys_param.card_select, sizeof(ui_sys_param.card_select));

    return 1;
}



static int sys_setCardSelTab(lua_State *L)
{
    extern const u32 ui_page_list[];
    extern u32 ui_show_page_list[9];

    u8 card_set_num = get_ui_sys_param(CardSetNum);
    get_integer_array_from_table(L, ui_sys_param.card_select, sizeof(ui_sys_param.card_select));
    memset(ui_show_page_list + 3, 0, sizeof(ui_show_page_list) - 3 * sizeof(ui_show_page_list[0]));
    for (u8 i = 0; i < card_set_num; i++) {
        ui_show_page_list[i + 3] = ui_page_list[ui_sys_param.card_select[i]];
    }
    ui_page_list_update(ui_show_page_list, card_set_num + 3);

    return 0;
}


static int sys_getCardTab(lua_State *L)
{
    lua_newtable(L);
    set_integer_array_to_table(L, set.card, sizeof(set.card));

    return 1;
}


static int sys_setCardTab(lua_State *L)
{
    get_integer_array_from_table(L, set.card, MAX_CARD_BACKUP_SELECT);

    return 0;
}


static int sys_ResetOrShutdown(lua_State *L)
{
    int action = lua_tointeger(L, -1);

    if (action) {
        cpu_reset();
    } else {
        extern void sys_enter_soft_poweroff(void *priv);
        sys_enter_soft_poweroff(NULL);
    }
    return 0;
}


static int sys_syncSetupParam(lua_State *L)
{
    bool type = lua_toboolean(L, -1);
    int value = lua_tointeger(L, -2);
    u8 label = lua_tointeger(L, -3);

    g_printf("type:%d value:%d label:%d", type, value, label);
    if (type) {     //获取系统设置参数
        lua_pushinteger(L, get_ui_sys_param(label));
        return 1;
    } else {        //设置系统设置参数
        set_ui_sys_param(label, value);
        return 0;
    }
}


static int sys_sysParamVmOpt(lua_State *L)
{
    int action = lua_tointeger(L, -1);

    if (action == 0) {
        restore_sys_settings();
    } else if (action == 1) {
        read_UIInfo_from_vm();
    } else if (action == 2) {
        write_UIInfo_to_vm(NULL);
    }

    return 0;
}



static char buf_temp[] = {
    0x00, 0x00, 0x05, 0x00, 0x2E, 0xA4, 0xDE, 0x67, 0x00, 0x1A, 0x01, 0x22, 0x6F, 0x6D, 0x2E, 0x61, 0x6C, 0x69,
    0x62, 0x61, 0x62, 0x61, 0x2E, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64, 0x2E, 0x72, 0x69, 0x6D, 0x65, 0x74,
    0x00, 0x02, 0x02, 0x04, 0x00, 0x10, 0x03, 0xE4, 0xBA, 0x94, 0xE4, 0xBA, 0x8C, 0xE9, 0x9B, 0xB6, 0xE8, 0xBD,
    0xA6, 0xE9, 0x98, 0x9F, 0x00, 0x3A, 0x04, 0xE8, 0xA5, 0xBF, 0xE5, 0x8D, 0x97, 0xE7, 0xA7, 0x91, 0xE6, 0x8A,
    0x80, 0xE5, 0xA4, 0xA7, 0xE5, 0xAD, 0xA6, 0xE4, 0xB8, 0x9C, 0xE4, 0xBA, 0x94, 0x42, 0x35, 0x32, 0x30, 0xE5,
    0xAD, 0xA6, 0xE7, 0x94, 0x9F, 0xE5, 0xAE, 0xBF, 0xE8, 0x88, 0x8D,

    0x00, 0x00, 0x05, 0x00, 0x2E, 0xA4, 0xDE, 0x67, 0x00, 0x1A, 0x01, 0x33, 0x6F, 0x6D, 0x2E, 0x61,
    0x6C, 0x69, 0x62, 0x61, 0x62, 0x61, 0x2E, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64, 0x2E, 0x72,
    0x69, 0x6D, 0x65, 0x74, 0x00, 0x02, 0x02, 0x04, 0x00, 0x10, 0x03, 0xE5, 0xBD, 0x92, 0xE5, 0x8E,
    0xBB, 0xE6, 0x9D, 0xA5, 0xE5, 0x85, 0xAE, 0xE8, 0xBE, 0x9E, 0x00, 0x3A, 0x04, 0xE6, 0x95, 0x85,
    0xE4, 0xBA, 0xBA, 0xE8, 0xA5, 0xBF, 0xE8, 0xBE, 0x9E, 0xE9, 0xBB, 0x84, 0xE9, 0xB9, 0xA4, 0xE6,
    0xA5, 0xBC, 0xEF, 0xBC, 0x8C, 0xE7, 0x99, 0xBD, 0xE4, 0xBA, 0x91, 0xE5, 0x8D, 0x83, 0xE8, 0xBD,
    0xBD, 0xE7, 0xA9, 0xBA, 0xE6, 0x82, 0xA0, 0xE6, 0x82, 0xA0,

    0x00, 0x00, 0x05, 0x00, 0x2E, 0xA4, 0xDB, 0xD2, 0x00, 0x1A, 0x01, 0x63, 0x6F, 0x6D, 0x2E, 0x61, 0x6C, 0x69,
    0x62, 0x61, 0x62, 0x61, 0x2E, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64, 0x2E, 0x72, 0x69, 0x6D, 0x65, 0x74,
    0x00, 0x02, 0x02, 0x04, 0x00, 0x10, 0x03, 0xE4, 0xBA, 0x8C, 0xE5, 0x93, 0x88, 0xE7, 0x9A, 0x84, 0xE5, 0xB0,
    0x8F, 0xE7, 0xAA, 0x9D, 0x00, 0x26, 0x04, 0xE5, 0x88, 0x98, 0xE4, 0xBA, 0x9A, 0xE9, 0xBE, 0x99, 0x3A, 0xE7,
    0x99, 0xBD, 0xE9, 0x85, 0x92, 0xE7, 0xAD, 0x89, 0xE7, 0xA8, 0xB3, 0xE4, 0xBA, 0x86, 0xE5, 0x9C, 0xA8, 0xE5,
    0x8A, 0xA0, 0xE4, 0xBB, 0x93, 0xE5, 0x90, 0xA7,

    0x00, 0x00, 0x05, 0x00, 0x2E, 0xA4, 0xE6, 0xC8, 0x00, 0x1A, 0x01, 0x63, 0x6F, 0x6D, 0x2E, 0x61,
    0x6C, 0x69, 0x62, 0x61, 0x62, 0x61, 0x2E, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64, 0x2E, 0x72,
    0x69, 0x6D, 0x65, 0x74, 0x00, 0x02, 0x02, 0x04, 0x00, 0x07, 0x03, 0xE9, 0x92, 0x89, 0xE9, 0x92,
    0x89, 0x00, 0x2F, 0x04, 0xE8, 0x83, 0xA1, 0xE7, 0x8E, 0xBA, 0x3A, 0x20, 0x77, 0x61, 0x74, 0x63,
    0x68, 0xE9, 0x87, 0x8C, 0xE9, 0x9D, 0xA2, 0xE7, 0x9A, 0x84, 0xE4, 0xB8, 0x8D, 0xE7, 0x94, 0xA8,
    0xEF, 0xBC, 0x8C, 0xE6, 0xB2, 0xA1, 0xE5, 0x8C, 0x85, 0xE8, 0xBF, 0x9B, 0xE5, 0x8E, 0xBB, 0xE7,
    0x9A, 0x84,

    0x00, 0x00, 0x05, 0x00, 0x2E, 0xA4, 0xE6, 0xF6, 0x00, 0x1A, 0x01, 0x63, 0x6F, 0x6D, 0x2E, 0x61,
    0x6C, 0x69, 0x62, 0x61, 0x62, 0x61, 0x2E, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64, 0x2E, 0x72,
    0x69, 0x6D, 0x65, 0x74, 0x00, 0x02, 0x02, 0x04, 0x00, 0x07, 0x03, 0xE9, 0x92, 0x89, 0xE9, 0x92,
    0x89, 0x00, 0x2A, 0x04, 0xE6, 0x9C, 0xB1, 0xE5, 0xBD, 0xA6, 0xE9, 0x9C, 0x96, 0x3A, 0x20, 0xE6,
    0x9C, 0x8D, 0xE5, 0x8A, 0xA1, 0xE5, 0x99, 0xA8, 0xEF, 0xBC, 0x9F, 0xE8, 0xBF, 0x98, 0xE6, 0x98,
    0xAF, 0xE4, 0xB8, 0x8B, 0xE8, 0xBD, 0xBD, 0xE7, 0x9B, 0xAE, 0xE5, 0xBD, 0x95,

    0x00, 0x00, 0x05, 0x00, 0x2E, 0xA4, 0xE8, 0x6F, 0x00, 0x1A, 0x01, 0x63, 0x6F, 0x6D, 0x2E, 0x61,
    0x6C, 0x69, 0x62, 0x61, 0x62, 0x61, 0x2E, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64, 0x2E, 0x72,
    0x69, 0x6D, 0x65, 0x74, 0x00, 0x02, 0x02, 0x04, 0x00, 0x10, 0x03, 0xE4, 0xBA, 0x8C, 0xE5, 0x93,
    0x88, 0xE7, 0x9A, 0x84, 0xE5, 0xB0, 0x8F, 0xE7, 0xAA, 0x9D, 0x00, 0x3B, 0x04, 0xE5, 0xB0, 0x8F,
    0xE7, 0x89, 0x9B, 0xE6, 0x9C, 0x89, 0xE8, 0x8D, 0x89, 0xE5, 0x90, 0x83, 0x3A, 0x20, 0xE7, 0x99,
    0xBD, 0xE9, 0x85, 0x92, 0xEF, 0xBC, 0x8C, 0xE5, 0x8C, 0xBB, 0xE8, 0x8D, 0xAF, 0xE8, 0xBF, 0x99,
    0xE9, 0x98, 0xB5, 0xE9, 0x83, 0xBD, 0xE6, 0xB2, 0xA1, 0xE6, 0xAD, 0xA2, 0xE7, 0x9B, 0x88, 0x5B,
    0xE5, 0x82, 0xBB, 0xE7, 0xAC, 0x91, 0x5D, 0x00, 0x00, 0x00
};
#define MESSAGE_STORE_PATH      "storage/sd1/C/DOWNLOAD/message.txt"
//动态获取消息
static int message_len[10][2] = {0};
static int sys_loadMessage(lua_State *L)
{
    int num = 0; //消息条数
    u8 message_flag = 0;
    static char *buf = 0;
    int message_type = 0;
    buf = buf_temp;
    lua_newtable(L);
    while (1) {
        message_flag = 0;
        *buf &= 0x000000ff;
        if (*buf == 0x00 && *(buf + 1) == 0x00 && *(buf + 2) == 0x00) {
            break;
        }
        if (*buf == 0x00 && *(buf + 2) == 0x01) {
            num++;
            /* printf(">>>>> 0x%x  0x%x  0x%x\n",*buf, *(buf + 1), *(buf + 2)); */
            lua_pushinteger(L, num);
            lua_newtable(L);
            if (*(buf + 3) == 0x63 && *(buf + 9) == 0x69 && *(buf + 15) == 0x61) {
                message_type = 1; //钉钉
            }
            if (*(buf + 3) == 0x22 && *(buf + 9) == 0x69 && *(buf + 15) == 0x61) {
                message_type = 2; //微信
            }
            if (*(buf + 3) == 0x33 && *(buf + 9) == 0x69 && *(buf + 15) == 0x61) {
                message_type = 3; //QQ
            }
            if (*(buf + 3) == 0x44 && *(buf + 9) == 0x69 && *(buf + 15) == 0x61) {
                message_type = 4; //短信
            }
        }
        if (*buf == 0x00 && *(buf + 2) == 0x03) {
            lua_pushstring(L, "title");	// title
            /* printf(">>>>> 0x%x  0x%x  0x%x\n",*buf, *(buf + 1), *(buf + 2)); */
            lua_pushinteger(L, buf + 3);
            message_len[num - 1][0] = *(buf + 1) - 1;
            lua_settable(L, -3);
        }
        if (*buf == 0x00 && *(buf + 2) == 0x04) {
            lua_pushstring(L, "content");
            /* printf(">>>>> 0x%x  0x%x  0x%x\n",*buf, *(buf + 1), *(buf + 2)); */
            lua_pushinteger(L, buf + 3);
            message_len[num - 1][1] = *(buf + 1) - 1;
            lua_settable(L, -3);
            message_flag = 1;
        }
        if (message_flag == 1) {
            /* printf(">>>>> 0x%x  0x%x  0x%x\n",*buf, *(buf + 1), *(buf + 2)); */
            lua_pushstring(L, "type");
            lua_pushinteger(L, message_type);
            lua_settable(L, -3);

            lua_settable(L, -3);
        }
        buf++;
        /* printf("0x%x\n",*buf); */
    }
    return 1;	// return tabt
}


// 获取短信长度
static int sys_getMessageLen(lua_State *L)
{
    int index = 0;
    index = lua_tointeger(L, -1);
    /* g_printf(">>>> index == %d\n",index); */
    lua_pop(L, 1);
    lua_newtable(L);
    lua_pushstring(L, "title_len");
    lua_pushinteger(L, message_len[index][0]);
    lua_settable(L, 2);
    stackDump(L);

    lua_pushstring(L, "content_len");
    lua_pushinteger(L, message_len[index][1]);
    lua_settable(L, 2);
    return 1;
}



static const luaL_Reg stepup_Method[] = {
    // 设置LCD背光亮度等级
    {"setLightLevel",       sys_setLightLevel},

    // 设置LCD息屏时间
    {"setDarkTime",         sys_setDarkTime},
    {"setAutoDark",         sys_setAutoDark},

    // 设置音量百分比
    {"setSysVolByPercent",  sys_setSysVolByPercent},
    {"getSysVolPercent",    sys_getSysVolPercent},

    // 设置音量增、减
    {"setSysVolUp",         sys_setSysVolUp},
    {"setSysVolDown",       sys_setSysVolDown},

    // 设置静音
    {"setSysVoiceMute",     sys_setSysVoiceMute},

    // 获取手表名称
    {"getWatchName",        sys_getWatchName},

    // 卡片相关
    {"getCardSelTab",       sys_getCardSelTab},
    {"setCardSelTab",       sys_setCardSelTab},
    {"getCardTab",          sys_getCardTab},
    {"setCardTab",          sys_setCardTab},

    // 重启系统
    {"ResetOrShutdown",     sys_ResetOrShutdown},
    {"sysParamVmOpt",       sys_sysParamVmOpt},
    {"syncSetupParam",      sys_syncSetupParam},

    {NULL, NULL}
};


static int sys_getSetupHandler(lua_State *L)
{
    lua_newtable(L);	/* 创建一个新table */
    luaL_setfuncs(L, stepup_Method, 0);

    return 1;	/* 返回这个table */
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end system steup


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ alarm
#if TCFG_APP_RTC_EN

extern void alarm_snooze();
extern void alarm_delete(u8 index);
extern u8 alarm_get_total(void);
extern u8 get_alarm_number2table(u8 num, u8 *table);

static int stata[5] = {0};
//设置指定闹钟，并将数据保存进vm区
static int sys_addAlarm(lua_State *L)
{
    int index = alarm_get_total();
    PT_ALARM alarm = (PT_ALARM)malloc(sizeof(T_ALARM));

    lua_getfield(L, -1, "sw");
    if (lua_isnumber(L, -1)) {
        alarm->sw = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "mode");
    if (lua_isnumber(L, -1)) {
        alarm->mode = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "hour");
    if (lua_isnumber(L, -1)) {
        alarm->time.hour = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "minute");
    if (lua_isnumber(L, -1)) {
        alarm->time.min = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_db("%s, hour:%d, min:%d, sw:%d, mode:0x%x\n", __FUNCTION__, \
           alarm->time.hour, alarm->time.min, alarm->sw, alarm->mode);
    u8 add_index;
    get_alarm_number2table(index, &add_index);
    alarm_add(alarm, add_index);
    free(alarm);

    return 0;
}


//读取指定闹钟数据，并返回给lua
//() --> 无参数，一次获取所有闹钟
//(index) --> 获取指定索引闹钟
static int sys_readAlarm(lua_State *L)
{
    PT_ALARM alarm = (PT_ALARM)malloc(sizeof(T_ALARM));

    if (lua_type(L, -1) == LUA_TNUMBER) {
        int index = lua_tointeger(L, -1);
        u8 read_index;
        get_alarm_number2table(index, &read_index);
        alarm_get_info(alarm, read_index);
        lua_db("%s, index:%d\n", __FUNCTION__, index);

        lua_newtable(L);
        lua_pushstring(L, "mode");
        lua_pushinteger(L, alarm->mode);
        lua_settable(L, -3);

        lua_pushstring(L, "sw");
        lua_pushinteger(L, alarm->sw);
        lua_settable(L, -3);

        lua_pushstring(L, "hour");
        lua_pushinteger(L, alarm->time.hour);
        lua_settable(L, -3);

        lua_pushstring(L, "minute");
        lua_pushinteger(L, alarm->time.min);
        lua_settable(L, -3);

        /* ui上没有闹钟名称显示，所以lua不获取闹钟名称 */
        lua_pushstring(L, "index");
        lua_pushinteger(L, alarm->index);
        lua_settable(L, -3);

        free(alarm);
        return 1;
    }

    lua_newtable(L);
    u8 alarm_total = alarm_get_total(); /* 已设置闹钟个数 */
    lua_db("%s, alarm_total:%d\n", __FUNCTION__, alarm_total);
    for (int i = 0; i < alarm_total; i++) {
        lua_pushinteger(L, i + 1);
        //获取指定闹钟数据，保存进alarm结构体
        alarm_get_info(alarm, i);

        lua_newtable(L);
        //将alarm中的值赋给table，返回给lua
        lua_pushstring(L, "mode");
        lua_pushinteger(L, alarm->mode);
        lua_settable(L, -3);

        lua_pushstring(L, "sw");
        lua_pushinteger(L, alarm->sw);
        lua_settable(L, -3);

        lua_pushstring(L, "hour");
        lua_pushinteger(L, alarm->time.hour);
        lua_settable(L, -3);

        lua_pushstring(L, "minute");
        lua_pushinteger(L, alarm->time.min);
        lua_settable(L, -3);

        /* ui上没有闹钟名称显示，所以lua不获取闹钟名称 */
        lua_pushstring(L, "index");
        lua_pushinteger(L, alarm->index);
        lua_settable(L, -3);

        lua_settable(L, -3);
    }
    free(alarm);

    return 1;   //返回数据的个数，1可返回整个table
}


//删除指定闹钟
static int sys_delAlarm(lua_State *L)
{
    u8 del_index;
    int index = lua_tointeger(L, -1);

    lua_db("%s, index:%d\n", __FUNCTION__, index);
    get_alarm_number2table(index, &del_index);
    alarm_delete(del_index);

    return 0;
}


// 闹钟开关设置
// (ONOFF, index, onoff) --> 索引，开关标志, 设置开关
// (TIME, index, hour, minute) --> 设置时间
// (MODE, index, mode) --> 设置模式
static int sys_setAlarm(lua_State *L)
{
    u8 set_index;
    T_ALARM alarm = {0};

    int set = lua_tointeger(L, 2);
    int index = lua_tointeger(L, 3); // 索引

    get_alarm_number2table(index, &set_index);
    alarm_get_info(&alarm, set_index);
    switch (set) {
    case SYS_ALARM_SET_ONOFF:
        int onoff = lua_tointeger(L, -1); // 开关
        lua_db("%s, index:%d, onoff:%d\n", __FUNCTION__, index, onoff);
        alarm.sw = onoff;
        break;
    case SYS_ALARM_SET_TIME:
        int minute = lua_tointeger(L, -1);
        int hour = lua_tointeger(L, -2);
        alarm.time.hour = hour;
        alarm.time.min = minute;
        lua_db("%s, index:%d, hour:%d, minute:%d\n", __FUNCTION__, index, hour, minute);
        break;
    case SYS_ALARM_SET_MODE:
        int mode = lua_tointeger(L, -1);
        alarm.mode = mode;
        lua_db("%s, index:%d, mode:%d\n", __FUNCTION__, index, mode);
        break;
    case SYS_ALARM_SET_SNOOZE:
        if (alarm_active_flag_get()) {
            alarm_stop(0);
        }
        alarm_snooze();
        break;
    case SYS_ALARM_SET_CLOSE:
        if (alarm_active_flag_get()) {
            alarm_stop(0);
        }
        break;
    default:
        printf("ERROR!, unknow set alarm flag!\n");
        break;
    }
    alarm_add(&alarm, set_index);

    return 0;
}


/* 将最终数据压入lua栈中,返回值通过数组下标获取，table索引从1开始 */
static void lua_push_stack(lua_State *L, int *data, int num)
{
    for (int index = 1; index < num + 1; index++) {
        lua_pushinteger(L, index);
        lua_pushinteger(L, *data);
        lua_settable(L, -3);
        data++;
    }
}


static int sys_orderDate(lua_State *L)
{
    int date_num = 0;
    int date_list[8] = {0};
    int cnt = 0;
    int flag_1 = 0;
    int flag_2 = 0;
    int flag_3 = 0;
    int flag_4 = 0;
    int flag_5 = 0;
    int flag_6 = 0;
    int flag_7 = 0;

    date_num = lua_tointeger(L, -1);
    /* y_printf("date_num is ->->-> %d\n",date_num); */
    /* stackDump(L); */
    lua_pop(L, 1);
    if (date_num >= 64) {
        date_num = date_num - 64;
        cnt = cnt + 1;
        flag_7 = 1;
    }
    if (date_num >= 32) {
        date_num = date_num - 32;
        cnt = cnt + 1;
        flag_6 = 1;
    }
    if (date_num >= 16) {
        date_num = date_num - 16;
        cnt = cnt + 1;
        flag_5 = 1;
    }
    if (date_num >= 8) {
        date_num = date_num - 8;
        cnt = cnt + 1;
        flag_4 = 1;
    }
    if (date_num >= 4) {
        date_num = date_num - 4;
        cnt = cnt + 1;
        flag_3 = 1;
    }
    if (date_num >= 2) {
        date_num = date_num - 2;
        cnt = cnt + 1;
        flag_2 = 1;
    }
    if (date_num == 1) {
        date_num = date_num - 1;
        cnt = cnt + 1;
        flag_1 = 1;
    }
    /* y_printf("value is ->->-> %d%d%d%d%d%d%d",flag_7, flag_6, flag_5, flag_4, flag_3, flag_2, flag_1); */
    date_list[0] = cnt;
    for (int num_cnt = 0; num_cnt < cnt; num_cnt++) {
        if (flag_1 == 1) {
            date_list[num_cnt + 1] = 1;
            flag_1 = 0;
            continue;
        }
        if (flag_2 == 1) {
            date_list[num_cnt + 1] = 2;
            flag_2 = 0;
            continue;
        }
        if (flag_3 == 1) {
            date_list[num_cnt + 1] = 3;
            flag_3 = 0;
            continue;
        }
        if (flag_4 == 1) {
            date_list[num_cnt + 1] = 4;
            flag_4 = 0;
            continue;
        }
        if (flag_5 == 1) {
            date_list[num_cnt + 1] = 5;
            flag_5 = 0;
            continue;
        }
        if (flag_6 == 1) {
            date_list[num_cnt + 1] = 6;
            flag_6 = 0;
            continue;
        }
        if (flag_7 == 1) {
            date_list[num_cnt + 1] = 7;
            flag_7 = 0;
            continue;
        }
    }
    lua_newtable(L);
    lua_push_stack(L, date_list, 8);

    return 1;
}


static int sys_stataWrite(lua_State *L)
{
    lua_getfield(L, -1, "one");
    if (lua_isnumber(L, -1)) {
        stata[0] = lua_tointeger(L, -1);
        /* printf(">>>>>> alarm index is:%d\n",alarm->index); */
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "two");
    if (lua_isnumber(L, -1)) {
        stata[1] = lua_tointeger(L, -1);
        /* printf(">>>>>> alarm sw is:%d\n",alarm->sw); */
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "three");
    if (lua_isnumber(L, -1)) {
        stata[2] = lua_tointeger(L, -1);
        /* printf(">>>>>> alarm mode is:%d\n",alarm->mode); */
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "four");
    if (lua_isnumber(L, -1)) {
        stata[3] = lua_tointeger(L, -1);
        /* printf(">>>>>> alarm hour is:%d\n",alarm->time.hour); */
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "five");
    if (lua_isnumber(L, -1)) {
        stata[4] = lua_tointeger(L, -1);
        /* printf(">>>>>> alarm min is:%d\n",alarm->time.min); */
    }
    lua_pop(L, 1);
    /* printf("coming in alarm_add!!\n"); */

    return 0;
}


static int sys_stataRead(lua_State *L)
{
    lua_newtable(L);
    lua_pushstring(L, "one");  //将alarm中的值赋给table，返回给lua
    lua_pushinteger(L, stata[0]);
    lua_settable(L, 2);

    lua_pushstring(L, "two");
    lua_pushinteger(L, stata[1]);
    lua_settable(L, 2);

    lua_pushstring(L, "three");
    lua_pushinteger(L, stata[2]);
    lua_settable(L, 2);

    lua_pushstring(L, "four");
    lua_pushinteger(L, stata[3]);
    lua_settable(L, 2);

    lua_pushstring(L, "five");
    lua_pushinteger(L, stata[4]);
    lua_settable(L, 2);

    return 1;   //返回数据的个数，1可返回整个table
}



static const luaL_Reg alarm_Method[] = {
    {"addAlarm",	sys_addAlarm},
    {"readAlarm",	sys_readAlarm},
    {"setAlarm",	sys_setAlarm},
    {"delAlarm",	sys_delAlarm},
    {"orderDate",	sys_orderDate},
    {"stataWrite",	sys_stataWrite},
    {"stataRead",	sys_stataRead},
};

static int sys_getAlarmHandler(lua_State *L)
{
    lua_newtable(L);	/* 创建一个新table */
    luaL_setfuncs(L, alarm_Method, 0);

    return 1;	/* 返回这个table */
}
#endif
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end alarm


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ system time
int get_system_time(struct sys_time *time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        return -1;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
    dev_close(fd);
    return 0;
}

int set_system_time(struct sys_time *time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        return -1;
    }
    dev_ioctl(fd, IOCTL_SET_SYS_TIME, (u32)time);
    dev_close(fd);
    return 0;
}


// 获取系统时间
// 参数：无
// 返回：table
// table.year, table.month, table.day, table.hour, table.minute, table.second
static int sys_getSystemTime(lua_State *L)
{
    struct sys_time time = {0};

    get_system_time(&time);
    lua_db("get time: %d-%d-%d %d:%d:%d\n", \
           time.year, time.month, time.day, time.hour, time.min, time.sec);

    lua_newtable(L);

    lua_pushstring(L, "year");
    lua_pushinteger(L, time.year);
    lua_settable(L, -3);

    lua_pushstring(L, "month");
    lua_pushinteger(L, time.month);
    lua_settable(L, -3);

    lua_pushstring(L, "day");
    lua_pushinteger(L, time.day);
    lua_settable(L, -3);

    lua_pushstring(L, "hour");
    lua_pushinteger(L, time.hour);
    lua_settable(L, -3);

    lua_pushstring(L, "minute");
    lua_pushinteger(L, time.min);
    lua_settable(L, -3);

    lua_pushstring(L, "second");
    lua_pushinteger(L, time.sec);
    lua_settable(L, -3);

    return 1;
}


// 设置系统时间
// 参数：table
// table.year, table.month, table.day, table.hour, table.minute, table.second
// 如果table中某个参数为nil，则该参数不设置，即设置table中可省略参数
// 返回：无
static int sys_setSystemTime(lua_State *L)
{
    struct sys_time time = {0};
    // 先获取系统时间，如果有参数省略则用原来系统时间参数，防止被设置成0
    get_system_time(&time);

    lua_getfield(L, -1, "year");
    if (lua_isnumber(L, -1)) {
        time.year = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "month");
    if (lua_isnumber(L, -1)) {
        time.month = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "day");
    if (lua_isnumber(L, -1)) {
        time.day = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "hour");
    if (lua_isnumber(L, -1)) {
        time.hour = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "minute");
    if (lua_isnumber(L, -1)) {
        time.min = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "second");
    if (lua_isnumber(L, -1)) {
        time.sec = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_db("set time: %d-%d-%d %d:%d:%d\n", time.year, time.month, time.day, time.hour, time.min, time.sec);
    set_system_time(&time);

    return 0;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end system time


// 系统延时
// 参数：
// (ms)
static int sys_systemSleep(lua_State *L)
{
    int time = 0;

    time = lua_tointeger(L, -1);
    lua_db("lua sleep time:%d\n", time);
    os_time_dly(time / 2);

    return 0;
}

// 获取随机数
// 参数：
// ()	--> 省略
// (min, max) --> 指定最小值、最大值
static int sys_random(lua_State *L)
{
    int min, max;
    int n = lua_gettop(L);
    /* unsigned int rand = (JL_RAND->R64H << 32) | JL_RAND->R64L; */
    unsigned int rand = JL_RAND->R64L;

    if (3 == n) {
        max = lua_tointeger(L, -1);
        min = lua_tointeger(L, -2);
        rand = ((rand % (max - min)) + min);
        lua_pop(L, 2);
    }
    lua_pushinteger(L, rand);

    return 1;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ file io
const struct luaL_Reg file_method[];
// (type) --> 整形type，打开通话记录、通讯录的时候传入对应type
// (path, mode) --> 普通文件操作，传入路径、模式，均为字符串
static int sys_fileIoOpen(lua_State *L)
{
    /* 判断第一个参数，如果是整形，说明操作通讯录和联系人，如果是字符串说明是路径 */
    int arg_type = lua_type(L, 2);

    if (arg_type == LUA_TSTRING) {
        char *file_mode = lua_tostring(L, -1);	// 读操作模式
        char *file_path = lua_tostring(L, -2);	// 读文件路径
        lua_pop(L, 2);

        /*
         * 对文件的操作有两种方式，open的时候返回文件对象
         * 方式1：file = io:open(path, mode)
         * 			io:read(file, arg)
         * 方式2：file = io:open(path, mode)
         * 			file:read(arg)
         */

        FILE *fp = fopen(file_path, file_mode);
        if (!fp) {
            printf("ERROR! open file \"%s\" faild!\n", file_path);
            return 0;
        }
        lua_db("%s, path: %s, mode: %s\n", __FUNCTION__, file_path, file_mode);

        /* 文件打开成功，构建返回table */
        lua_newtable(L);

        lua_pushstring(L, "path");
        lua_pushstring(L, file_path);	// 保存路径
        lua_settable(L, -3);

        lua_pushstring(L, "mode");
        lua_pushstring(L, file_mode);	// 保存模式
        lua_settable(L, -3);

        lua_pushstring(L, "hdl");
        lua_pushlightuserdata(L, fp);	// 保存句柄
        lua_settable(L, -3);

        luaL_setfuncs(L, file_method, 0); // 添加文件操作方法
    } else if (arg_type == LUA_TNUMBER) {
        int err = 0;
        u8 type = 0;	// 读取类型：0联系人，1通话记录
        u32 count = 0;

        type = lua_tointeger(L, -1);
        lua_pop(L, 1);

        err = open_file_handler(type, 0);
        if (err) {
            printf("error, open_file_handler err!");
            return 0;
        }

        // 如果打开成功
        // 1. 记录本对象操作的类型
        lua_pushstring(L, "hdl_typ");
        lua_pushinteger(L, type);
        lua_settable(L, -3);

        // 2. 返回文件数量
        count = book_list_get_count(type);
        lua_pushinteger(L, count);
        lua_db("%s, open list count:%d\n", __FUNCTION__, count);
    }

    return 1;
}

// 拿文件句柄
static FILE *get_file_handler(lua_State *L)
{
    int type = 0;
    FILE *fp = NULL;
    lua_pushstring(L, "hdl");
    lua_gettable(L, 1);

    type = lua_type(L, -1);
    if (type != LUA_TNIL) {
        fp = (FILE *)lua_touserdata(L, -1);
        lua_pop(L, 1);
    } else {
        lua_pop(L, 1);
        type = lua_type(L, 2);
        if (type == LUA_TTABLE) {
            lua_pushstring(L, "hdl");
            lua_gettable(L, 2);
            fp = (FILE *)lua_touserdata(L, -1);
            lua_pop(L, 1);
        }
    }

    return fp;
}

// (index)
// io:(file, arg)	--> 把文件table作为参数，后面跟其它参数
// file:read(arg) --> 文件对象的read方法，只传入参数
static int sys_fileIoRead(lua_State *L)
{
    /*
     * 首先判断self table 中是否有FILE句柄，
     * 有：文件对象调用read方法；
     * 无：判断第一个参数是否为tabel
     * 	是：传入文件对象的read方法
     * 	否：读写通讯录、联系人方法
     */

    FILE *fp = get_file_handler(L);

    if (fp) {
        /* 有句柄，普通文件操作 */
        int len = lua_tointeger(L, -1);
        char *buf = (char *)malloc(len + 1);
        len = fread(fp, buf, len);
        lua_pushstring(L, buf);
        free(buf);
    } else {
        /* 无句柄，通讯录、联系人操作 */
        u8 type = 0;
        int read_mode = 0;
        read_mode = lua_tointeger(L, 2);

        lua_getfield(L, 1, "hdl_typ");

        type = lua_tointeger(L, -1);
        lua_db("%s, type:%d\n", __FUNCTION__, type);
        lua_pop(L, 1);

        if (read_mode == SYS_VM_READ_LIST_COUNT) {
            u32 count = book_list_get_count(type);
            lua_pushinteger(L, count);
        } else if (read_mode == SYS_VM_READ_LIST_INDEX) {
            phonebook *pb = NULL;
            u8 index = lua_tointeger(L, -1);
            lua_pop(L, 1);
            /* u8 index_tmp = lua_tointeger(L, -2); */
            pb = book_list_read_by_index(type, index);
            lua_db("%s, name:%s, number:%s, date:%s\n", pb->name, pb->number, pb->date);

            lua_newtable(L);

            lua_pushstring(L, "name");
            /* lua_pushstring(L, pb->name); */
            lua_pushinteger(L, (u32) & (pb->name));
            lua_settable(L, -3);

            lua_pushstring(L, "name_len");
            lua_pushinteger(L, strlen(pb->name));
            lua_settable(L, -3);

            lua_pushstring(L, "number");
            lua_pushstring(L, pb->number);
            lua_settable(L, -3);

            /* lua_pushstring(L, "date"); */
            /* lua_pushstring(L, pb->date); */
            /* lua_settable(L, -3); */

            /* lua_pushinteger(L, pb->type); */
            /* lua_setfield(L, -2, "type"); */

            /* lua_pushinteger(L, pb->mask); */
            /* lua_setfield(L, -2, "mask"); */
        } else if (read_mode == SYS_VM_READ_NAME_BY_NUMBER) {
            u8 name[10] = {0};
            u8 *number = lua_tostring(L, -1);

            phonebook_get_name_by_number(number, name);
            lua_db("%s, name:%s, number:%s\n", __FUNCTION__, name, number);
            lua_pushstring(L, name);
        }
    }

    return 1;
}

static int sys_fileIoWrite(lua_State *L)
{
    FILE *fp = get_file_handler(L);

    if (fp) {
        int len = lua_tointeger(L, -1);
        char *buf = (char *)malloc(len);
        buf = lua_tostring(L, -2);
        len = fwrite(fp, buf, len);
        free(buf);
    } else {
        int type = 0;
        int mode = lua_tointeger(L, 2);
        if (mode == SYS_VM_SAVE_LIST_BOOK) {
            lua_getfield(L, 1, "hdl_typ");
            type = lua_tointeger(L, -1);
            lua_pop(L, 1);

            u8 *date = lua_tostring(L, -1);
            u8 *number = lua_tostring(L, -2);
            u8 *name = lua_tostring(L, -3);
            /* lua_pop(L, 4); */
            lua_db("%s, name:%s, number:%s, date:%s\n", __FUNCTION__, name, number, date);

            extern void book_list_save(u8 type, const u8 * name, const u8 * number, const u8 * date);
            book_list_save(type, name, number, date);
        }
    }

    return 0;
}

static int sys_fileIoDelete(lua_State *L)
{
    FILE *fp = get_file_handler(L);

    if (fp) {
        fdelete(fp);
    } else {
        int type = 0;
        lua_getfield(L, 1, "hdl_typ");
        type = lua_tointeger(L, -1);
        lua_db("%s, file io handler type:%d\n", type);

        if (type == SYS_OPERATE_CALLLOG) {
            delete_call_file();
        }
    }
    return 0;
}

static int sys_fileIoSeek(lua_State *L)
{
    FILE *fp = get_file_handler(L);

    if (fp) {
        int seek_mode = lua_tointeger(L, -1);
        int offset = lua_tointeger(L, -2);
        lua_db("%s, offset:%d, mode:%d\n", __FUNCTION__, offset, seek_mode);
        fseek(fp, offset, seek_mode);
    }

    return 0;
}

static int sys_fileIoAttrs(lua_State *L)
{
    FILE *fp = get_file_handler(L);
    if (fp) {
        struct vfs_attr attr = {0};
        fget_attrs(fp, &attr);

        lua_newtable(L);

        lua_pushstring(L, "fsize");
        lua_pushinteger(L, attr.fsize);
        lua_settable(L, -3);
        return 1;
    }
    return 0;
}

static int sys_fileIoClose(lua_State *L)
{
    FILE *fp = get_file_handler(L);

    if (fp) {
        fclose(fp);
    } else {
        int type = 0;
        lua_getfield(L, 1, "hdl_typ");
        type = lua_tointeger(L, -1);
        lua_db("%s, file io handler type:%d\n", type);

        if (type == SYS_OPERATE_CONTACTS || type == SYS_OPERATE_CALLLOG) {
            close_file_handler(type);
        }
    }

    return 0;
}


// 单个文件的方法
const struct luaL_Reg file_method[] = {
    {"read",	sys_fileIoRead},
    {"write",	sys_fileIoWrite},
    {"seek",	sys_fileIoSeek},
    {"attrs",	sys_fileIoAttrs},
    {"delete",	sys_fileIoDelete},
    {"close",	sys_fileIoClose},

    {NULL, NULL}
};


// 文件的通用方法
const struct luaL_Reg fileIo_method[] = {
    {"open",	sys_fileIoOpen},
    {"read",	sys_fileIoRead},
    {"write",	sys_fileIoWrite},
    {"seek",	sys_fileIoSeek},
    {"attrs",	sys_fileIoAttrs},
    {"delete",	sys_fileIoDelete},
    {"close",	sys_fileIoClose},

    {NULL, NULL}
};

/*
 * 现在文件系统只有读写通话记录、联系人功能
 * */
static int sys_getIoHandler(lua_State *L)
{
    lua_newtable(L);	/* 创建一个新table */
    luaL_setfuncs(L, fileIo_method, 0);
    return 1;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end file io


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ timer
struct lua_timer_def {
    int ms;
    int tid;
    int fid;
    int arg;
    lua_State *lua;
    struct lua_timer_def *self;
};


// timer的C回调函数
static void timer_callback(void *priv)
{
    struct lua_timer_def *timer = (struct lua_timer_def *)priv;

    /* lua_db("tiemr run: func_index %d, tiemr_id %d, msec %d\n", timer->fid, timer->tid, timer->ms); */

    lua_rawgeti(L, LUA_REGISTRYINDEX, timer->fid);	/* 把定时器回调函数放到栈顶 */
    if (timer->arg == LUA_REFNIL) {
        /* 回调函数没有参数 */
        lua_call(L, 0, 0);
    } else {
        /* 回调函数有参数 */
        lua_rawgeti(L, LUA_REGISTRYINDEX, timer->arg);	/* 把定时器回调参数放到栈顶 */
        lua_call(L, 1, 0);	/* 执行栈顶的函数 */
    }
    /* mem_stats(); */
}

// timeout的C回调函数
static void timeout_callback(void *priv)
{
    struct lua_timer_def *timer = (struct lua_timer_def *)priv;

    /* lua_db("tiemr run: func_index %d, tiemr_id %d, msec %d\n", timer->fid, timer->tid, timer->ms); */

    lua_rawgeti(L, LUA_REGISTRYINDEX, timer->fid);	/* 把定时器回调函数放到栈顶 */
    if (timer->arg == LUA_REFNIL) {
        /* 回调函数没有参数 */
        lua_call(L, 0, 0);
    } else {
        /* 回调函数有参数 */
        lua_rawgeti(L, LUA_REGISTRYINDEX, timer->arg);	/* 把定时器回调参数放到栈顶 */
        lua_call(L, 1, 0);	/* 执行栈顶的函数 */
    }
    /* mem_stats(); */
#if 0
    // 还未测试，先添加代码
    /* 释放掉函数引用 */
    lua_getfield(L, -1, "fid");
    func_index = lua_tointeger(L, -1);
    luaL_unref(L, LUA_REGISTRYINDEX, func_index);
    lua_pop(L, 1);

    lua_getfield(L, -1, "arg");
    func_argInd = lua_tointeger(L, -1);
    luaL_unref(L, LUA_REGISTRYINDEX, func_argInd);
    lua_pop(L, 1);

    /* 清空定时器id */
    lua_pushstring(timer->lua, "tid");
    lua_pushinteger(timer->lua, 0);
    lua_settable(timer->lua, -3);

    /* 释放参数buffer */
    free(timer);
#endif
}



// 从table中拿到参数配置并注册定时器
// 启动定时器
static int bsp_timerRun(lua_State *L)
{
    int msec = 0;
    int type = 0;
    int timer_id = 0;
    int func_index = 0;
    int func_arg = 0;
    struct lua_timer_def *timer;

    msec = lua_get_interget_from_self(L, "ms");			/* 拿定时时间 */
    timer_id = lua_get_interget_from_self(L, "tid");	/* 拿定时器id */

    if (timer_id) {
        /* 如果定时器已经在跑则直接退出 */
        printf("timer %d is running\n");
        return 0;
    }

    lua_pushstring(L, "fid");	/* 拿函数索引 */
    lua_gettable(L, 1);
    func_index = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "arg");	/* 拿参数索引 */
    lua_gettable(L, 1);
    func_arg = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "typ");	/* 拿回调类型 */
    lua_gettable(L, 1);
    type = lua_tointeger(L, -1);
    lua_pop(L, 1);


    /* 申请timer内存，stop时释放 */
    timer = (struct lua_timer_def *)malloc(sizeof(struct lua_timer_def));
    timer->ms  = msec;
    timer->fid = func_index;
    timer->arg = func_arg;
    timer->lua = L;
    /* timer.self = &tiemr; */

    // 创建定时器
    if (type == BSP_TIMEOUT_HANDLER) {
        timer->tid = sys_timeout_add(timer, timeout_callback, timer->ms);
        lua_db("timeout run:0x%x fid:%d, ms:%d, tiemr_id:%d\n", timer, timer->fid, timer->ms, timer->tid);
    } else {
        timer->tid = sys_timer_add(timer, timer_callback, timer->ms);
        lua_db("timer out:0x%x fid:%d, ms:%d, tiemr_id:%d\n", timer, timer->fid, timer->ms, timer->tid);
    }

    if (timer->tid) {
        // 创建定时器有定时器id返回，认为成功创建定时器，保存定时器id和timer buffer地址到table
        // 保存tiemr结构体地址
        lua_pushstring(L, "timer");
        lua_pushinteger(L, (u32)timer);
        lua_settable(L, 1);

        // 保存定时器id
        lua_pushstring(L, "tid");
        lua_pushinteger(L, timer->tid);
        lua_settable(L, 1);

        lua_pushinteger(L, timer->tid);	/* 把定时器id再返回给lua */
        return 1;
    } else {
        // 定时器id返回0，认为创建定时器失败
        free(timer);
        printf("ERROR: func:%s, run tiemr faild!\n", __FUNCTION__);
        return 0;
    }
}


// 删除定时器并释放配置
// 注意：这里只释放C的资源，lua部分的资源要在deleteTimer里面释放
static int bsp_timerStop(lua_State *L)
{
    int timer_id = 0;
    int type = 0;
    struct lua_timer_def *timer;

    timer_id = lua_get_interget_from_self(L, "tid");
    type = lua_get_interget_from_self(L, "typ");
    timer = (struct lua_timer_def *)lua_get_interget_from_self(L, "timer");
    lua_db("%s: tiemr_id:%d, timer:0x%x\n", __FUNCTION__, timer_id, timer);

    if (timer_id) {
        lua_db("stop timer %d\n", timer_id);
        if (type == BSP_TIMEOUT_HANDLER) {
            sys_timeout_del(timer_id);	/* 删除定时器 */
        } else {
            sys_timer_del(timer_id);	/* 删除定时器 */
        }

        lua_pushstring(L, "tid");
        lua_pushinteger(L, 0);
        lua_settable(L, 1);

        lua_pushstring(L, "timer");
        lua_pushinteger(L, 0);
        lua_settable(L, 1);

        // 释放malloc的tiemr内存
        free(timer);
    } else {
        printf("tiemr %d has stop!\n", timer_id);
    }

    return 0;
}


const luaL_Reg timer_Method[] = {
    {"run",  bsp_timerRun},
    {"stop", bsp_timerStop},

    {NULL, NULL}
};


// 创建定时器对象
// 这里不注册，只是把传入的参数保存到table中准备给回调
// 参数：
// (func, ms) timer
// (func, ms, flag) timer / timeout
// (table, func, ms) timer
// (table, func, ms, flag) timer / timeout
// flag可选参数，省略或传timer则为定时器，timeout则为timeout句柄
//
// 返回：table
static int bsp_createTimer(lua_State *L)
{
    int msec = 0;
    int flag = 0x00;
    int timer_id = 0;
    int func_index = LUA_REFNIL;
    int func_argInd = LUA_REFNIL;

    /* 获取栈顶参数 */
    msec = lua_tointeger(L, -1);
    lua_pop(L, 1);

    if (lua_isnumber(L, -1)) {
        /* 如果栈顶第二个参数还是数字，说明传了flag */
        flag = msec;
        msec = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    luaL_checktype(L, -1, LUA_TFUNCTION); /* 检查是不是func */
    func_index = luaL_ref(L, LUA_REGISTRYINDEX);	/* 获取函数索引 */

    int n = lua_gettop(L);
    if (2 == n) {
        /* 如果取完函数后还有两个元素，说明有参数, 回调函数的参数只能是table */
        luaL_checktype(L, -1, LUA_TTABLE); /* 检查是不是table */
        func_argInd = luaL_ref(L, LUA_REGISTRYINDEX);	/* 获取参数table的索引 */
    }

    lua_newtable(L);	/* 创建table，用于保存timer信息 */

    // 保存注册的事件类型
    lua_pushstring(L, "typ");
    lua_pushinteger(L, flag);
    lua_settable(L, -3);

    // 把延时时间保存到table
    lua_pushstring(L, "ms");
    lua_pushinteger(L, msec);
    lua_settable(L, -3);

    // 把回调函数索引保存到table
    lua_pushstring(L, "fid");
    lua_pushinteger(L, func_index);
    lua_settable(L, -3);

    // 把回调函数参数table的索引保存到table中
    lua_pushstring(L, "arg");
    lua_pushinteger(L, func_argInd);
    lua_settable(L, -3);

    // 定时器id保存到table
    lua_pushstring(L, "tid");
    lua_pushinteger(L, timer_id);
    lua_settable(L, -3);

    lua_db("cerate timer (func_index: %d, arg_index: %d, msec: %d, type:%d)\n", func_index, func_argInd, msec, flag);

    // 加载定时器方法到table
    luaL_setfuncs(L, timer_Method, 0);

    return 1;
}

// 删除定时器，释放引用资源
// 如果定时器还未停止，先停止定时器，然后再删除
static int bsp_deleteTimer(lua_State *L)
{
    int timer_id = 0;
    int type = 0;
    struct lua_timer_def *timer;
    int func_index = LUA_REFNIL;
    int func_argInd = LUA_REFNIL;

    if (lua_isnil(L, -1)) {
        printf("ERROR! %s delete a nil timer!\n", __FUNCTION__);
        return 0;
    }

    lua_getfield(L, -1, "tid");
    timer_id = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "typ");
    type = lua_tointeger(L, -1);
    lua_pop(L, 1);

    if (timer_id) {
        lua_db("stop timer %d\n", timer_id);
        if (type == BSP_TIMEOUT_HANDLER) {
            sys_timeout_del(timer_id);	/* 删除定时器 */
        } else {
            sys_timer_del(timer_id);	/* 删除定时器 */
        }

        lua_pushstring(L, "tid");
        lua_pushinteger(L, 0);
        lua_settable(L, -3);

        // 释放malloc的tiemr内存
        lua_getfield(L, -1, "timer");
        timer = lua_tointeger(L, -1);
        lua_pop(L, 1);
        if (timer) {
            free(timer);
        }

        lua_pushstring(L, "timer");
        lua_pushinteger(L, 0);
        lua_settable(L, -3);
        lua_pop(L, 1);	// table出栈
    }

    lua_getfield(L, -1, "fid");
    func_index = lua_tointeger(L, -1);
    luaL_unref(L, LUA_REGISTRYINDEX, func_index);
    lua_pop(L, 1);

    lua_getfield(L, -1, "arg");
    func_argInd = lua_tointeger(L, -1);
    if (func_argInd != LUA_REFNIL) {
        luaL_unref(L, LUA_REGISTRYINDEX, func_argInd);
        lua_pop(L, 1);
    }

    return 0;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end timer



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ event handler
/*
 * 事件处理回调注册功能：
 * 1. luaHandlerTable 表中只需添加事件类型名称即可，第二个参数保持为NULL，例：{"bt_state", NULL}
 * 2. SDK在对应事件需触发回调位置添加回调触发功能，需导入lua_bsp.h头文件，lua相关头文件根据需求添加
 * 2.1 --> 触发回调示例：无参数传递给lua回调函数
 * 		--> lua注册回调
 * 		function testHandler()
 * 			print("lua test handler")
 * 		end
 * 		bsp:registerHandler("handler1", testhandler) -- 将testhandler注册为handler1的回调
 *
 * 		--> SDK在需要触发handler1的位置触发回调
 * 		#include "lua_bsp.h"
 *
 * 		LuaRegHandler_def *hdl = NULL;
 * 		hdl = getLuaHandler("handler1", NULL);
 * 		callLuaHandler(hdl);
 *
 * 2.2 --> 触发回调示例：给lua回调传递一个整数参数
 * 		--> lua注册回调
 * 		function testHandler2(arg)
 * 			print("lua test handler, arg: "..tostring(arg))
 * 		end
 * 		bsp:registerHandler("handler2", testhandler2) -- 将testhandler2注册为handler2的回调
 *
 * 		--> SDK在需要触发handler1的位置触发回调
 * 		#include "lua.h"
 * 		#include "lua_bsp.h"
 *
 * 		LuaRegHandler_def *hdl = NULL;
 * 		hdl = getLuaHandler("handler2", NULL);
 * 		lua_pushinteger(hdl->lua, 10); // 整数10压到lua栈
 * 		hdl->arg_number = 1;	// 一个参数给回调
 * 		callLuaHandler(hdl);
 *
 */
LuaHandlerTable_def luaHandlerTable[] = {
    {"bt_list", NULL},
    {"bt_status", NULL},
    {"bt_connect_info", NULL},
    {"message_status", NULL},
    {"music_start", NULL},
    {"music_status", NULL},
    {"heart_rate", NULL},
    {"weather_info", NULL},
    {"phone_num", NULL},
    {"upgrade", NULL},

    {NULL, NULL}
};

static int getHandlerIndex(char *type)
{
    for (int i = 0; luaHandlerTable[i].type; i++) {
        if (!strcmp(luaHandlerTable[i].type, type)) {
            return i;
        }
    }
    return -1;
}

// 获取指定事件类型的结构
LuaRegHandler_def *getLuaHandler(const char *type, int *type_len)
{
    lua_db("|---------> %s, lua event:%s, type_len:%s", __FUNCTION__, type, type_len);
    LuaRegHandler_def *handler = NULL;
    for (int i = 0; luaHandlerTable[i].type; i++) {
        int len = strlen(luaHandlerTable[i].type);
        /* if (!strcmp(luaHandlerTable[i].type, type)) { */
        if (!memcmp(luaHandlerTable[i].type, type, len)) {
            lua_db("get lua server handler, type:%s, handler:0x%x\n", \
                   luaHandlerTable[i].type, luaHandlerTable[i].LuaRegHandler);
            handler = luaHandlerTable[i].LuaRegHandler;
            if (type_len) {
                *type_len = len;
            }
            return handler;
        }
    }
    // 表中没有对应事件
    printf("ERROR : no such server handler:%s\n", type);
    return NULL;
}

// 启动lua回调
void callLuaHandler(LuaRegHandler_def *hdl)
{
    if (hdl) {
        lua_rawgeti(hdl->lua, LUA_REGISTRYINDEX, hdl->func_index);
        if (hdl->arg_number > 0) {
            /* 将函数在栈中的位置移动到参数底下 */
            lua_insert(hdl->lua, -(hdl->arg_number + 1));
            /* stackDump(hdl->lua); */
        }
        lua_call(hdl->lua, hdl->arg_number, 0);
    } else {
        printf("ERROR : try to call a empty handler.");
    }
}

// 打印事件列表
static void printTable()
{
    printf("\n>>> ----- lua handler table ----- <<<\n");
    for (int i = 0; luaHandlerTable[i].type; i++) {
        printf("type: %s, handler:0x%x\n", \
               luaHandlerTable[i].type, \
               luaHandlerTable[i].LuaRegHandler);
    }
    printf(">>> -------------end------------- <<<\n\n");
}

// 注册回调函数
// (type, function)
static int bsp_registerServerHandler(lua_State *L)
{
    int handler_ind = 0;
    int func_index = LUA_REFNIL;
    char *handler_type = NULL;
    LuaRegHandler_def *handler = NULL;

    luaL_checktype(L, -1, LUA_TFUNCTION);
    func_index = luaL_ref(L, LUA_REGISTRYINDEX);
    handler_type = lua_tostring(L, -1);

    printTable();
    handler_ind = getHandlerIndex(handler_type); /* 获取要注册的回调类型索引 */
    if (handler_ind == -1) {
        /* 要注册的回调类型不在允许的范围内 */
        printf("ERROR : no such handler type:%s\n", handler_type);
        return 0;
    }
    lua_db("register handler type:%s, func_index:%d\n", handler_type, func_index);

    // 申请handler空间
    if (!luaHandlerTable[handler_ind].LuaRegHandler) {
        /* 如果没注册过就重新申请空间 */
        handler = (LuaRegHandler_def *)malloc(sizeof(LuaRegHandler_def));
    } else {
        /* 如果已经注册过就用原来空间 */
        handler = luaHandlerTable[handler_ind].LuaRegHandler;
        /* 把原本已经注册的回调索引释放 */
        luaL_unref(L, LUA_REGISTRYINDEX, handler->func_index);
    }
    handler->lua = getLuaState();	/* 配置lua状态机 */
    handler->func_index = func_index;	/* 回调函数索引 */
    handler->arg_number = 0;	/* 给回调函数的参数，初始化为0，由调用的地方决定传多少参数给回调函数 */

    // 把handler保存到索引表里面
    luaHandlerTable[handler_ind].LuaRegHandler = handler;
    printTable();

    return 0;
}

// 删除回调函数
// (type)
static int bsp_relaseServerHandler(lua_State *L)
{
    int handler_ind = 0;
    char *handler_type = NULL;
    LuaRegHandler_def *hdl = NULL;

    handler_type = lua_tostring(L, -1);
    handler_ind = getHandlerIndex(handler_type);
    printTable();
    lua_db("release lua handler:%s\n", handler_type);

    if ((handler_ind != -1) && luaHandlerTable[handler_ind].LuaRegHandler) {
        /* 释放注册时创建的函数索引，解除lua引用，防止lua内存溢出 */
        hdl = getLuaHandler(handler_type, NULL);
        luaL_unref(L, LUA_REGISTRYINDEX, hdl->func_index);

        /* 释放动态申请的内存 */
        free(luaHandlerTable[handler_ind].LuaRegHandler);
        luaHandlerTable[handler_ind].LuaRegHandler = NULL;
    }
    printTable();

    return 0;
}

// 检查回调是否注册
// 参数：
// type --> 事件类型
// 返回：
// true --> 已经注册
// false --> 还未注册
static int bsp_checkServerHandler(lua_State *L)
{
    int handler_ind = 0;
    char *handler_type = NULL;

    handler_type = lua_tostring(L, -1);
    handler_ind = getHandlerIndex(handler_type);

    if ((handler_ind != -1) && luaHandlerTable[handler_ind].LuaRegHandler) {
        lua_db("%s, handler:%s has register\n", __FUNCTION__, handler_type);
        lua_pushboolean(L, 1);
    } else {
        lua_db("%s, handler:%s unregister\n", __FUNCTION__, handler_type);
        lua_pushboolean(L, 0);
    }

    return 1;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ event handler end



// 加载天气方法
extern const luaL_Reg obj_WeatherMethod[];
static int sys_loadWeather(lua_State *L)
{
#if TCFG_UI_ENABLE_WEATHER
    lua_newtable(L);	/* 创建一个新table */
    luaL_setfuncs(L, obj_WeatherMethod, 0);
    return 1;
#else
    return 0;
#endif
}

static int sys_getNameByPhoneNum(lua_State *L)
{
    static u8 phone_num[16] = {"               "};
    static u8 phone_name[20] = {0};
    int len = 0;
    extern int phonebook_get_name_by_number(u8 * number, u8 * name);
    len = phonebook_get_name_by_number(phone_num, phone_name);
    if (len > 0) {
        lua_pushstring(L, phone_name);
    } else {
        lua_pushstring(L, "UNKNOW");
    }

    return 1;
}

// sys模块方法列表
const struct luaL_Reg sys_method[] = {
    //获取系统设置对象
    {"getSetupHandler",	sys_getSetupHandler},

    // 系统时间
    {"getTime",		sys_getSystemTime},
    {"setTime",		sys_setSystemTime},
    {"sleep",		sys_systemSleep},
    {"random",		sys_random},

#if TCFG_APP_RTC_EN
    // 闹钟对象
    {"getAlarmHandler",	sys_getAlarmHandler},
#endif

    // 获取文件操作对象
    {"getFileIoHandler",	sys_getIoHandler},

    // 获取VM操作对象
    /* {"getVmHandler",	sys_getVmHandler}, */


    // 获取tiemr对象
    {"createTimer",		bsp_createTimer},
    {"deleteTimer",		bsp_deleteTimer},

    // 注册事件回调
    {"registerHandler",	bsp_registerServerHandler},
    // 释放事件回调
    {"releaseHandler",	bsp_relaseServerHandler},
    // 检查事件是否已经注册
    {"checkHandler",	bsp_checkServerHandler},

    // 加载短信
    {"loadMessage",		sys_loadMessage},
    {"getMessageLen",	sys_getMessageLen},

    // 加载天气
    {"loadWeather",		sys_loadWeather},

    //通过号码获取电话本里姓名
    {"getNameByPhoneNum",	sys_getNameByPhoneNum},

    {NULL, NULL}
};



// 模块注册接口，需在lua_module.h中注册，并在解释器初始化时调用
LUALIB_API int luaopen_sys(lua_State *L)
{
    luaL_newlib(L, sys_method);
    return 1;
}


#endif



