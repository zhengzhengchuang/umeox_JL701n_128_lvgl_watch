
#include "ui/control.h"
#include "ui/ui_core.h"
#include "ui/ui.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_obj.h"
#include "lua_gui.h"

#if (TCFG_LUA_ENABLE)


#define UI_ROTATE 0x00
static void ui_css_get_abs_rect(struct element_css *css, struct rect *rect);


// 显示控件
// 参数：(redraw)
// true 触发redraw
// false 不触发redraw
// 返回：无
static int obj_show(lua_State *L)
{
    int id = 0;
    int type = 0;
    int redraw = true; /* 没传参或传入true要redraw */

    int n = lua_gettop(L);
    if (2 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    id = lua_get_self_id(L);
    type = ui_id2type(id);
    lua_db("%s, id:0x%x, redraw:%d\n", __FUNCTION__, id, redraw);
    if ((2 == n) && \
        (type != CTRL_TYPE_WINDOW) && \
        (type != CTRL_TYPE_LAYER) && \
        (type != CTRL_TYPE_LAYOUT)) {
        struct element *elm = ui_core_get_element_by_id(id);
        elm->css.invisible = 0;
        if (redraw) {
            // 只有传入false才不会进这里
            ui_core_redraw(elm);
        }
    } else {
        ui_show(id);
    }

    return 0;
}


// 隐藏控件
// 参数：redraw
// true 触发redraw
// false 不触发redraw
// 返回：无
static int obj_hide(lua_State *L)
{
    int id = 0;
    int type = 0;
    int redraw = true;

    int n = lua_gettop(L);
    if (2 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    id = lua_get_self_id(L);
    type = ui_id2type(id);
    lua_db("%s, id:0x%x, redraw:%d\n", __FUNCTION__, id, redraw);
    if ((2 == n) && \
        (type != CTRL_TYPE_WINDOW) && \
        (type != CTRL_TYPE_LAYER) && \
        (type != CTRL_TYPE_LAYOUT)) {
        struct element *elm = ui_core_get_element_by_id(id);

        if (ui_core_get_element(elm) < 0) {
            printf("ERROR, ui_core_get_element(elm) < 0\n");
            return 0;//-EINVAL;
        }

        if (elm->css.invisible) {
            ui_core_put_element(elm);
            return 0;
        }
        elm->state = ELM_STA_HIDE;
        elm->css.invisible = 1;
        if (elm->handler && elm->handler->onchange) {
            elm->handler->onchange(elm, ON_CHANGE_HIDE, NULL);
        }
        if ((elm->dc->elm != elm) && redraw) {
            ui_core_redraw(elm);
        }

        ui_core_put_element(elm);

    } else {
        ui_hide(id);
    }

    return 0;
}

// 是否隐藏状态
// 参数：无
// 返回：
// true 隐藏状态
// false 显示状态
static int obj_isHide(lua_State *L)
{
    int id = 0;
    struct element *elm = NULL;
    struct element_css *css = NULL;

    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);
    css = ui_core_get_element_css(elm);

    lua_db("%s, id:0x%x, is_hide:%s\n", __FUNCTION__, id, (css->invisible ? "true" : "false"));
    lua_pushboolean(L, css->invisible);
    return 1;
}

// 高亮控件
// 参数：redraw
// true 触发redraw
// false 不触发redraw
// 返回：无
static int obj_highLight(lua_State *L)
{
    int id = 0;
    int redraw = true; /* 如果没传参或传入true要redraw */
    struct element *elm = NULL;

    int n = lua_gettop(L);
    if (2 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    id = lua_get_self_id(L);
    lua_db("%s, id:0x%x, redraw:%d\n", __FUNCTION__, id, redraw);
    elm = ui_core_get_element_by_id(id);
    ui_core_highlight_element(elm, 1);
    if (redraw) {
        ui_core_redraw(elm);
    }

    return 0;
}


// 不高亮控件
// 参数：redraw
// true 触发redraw
// false 不触发redraw
// 返回：无
static int obj_noHighLight(lua_State *L)
{
    int id = 0;
    int redraw = true; /* 如果没传参或传入true要redraw */
    struct element *elm = NULL;

    int n = lua_gettop(L);
    if (2 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    id = lua_get_self_id(L);
    lua_db("%s, id:0x%x, redraw:%d\n", __FUNCTION__, id, redraw);
    elm = ui_core_get_element_by_id(id);
    ui_core_highlight_element(elm, 0);
    if (redraw) {
        ui_core_redraw(elm);
    }
    /* ui_no_highlight_element_by_id(id); */

    return 0;
}


// 控件是否高亮状态
// 参数：无
// 返回：
// true 高亮
// false 非高亮
static int obj_isHighLight(lua_State *L)
{
    int id = 0;
    struct element *elm = NULL;

    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);

    lua_db("%s, id:0x%x, is_highLight:%s\n", __FUNCTION__, id, (elm->highlight ? "true" : "false"));
    lua_pushboolean(L, elm->highlight);
    return 1;
}


// 重新绘制自己
// 参数：无
// 返回：无
static int obj_redrawSelf(lua_State *L)
{
    int id = 0;
    struct element *elm = NULL;

    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);
    /* ui_core_redraw(elm); */
    ui_core_redraw(elm->parent);

    return 0;
}


void ui_css_get_abs_rect(struct element_css *css, struct rect *rect)
{
    rect->left += css->left * rect->width  / 10000;
    rect->top  += css->top  * rect->height / 10000;
    rect->width  = rect->width  * css->width  / 10000;
    rect->height = rect->height * css->height / 10000;
}


// 获取控件坐标及宽高
void ui_get_coordinate(int id, int *x, int *y, int *w, int *h)
{
    u32 c = 0 ;
    struct rect r = {0};
    struct element *elm;
    struct element_css *css;

    elm = ui_core_get_element_by_id(id);
    css = &elm->css;
    ui_core_get_element_abs_rect(elm->parent, &r);
    ui_css_get_abs_rect(css, &r);

#if (UI_ROTATE == 0x0)
    printf("rect: %d, %d, %d, %d css.invisible=%d\n", r.left, r.top,
           r.width, r.height, css->invisible);
#else
    //旋转
    c = r.left;
    r.left = r.top;
    r.top = c;
    c = r.height;
    r.height = r.width;
    r.width = c;

    printf("rect: %d, %d, %d, %d css.invisible=%d\n", r.left, r.top,
           r.width, r.height, css->invisible);

#endif

    *x = r.left;
    *y = r.top;
    *w = r.width;
    *h = r.height;
}


// 把栈中指定索引的函数添加到self表中
void set_func_to_self_table(lua_State *L, unsigned char index, char *func_name)
{
    luaL_checktype(L, index, LUA_TFUNCTION);
    lua_setfield(L, 1, func_name);
}


// 执行self表中指定名称的函数
void run_func_from_self_table(lua_State *L, char *func_name)
{
    if (lua_getfield(L, 1, func_name)) {
        lua_call(L, 0, 0); //call func
    }
}


static int obj_bindTouchDown(lua_State *L)
{
    lua_db(">>>>> bind Touch Down\n");

    /* luaL_checktype(L, 2, LUA_TFUNCTION); */
    /* lua_setfield(L, 1, "__touchd_func"); */
    set_func_to_self_table(L, 2, "__touchd_func");

    return 0;
}


static int obj_touchDown(lua_State *L)
{
    lua_db(">>>>> Touch Down \n");

    /* if (lua_getfield(L, 1, "__touchd_func")) { */
    /* lua_call(L, 0, 0); //call func */
    /* } */
    run_func_from_self_table(L, "__touchd_func");

    return 0;
}


static int obj_bindTouchUp(lua_State *L)
{
    lua_db(">>>>> bind Touch Up\n");

    /* luaL_checktype(L, 2, LUA_TFUNCTION); */
    /* lua_setfield(L, 1, "__touchu_func"); */
    set_func_to_self_table(L, 2, "__touchu_func");

    return 0;
}


static int obj_touchUp(lua_State *L)
{
    lua_db(">>>>> Touch Up\n");

    /* if (lua_getfield(L, 1, "__touchu_func")) { */
    /* lua_call(L, 0, 0); //call func */
    /* } */
    run_func_from_self_table(L, "__touchu_func");

    return 0;
}

static int obj_bindTouchUMove(lua_State *L)
{
    lua_db(">>>>> bind Touch Up move\n");

    /* luaL_checktype(L, 2, LUA_TFUNCTION); */
    /* lua_setfield(L, 1, "__touchum_func"); */
    set_func_to_self_table(L, 2, "__touchum_func");

    return 0;
}

//
static int obj_touchUMove(lua_State *L)
{
    lua_db(">>>>> Touch up move\n");

    /* if (lua_getfield(L, 1, "__touchum_func")) { */
    /* lua_call(L, 0, 0); //call func */
    /* } */
    run_func_from_self_table(L, "__touchum_func");

    return 0;
}

static int obj_bindTouchDMove(lua_State *L)
{
    lua_db(">>>>> bind Touch D move\n");

    /* luaL_checktype(L, 2, LUA_TFUNCTION); */
    /* lua_setfield(L, 1, "__touchdm_func"); */
    set_func_to_self_table(L, 2, "__touchdm_func");

    return 0;
}

//
static int obj_touchDMove(lua_State *L)
{
    lua_db(">>>>> Touch d move\n");

    /* if (lua_getfield(L, 1, "__touchdm_func")) { */
    /* lua_call(L, 0, 0); //call func */
    /* } */
    run_func_from_self_table(L, "__touchdm_func");

    return 0;
}


static int obj_bindTouchLMove(lua_State *L)
{
    lua_db(">>>>> bind Touch L move\n");

    /* luaL_checktype(L, 2, LUA_TFUNCTION); */
    /* lua_setfield(L, 1, "__touchlm_func"); */
    set_func_to_self_table(L, 2, "__touchlm_func");

    return 0;
}

//
static int obj_touchLMove(lua_State *L)
{
    lua_db(">>>>> Touch l move\n");

    /* if (lua_getfield(L, 1, "__touchlm_func")) { */
    /* lua_call(L, 0, 0); //call func */
    /* } */
    run_func_from_self_table(L, "__touchlm_func");

    return 0;
}


static int obj_bindTouchRMove(lua_State *L)
{
    lua_db(">>>>> bind Touch R move\n");

    /* luaL_checktype(L, 2, LUA_TFUNCTION); */
    /* lua_setfield(L, 1, "__touchrm_func"); */
    set_func_to_self_table(L, 2, "__touchrm_func");

    return 0;
}


//
static int obj_touchRMove(lua_State *L)
{
    lua_db(">>>>> Touch r move\n");

    /* if (lua_getfield(L, 1, "__touchrm_func")) { */
    /* lua_call(L, 0, 0); //call func */
    /* } */
    run_func_from_self_table(L, "__touchrm_func");

    return 0;
}

#if 0
// 设置css属性页，与高亮一样功能
static int obj_setCss(lua_State *L)
{
    int id = 0;
    int index = 0;

    index = luaL_checkinteger(L, 2);
    lua_db(">>>>> set css %d\n", index);
    id = lua_get_self_id(L);

    if (index == 1) {
        ui_highlight_element_by_id(id);
    } else {
        ui_no_highlight_element_by_id(id);
    }

    return 0;
}


// 生成css属性页列表
static void make_css_table(lua_State *L, struct element_css *css, struct rect *r)
{
    lua_newtable(L);//创建一个表格，放在栈顶i
    set_value_to_table(L, "color", css->border.color);
    set_value_to_table(L, "bottom", css->border.bottom);
    set_value_to_table(L, "right", css->border.right);
    set_value_to_table(L, "left", css->border.left);
    set_value_to_table(L, "top", css->border.top);
    /* stackDump(L); */

    lua_newtable(L);//创建一个表格，放在栈顶
    set_table_to_table(L, "border", -2);
    set_value_to_table(L, "hide", css->invisible);
    /* stackDump(L); */

    lua_newtable(L);//创建一个表格，放在栈顶i
    set_value_to_table(L, "width", r->width);
    set_value_to_table(L, "height", r->height);
    set_value_to_table(L, "x", r->left);
    set_value_to_table(L, "y", r->top);
    set_table_to_table(L, "rect", -1); //栈顶的table设置为子table
    /* stackDump(L); */
}

// 获取控件css属性页
static int obj_getCss(lua_State *L)
{
    u32 c = 0 ;
    int id = 0;
    int type = 0;
    int index = 0;
    struct rect r = {0};
    struct element *elm;

    index = luaL_checkinteger(L, 2);
    lua_db(">>>>> get css %d\n", index);

    id = lua_get_self_id(L);
    type = (id >> 16) & 0x3f;
    elm = ui_core_get_element_by_id(id);

    switch (type) {
    case CTRL_TYPE_WINDOW:
        break;
    case CTRL_TYPE_LAYER:
        break;
    case CTRL_TYPE_LAYOUT:
        break;
    case CTRL_TYPE_BUTTON: {
        struct button *btn = (struct button *)elm;
        if (index <= btn->info->head.css_num) {
            struct element_css *css = &btn->info->head.css[index];
            ui_core_get_element_abs_rect(elm->parent, &r);
            ui_css_get_abs_rect(css, &r);
#if (UI_ROTATE == 0x0)
            printf("rect: %d, %d, %d, %d css.invisible=%d\n", r.left, r.top,
                   r.width, r.height, css->invisible);
#else
            //旋转
            c = r.left;
            r.left = r.top;
            r.top = c;
            c = r.height;
            r.height = r.width;
            r.width = c;
            printf("rect: %d, %d, %d, %d css.invisible=%d\n", r.left, r.top,
                   r.width, r.height, css->invisible);
            printf("border rect: %d, %d, %d, %d color:%x\n", css->border.left, css->border.top,
                   css->border.right, css->border.bottom, css->border.color);
            make_css_table(L, css, &r);
            return 1; //返回栈顶table
#endif
        }
    }
    break;
    default:
        break;
    }

    return 0;	// 失败，无数据返回
}
#endif


// 获取控件的矩形区域
// 参数：无
// 返回：table
// table.x 控件起始的x坐标
// table.y 控件起始的y坐标
// table.width 控件宽度
// table.height 控件高度
static int obj_getRect(lua_State *L)
{
    lua_db(">>>>> get rect \n");
    u32 c = 0 ;
    int id = 0;
    struct rect r = {0};
    struct element *elm;
    struct element_css *css;

    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);
    css = &elm->css;
    ui_core_get_element_abs_rect(elm->parent, &r);
    ui_css_get_abs_rect(css, &r);

#if (UI_ROTATE == 0x0)
    printf("rect: %d, %d, %d, %d css.invisible=%d\n", r.left, r.top,
           r.width, r.height, css->invisible);
#else
    //旋转
    c = r.left;
    r.left = r.top;
    r.top = c;
    c = r.height;
    r.height = r.width;
    r.width = c;

    printf("rect: %d, %d, %d, %d css.invisible=%d\n", r.left, r.top,
           r.width, r.height, css->invisible);

#endif

    lua_newtable(L);//创建一个表格，放在栈顶i
    /* stackDump(L); */
    set_value_to_table(L, "width", r.width);
    set_value_to_table(L, "height", r.height);
    set_value_to_table(L, "x", r.left);
    set_value_to_table(L, "y", r.top);

    return 1; //返回栈顶table
}

