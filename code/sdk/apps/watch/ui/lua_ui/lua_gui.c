/* sdk 头文件 */
#include "system/includes.h"
#include "ui/ui_page_manager.h"
#include "ui/res_config.h"
#include "asm/imd.h"

/* ui框架及接口头文件 */
#include "ui/control.h"
#include "ui/ui.h"

/* lua解释器头文件 */
#include "lua/lauxlib.h"
#include "lua/lualib.h"

/* lua_ui模块头文件 */
#include "lua_module.h"
#include "lua_gui.h"
#include "lua_obj.h"



#if (TCFG_LUA_ENABLE)


/* 页面最大深度，即使用返回上一页面时可返回的最大层数 */
#define MAX_PAGE_BACK_NUM	8


static struct _lua_gui_private {
    /* 记录切换页面列表 */
    int page_list[MAX_PAGE_BACK_NUM];

    /* 当前页面在 page_list 中的索引，switchPageByName时加一，backToPreviousPage时减一 */
    u8 page_item;

    /* 页面滑动初始化标志 */
    u8 slide_init;
};

static struct _lua_gui_private lua_gui = {
    .page_item = 0,
};
#define __this (&lua_gui)


extern char *watch_get_item(int style);

/* 从self table获取指定参数  */
int lua_get_interget_from_self(lua_State *L, char *name)
{
    int value = 0;

    lua_pushstring(L, name);
    lua_gettable(L, 1);

    if (lua_isnumber(L, -1)) {
        value = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    return value;
}


/* 设置self table中指定key的value  */
int lua_set_interget_to_self(lua_State *L, char *key, int val)
{
    lua_pushstring(L, key);
    lua_pushinteger(L, val);
    lua_settable(L, 1);
    lua_pop(L, 2);

    return 0;
}


/* 获取实例self的id */
int lua_get_self_id(lua_State *L)
{
    return lua_get_interget_from_self(L, "__id");
}


// 创建控件实例
// (ename) 控件名称
static int ui_getComponentByName(lua_State *L)
{
    int id = 0;
    int type = 0;

    id = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_newtable(L);	/* 创建对象table */

    lua_pushstring(L, "__id");	/* 保存控件ID */
    lua_pushinteger(L, id);
    lua_settable(L, -3);

    type = ui_id2type(id);

    /* 注册公共的OBJ方法(即所有控件都能调用的方法) */
    luaL_setfuncs(L, obj_method, 0);

    /* 下面根据不同的控件类型选择注册其私有方法， */
    /* 防止出现类似text可以调用到image操作方法 */
    switch (type) {
    case CTRL_TYPE_WINDOW:
        lua_db(">>>>>>> windows method load\n");
        printf("[ WARNING ] %s, no window self method!!!\n", __FUNCTION__);
        break;
    case CTRL_TYPE_LAYOUT:
        lua_db(">>>>>>> layout method load\n");
        luaL_setfuncs(L, obj_DrawMethod, 0);
        break;
    case CTRL_TYPE_LAYER:
        lua_db(">>>>>>> layer method load\n");
        printf("[ WARNING ] %s, no layer self method!!!\n", __FUNCTION__);
        break;
    case CTRL_TYPE_GRID:
        luaL_setfuncs(L, obj_ListMethod, 0);
        lua_db(">>>>>>> grid method load\n");
        break;
    case CTRL_TYPE_LIST:
        luaL_setfuncs(L, obj_ListMethod, 0);
        lua_db(">>>>>>> list method load\n");
        break;
    case CTRL_TYPE_BUTTON:
        lua_db(">>>>>>> button method load\n");
        break;
    case CTRL_TYPE_PIC:
        lua_db(">>>>>>> Image method load\n");
        luaL_setfuncs(L, obj_ImageMethod, 0);
        break;
    case CTRL_TYPE_BATTERY:
        lua_db(">>>>>>> battery method load\n");
        luaL_setfuncs(L, obj_BatteryMethod, 0);
        break;
    case CTRL_TYPE_TIME:
        lua_db(">>>>>>> Time method load\n");
        luaL_setfuncs(L, obj_TimeMethod, 0);
        break;
    case CTRL_TYPE_TEXT:
        lua_db(">>>>>>> Text method load\n");
        luaL_setfuncs(L, obj_TextMethod, 0);
        break;
    case CTRL_TYPE_NUMBER:
        lua_db(">>>>>>> number method load\n");
        luaL_setfuncs(L, obj_NumberMethod, 0);
        break;
    case CTRL_TYPE_WATCH:
        lua_db(">>>>>>> watch method load\n");
        luaL_setfuncs(L, obj_WatchMethod, 0);
        break;
    case CTRL_TYPE_SLIDER:
    case CTRL_TYPE_VSLIDER:
        lua_db(">>>>>>> slider method load\n");
        luaL_setfuncs(L, obj_SliderMethod, 0);
        break;
    case CTRL_TYPE_COMPASS:
        lua_db(">>>>>>> compass method load\n");
        luaL_setfuncs(L, obj_CompassMethod, 0);
        break;
    case CTRL_TYPE_PROGRESS:
        lua_db(">>>>>>> progress method load\n");
        luaL_setfuncs(L, obj_ProgressMethod, 0);
        break;
    case CTRL_TYPE_MULTIPROGRESS:
        lua_db(">>>>>>> multiprogress method load\n");
        luaL_setfuncs(L, obj_MultiProgressMethod, 0);
        break;
    default:
        log_e(">>> err! unknow ctrl type!\n");
        break;
    }

    return 1;
}


/* PAGE切换 */
// (PAGE, mode)
// (page, mode, record)
static int ui_switchPageByName(lua_State *L)
{
    int record = true;
    int current_page = 0;
    int target_page = 0;
    char *switch_mode = NULL;

    int n = lua_gettop(L);
    /* 先获取传入的参数 */
    if (4 == n) {
        record = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    switch_mode = lua_tostring(L, -1);
    target_page = lua_tointeger(L, -2);
    current_page = UI_GET_WINDOW_ID();

    lua_db(">>> current_page:0x%x, target_page:0x%x, switch_mode:%s\n",
           current_page, target_page, switch_mode);

    if (record) {
        if ((__this->page_list[__this->page_item] != current_page) || (__this->page_item == 0)) {
            /* 如果当前页面和记录的不一致，重新开始记录页面切换历史 */
            __this->page_item = 0;
            __this->page_list[__this->page_item] = current_page;	/* 第0个为当前页面 */
            __this->page_item ++;
            __this->page_list[__this->page_item] = target_page;		/* 第1个为目标页面 */
        } else {
            /* 如果超过最大记录, 后面继续追加，丢掉前面的 */
            if ((++ __this->page_item) >= MAX_PAGE_BACK_NUM) {
                for (int i = 0; i < MAX_PAGE_BACK_NUM - 1; i++) {
                    __this->page_list[i] = __this->page_list[i + 1];
                }
                __this->page_item = MAX_PAGE_BACK_NUM - 1;
            }
            /* 记录目标页面 */
            __this->page_list[__this->page_item] = target_page;
        }

#if 0
        if (__this->page_item == 0) {
            /* 记录列表中没有元素 */
            __this->page_list[__this->page_item++] = current_page;
            __this->page_list[__this->page_item] = target_page;
        } else if (__this->page_list[__this->page_item] != current_page) {
            /* 如果当前页面和记录的不一致，重新开始记录页面切换历史 */
            __this->page_item = 0;
            __this->page_list[0] = current_page;
        } else {
            if ((++ __this->page_item) >= MAX_PAGE_BACK_NUM) {
                /* 如果超过最大记录, 后面继续追加，丢掉前面的 */
                printf("[ WARNING ]: maximum number of pages exceeded, throw page[0] 0x%x\n", __this->page_list[0]);
                for (int i = 0; i < MAX_PAGE_BACK_NUM - 1; i++) {
                    __this->page_list[i] = __this->page_list[i + 1];
                }
                __this->page_item = MAX_PAGE_BACK_NUM - 1;
                __this->page_list[__this->page_item] = current_page;
                /* 如果记录的页面超过最大记录，覆盖掉前面的记录重新开始 */
                /* __this->page_item = 1; */
                /* __this->page_list[0] = current_page; */
            }
            __this->page_list[__this->page_item] = target_page;
        }
#endif
    }

#if 1
    for (int i = 0; i < __this->page_item; i++) {
        printf("switch page[%d] = 0x%x\n", i, __this->page_list[i]);
    }
#endif

    /* 切换页面, 目前只接受hide left right三种切换方式 */
    int lcd_width = get_lcd_width_from_imd();
    int step = lcd_width / 40;
    if (!strcmp(switch_mode, "left")) {
        for (int i = 0; i < step; i++) {
            ui_page_switch(current_page, target_page, i * 40, 0);
        }
        /* 需要把当前页面hide掉才能让消息被新页面接管 */
        ui_hide(current_page);
    } else if (!strcmp(switch_mode, "right")) {
        for (int i = 0; i < step; i++) {
            ui_page_switch(current_page, target_page, i * (-40), 0);
        }
        ui_hide(current_page);
    } else {
        /* 默认切换方式为直接show hide */
        ui_hide(current_page);
        ui_show(target_page);
    }

    return 0;	// 返回参数的个数
}


// 返回上一个页面
// 1、判断上一个页面是否存在，如果存在则判断是否可切换回上一个页面
// 2、如果上一个页面不存在，则返回链表第一个页面
// 3、如果两个都不存在，则不切换
static int ui_backToPreviousPage(lua_State *L)
{
    int current_page = 0;	/* 当前页面 */
    int previou_page = 0;	/* 上一个页面 */
    char *mode = NULL;		/* 切换模式 */
    struct ui_page *page = NULL;

#if 1
    for (int i = 0; i < __this->page_item; i++) {
        printf("back page[%d] = 0x%x\n", i, __this->page_list[i]);
    }
#endif

    mode = lua_tostring(L, -1);
    current_page = UI_GET_WINDOW_ID();

    if ((__this->page_list[__this->page_item] != current_page) || \
        (__this->page_item < 1) || (__this->page_item >= MAX_PAGE_BACK_NUM)) {
        page = ui_page_get_first();
        previou_page = page->id;
    } else {
        __this->page_list[__this->page_item] = 0;	/* 删除当前页面ID */
        if ((--__this->page_item) < 0) {
            __this->page_item = 0;
        }
        previou_page = __this->page_list[__this->page_item];	/* 获取上一页页面id */
    }

    if (!previou_page) {
        printf("ERROR: previous page not find!\n");
        return 0;
    }

    /* 切换页面 */
    if (previou_page && current_page) {
        lua_db("current_page:0x%x, previou_page:0x%x, mode:%s\n", current_page, previou_page, mode);
        int lcd_width = get_lcd_width_from_imd();
        int step = lcd_width / 40;
        if (!strcmp(mode, "left")) {
            for (int i = 0; i < step; i++) {
                ui_page_switch(current_page, previou_page, i * 40, 0);
            }
            /* 需要把当前页面hide掉才能让消息被新页面接管 */
            ui_hide(current_page);
        } else if (!strcmp(mode, "right")) {
            for (int i = 0; i < step; i++) {
                ui_page_switch(current_page, previou_page, i * (-40), 0);
            }
            ui_hide(current_page);
        } else {
            /* 默认切换方式为直接show hide */
            ui_hide(current_page);
            ui_show(previou_page);
        }
    } else {
        lua_db("%s, no switch page (current_page:0x%x, previou_page:0x%x)\n", __FUNCTION__, current_page, previou_page);
    }

    return 0;
}


// 检查前一个页面是否为某个页面
static int ui_checkPreviousPage(lua_State *L)
{
    int id = 0;
    int check = 0;
    int previous_id = 0;

    id = lua_tointeger(L, -1);
    previous_id = __this->page_list[__this->page_item - 1];
    lua_db("%s, check id:0x%x, previous id:0x%x\n", __FUNCTION__, id, previous_id);
    if (id == previous_id) {
        check = 1;
    } else {
        check = 0;
    }

    lua_pushboolean(L, check);

    return 1;
}


// 获取控件类型
static int ui_getControlType(lua_State *L)
{
    int id = 0;
    int type = 0;
    char *ret = NULL;

    /* 这里获取ID是假设栈里只有根table或者将self作为参数传入进来而获取的 */
    lua_pushstring(L, "__id");
    lua_gettable(L, 1);

    if (lua_isnumber(L, -1)) {
        id = lua_tointeger(L, -1);
    } else {
        return 0;
    }
    lua_pop(L, 1);

    type = (id >> 16) & 0x3f;

    switch (type) {
    case CTRL_TYPE_WINDOW:
        ret = "Window";
        break;
    case CTRL_TYPE_LAYOUT:
        ret = "NewLayout";
        break;
    case CTRL_TYPE_LAYER:
        ret = "NewLayer";
        break;
    case CTRL_TYPE_GRID:
        ret = "NewGrid";
        break;
    case CTRL_TYPE_LIST:
        ret = "List";
        break;
    case CTRL_TYPE_BUTTON:
        ret = "Button";
        break;
    case CTRL_TYPE_PIC:
        ret = "ImageList";
        break;
    case CTRL_TYPE_BATTERY:
        ret = "Battery";
        break;
    case CTRL_TYPE_TIME:
        ret = "Time";
        break;
    case CTRL_TYPE_TEXT:
        ret = "Text";
        break;
    case CTRL_TYPE_NUMBER:
        ret = "Number";
        break;
    case CTRL_TYPE_WATCH:
        ret = "Watch";
        break;
    case CTRL_TYPE_SLIDER:
    case CTRL_TYPE_VSLIDER:
        ret = "Slider";
        break;
    default:
        ret = "unknow";
        break;
    }

    lua_pushstring(L, ret);
    return 1;
}


// 页面滑动初始化
static int ui_pageSlideInit(lua_State *L)
{
    if (!__this->slide_init) {
        if (!ui_page_has_been_init()) {
            ui_page_init();
        }
        __this->slide_init = true;
    } else {
        lua_db("page slide has init!\n");
    }

    return 0;
}


// 添加滑动页面
static int ui_pageSlideAdd(lua_State *L)
{
    int page_id = 0;

    if (__this->slide_init || ui_page_has_been_init()) {
        page_id = lua_tointeger(L, -1);
        struct ui_page *page = ui_page_search(page_id);
        if (page) {
            lua_db("page 0x%x overlay ", page_id);
        } else {
            ui_page_add(page_id);
        }
    } else {
        lua_db("page slide no init!\n");
    }

    return 0;
}

// 删除滑动页面
static int ui_pageSlideDel(lua_State *L)
{
    int page_id = 0;

    if (__this->slide_init || ui_page_has_been_init()) {
        page_id = lua_tointeger(L, -1);
        ui_page_del(page_id);
    } else {
        lua_db("page slide no init!\n");
    }

    return 0;
}


// 页面滑动初始化
static int ui_pageSlideFree(lua_State *L)
{
    if (__this->slide_init || ui_page_has_been_init()) {
        ui_page_free();
        __this->slide_init = false;
    } else {
        lua_db("page slide no init!\n");
    }
    return 0;
}


// 下一个页面
static int ui_pageSlideNext(lua_State *L)
{
    int ret = 0;
    int page_id = 0;

    if (__this->slide_init || ui_page_has_been_init()) {
        page_id = lua_tointeger(L, -1);
        ret = ui_page_next(page_id);

        lua_pushinteger(L, ret);
        return 1;
    } else {
        lua_db("page slide no init!\n");
        return 0;
    }
}


// 下一个页面
static int ui_pageSlidePrev(lua_State *L)
{
    int ret = 0;
    int page_id = 0;

    if (__this->slide_init || ui_page_has_been_init()) {
        page_id = lua_tointeger(L, -1);
        ret = ui_page_prev(page_id);

        lua_pushinteger(L, ret);
        return 1;
    } else {
        lua_db("page slide no init!\n");
        return 0;
    }
}


// 消息接管控制接口
static int ui_touchEventTakeover(lua_State *L)
{
    int takeover = 0;

    takeover = lua_toboolean(L, -1);
    if (takeover) {
        // true --> 接管
        lua_touch_event_takeover |= 0x01;	// 低四位置1
    } else {
        // false --> 不接管
        lua_touch_event_takeover &= 0xfe;	// 低四位置0
    }

    lua_touch_event_takeover |= 0x02;	// 设置标志
    lua_db("set touch event takeover : %s\n", ((lua_touch_event_takeover & 0x01) ? "true" : "false"));

    return 0;
}


// 消息接管默认设置
static int ui_touchEventDefault(lua_State *L)
{
    int takeover = 0;

    takeover = lua_toboolean(L, -1);
    if (takeover) {
        // true --> 接管
        lua_touch_event_takeover |= 0xf0;	// 高四位置1
    } else {
        // false --> 不接管
        lua_touch_event_takeover &= 0x0f;	// 高四位置0
    }

    lua_db("touch event takeover default: %s\n", ((lua_touch_event_takeover & 0xf0) ? "true" : "false"));

    return 0;
}

static int obj_getWatchNum(lua_State *L)
{
    int items_num;
    printf("get watch num.\n");

    extern int watch_get_items_num();
    items_num = watch_get_items_num();

    lua_pushinteger(L, items_num);

    return 1;
}


#define MAX_GRID_ITEM       (6)
static VIEWFILE view_file[MAX_GRID_ITEM] = {0};
static u32 open_flag = 0;
int sel_item;
char tmp_name[100];
char *sty_suffix = ".sty";
char *view_suffix = ".view";
char *watch_item;
u32 tmp_strlen;
u32 sty_strlen;

static int obj_openWatchPreview(lua_State *L)
{
    int items_num = 0;

    items_num = watch_get_items_num();
    items_num = (items_num >= MAX_GRID_ITEM) ? MAX_GRID_ITEM : items_num;

    sel_item = lua_tointeger(L, -1);

#if 1
    sty_strlen = strlen(sty_suffix);
    /* for (sel_item = 0; sel_item < items_num; sel_item++) { */
    watch_item = watch_get_item(sel_item);
    if (watch_item == NULL) {
        printf("prew get item err %d\n", sel_item);
        lua_pushinteger(L, 0);
        return 1;
    }

    tmp_strlen = strlen(watch_item);
    strcpy(tmp_name, watch_item);
    strcpy(&tmp_name[tmp_strlen - sty_strlen], view_suffix);
    tmp_name[tmp_strlen - sty_strlen + strlen(view_suffix)] = '\0';
    printf("prew name %s\n", tmp_name);
    view_file[sel_item].file = res_fopen(tmp_name, "r");
    if (!view_file[sel_item].file) {
        printf("open_prewfile fail %s\n", tmp_name);
        lua_pushinteger(L, 0);
        return 1;
    }
    if (ui_res_flash_info_get(&view_file[sel_item].info, tmp_name, "res")) {
        printf("get_prewfile tab fail %s\n", tmp_name);
        lua_pushinteger(L, 0);
        return 1;
    }

    printf("prew out\n");
    /* } */
#endif

    lua_pushinteger(L, 1);
    return 1;
}

static int obj_closeWatchPreview(lua_State *L)
{
    int items_num = 0;

    sel_item = lua_tointeger(L, -1);

#if 1
    /* for (sel_item = 0; sel_item < MAX_GRID_ITEM; sel_item++) { */
    printf("obj_closeWatchPreview %d\n", sel_item);
    if (view_file[sel_item].file) {
        res_fclose(view_file[sel_item].file);
        ui_res_flash_info_free(&view_file[sel_item].info, "res");
        view_file[sel_item].file = NULL;
    }
    /* } */
#endif
    printf("%s : %d", __FUNCTION__, __LINE__);

    return 0;
}

static int obj_selectWatchPreview(lua_State *L)
{
    int sel_item;
    VIEWFILE *file;

    sel_item = lua_tointeger(L, -1);
    lua_pop(L, 1);

    printf("sel_item : %d\n", sel_item);

    lua_newtable(L);
    if ((sel_item >= watch_get_items_num()) || (view_file[sel_item].file == NULL)) {
        /* printf("select preview err1 %x, %d\n", pic->elm.id, sel_item); */
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "ret");
        return 1;
    }

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "ret");

    file = &view_file[sel_item];
    lua_pushinteger(L, (u32)file);
    lua_setfield(L, -2, "view");

    printf("file : 0x%x\n", (u32)file);

    return 1;
}


