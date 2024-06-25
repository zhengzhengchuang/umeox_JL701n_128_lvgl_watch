#include "ui/ui.h"
#include "jiffies.h"
#include "ioctl_cmds.h"
#include "device/device.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_gui.h"
#include "lua_obj.h"


#if (TCFG_LUA_ENABLE)

#define ui_time_for_id(id) \
	({ \
		struct element *elm = ui_core_get_element_by_id(id); \
	 	elm ? (struct ui_time *)elm: NULL; \
	 })


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



// 获取系统时间
static void get_sys_time(struct utime *time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        get_elapse_time(time);
        /* memset(time, 0, sizeof(*time)); */
        return;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
    dev_close(fd);
}

// 设置系统时间
static void set_sys_time(struct utime *time)
{
    printf("set time: %d-%d-%d %d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec);

#if 0
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        log_e("open rtc device field!\n");
        return;
    }
    dev_ioctl(fd, IOCTL_SET_SYS_TIME, (u32)time);
    dev_close(fd);
#endif
}


// 设置系统时间
// 参数：table = {year, month, day, hour, minute, second}
// (table)
// (table, redraw)
// (table, redraw, sync)
// 返回：无
static int obj_setDateTime(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int sync = false;	// 默认设置时间不同步到系统时间
    struct utime time;
    struct ui_time *ui_time = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        sync = lua_toboolean(L, -1);
        lua_pop(L, 1);
        n = n - 1;
    }
    if (n == 3) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    lua_getfield(L, -1, "year");
    time.year = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "month");
    time.month = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "day");
    time.day = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "hour");
    time.hour = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "minute");
    time.min = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "second");
    time.sec = lua_tointeger(L, -1);
    lua_pop(L, 1);

    // 设置系统时间
    if (sync) {
        set_sys_time(&time);
    }
    lua_db("%s: %d-%d-%d %d:%d:%d\n", __FUNCTION__, time.year, time.month, time.day, time.hour, time.min, time.sec);

    // ui更新时间
    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    ui_time_update(ui_time, &time);
    if (redraw) {
        ui_core_redraw(&ui_time->text.elm);
    }
    /* ui_time_update_by_id(id, &time); */

    return 0;
}


// 设置系统日期
// 参数：table = {year, month, day}
// (table)
// (table, redraw)
// (table, redraw, sync)
// 返回：无
static int obj_setDate(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int sync = false;
    struct utime time = {0};
    struct ui_time *ui_time = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        sync = lua_toboolean(L, -1);
        lua_pop(L, 1);
        n = n - 1;
    }
    if (n == 3) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    get_sys_time(&time);

    lua_getfield(L, -1, "year");
    time.year = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "month");
    time.month = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "day");
    time.day = lua_tointeger(L, -1);
    lua_pop(L, 1);

    // 设置系统时间
    if (sync) {
        set_sys_time(&time);
    }
    lua_db("%s: %d-%d-%d\n", __FUNCTION__, time.year, time.month, time.day);

    // ui更新时间
    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    ui_time_update(ui_time, &time);
    if (redraw) {
        ui_core_redraw(&ui_time->text.elm);
    }
    /* ui_time_update_by_id(id, &time); */

    return 0;
}

// 设置系统时间
// 参数：table = {hour, minute, second}
// (table)
// (table, redraw)
// (table, redraw, sync)
// 返回：无
static int obj_setTime(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int sync = false;
    struct utime time = {0};
    struct ui_time *ui_time = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        sync = lua_toboolean(L, -1);
        lua_pop(L, 1);
        n = n - 1;
    }
    if (n == 3) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    get_sys_time(&time);

    lua_getfield(L, -1, "hour");
    time.hour = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "minute");
    time.min = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "second");
    time.sec = lua_tointeger(L, -1);
    lua_pop(L, 1);

    // 设置系统时间
    if (sync) {
        set_sys_time(&time);
    }
    lua_db("%s: %d:%d:%d\n", __FUNCTION__, time.hour, time.min, time.sec);

    // ui更新时间
    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    ui_time_update(ui_time, &time);
    if (redraw) {
        ui_core_redraw(&ui_time->text.elm);
    }
    /* ui_time_update_by_id(id, &time); */

    return 0;
}

