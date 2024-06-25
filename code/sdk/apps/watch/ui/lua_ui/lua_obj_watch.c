#include "ui/ui.h"
#include "jiffies.h"
#include "ioctl_cmds.h"
#include "ui/res_config.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_gui.h"
#include "lua_obj.h"


#if (TCFG_LUA_ENABLE)


// 设置表盘时间
// (hour, min, sec, redraw)
static int obj_setWatchTime(lua_State *L)
{
    int id = 0;
    int redraw = true;
    struct ui_watch *watch = NULL;

    int n = lua_gettop(L);
    if (5 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    id = lua_get_self_id(L);
    watch = ui_core_get_element_by_id(id);
    if (lua_isnumber(L, -1)) {
        watch->sec = lua_tointeger(L, -1);
    }
    if (lua_isnumber(L, -2)) {
        watch->min = lua_tointeger(L, -2);
    }
    if (lua_isnumber(L, -3)) {
        watch->hour = lua_tointeger(L, -3);
    }
    lua_db("%s, hour:%d, min:%d, sec:%d, redraw:%d\n", \
           __FUNCTION__, watch->hour, watch->min, watch->sec, redraw);

    if (redraw) {
        ui_watch_update(watch, redraw);
    } else {
        ui_watch_set_time(watch, watch->hour, watch->min, watch->sec);
    }

    return 0;
}


// 获取表盘时间
// hour, min, sec
static int obj_getWatchTime(lua_State *L)
{
    int id = 0;
    struct ui_watch *watch = NULL;

    id = lua_get_self_id(L);
    watch = ui_core_get_element_by_id(id);
    lua_db("%s, hour:%d, minute:%d, second:%d\n", \
           __FUNCTION__, watch->hour, watch->min, watch->sec);

    lua_pushinteger(L, watch->hour);
    lua_pushinteger(L, watch->min);
    lua_pushinteger(L, watch->sec);

    return 3;
}







static int get_elapse_time(struct sys_time *time)
{
    static struct sys_time cur_time = {.hour = 3, .min = 30, .sec = 23};
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

    return 1;
}

// (wacth_table)
// (watch_table, redraw)
static int obj_setWatch(lua_State *L)
{
    int id;
    int redraw = true;
    struct sys_time time;
    struct ui_watch *watch = NULL;

    lua_db("watch : obj_setWatch");

    int n = lua_gettop(L);

    if (n == 3) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    id = lua_get_self_id(L);
    lua_db("id : 0x%x\n", id);

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


    printf("obj_setWatch : %04d-%02d-%02d %02d:%02d:%02d\n", time.year, time.month, time.day, time.hour, time.min, time.sec);

    watch = ui_core_get_element_by_id(id);
    if (!watch) {
        return -EINVAL;
    }

    if (watch->hour < 0 || watch->hour > 24) {
        return -EINVAL;
    }
    if (watch->min < 0 || watch->min > 60) {
        return -EINVAL;
    }
    if (watch->sec < 0 || watch->sec > 60) {
        return -EINVAL;
    }

    watch->hour = time.hour;
    watch->min = time.min;
    watch->sec = time.sec;

    if (redraw) {
        ui_watch_update(watch, redraw);
    } else {
        ui_watch_set_time(ui_core_get_element_by_id(id), time.hour, time.min, time.sec);
    }

    /* ui_watch_set_time_by_id(id, time.hour, time.min, time.sec); */

    return 1;
}

static int obj_setWatchInitTime(lua_State *L)
{
    struct sys_time time;
    int id;

    lua_db("watch : obj_setWatch");


    id = lua_get_self_id(L);
    lua_db("id : 0x%x\n", id);

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


    printf("%04d-%02d-%02d %02d:%02d:%02d\n", time.year, time.month, time.day, time.hour, time.min, time.sec);

    ui_watch_set_time(ui_core_get_element_by_id(id), time.hour, time.min, time.sec);

    return 0;
}

static int obj_getTime(lua_State *L)
{
    int id;
    struct sys_time time;

    lua_db("watch : obj_getTime");

    get_elapse_time(&time);
    printf("%04d-%02d-%02d %02d:%02d:%02d\n", time.year, time.month, time.day, time.hour, time.min, time.sec);

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


const luaL_Reg obj_WatchMethod[] = {
    // 设置表盘时间
    {"setWatchTime",        obj_setWatchTime},
    // 获取表盘时间
    {"getWatchTime",        obj_getWatchTime},


    /* {"setWatchInitTime",    obj_setWatchInitTime}, */
    /* {"setWatchTime",        obj_setWatch}, */
    /* {"getWatchTime",        obj_getWatch}, */
    /* {"getSystemTime",       obj_getTime}, */
    {NULL, NULL}
};

#endif /* #if (TCFG_LUA_ENABLE) */