static int obj_swichWatch(lua_State *L)
{
    int sel_item;
    int ret;

    sel_item = lua_tointeger(L, -1);

    printf("obj_switchwatch : %d\n", sel_item);
    mem_stats();

    extern char *watch_get_item(int style);
    extern int watch_version_juge(char *watch_item);
    ret = watch_version_juge(watch_get_item(sel_item));
    if (ret != 0) {
    }

    extern int watch_set_style(int style);
    ret = watch_set_style(sel_item);
    if (ret != true) {
        printf("watch_set_style err %d\n", sel_item);
    }

#if 0
    struct rect r;
    struct element *p, *n;
    struct element *elm = &grid->item[sel_item].elm;
    list_for_each_child_element_reverse(p, n, elm) {
        struct ui_text *text = (struct ui_text *)p;
        if (!p->css.invisible && ((ui_id2type(p->id)) == CTRL_TYPE_TEXT) && !strcmp(text->source, "expand")) { //判断是否字符控件以及是否可见
            ui_core_get_element_abs_rect(p, &r);
            if (in_rect(&r, &e->pos)) {
                printf("<<<<< __FUNCTION__ = %s __LINE__ =%d >>>>>\n", __FUNCTION__, __LINE__);
                ui_send_event(KEY_CHANGE_PAGE, BIT(31) | PAGE_37);
                return TRUE;
                break;
            }
        }
    }
#endif

    /* ui_send_event(KEY_CHANGE_PAGE, BIT(31) | DIAL_PAGE_0); */

    return 0;
}

