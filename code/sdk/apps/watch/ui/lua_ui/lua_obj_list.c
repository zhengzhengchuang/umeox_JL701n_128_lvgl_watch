#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_obj.h"
#include "lua_gui.h"

#include "ui/ui_grid.h"

#if (TCFG_LUA_ENABLE)

#define ui_grid_for_id(id) \
	({ \
		struct element *elm = ui_core_get_element_by_id(id); \
	 	elm ? container_of(elm, struct ui_grid, elm): NULL; \
	 })

extern void *ui_core_load_widget_info(void *__head, u8 page);

// 初始化动态列表（row, col, redraw）
// 参数：
// (row, col)
// (row, col, redraw)
// 返回：无
static int obj_initGridDynamic(lua_State *L)
{
    int id = 0;
    int row = 0;
    int col = 0;
    int redraw = true;
    struct ui_grid *grid = NULL;
    /* static struct scroll_area area = {0, 0, 10000, 10000}; */

    int n = lua_gettop(L);
    if (4 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    col = lua_tointeger(L, -1);
    row = lua_tointeger(L, -2);

    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    lua_db("%s, id:0x%x, row:%d, col:%d, redraw:%d\n", __FUNCTION__, id, row, col, redraw);
    ui_grid_init_dynamic(grid, &row, &col);
    /* ui_grid_set_scroll_area(grid, &area); */

    if (redraw) {
        ui_core_redraw(&grid->elm);
    }

    return 0;
}


// 动态添加行、列(row, col, redraw)
// 参数：
// (row, col)
// (row, col, redraw)
// 返回：无
static int obj_addGridDynamic(lua_State *L)
{
    int id = 0;
    int row = 0;
    int col = 0;
    int redraw = true;
    struct ui_grid *grid = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    col = lua_tointeger(L, -1);
    row = lua_tointeger(L, -2);

    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);

    lua_db("%s, id:0x%x, row:%d, col:%d, redraw:%d\n", __FUNCTION__, id, row, col, redraw);
    /* ui_grid_add_dynamic_by_id(id, &row, &col, 0); */
    ui_grid_add_dynamic(grid, &row, &col, redraw);

    return 0;
}


// 动态删除行、列
// 参数：
// (row, col)
// (row, col, redraw)
// 返回：无
static int obj_delGridDynamic(lua_State *L)
{
    int id = 0;
    int row = 0;
    int col = 0;
    int redraw = true;
    struct ui_grid *grid = NULL;

    int n = lua_gettop(L);
    if (4 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    col = lua_tointeger(L, -1);
    row = lua_tointeger(L, -2);

    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);

    lua_db("%s, id:0x%x, row:%d, col:%d, redraw:%d\n", __FUNCTION__, id, row, col, redraw);
    /* ui_grid_del_dynamic_by_id(id, &row, &col, 0); */
    ui_grid_del_dynamic(grid, &row, &col, redraw);

    return 0;
}


static int obj_setGridDynamicHiIndex(lua_State *L)
{
    int id = 0;
    int dhindex = 0;
    int init = 0;
    int hi_index = 0;
    struct ui_grid *grid = NULL;

    hi_index = lua_tointeger(L, -1);
    init = lua_tointeger(L, -2);
    dhindex = lua_tointeger(L, -3);
    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);

    ui_grid_set_hindex_dynamic(grid, dhindex, init, hi_index);

    return 0;
}


static int obj_getGridDynamicHiIndex(lua_State *L)
{
    int id = 0;
    int hidex = 0;
    struct ui_grid *grid = NULL;

    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    hidex = ui_grid_get_hindex_dynamic(grid);

    lua_pushinteger(L, hidex);

    return 1;
}


static int obj_setGridDynamicBase(lua_State *L)
{
    int id = 0;
    int base_index = 0;
    struct ui_grid *grid = NULL;

    base_index = lua_tointeger(L, -1);
    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    ui_grid_set_base_dynamic(grid, base_index);

    return 0;
}


