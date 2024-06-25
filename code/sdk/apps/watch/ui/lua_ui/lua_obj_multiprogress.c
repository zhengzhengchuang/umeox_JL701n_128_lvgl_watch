#include "ui/ui.h"
#include "ui/ui_progress_multi.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_gui.h"
#include "lua_obj.h"


#if (TCFG_LUA_ENABLE)


#define ui_multiprogress_for_id(id) \
    ({ \
        struct element *elm = ui_core_get_element_by_id(id); \
        elm ? container_of(elm, struct ui_multiprogress, elm): NULL; \
     })


static int obj_setMultiProgressFirstPercent(lua_State *L)
{
    int id = 0;
    int percent = 0;
    int redraw = true;
    struct ui_multiprogress *multiprogress = NULL;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    percent = lua_tointeger(L, -1);

    id = lua_get_self_id(L);
    multiprogress = ui_multiprogress_for_id(id);
    lua_db("%s, id:0x%x, percent:%d\n", __FUNCTION__, id, percent);
    ui_multiprogress_set_persent(multiprogress, percent);

    if (redraw) {
        if (multiprogress->elm.css.invisible) {
            ui_core_show(&multiprogress->elm, false);
        } else {
            ui_core_redraw(&multiprogress->elm);
        }
    }

    return 0;
}

static int obj_setMultiProgressSecondPercent(lua_State *L)
{
    int id = 0;
    int percent = 0;
    int redraw = true;
    struct ui_multiprogress *multiprogress = NULL;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    percent = lua_tointeger(L, -1);

    id = lua_get_self_id(L);
    multiprogress = ui_multiprogress_for_id(id);
    lua_db("%s, id:0x%x, percent:%d\n", __FUNCTION__, id, percent);
    ui_multiprogress_set_second_persent(multiprogress, percent);

    if (redraw) {
        /* if (multiprogress->elm.css.invisible) { */
        /* ui_core_show(&multiprogress->elm, false); */
        /* } else { */
        ui_core_redraw(&multiprogress->elm);
        /* } */
    }

    return 0;
}

static int obj_setMultiProgressThirdPercent(lua_State *L)
{
    int id = 0;
    int percent = 0;
    int redraw = true;
    struct ui_multiprogress *multiprogress = NULL;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    percent = lua_tointeger(L, -1);

    id = lua_get_self_id(L);
    multiprogress = ui_multiprogress_for_id(id);
    lua_db("%s, id:0x%x, percent:%d\n", __FUNCTION__, id, percent);
    ui_multiprogress_set_third_persent(multiprogress, percent);

    if (redraw) {
        /* if (multiprogress->elm.css.invisible) { */
        /* ui_core_show(&multiprogress->elm, false); */
        /* } else { */
        ui_core_redraw(&multiprogress->elm);
        /* } */
    }

    return 0;
}


static int obj_getMultiProgressFirstPercent(lua_State *L)
{
    int id = 0;
    int percent = 0;
    struct ui_multiprogress *multiprogress = NULL;

    id = lua_get_self_id(L);
    multiprogress = ui_multiprogress_for_id(id);
    percent = multiprogress->percent[0];
    lua_db("%s, id:0x%x, percent:%d\n", __FUNCTION__, id, percent);
    lua_pushinteger(L, percent);

    return 1;
}

static int obj_getMultiProgressSecondPercent(lua_State *L)
{
    int id = 0;
    int percent = 0;
    struct ui_multiprogress *multiprogress = NULL;

    id = lua_get_self_id(L);
    multiprogress = ui_multiprogress_for_id(id);
    percent = multiprogress->percent[1];
    lua_db("%s, id:0x%x, percent:%d\n", __FUNCTION__, id, percent);
    lua_pushinteger(L, percent);

    return 1;
}

static int obj_getMultiProgressThirdPercent(lua_State *L)
{
    int id = 0;
    int percent = 0;
    struct ui_multiprogress *multiprogress = NULL;

    id = lua_get_self_id(L);
    multiprogress = ui_multiprogress_for_id(id);
    percent = multiprogress->percent[2];
    lua_db("%s, id:0x%x, percent:%d\n", __FUNCTION__, id, percent);
    lua_pushinteger(L, percent);

    return 1;
}




const luaL_Reg obj_MultiProgressMethod[] = {
    {"setFirstPercent",		obj_setMultiProgressFirstPercent},
    {"setSecondPercent",	obj_setMultiProgressSecondPercent},
    {"setThirdPercent",		obj_setMultiProgressThirdPercent},

    {"getFirstPercent",		obj_getMultiProgressFirstPercent},
    {"getSecondPercent",	obj_getMultiProgressSecondPercent},
    {"getThirdPercent",		obj_getMultiProgressThirdPercent},

    {NULL, NULL}
};

#endif