static int obj_getWatchBgpNum(lua_State *L)
{
    int items_num = 0;

    extern u32 watch_bgp_get_nums();
    items_num = watch_bgp_get_nums();

    lua_pushinteger(L, items_num);

    return 1;
}


#define MAX_BGP_GRID_ITEM       (10)
static VIEWFILE bgp_file[MAX_BGP_GRID_ITEM] = {0};
char *watch_bgp_item;
char *bgp = NULL;
static int cur_grid_watch = 0;

static int obj_openWatchBgpPreview(lua_State *L)
{
    int items_num = 0;
    int sel_item;

    sel_item = lua_tointeger(L, -1);

    items_num = watch_bgp_get_nums();
    if (items_num == 0) {
        lua_pushinteger(L, 0);
        return 1;
    }
    items_num = (items_num >= MAX_BGP_GRID_ITEM) ? MAX_BGP_GRID_ITEM : items_num;
    if (sel_item >= items_num) {
        lua_pushinteger(L, 0);
        return 1;
    }


    /* for (sel_item = 0; sel_item < items_num; sel_item++) { */
    extern char *watch_bgp_get_item(u8 sel_item);
    watch_bgp_item = watch_bgp_get_item(sel_item);
    if (watch_bgp_item == NULL) {
        printf("prew get item err %d\n", sel_item);
        lua_pushinteger(L, 0);
        return 1;
    }
    bgp_file[sel_item].file = res_fopen(watch_bgp_item, "r");
    if (!bgp_file[sel_item].file) {
        printf("open_bgp_prewfile fail %s\n", watch_bgp_item);
        lua_pushinteger(L, 0);
        return 1;
    }
    if (ui_res_flash_info_get(&bgp_file[sel_item].info, watch_bgp_item, "res")) {
        printf("get_prewfile tab fail %s\n", watch_bgp_item);
        lua_pushinteger(L, 0);
        return 1;
    }
    printf("prew out\n");
    /* } */

    lua_pushinteger(L, 1);

    return 1;
}