static int obj_setGridDynamicSlideDir(lua_State *L)
{
    int id = 0;
    int slide_dir = 0;
    struct ui_grid *grid = NULL;

    slide_dir = lua_tointeger(L, -1);
    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    ui_grid_set_slide_direction(grid, slide_dir);

    return 0;
}

static int obj_getGridDynamicCurrentItem(lua_State *L)
{
    int id = 0;
    int item = 0;
    struct ui_grid *grid = NULL;

    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    item = ui_grid_cur_item_dynamic(grid);
    lua_db("%s, id:0x%x, item:%d\n", __FUNCTION__, id, item);
    lua_pushinteger(L, item);

    return 1;
}


static int obj_updateGridDynamic(lua_State *L)
{
    int id = 0;
    int item_sel = 0;
    int redraw = 0;
    struct ui_grid *grid = NULL;

    redraw = lua_toboolean(L, -1);
    item_sel = lua_tointeger(L, -2);
    id = lua_get_self_id(L);
    ui_grid_update_by_id_dynamic(id, item_sel, redraw);
    lua_db("%s, id:0x%x, item_sel:%d, redraw:%d\n", __FUNCTION__, id, item_sel, redraw);
    return 0;
}


static int obj_getGridTouchItem(lua_State *L)
{
    int id = 0;
    struct ui_grid *grid = NULL;

    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    lua_db("%s, id:0x%x, touch_item:%d\n", __FUNCTION__, id, grid->touch_index);

    lua_pushinteger(L, grid->touch_index);

    return 1;
}

static int obj_getGridCurIndex(lua_State *L)
{
    int id = 0;
    struct ui_grid *grid = NULL;

    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    lua_db("%s, id:0x%x, cur_dindex:%d\n", __FUNCTION__, id, grid->cur_dindex);

    lua_pushinteger(L, grid->cur_dindex);

    return 1;
}





// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 以下为静态列表

static int obj_setGridOnFocus(lua_State *L)
{
    int id = 0;
    struct ui_grid *grid = NULL;

    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    ui_grid_on_focus(grid);

    return 0;
}

static int obj_setGridLoseFocus(lua_State *L)
{
    int id = 0;
    struct ui_grid *grid = NULL;

    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    ui_grid_lose_focus(grid);

    return 0;
}

static int obj_setGridStateReset(lua_State *L)
{
    int id = 0;
    int highlight_item = 0;
    struct ui_grid *grid = NULL;

    highlight_item = lua_tointeger(L, -1);
    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    ui_grid_state_reset(grid, highlight_item);

    return 0;
}


static int obj_setGridHighLightItem(lua_State *L)
{
    int id = 0;
    int highlight_item = 0;
    int yes = 0;
    struct ui_grid *grid = NULL;

    yes = lua_toboolean(L, -1);
    highlight_item = lua_tointeger(L, -2);
    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    if (grid) {
        /* 初始化列表子控件 */
        struct ui_grid_info *info;
        info = (struct ui_grid_info *)ui_core_load_widget_info(grid->info, -1);
        if (!grid->child_init) {
            ui_grid_child_init(grid, info);
        }
        ui_grid_highlight_item(grid, highlight_item, yes);
    }

    return 0;
}

static int obj_setGridSlide(lua_State *L)
{
    int id = 0;
    int steps = 0;
    int direction = 0;
    struct ui_grid *grid = NULL;

    steps = lua_tointeger(L, -1);
    direction = lua_tointeger(L, -2);
    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    ui_grid_slide(grid, direction, steps);

    return 0;
}


// 设置选项数量
static int obj_setGridItemNumber(lua_State *L)
{
    int id = 0;
    int item_num = 0;
    struct ui_grid *grid = NULL;

    item_num = lua_tointeger(L, -1);
    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    lua_db("%s, id:0x%x, item_num:%d\n", __FUNCTION__, id, item_num);
    ui_grid_set_item_num(grid, item_num);

    return 0;
}