// 设置系统年份
// 参数：
// (year)
// (year, redraw)
// (year, redraw, sync)
// 返回：无
static int obj_setYear(lua_State *L)
{
    int id = 0;
    int num = 0;
    int redraw = true;
    int sync = false;
    struct utime time = {0};
    struct ui_time *ui_time = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        sync = lua_toboolean(L, -1);
        lua_pop(L, 1);
        n = n - 1;
    }
    if (n == 3) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    // 获取传入的年份
    num = lua_tointeger(L, -1);

    get_sys_time(&time);
    time.year = num;
    if (sync) {
        set_sys_time(&time);
    }
    lua_db("%s (%d)\n", __FUNCTION__, num);

    // 更新时间控件
    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    ui_time_update(ui_time, &time);
    if (redraw) {
        ui_core_redraw(&ui_time->text.elm);
    }
    /* ui_time_update_by_id(id, &time); */

    return 0;
}


// 设置月份
// 参数：
// (month)
// (month, redraw)
// (month, redraw, sync)
// 返回：无
static int obj_setMonth(lua_State *L)
{
    int id = 0;
    int num = 0;
    int redraw = true;
    int sync = false;
    struct utime time = {0};
    struct ui_time *ui_time = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        sync = lua_toboolean(L, -1);
        lua_pop(L, 1);
        n = n - 1;
    }
    if (n == 3) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    num = lua_tointeger(L, -1);
    get_sys_time(&time);
    time.month = num;
    if (sync) {
        set_sys_time(&time);
    }
    lua_db("%s (%d)\n", __FUNCTION__, num);

    // 更新时间控件
    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    ui_time_update(ui_time, &time);
    if (redraw) {
        ui_core_redraw(&ui_time->text.elm);
    }
    /* ui_time_update_by_id(id, &time); */

    return 0;
}

// 设置日期
// 参数：
// (day)
// (day, redraw)
// (day, redraw, sync)
// 返回：无
static int obj_setDay(lua_State *L)
{
    int id = 0;
    int num = 0;
    int redraw = true;
    int sync = false;
    struct utime time = {0};
    struct ui_time *ui_time = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        sync = lua_toboolean(L, -1);
        lua_pop(L, 1);
        n = n - 1;
    }
    if (n == 3) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    num = lua_tointeger(L, -1);
    get_sys_time(&time);
    time.day = num;
    if (sync) {
        set_sys_time(&time);
    }
    lua_db("%s (%d)\n", __FUNCTION__, num);

    // 更新时间控件
    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    ui_time_update(ui_time, &time);
    if (redraw) {
        ui_core_redraw(&ui_time->text.elm);
    }
    /* ui_time_update_by_id(id, &time); */

    return 0;
}


// 设置小时
// 参数：
// (hour)
// (hour, redraw)
// (hour, redraw, sync)
// 返回：无
static int obj_setHour(lua_State *L)
{
    int id = 0;
    int num = 0;
    int redraw = true;
    int sync = false;
    struct utime time = {0};
    struct ui_time *ui_time = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        sync = lua_toboolean(L, -1);
        lua_pop(L, 1);
        n = n - 1;
    }
    if (n == 3) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    num = lua_tointeger(L, -1);
    get_sys_time(&time);
    time.hour = num;
    if (sync) {
        set_sys_time(&time);
    }
    lua_db("%s (%d)\n", __FUNCTION__, num);

    // 更新时间控件
    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    ui_time_update(ui_time, &time);
    if (redraw) {
        ui_core_redraw(&ui_time->text.elm);
    }
    /* ui_time_update_by_id(id, &time); */

    return 0;
}