static int obj_closeWatchBgpPreview(lua_State *L)
{
    int sel_item;

    sel_item = lua_tointeger(L, -1);
    if (sel_item >= MAX_BGP_GRID_ITEM) {
        lua_pushinteger(L, 0);
        return 1;
    }

    /* for (sel_item = 0; sel_item < MAX_BGP_GRID_ITEM; sel_item++) { */
    if (bgp_file[sel_item].file) {
        res_fclose(bgp_file[sel_item].file);
        ui_res_flash_info_free(&bgp_file[sel_item].info, "res");
        bgp_file[sel_item].file = NULL;
    }
    /* } */

    lua_pushinteger(L, 1);

    return 1;
}

static int obj_selectWatchBgpPreview(lua_State *L)
{
    int sel_item;
    int ret;
    VIEWFILE *file;

    sel_item = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_newtable(L);

    if (sel_item > 0) {
        bgp = watch_bgp_get_item(sel_item - 1);
        bgp = &bgp[strlen(RES_PATH)];
    }
    extern int watch_bgp_set_related(char *bgp, u8 cur_watch, u8 del);
    ret = watch_bgp_set_related(bgp, cur_grid_watch, 0);

    if (ret != 0) {
        printf("watch_set_style err %d\n", sel_item);
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "ret");
        return 1;
    }

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "ret");

    file = &bgp_file[sel_item];
    lua_pushinteger(L, (u32)file);
    lua_setfield(L, -2, "view");

    return 1;
}


