// 注意：暂时链接不到power_manage.c里面函数，获取电量，判断充电暂时不能用
#include "app_power_manage.h"
#include "app_charge.h"
#include "asm/charge.h"
#include "event.h"

#include "ui/ui_battery.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_gui.h"
#include "lua_obj.h"

#if (TCFG_LUA_ENABLE)


// 获取电池电量百分比
// 参数：无
// 返回：电池电量百分比
static int obj_getBatteryValue(lua_State *L)
{
    int battery_val = 0;
    battery_val = get_vbat_percent();

    if (battery_val > 100) {
        battery_val = 100;
    }
    lua_db(">>>>> battery value: %d\n", battery_val);

    lua_pushinteger(L, battery_val);

    return 1;
}


// 判断是否在充电状态
// 参数：无
// 返回：
// true 正在充电
// false 未充电
static int obj_batteryIsCharging(lua_State *L)
{
    int battery_char = 0;

    battery_char = (get_charge_online_flag() ? 1 : 0);
    lua_db(">>>>> battery is charging:%d, %s\n", battery_char, battery_char ? "true" : "false");
    lua_pushboolean(L, battery_char);

    return 1;
}


// 设置电量图标
// 参数：
// (level, is_charging)
// (level, is_charging, redraw)
// 返回：无
static int obj_setBatteryLevel(lua_State *L)
{
    int id = 0;
    int level = 0;
    int charging = 0;
    int redraw = true;
    struct ui_battery *battery = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    charging = lua_toboolean(L, -1);
    level = lua_tointeger(L, -2);
    lua_db(">>>>> set battery level; charging:%d, level:%d\n", charging, level);

    id = lua_get_self_id(L);
    if (redraw) {
        ui_battery_set_level_by_id(id, level, charging);
    } else {
        battery = ui_core_get_element_by_id(id);
        ui_battery_set_level(battery, level, charging);
    }

    return 0;
}


// 方法列表
const struct luaL_Reg obj_BatteryMethod[] = {
    // 获取电池电量
    // b_val=obj:getbatteryvalue()
    {"getBatteryValue",		obj_getBatteryValue},

    // 获取充电状态
    // status = obj:batteryIsChargine()
    {"batteryIsCharging",	obj_batteryIsCharging},

    // 修改电量控件的图标
    // obj:setBatteryLevel(val, char)
    {"setBatteryLevel",		obj_setBatteryLevel},

    {NULL, NULL}
};


#endif /* #if (TCFG_LUA_ENABLE) */

