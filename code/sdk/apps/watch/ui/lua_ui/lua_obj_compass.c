#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_obj.h"
#include "lua_gui.h"

#include "ui/ui_compass.h"

#if (TCFG_LUA_ENABLE)

// 设置指南针控件角度
// 参数：
// (bk_angle, indicator_angle)
// (bk_angle, indicator_angle, redraw)
// 返回：无
static int obj_setCompassAngle(lua_State *L)
{
    int id = 0;
    int bk_angle = 0;
    int indicator_angle = 0;
    int redraw = true;
    struct ui_compass *compass = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    id = lua_get_self_id(L);

    indicator_angle = lua_tointeger(L, -1);
    bk_angle = lua_tointeger(L, -2);
    /* indicator_angle = luaL_checkinteger(L, -1); */
    /* lua_pop(L, 1); */
    /* bk_angle = luaL_checkinteger(L, -1); */
    /* lua_pop(L, 1); */
    compass = ui_core_get_element_by_id(id);
    if (!compass) {
        return 0;
    }
    lua_db("%s, id:0x%x, bk_angle:%d, indicator_angle:%d, redraw:%d\n", __FUNCTION__, id, bk_angle, indicator_angle, redraw);
    ui_compass_set_angle(compass, bk_angle, indicator_angle);

    if (redraw) {
        ui_core_redraw(&compass->elm);
    }

    return 0;
}


static int obj_getCompassAngle(lua_State *L)
{
    int id = 0;
    int bk_angle = 0;
    int indicator_angle = 0;
    struct ui_compass *compass = NULL;

    id = lua_get_self_id(L);
    compass = ui_core_get_element_by_id(id);

    bk_angle = compass->bk_angle;
    indicator_angle = compass->indicator_angle;
    lua_db("%s, id:0x%x, bk_angle:%d, indicator_angle:%d\n", __FUNCTION__, id, bk_angle, indicator_angle);

    lua_pushinteger(L, bk_angle);
    lua_pushinteger(L, indicator_angle);

    return 2;
}

const luaL_Reg obj_CompassMethod[] = {
    {"setCompassAngle",	obj_setCompassAngle},
    {"getCompassAngle",	obj_getCompassAngle},
    {NULL, NULL}
};

#endif /* #if (TCFG_LUA_ENABLE) */