//动态加载页面
static int ui_dynamicUILoading(lua_State *L)
{
    char *str = (char *)malloc(50);
    int page_id = 0;
    int layout_id = 0;
    int parent_id = 0;

    lua_getfield(L, -1, "tab_file");
    if (lua_isstring(L, -1)) {
        str = lua_tostring(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "page_id");
    if (lua_isnumber(L, -1)) {
        page_id = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "layout_id");
    if (lua_isnumber(L, -1)) {
        layout_id = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "parent_id");
    if (lua_isnumber(L, -1)) {
        parent_id = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    create_control_by_id(str, page_id, layout_id, parent_id);
    free(str);

    return 0;
}


//释放动态加载页面时的内存
static int ui_dynamicUIRealese(lua_State *L)
{
    int layout_id = 0;

    if (lua_isnumber(L, -1)) {
        layout_id = lua_tointeger(L, -1);
    }
    /* stackDump(L); */
    lua_pop(L, 1);

    if (ui_core_get_element_by_id(layout_id)) {
        delete_control_by_id(layout_id);
    }

    return 0;
}


const struct luaL_Reg watch_method[] = {
    // watch相关
    {"getWatchNum",         obj_getWatchNum},
    {"openWatchPreview",    obj_openWatchPreview},
    {"closeWatchPreview",   obj_closeWatchPreview},
    {"selectWatchPreview",  obj_selectWatchPreview},
    {"switchWatch",         obj_swichWatch},

    {"getWatchBgpNum",         obj_getWatchBgpNum},
    {"openWatchBgpPreview",    obj_openWatchBgpPreview},
    {"closeWatchBgpPreview",   obj_closeWatchBgpPreview},
    {"selectWatchBgpPreview",  obj_selectWatchBgpPreview},

    {NULL, NULL}
};

static int ui_getWatchHandler(lua_State *L)
{
    lua_newtable(L);
    luaL_setfuncs(L, watch_method, 0);
    return 1;
}

// ui模块方法列表
const struct luaL_Reg ui_method[] = {
    // 创建控件实例
    {"getComponentByName",	ui_getComponentByName},

    // 界面切换
    {"switchPageByName",	ui_switchPageByName},
    {"backToPreviousPage",	ui_backToPreviousPage},
    {"checkPreviousPage",	ui_checkPreviousPage},

    // touch 消息接管
    {"touchEventTakeover",	ui_touchEventTakeover},
    {"touchEventDefault",	ui_touchEventDefault},

    // 获取控件类型
    {"getControlType",	ui_getControlType},

    // 页面滑动
    {"pageSlideInit",	ui_pageSlideInit},
    {"pageSlideAdd",	ui_pageSlideAdd},
    {"pageSlideDel",	ui_pageSlideDel},
    {"pageSlideFree",	ui_pageSlideFree},
    {"pageSlideNext",	ui_pageSlideNext},
    {"pageSlidePrev",	ui_pageSlidePrev},

    // watch相关
    {"getWatchHandler",		ui_getWatchHandler},

    // 动态加载UI页面
    {"dynamicUILoading",	ui_dynamicUILoading},
    {"dynamicUIRealese",	ui_dynamicUIRealese},

    {NULL, NULL}
};


// 模块注册接口，需在lua_module.h中注册，并在解释器初始化时调用
LUALIB_API int luaopen_ui(lua_State *L)
{
    luaL_newlib(L, ui_method);
    return 1;
}

#endif /* #if (TCFG_LUA_ENABLE) */