static int obj_setGridSlideDir(lua_State *L)
{
    int id = 0;
    int dir = 0;
    struct ui_grid *grid = NULL;

    dir = lua_tointeger(L, -1);
    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    lua_db("%s, id:0x%x, dir:%d\n", __FUNCTION__, id, dir);
    ui_grid_set_slide_direction(grid, dir);

    return 0;
}


static int obj_setGridPixScroll(lua_State *L)
{
    int id = 0;
    int enable = 0;
    struct ui_grid *grid = NULL;

    enable = lua_toboolean(L, -1);
    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    lua_db("%s, id:0x%x, enable:%d\n", __FUNCTION__, id, enable);
    ui_grid_set_pix_scroll(grid, enable);

    return 0;
}


// 参数（left, top, right, bottom）
static int obj_setGridScrollArea(lua_State *L)
{
    int id = 0;
    struct scroll_area area = {0};
    struct ui_grid *grid = NULL;

    area.bottom = lua_tointeger(L, -1);
    area.right = lua_tointeger(L, -2);
    area.top = lua_tointeger(L, -3);
    area.left = lua_tointeger(L, -4);

    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    lua_db("%s, id:0x%x, left:%d, top:%d, right:%d, bottom:%d\n", __FUNCTION__, id, area.left, area.top, area.right, area.bottom);
    ui_grid_set_scroll_area(grid, &area);

    return 0;
}

// 获取高亮选项
static int obj_getGridHiIndex(lua_State *L)
{
    int id = 0;
    int index = 0;
    struct ui_grid *grid = NULL;

    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    index = ui_grid_get_hindex(grid);

    lua_db("%s, id:0x%x, index:%d\n", __FUNCTION__, id, index);
    lua_pushinteger(L, index);

    return 1;
}


// 设置高亮选项
static int obj_setGridHiIndex(lua_State *L)
{
    int id = 0;
    int err = 0;
    int index = 0;
    struct ui_grid *grid = NULL;

    index = lua_tointeger(L, -1);
    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    lua_db("%s, id:0x%x, index:%d\n", __FUNCTION__, id, index);
    /* 初始化列表子控件 */
    if (!grid->child_init) {
        struct ui_grid_info *info;
        info = (struct ui_grid_info *)ui_core_load_widget_info(grid->info, -1);
        ui_grid_child_init(grid, info);
    }
    err = ui_grid_set_hi_index(grid, index);

    if (err) {
        printf("ERROR: set grid hi index faild: %d\n", index);
    }

    return 0;
}


static int obj_setStaticGridItemNum(lua_State *L)
{
    int items_num = 0;
    int limit_num = 0;
    int watch_num;
    int id;

    id = lua_get_self_id(L);
    struct ui_grid *grid = ui_grid_for_id(id);

    watch_num = lua_tointeger(L, -1);
    lua_pop(L, 1);

    items_num = watch_num;
    extern int ui_grid_get_item_limit_num(struct ui_grid * grid);
    limit_num = ui_grid_get_item_limit_num(grid);
    items_num = (items_num >= limit_num) ? limit_num : items_num;
    items_num = (items_num == 0) ? 1 : items_num;

    printf("grid num %d\n", items_num);
    printf("id : %d\n", id);

    if (grid) {

        /* 修改列表条目数量,不能超过列表的最大数量 */
        ui_grid_set_item_num(grid, items_num);

        /* 初始化列表子控件 */
        struct ui_grid_info *info;
        info = (struct ui_grid_info *)ui_core_load_widget_info(grid->info, -1);
        if (!grid->child_init) {
            ui_grid_child_init(grid, info);
        }
    }

    return 0;
}


