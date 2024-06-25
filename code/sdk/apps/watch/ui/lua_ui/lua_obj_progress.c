#include "ui/ui.h"
#include "ui/ui_progress.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_obj.h"
#include "lua_gui.h"


#if (TCFG_LUA_ENABLE)


#define ui_progress_for_id(id) \
    ({ \
        struct element *elm = ui_core_get_element_by_id(id); \
        elm ? container_of(elm, struct ui_progress, elm): NULL; \
     })



static int obj_setProgressPercent(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int percent = 0;
    struct ui_progress *progress = NULL;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    percent = lua_tointeger(L, -1);

    id = lua_get_self_id(L);
    progress = ui_progress_for_id(id);
    lua_db("%s, id:0x%x, percent:%d\n", __FUNCTION__, id, percent);
    ui_progress_set_persent(progress, percent);

    if (redraw) {
        /* if (progress->elm.css.invisible) { */
        /* ui_core_show(&progress->elm, false); */
        /* } else { */
        ui_core_redraw(&progress->elm);
        /* } */
    }

    return 0;
}

static int obj_getProgressPercent(lua_State *L)
{
    int id = 0;
    int percent = 0;
    struct ui_progress *progress = NULL;

    id = lua_get_self_id(L);
    progress = ui_progress_for_id(id);
    percent = progress->percent;
    lua_db("%s, id:0x%x, percent:%d\n", __FUNCTION__, id, percent);
    lua_pushinteger(L, percent);

    return 1;
}

const luaL_Reg obj_ProgressMethod[] = {
    {"setProgressPercent", obj_setProgressPercent},
    {"getProgressPercent", obj_getProgressPercent},

    {NULL, NULL}
};

#endif /* #if (TCFG_LUA_ENABLE) */