// 获取控件的X坐标
// 参数：无
// 返回：X坐标
static int obj_getX(lua_State *L)
{
    int x = 0;
    int id = 0;
    struct rect r = {0};
    struct element *elm;
    struct element_css *css;

    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);
    css = &elm->css;
    ui_core_get_element_abs_rect(elm->parent, &r);
    ui_css_get_abs_rect(css, &r);

    printf("rect: %d, %d, %d, %d css.invisible=%d\n", \
           r.left, r.top, r.width, r.height, css->invisible);

#if (UI_ROTATE == 0x0)
    x = r.left;
#else
    x = r.top;
#endif

    lua_pushinteger(L, x);

    return 1;
}

// 获取控件的Y坐标
// 参数：无
// 返回：控件的Y坐标
static int obj_getY(lua_State *L)
{
    int y = 0;
    int id = 0;
    struct rect r = {0};
    struct element *elm;
    struct element_css *css;

    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);
    css = &elm->css;
    ui_core_get_element_abs_rect(elm->parent, &r);
    ui_css_get_abs_rect(css, &r);

    printf("rect: %d, %d, %d, %d css.invisible=%d\n", \
           r.left, r.top, r.width, r.height, css->invisible);

#if (UI_ROTATE == 0x0)
    y = r.top;
#else
    y = r.left;
