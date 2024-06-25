#include "ui/ui.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_obj.h"
#include "lua_gui.h"

#if (TCFG_LUA_ENABLE)


// 显示图片列表中指定index的图片
// 参数：
// (index)
// (index, redraw)
// 返回：无
static int obj_showImageByIndex(lua_State *L)
{
    int id = 0;
    int index = 0;
    int redraw = true;
    struct ui_pic *pic = NULL;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    index = luaL_checkinteger(L, -1);
    id = lua_get_self_id(L);
    pic = ui_core_get_element_by_id(id);
    ui_pic_set_image_index(pic, index);
    lua_db("%s, id:0x%x, index:%d, redraw:%d\n", __FUNCTION__, id, index, redraw);

    if (redraw) {
        if (pic->elm.css.invisible) {
            ui_core_show(&pic->elm, false);
        } else {
            ui_core_redraw(&pic->elm);
        }
    }
    /* ui_pic_show_image_by_id(id, index); */

    return 0;
}


// 获取图片控件显示的第几张图片
// 参数：无
// 返回：index
static int obj_getImageIndex(lua_State *L)
{
    int id = 0;
    int index = 0;
    struct ui_pic *pic = NULL;

    id = lua_get_self_id(L);
    pic = (struct ui_pic *)ui_core_get_element_by_id(id);
    index = pic->index;
    lua_db("%s, id:0x%x, index:%d\n", __FUNCTION__, id, index);

    lua_pushinteger(L, index);

    return 1;
}


// 获取普通图片列表中图片的数量
// 参数：无
// 返回：number
static int obj_getNormalImageNumber(lua_State *L)
{
    int id = 0;
    int number = 0;

    id = lua_get_self_id(L);

    number = ui_pic_get_normal_image_number_by_id(id);
    lua_db("%s, id:0x%x, normal image number:%d\n", __FUNCTION__, id, number);

    lua_pushinteger(L, number);

    return 1;
}


// 获取高亮图片列表的图片数量
// 参数：无
// 返回：number
static int obj_getHighLightImageNumber(lua_State *L)
{
    int id = 0;
    int number = 0;

    id = lua_get_self_id(L);

    number = ui_pic_get_highlgiht_image_number_by_id(id);
    lua_db("%s, id:0x%x, highlight image number:%d\n", __FUNCTION__, id, number);

    lua_pushinteger(L, number);

    return 1;
}


struct draw_context *ui_core_get_draw_dc();
static int obj_setDrawImage(lua_State *L)
{
    VIEWFILE *file;
    struct draw_context *dc = ui_core_get_draw_dc();
    int id = lua_get_self_id(L);

    struct ui_pic *pic = (struct ui_pic *)ui_core_get_element_by_id(id);
    pic->elm.css.background_image = 1;

    lua_getfield(L, -1, "view");
    file = lua_tointeger(L, -1);
    lua_pop(L, 1);
    printf("file : 0x%x\n", (u32)file);

    dc->preview.file = file->file;
    dc->preview.file_info = &file->info;

    dc->preview.id = 1;
    dc->preview.page = 0;

    return 0;
}


// pic图片控件方法
const luaL_Reg obj_ImageMethod[] = {
    // obj:showImageByIndex(int index)
    {"showImageByIndex",	obj_showImageByIndex},

    // index = obj:getImageIndex()
    {"getImageIndex",		obj_getImageIndex},

    // num = obj:getNormalNumber()
    {"getNormalNumber",		obj_getNormalImageNumber},

    // hnum = obj:getHighLightNumber()
    {"getHighLightNumber",	obj_getHighLightImageNumber},

    {"setDrawImage",	obj_setDrawImage},

    {NULL, NULL}
};

#endif /* #if (TCFG_LUA_ENABLE) */

