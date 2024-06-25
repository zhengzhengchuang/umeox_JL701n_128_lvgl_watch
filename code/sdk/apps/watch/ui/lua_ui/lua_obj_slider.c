#include "ui/ui.h"
#include "ui/ui_slider.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_obj.h"
#include "lua_gui.h"

#if (TCFG_LUA_ENABLE)


#define ui_slider_for_id(id) \
	({ \
		struct element *elm = ui_core_get_element_by_id(id); \
	 	elm ? container_of(elm, struct ui_slider, elm): NULL; \
	 })

#define ui_vslider_for_id(id) \
    ({ \
        struct element *elm = ui_core_get_element_by_id(id); \
        elm ? container_of(elm, struct ui_vslider, elm): NULL; \
     })



// 设置进度条的最小值，最大值和步长
// 参数：
// (min, max, step)
// (min, max, step, redraw)
// 返回：无
static int obj_setSliderAttribute(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int min, max, step;
    struct ui_slider *slider = NULL;
    struct ui_vslider *vslider = NULL;

    int n = lua_gettop(L);
    if (5 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    step = lua_tointeger(L, -1);
    max = lua_tointeger(L, -2);
    min = lua_tointeger(L, -3);

    id = lua_get_self_id(L);
    if (ui_id2type(id) == CTRL_TYPE_SLIDER) {
        slider = ui_slider_for_id(id);
        slider->step = step;
        slider->min_value = min;
        slider->max_value = max;
        lua_db("hslider set attribute: %d, %d, %d, redraw:%d\n", min, max, step, redraw);

        if (redraw) {
            if (slider->elm.css.invisible) {
                ui_core_show(&slider->elm, false);
            } else {
                ui_core_redraw(&slider->elm);
            }
        }
    } else {
        vslider = ui_vslider_for_id(id);
        vslider->step = step;
        vslider->min_value = min;
        vslider->max_value = max;
        lua_db("vslider set attribute: %d, %d, %d, redraw:%d\n", min, max, step, redraw);

        if (redraw) {
            if (vslider->elm.css.invisible) {
                ui_core_show(&vslider->elm, false);
            } else {
                ui_core_redraw(&vslider->elm);
            }
        }
    }

    return 0;
}


// 获取进度条的最小值，最大值和步长
// 参数：无
// 返回: min, max, step
static int obj_getSliderAttribute(lua_State *L)
{
    int id = 0;
    int min, max, step;
    struct ui_slider *slider = NULL;
    struct ui_vslider *vslider = NULL;

    id = lua_get_self_id(L);

    if (ui_id2type(id) == CTRL_TYPE_SLIDER) {
        slider = ui_slider_for_id(id);
        step = slider->step;
        min = slider->min_value;
        max = slider->max_value;
        lua_db("hslider get attribute: min = %d, max = %d, step = %d\n", min, max, step);
    } else {
        vslider = ui_vslider_for_id(id);
        step = vslider->step;
        min = vslider->min_value;
        max = vslider->max_value;
        lua_db("vslider get attribute: min = %d, max = %d, step = %d\n", min, max, step);
    }

    lua_pushinteger(L, min);
    lua_pushinteger(L, max);
    lua_pushinteger(L, step);

    return 3;
}


// 设置进度条的百分比
// 参数：
// (percent)
// (percent, redraw)
// 返回：无
static int obj_setSliderPercent(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int percent = 0;
    struct ui_slider *slider = NULL;
    struct ui_vslider *vslider = NULL;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    percent = lua_tointeger(L, -1);
    id = lua_get_self_id(L);
    if (ui_id2type(id) == CTRL_TYPE_SLIDER) {
        slider = ui_slider_for_id(id);
        ui_slider_set_persent(slider, percent);
        lua_db("hslider set percent:%d, redraw:%d\n", percent, redraw);

        if (redraw) {
            if (slider->elm.css.invisible) {
                ui_core_show(&slider->elm, false);
            } else {
                ui_core_redraw(&slider->elm);
            }
        }
    } else {
        vslider = ui_vslider_for_id(id);
        ui_vslider_set_persent(vslider, percent);
        lua_db("vslider set percent:%d, redraw:%d\n", percent, redraw);

        if (redraw) {
            if (vslider->elm.css.invisible) {
                ui_core_show(&vslider->elm, false);
            } else {
                ui_core_redraw(&vslider->elm);
            }
        }
    }
    return 0;
}


// 获取进度条的百分比
// 参数：无
// 返回：percent
static int obj_getSliderPercent(lua_State *L)
{
    int id = 0;
    int percent = 0;
    struct ui_slider *slider = NULL;
    struct ui_vslider *vslider = NULL;

    id = lua_get_self_id(L);
    if (ui_id2type(id) == CTRL_TYPE_SLIDER) {
        slider = ui_slider_for_id(id);
        percent = slider_get_percent(slider);
        lua_db("hslider get percent:%d\n", percent);
    } else {
        vslider = ui_vslider_for_id(id);
        percent = vslider_get_percent(vslider);
        lua_db("vslider get percent:%d\n", percent);
    }
    lua_pushinteger(L, percent);

    return 1;
}


// 设置进度条滑动功能
// 参数：
// true 使能滑动
// false 不使能滑动
// 返回：无
static int obj_setSliderMove(lua_State *L)
{
    int id = 0;
    int move = false;
    struct ui_slider *slider = NULL;
    struct ui_vslider *vslider = NULL;

    move = lua_toboolean(L, -1);
    id = lua_get_self_id(L);
    if (ui_id2type(id) == CTRL_TYPE_SLIDER) {
        slider = ui_slider_for_id(id);
        slider->move = move;
    } else {
        vslider = ui_vslider_for_id(id);
        vslider->move = move;
    }
    printf("set slider move: %s\n", move ? "true" : "false");

    return 0;
}


// 获取进度条滑动功能状态
// 参数：无
// 返回：
// true 能滑动
// false 不能滑动
static int obj_getSliderMove(lua_State *L)
{
    int id = 0;
    int move = false;
    struct ui_slider *slider = NULL;
    struct ui_vslider *vslider = NULL;

    id = lua_get_self_id(L);
    if (ui_id2type(id) == CTRL_TYPE_SLIDER) {
        slider = ui_slider_for_id(id);
        move = slider->move;
    } else {
        vslider = ui_vslider_for_id(id);
        move = vslider->move;
    }
    printf("get slider move: %s\n", move ? "true" : "false");
    lua_pushboolean(L, move);

    return 1;
}



const luaL_Reg obj_SliderMethod[] = {
    // 属性设置：最小值、最大值、步长
    /* {"setSliderAttribute",	obj_setSliderAttribute}, */
    /* {"getSliderAttribute",	obj_getSliderAttribute}, */

    // 百分比设置、获取
    {"setSliderPercent",	obj_setSliderPercent},
    {"getSliderPercent",	obj_getSliderPercent},

    // 滑动设置、获取
    {"setSliderMove",		obj_setSliderMove},
    {"getSliderMove",		obj_getSliderMove},

    {NULL, NULL}
};


#endif