#endif

    lua_pushinteger(L, y);

    return 1;
}

// 获取控件的宽度
// 参数：无
// 返回：控件的宽度
static int obj_getWidth(lua_State *L)
{
    int w = 0;
    int id = 0;
    struct rect r = {0};
    struct element *elm;
    struct element_css *css;

    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);
    css = &elm->css;
    ui_core_get_element_abs_rect(elm->parent, &r);
    ui_css_get_abs_rect(css, &r);

    printf("rect: %d, %d, %d, %d css.invisible=%d\n", \
           r.left, r.top, r.width, r.height, css->invisible);

#if (UI_ROTATE == 0x0)
    w = r.width;
#else
    w = r.height;
#endif

    lua_pushinteger(L, w);

    return 1;
}

// 获取控件的高度
// 参数：无
// 返回：控件的高度
static int obj_getHeight(lua_State *L)
{
    int h = 0;
    int id = 0;
    struct rect r = {0};
    struct element *elm;
    struct element_css *css;

    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);
    css = &elm->css;
    ui_core_get_element_abs_rect(elm->parent, &r);
    ui_css_get_abs_rect(css, &r);

    printf("rect: %d, %d, %d, %d css.invisible=%d\n", \
           r.left, r.top, r.width, r.height, css->invisible);

#if (UI_ROTATE == 0x0)
    h = r.height;