// 设置成圆弧列表，只能在初始化时调用
// 参数：
// () --> 省略，设置为圆弧列表并redraw
// (rotate) --> 配置圆弧列表参数：true圆弧，false取消圆弧，触发redraw
static int obj_setGridToRotate(lua_State *L)
{
    int rotate = true;
    int id = 0;
    struct ui_grid *grid = NULL;

    int n = lua_gettop(L);
    if (2 == n) {
        rotate = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    id = lua_get_self_id(L);
    grid = ui_grid_for_id(id);
    grid->rotate = rotate;
    lua_db("%s, id:%d, rotate:%d\n", __FUNCTION__, id, rotate);

    return 0;
}

const luaL_Reg obj_ListMethod[] = {
    /* *
     * 列表/表格控件方法：
     * 		除动态列表初始化、增加、删除项之外，其余部分应与静态列表方法相同
     * 		动态列表专用：
     * 			初始化 initDynamicGrid
     * 			增加项 addDynamicGrid
     * 			删除项 delDynamicGrid
     * 			获取刷新项 getRefreshItem
     *
     * 		动、静态列表共用：
     * 			设置高亮项 setHighLightItem
     * 			获取高亮项 getHighLightItem
     * 			获取touch项 getTouchItem
     * 			设置滑动区域 setScrollArea
     * 			设置滑动方向 setSliderDir
     *
     * 		静态列表专用，动态列表不生效：
     * 			设置子控件项数 setItemNumber
     * 			设置为圆弧列表 setGridToRotate
     *
     * */



    /*~~~~~~~~~~~~~~~~~~~~ 动态列表 ~~~~~~~~~~~~~~~~~~~~~*/
    /* 将静态列表初始化为动态列表 */
    {"initGridDynamic",			obj_initGridDynamic},

    /* 列表末尾追加选项 */
    {"addGridDynamic",			obj_addGridDynamic},

    /* 列表末尾删除选项 */
    {"delGridDynamic",			obj_delGridDynamic},

    /* 设置动态列表高亮项 */
    {"setGridDynamicHiIndex",	obj_setGridDynamicHiIndex},

    /* 获取动态列表高亮项 */
    {"getGridDynamicHiIndex",	obj_getGridDynamicHiIndex},

    /* set base */
    {"setGridDynamicBase",		obj_setGridDynamicBase},

    /* 手动update动态列表 */
    {"updateGridDynamic",		obj_updateGridDynamic},

    /* 获取动态列表当前项 */
    {"getGridCurIndex",			obj_getGridCurIndex},

    /* hi_index */
    {"getGridHiIndex",			obj_getGridHiIndex},
    {"setGridHiIndex",			obj_setGridHiIndex},



    /*~~~~~~~~~~~~~~~~~~~~ 静态列表 ~~~~~~~~~~~~~~~~~~~~~*/
    /* 静态列表聚焦 */
    {"setGridOnFocus",			obj_setGridOnFocus},

    /* 静态列表释放聚焦 */
    {"setGridLostFocus",		obj_setGridLoseFocus},

    /* state reset */
    {"setGridStateReset",		obj_setGridStateReset},

    {"setGridHighLightItem",	obj_setGridHighLightItem},

    /* 设置子控件项目 */
    {"setStaticGridItemNum",	obj_setStaticGridItemNum},


    /*~~~~~~~~~~~~~~~~~~~~ 列表共用 ~~~~~~~~~~~~~~~~~~~~~*/
    /* 设置列表滑动方向 */
    {"setGridSlideDirection",	obj_setGridSlideDir},

    /* 设置列表滑动区域 */
    {"setGridScrollArea",		obj_setGridScrollArea},

    /* 获取列表被touch的项 */
    {"getGridTouchItem",		obj_getGridTouchItem},

    /* 像素滚动 */
    {"setGridPixScroll",		obj_setGridPixScroll},

    /* 垂直列表设置成圆弧列表 */
    {"setGridToRotate",			obj_setGridToRotate},

    {NULL, NULL}
};

#endif /* #if (TCFG_LUA_ENABLE) */