// 设置分钟
// 参数：
// (minute)
// (minute, redraw)
// (minute, redraw, sync)
// 返回：无
static int obj_setMinute(lua_State *L)
{
    int id = 0;
    int num = 0;
    int redraw = true;
    int sync = false;
    struct utime time = {0};
    struct ui_time *ui_time = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        sync = lua_toboolean(L, -1);
        lua_pop(L, 1);
        n = n - 1;
    }
    if (n == 3) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    num = lua_tointeger(L, -1);
    get_sys_time(&time);
    time.min = num;
    if (sync) {
        set_sys_time(&time);
    }
    lua_db("%s (%d)\n", __FUNCTION__, num);

    // 更新时间控件
    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    ui_time_update(ui_time, &time);
    if (redraw) {
        ui_core_redraw(&ui_time->text.elm);
    }
    /* ui_time_update_by_id(id, &time); */

    return 0;
}


// 设置秒
// 参数：
// (second)
// (second, redraw)
// (second, redraw, sync)
// 返回：无
static int obj_setSecond(lua_State *L)
{
    int id = 0;
    int num = 0;
    int redraw = true;
    int sync = false;
    struct utime time = {0};
    struct ui_time *ui_time = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        sync = lua_toboolean(L, -1);
        lua_pop(L, 1);
        n = n - 1;
    }
    if (n == 3) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    num = lua_tointeger(L, -1);
    get_sys_time(&time);
    time.sec = num;
    if (sync) {
        set_sys_time(&time);
    }
    lua_db("%s (%d)\n", __FUNCTION__, num);

    // 更新时间控件
    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    ui_time_update(ui_time, &time);
    if (redraw) {
        ui_core_redraw(&ui_time->text.elm);
    }
    /* ui_time_update_by_id(id, &time); */

    return 0;
}


// 获取系统时间
// 注意：这里直接获取ui控件上的时间，后续根据需要是否修改为获取系统时间
// 参数：无
// 返回：table
// table.year, table.month, table.day, table.hour, table.minute, table.second
static int obj_getDateTime(lua_State *L)
{
    int id = 0;
    struct ui_time *t = NULL;

    id = lua_get_self_id(L);
    t = ui_time_for_id(id);

    lua_newtable(L);
    lua_db("%s, %d-%d-%d %d:%d:%d\n", __FUNCTION__, t->year, t->month, t->day, t->hour, t->min, t->sec);

    lua_pushstring(L, "year");
    lua_pushinteger(L, t->year);
    lua_settable(L, 2);

    lua_pushstring(L, "month");
    lua_pushinteger(L, t->month);
    lua_settable(L, 2);

    lua_pushstring(L, "day");
    lua_pushinteger(L, t->day);
    lua_settable(L, 2);

    lua_pushstring(L, "hour");
    lua_pushinteger(L, t->hour);
    lua_settable(L, 2);

    lua_pushstring(L, "minute");
    lua_pushinteger(L, t->min);
    lua_settable(L, 2);

    lua_pushstring(L, "second");
    lua_pushinteger(L, t->sec);
    lua_settable(L, 2);

    return 1;

#if 0
    struct sys_time time = {0};
    char system_time[20] = {'\0'};

    get_sys_time(&time);

    if (time.year == 0 && time.hour == 0) {
        return 0;
    } else {
        sprintf(system_time, "%d-%d-%d %d:%d:%d", \
                time.year, time.month, time.day, \
                time.hour, time.min, time.sec);
        lua_pushstring(L, system_time);
        return 1;
    }
#endif
}

// 获取系统日期
// 参数：无
// 返回：table
// table.year, table.month, table.day
static int obj_getDate(lua_State *L)
{
    int id = 0;
    struct ui_time *t = NULL;

    id = lua_get_self_id(L);
    t = ui_time_for_id(id);

    lua_newtable(L);

    lua_pushstring(L, "year");
    lua_pushinteger(L, t->year);
    lua_settable(L, 2);

    lua_pushstring(L, "month");
    lua_pushinteger(L, t->month);
    lua_settable(L, 2);

    lua_pushstring(L, "day");
    lua_pushinteger(L, t->day);
    lua_settable(L, 2);

    return 1;
}