#else
    h = r.width;
#endif

    lua_pushinteger(L, h);

    return 1;
}


// 设置控件的矩形区域
// 参数：
// (x, y, w, h)
// (x, y, w, h, redraw)
// 返回：无
static int obj_setRect(lua_State *L)
{
    int id = 0;
    int x, y, w, h;
    int redraw = true;
    struct rect r = {0};
    struct element *elm;
    struct element_css *css;

    int n = lua_gettop(L);
    if (6 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    h = luaL_checkinteger(L, -1);
    w = luaL_checkinteger(L, -2);
    y = luaL_checkinteger(L, -3);
    x = luaL_checkinteger(L, -4);

    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);
    ui_core_get_element_abs_rect(elm->parent, &r);

    lua_db("set rect: id:0x%x (x=%d, y=%d, w=%d, h=%d), redraw:%d\n", id, x, y, w, h, redraw);

#if (UI_ROTATE == 0x0)
    elm->css.left   = x * 10000 / r.width;
    elm->css.top    = y * 10000 / r.height;
    elm->css.width  = w * 10000 / r.width;
    elm->css.height = h * 10000 / r.height;
#else
    elm->css.left   = y * 10000 / r.width;
    elm->css.top    = x * 10000 / r.height;
    elm->css.width  = h * 10000 / r.width;
    elm->css.height = w * 10000 / r.height;
#endif

    if (redraw) {
        ui_core_redraw(elm->parent);
    }

    return 0;
}

// 设置控件的X坐标
// 参数：
// (x)
// (x, redraw)
// 返回：无
static int obj_setX(lua_State *L)
{
    int x = 0;
    int id = 0;
    int redraw = true;
    struct rect r = {0};
    struct element *elm;
    struct element_css *css;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    x = lua_tointeger(L, -1);
    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);
    ui_core_get_element_abs_rect(elm->parent, &r);

    lua_db("id:0x%x, set X: %d, redraw:%d\n", id, x, redraw);

#if (UI_ROTATE == 0x0)
    elm->css.left = x * 10000 / r.width;
#else
    elm->css.top = x * 10000 / r.height;
#endif

    if (redraw) {
        ui_core_redraw(elm->parent);
    }

    return 0;
}


// 设置控件的Y坐标
// 参数：
// (y)
// (y, redraw)
// 返回：无
static int obj_setY(lua_State *L)
{
    int y = 0;
    int id = 0;
    int redraw = true;
    struct rect r = {0};
    struct element *elm;
    struct element_css *css;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    y = lua_tointeger(L, -1);
    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);
    ui_core_get_element_abs_rect(elm->parent, &r);

    lua_db("id:0x%x, set Y: %d, redraw:%d\n", id, y, redraw);

#if (UI_ROTATE == 0x0)
    elm->css.top = y * 10000 / r.height;
#else
    elm->css.left = y * 10000 / r.width;
#endif

    if (redraw) {
        ui_core_redraw(elm->parent);
    }

    return 0;
}