// 获取系统时间
// 参数：无
// 返回：table
// table.hour, table.minute, table.second
static int obj_getTime(lua_State *L)
{
    int id = 0;
    struct ui_time *t = NULL;

    id = lua_get_self_id(L);
    t = ui_time_for_id(id);

    lua_newtable(L);

    lua_pushstring(L, "hour");
    lua_pushinteger(L, t->hour);
    lua_settable(L, 2);

    lua_pushstring(L, "minute");
    lua_pushinteger(L, t->min);
    lua_settable(L, 2);

    lua_pushstring(L, "second");
    lua_pushinteger(L, t->sec);
    lua_settable(L, 2);

    return 1;
}

// 获取系统年份
// 参数：无
// 返回：year
static int obj_getYear(lua_State *L)
{
    int id = 0;
    int num = 0;
    struct ui_time *t = NULL;

    id = lua_get_self_id(L);
    t = ui_time_for_id(id);

    num = t->year;
    lua_db("%s, %d\n", __FUNCTION__, num);

    lua_pushinteger(L, num);

    return 1;
}

// 获取系统月份
// 参数：无
// 返回：month
static int obj_getMonth(lua_State *L)
{
    int id = 0;
    int num = 0;
    struct ui_time *t = NULL;

    id = lua_get_self_id(L);
    t = ui_time_for_id(id);

    num = t->month;
    lua_db("%s, %d\n", __FUNCTION__, num);

    lua_pushinteger(L, num);

    return 1;
}

// 获取系统天数
// 参数：无
// 返回：day
static int obj_getDay(lua_State *L)
{
    int id = 0;
    int num = 0;
    struct ui_time *t = NULL;

    id = lua_get_self_id(L);
    t = ui_time_for_id(id);

    num = t->day;
    lua_db("%s, %d\n", __FUNCTION__, num);

    lua_pushinteger(L, num);

    return 1;
}

// 获取小时
// 参数：无
// 返回：hour
static int obj_getHour(lua_State *L)
{
    int id = 0;
    int num = 0;
    struct ui_time *t = NULL;

    id = lua_get_self_id(L);
    t = ui_time_for_id(id);

    num = t->hour;
    lua_db("%s, %d\n", __FUNCTION__, num);

    lua_pushinteger(L, num);

    return 1;
}

// 获取分钟
// 参数：无
// 返回：minute
static int obj_getMinute(lua_State *L)
{
    int id = 0;
    int num = 0;
    struct ui_time *t = NULL;

    id = lua_get_self_id(L);
    t = ui_time_for_id(id);

    num = t->min;
    lua_db("%s, %d\n", __FUNCTION__, num);

    lua_pushinteger(L, num);

    return 1;
}

// 获取系统秒数
// 参数：无
// 返回：second
static int obj_getSecond(lua_State *L)
{
    int id = 0;
    int num = 0;
    struct ui_time *t = NULL;

    id = lua_get_self_id(L);
    t = ui_time_for_id(id);

    num = t->sec;
    lua_db("%s, %d\n", __FUNCTION__, num);

    lua_pushinteger(L, num);

    return 1;
}







// 设置UI日期
// (year, month, day, redraw)
// 对不需要设置的参数传入nil即自动保留原本UI控件中的值
static int obj_setUIData(lua_State *L)
{
    int id = 0;
    int redraw = true;
    struct utime time = {0};
    struct ui_time *ui_time = NULL;

    int n = lua_gettop(L);
    if (5 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    time.hour = ui_time->hour;
    time.min = ui_time->min;
    time.sec = ui_time->sec;

    if (lua_isnil(L, -1)) {
        time.day = ui_time->day;
    } else {
        time.day = lua_tointeger(L, -1);
    }

    if (lua_isnil(L, -2)) {
        time.month = ui_time->month;
    } else {
        time.month = lua_tointeger(L, -2);
    }

    if (lua_isnil(L, -3)) {
        time.year = ui_time->year;
    } else {
        time.year = lua_tointeger(L, -3);
    }
    lua_db("%s, year:%d, month:%d, day:%d, redraw:%d\n", \
           __FUNCTION__, time.year, time.month, time.day, redraw);

    // ui更新时间
    ui_time_update(ui_time, &time);
    if (redraw) {
        ui_core_redraw(&ui_time->text.elm);
    }

    return 0;
}


// 设置UI时间
// (hour, minute, second, redraw)
// 对不需要设置的参数传入nil即保留原UI的值
static int obj_setUITime(lua_State *L)
{
    int id = 0;
    int redraw = true;
    struct utime time = {0};
    struct ui_time *ui_time = NULL;

    int n = lua_gettop(L);
    if (5 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    time.year = ui_time->year;
    time.month = ui_time->month;
    time.day = ui_time->day;

    if (lua_isnil(L, -1)) {
        time.sec = ui_time->sec;
    } else {
        time.sec = lua_tointeger(L, -1);
    }

    if (lua_isnil(L, -2)) {
        time.min = ui_time->min;
    } else {
        time.min = lua_tointeger(L, -2);
    }

    if (lua_isnil(L, -3)) {
        time.hour = ui_time->hour;
    } else {
        time.hour = lua_tointeger(L, -3);
    }
    lua_db("%s, hour:%d, minute:%d, second:%d, redraw:%d\n", \
           __FUNCTION__, time.hour, time.min, time.sec, redraw);

    ui_time_update(ui_time, &time);
    if (redraw) {
        ui_core_redraw(&ui_time->text.elm);
    }

    return 0;
}



// 获取日期
// 返回值：year, month, day
static int obj_getUIData(lua_State *L)
{
    int id = 0;
    struct ui_time *ui_time = NULL;

    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    lua_db("%s, year:%d, month:%d, day:%d\n", \
           __FUNCTION__, ui_time->year, ui_time->month, ui_time->day);

    lua_pushinteger(L, ui_time->year);
    lua_pushinteger(L, ui_time->month);
    lua_pushinteger(L, ui_time->day);

    return 3;
}


// 获取时间
// 返回值：hour, minute, second
static int obj_getUITime(lua_State *L)
{
    int id = 0;
    struct ui_time *ui_time = NULL;

    id = lua_get_self_id(L);
    ui_time = ui_time_for_id(id);
    lua_db("%s, hour:%d, minute:%d, second:%d\n", \
           __FUNCTION__, ui_time->hour, ui_time->min, ui_time->sec);

    lua_pushinteger(L, ui_time->hour);
    lua_pushinteger(L, ui_time->min);
    lua_pushinteger(L, ui_time->sec);

    return 3;
}



// Time
const luaL_Reg obj_TimeMethod[] = {
    /* *
     * 时间控件需要方法：
     * 		设置时间（可选择是否同时设置系统时间）
     * 		setUIData(年，月，日，redraw)
     * 		setUITime(时，分，秒，redraw)
     * 		获取时间（仅获取时间控件的时间）
     * 		getUIData --> 年，月，日
     * 		getUITime --> 时，分，秒
     * */
    {"setUIData", obj_setUIData},
    {"setUITime", obj_setUITime},
    {"getUIData", obj_getUIData},
    {"getUITime", obj_getUITime},
    /* 后续只保留上面四个接口，下面接口全部取消 */

    {"setSystemDateTime", obj_setDateTime},	// obj:setTime(table)
    {"setSystemDate", obj_setDate},
    {"setSystemTime", obj_setTime},
    {"setYear",		obj_setYear},
    {"setMonth",	obj_setMonth},
    {"setDay",		obj_setDay},
    {"setHour",		obj_setHour},
    {"setMinute",	obj_setMinute},
    {"setSecond",	obj_setSecond},

    {"getSystemDateTime", obj_getDateTime},
    {"getSystemDate", obj_getDate},
    {"getSystemTime", obj_getTime},
    {"getYear",		obj_getYear},
    {"getMonth",	obj_getMonth},
    {"getDay",		obj_getDay},
    {"getHour",		obj_getHour},
    {"getMinute",	obj_getMinute},
    {"getSecond",	obj_getSecond},

    {NULL, NULL}
};

#endif /* #if (TCFG_LUA_ENABLE) */