// 设置控件宽度
// 参数：
// (w)
// (w, redraw)
// 返回：无
static int obj_setWidth(lua_State *L)
{
    int w = 0;
    int id = 0;
    int redraw = true;
    struct rect r = {0};
    struct element *elm;
    struct element_css *css;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    w = lua_tointeger(L, -1);
    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);
    ui_core_get_element_abs_rect(elm->parent, &r);

    lua_db("id:0x%x, set width: %d, redraw:%d\n", id, w, redraw);

#if (UI_ROTATE == 0x0)
    elm->css.width = w * 10000 / r.width;
#else
    elm->css.height = w * 10000 / r.height;
#endif

    if (redraw) {
        ui_core_redraw(elm->parent);
    }

    return 0;
}


// 设置控件高度
// 参数：
// (h)
// (h, redraw)
// 返回：无
static int obj_setHeight(lua_State *L)
{
    int h = 0;
    int id = 0;
    int redraw = true;
    struct rect r = {0};
    struct element *elm;
    struct element_css *css;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    h = lua_tointeger(L, -1);
    id = lua_get_self_id(L);
    elm = ui_core_get_element_by_id(id);
    ui_core_get_element_abs_rect(elm->parent, &r);

    lua_db("id:0x%x, set height: %d, redraw:%d\n", id, h, redraw);

#if (UI_ROTATE == 0x0)
    elm->css.height = h * 10000 / r.height;
#else
    elm->css.width = h * 10000 / r.width;
#endif

    if (redraw) {
        ui_core_redraw(elm->parent);
    }

    return 0;
}



static int obj_test(lua_State *L)
{
    lua_db(">>>>>>>>>>>>>>>>>>>>>>>> lua obj test func\n");
    int n = lua_gettop(L);	// 获取参数个数
    printf("argument num:%d\n", n);

    int sum = 0;
    int val = 0;
    for (int i = 1 + 1; i <= n; i++) {	// 注意，+1 表示从第二个元素开始
        if (lua_isinteger(L, i)) {
            val = lua_tointeger(L, i);
            sum += val;
            printf("val:%d, sum:%d\n", val, sum);
        } else {
            lua_pushliteral(L, "error argument");	// 可以把错误信息当成返回值压入栈
            /* lua_error(L); */
            return 1;	// 返回值个数
        }
    }
    lua_pushinteger(L, sum); // 第一个返回值入栈
    return 1;	// 返回值的个数
}


// 方法注册
const luaL_Reg obj_method[] = {
    // show, hide方法
    {"show",		obj_show},		// obj:show()
    {"hide",		obj_hide},		// obj:hide()
    {"isHide",		obj_isHide},	// obj:hide()

    // 高亮方法
    {"highLight",	obj_highLight},		// obj:heigLight()
    {"noHighLight", obj_noHighLight},	// obj:noHeigLight()
    {"isHighLight",	obj_isHighLight},	// obj:heigLight()

    // 手动刷新
    {"redrawSelf",	obj_redrawSelf},	// obj:heigLight()

    // 控件属性
    {"setRect", 	obj_setRect},
    {"setX",		obj_setX},
    {"setY",		obj_setY},
    {"setWidth",	obj_setWidth},
    {"setHeight",	obj_setHeight},

    {"getRect",		obj_getRect},
    {"getX",		obj_getX},
    {"getY",		obj_getY},
    {"getWidth",	obj_getWidth},
    {"getHeight",	obj_getHeight},

    // css 属性，参数很多
    /* {"setCss", obj_setCss}, */
    /* {"getCss", obj_getCss}, */

    // touch event
    // 注意：这些回调只能注册一次，多次注册会覆盖导致不可预知的异常
    {"bindTouchDown",	obj_bindTouchDown},
    {"touchDown",		obj_touchDown},
    {"bindTouchUp",		obj_bindTouchUp},
    {"touchUp",			obj_touchUp},

    {"bindTouchUMove",	obj_bindTouchUMove},
    {"touchUMove",		obj_touchUMove},
    {"bindTouchDMove",	obj_bindTouchDMove},
    {"touchDMove",		obj_touchDMove},
    {"bindTouchLMove",	obj_bindTouchLMove},
    {"touchLMove",		obj_touchLMove},
    {"bindTouchRMove",	obj_bindTouchRMove},
    {"touchRMove",		obj_touchRMove},

    // 纯测试，没啥用
    {"test", obj_test},

    //TODO
    {NULL, NULL}
};

#endif /* #if (TCFG_LUA_ENABLE) */


