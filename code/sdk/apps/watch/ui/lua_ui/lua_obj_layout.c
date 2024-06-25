#include "ui/ui.h"
#include "ui/layout.h"
#include "ui_draw/ui_circle.h"
#include "ui_draw/ui_figure.h"
#include "asm/imd.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_obj.h"
#include "lua_gui.h"

#if (TCFG_LUA_ENABLE)


/* 自定义绘图类型 */
enum {
    CUSTOM_DRAW_SPOT,	/* 画点 */
    CUSTOM_DRAW_LINE,	/* 画线 */
    CUSTOM_DRAW_RECT,	/* 画矩形 */
    CUSTOM_DRAW_CIRCLE,	/* 画圆 */
    CUSTOM_DRAW_CLEAR	/* 清空 */
};


/* 自定义绘图链表头 */
static struct list_head custom_head = LIST_HEAD_INIT(custom_head);

static struct custom_draw {
    u8 wait;	/* 是否有待绘制内容 */
    u8 type;	/* 待绘制类型 */
    u8 size;	/* 线条大小 */
    u32 id;		/* 待绘制的控件ID */
    u16 sx;		/* 起始坐标 */
    u16 sy;
    u16 ex_w;	/* 结束坐标或矩形的宽高 */
    u16 ey_h;
    u16 r;		/* 圆的半径 */
    u32 color;	/* 指定的颜色 */
};

static struct custom_list {
    struct list_head entry;		/* 链表管理 */
    struct custom_draw draw;	/* 绘图信息 */
};


// 打印链表内容
static void print_list()
{
    struct custom_list *cld = NULL;

    printf("\n~~~~~~~~ custom draw list begin ~~~~~~~~~~~\n");
    list_for_each_entry(cld, &custom_head, entry) {
        printf("id:0x%x, type:%d, wait:%d\n", cld->draw.id, cld->draw.type, cld->draw.wait);
        printf("(%d, %d, %d, %d), r=%d, size=%d, color:0x%x\n", cld->draw.sx, cld->draw.sy, cld->draw.ex_w, cld->draw.ey_h, cld->draw.r, cld->draw.size, cld->draw.color);
    }
    printf("~~~~~~~~ custom draw list end ~~~~~~~~~~~\n\n");
}


// lua自定义绘图：链表记录绘图数据，刷新时一次性刷出
int custom_draw_callback(void *_layout, void *arg)
{
    struct layout *lay = (struct layout *)_layout;
    struct element *elm = (struct element *)&lay->elm;
    struct draw_context *dc = (struct draw_context *)arg;
    struct custom_list *cld = NULL;

    /* printf(">>>>>>> id:0x%x\n", elm->id); */
    /* ui_draw_ring(dc, 120, 120, 50, 20, 0, 180, 0x07e0); */
    list_for_each_entry(cld, &custom_head, entry) {
        if (cld->draw.wait && (elm->id == cld->draw.id)) {
            switch (cld->draw.type) {
            case CUSTOM_DRAW_SPOT:
                ui_fill_rect(dc, cld->draw.sx, cld->draw.sy, cld->draw.size, cld->draw.size, cld->draw.color);
                break;
            case CUSTOM_DRAW_LINE:
                ui_draw_line(dc, cld->draw.sx, cld->draw.sy, cld->draw.ex_w, cld->draw.ey_h, cld->draw.color);
                break;
            case CUSTOM_DRAW_RECT:
                ui_draw_rect(dc, cld->draw.sx, cld->draw.sy, cld->draw.ex_w, cld->draw.ey_h, cld->draw.color);
                break;
            case CUSTOM_DRAW_CIRCLE:
                ui_draw_ring(dc, cld->draw.sx, cld->draw.sy, cld->draw.r + cld->draw.size, cld->draw.r, \
                             cld->draw.ex_w, cld->draw.ey_h, cld->draw.color, 100);
                /* ui_draw_circle(dc, cld->draw.sx, cld->draw.sy, \ */
                /* cld->draw.r, cld->draw.r + cld->draw.size, 0, 360, cld->draw.color, 75); */
                break;
            default:
                printf("Error, unknow custom draw type!\n");
                break;
            }
        }
    }
    return 0;
}


// RGB颜色表，可以根据名称找到色值
static struct RGBColor {
    char *name;
    u32 value;
};


static struct RGBColor RGBColor_table[] = {
#if 0
    // RGB888
    {"red",		0xff0000},
    {"green",	0x00ff00},
    {"blue",	0x0000ff},
    {"black",	0x000000},
    {"white",	0xffffff},
#endif

#if 1
    // RGB565
    {"black",	0x0000},	// 黑色
    {"dgray",	0x7bef},	// 深灰
    {"gray",	0x8410},	// 灰色
    {"lgray",	0xc618},	// 灰白
    {"white",	0xffff},	// 白色

    {"navy",	0x000f},	// 深蓝
    {"blue",	0x001f},	// 蓝色

    {"dgreen",	0x03e0},	// 深绿
    {"green",	0x07e0},	// 绿色

    {"maroon",	0x7800},	// 深红
    {"red",		0xf800},	// 红色
    {"magenta",	0xf81f},	// 品红

    {"dcyan",	0x03ef},	// 深青
    {"cyan",	0x07ff},	// 青色
    {"purple",	0x780f},	// 紫色
    {"olive",	0x7be0},	// 橄榄绿
    {"yellow",	0xffe0},	// 黄色
#endif
};


// 根据颜色名称获取色值
static u32 get_color_value_by_name(char *name)
{
    int n = sizeof(RGBColor_table) / sizeof(RGBColor_table[0]);

    for (int i = 0; i < n; i++) {
        if (!strcmp(name, RGBColor_table[i].name)) {
            return RGBColor_table[i].value;
        }
    }

    return 0;
}


// 从颜色表中获取颜色值
static int get_color_from_tab(u32 page_id, u32 rgb)
{
    int index = 0;
    u32 *osd_tab = NULL;
    struct fb_map_user *map = NULL;

    osd_tab = (u32 *)malloc(256 * sizeof(u32));
    if (osd_tab == NULL) {
        log_e(" malloc osd tab err!!\n");
        return -1;
    }

    //加载颜色表
    load_pallet_table((page_id & 0xff), osd_tab);

    for (index = 0; index < 256; index++) {
        if (rgb == osd_tab[index]) {
            break;
        }
    }
    /* put_buf(osd_tab, 256*4); */

    if (osd_tab) {
        free(osd_tab);
    }

    return index;
}

// 自定义绘点
// 参数：table = {x, y, size, color}
// (table)
// (table, redraw)
// 返回：无
static int obj_drawSpot(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int x, y, size;				// 坐标及大小
    int color = 0;
    char *color_name = NULL;	// 画线颜色
    struct rect r = {0};
    struct element *elm = NULL;
    struct element_css *css;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    lua_getfield(L, -1, "x");
    if (lua_isnumber(L, -1)) {
        x = lua_tointeger(L, -1);
        if (x < 0) {
            /* 小于0的时候设置为0 */
            x = 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "y");
    if (lua_isnumber(L, -1)) {
        y = lua_tointeger(L, -1);
        if (y < 0) {
            /* 小于0的时候设置为0 */
            y = 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "size");
    if (lua_isnumber(L, -1)) {
        size = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "color");
    int type = lua_type(L, -1);
    if (type == LUA_TSTRING) {
        /* 传的颜色名称 */
        color_name = lua_tostring(L, -1);
        color = get_color_value_by_name(color_name);
    } else if (type == LUA_TNUMBER) {
        /* 传的颜色值 */
        color = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    id = lua_get_self_id(L);
    lua_db("%s: id:0x%x, x:%d, y:%d, size:%d, color:0x%x\n", __FUNCTION__, id, x, y, size, color);

    /* 获取控件坐标 */
    elm = ui_core_get_element_by_id(id);
    css = &elm->css;
    ui_core_get_element_abs_rect(elm->parent, &r);
    ui_css_get_abs_rect(css, &r);

#if 0
    /* 调整绘制坐标，让它落到控件内 */
    if ((x > (r.left + r.width)) || ((x + r.left) > (r.left + r.width))) {
        /* 如果X坐标在控件外，或者偏移后x坐标落到控件外，则x设置为控件最边 */
        x = r.left + r.width;
    } else {
        x = x + r.left;
    }

    if ((y > (r.top + r.height)) || ((y + r.top) > (r.top + r.height))) {
        y = r.top + r.height;
    } else {
        y = y + r.top;
    }
#endif

    struct custom_list *cld = malloc(sizeof(struct custom_list));
    cld->draw.wait = true;
    cld->draw.id   = id;
    cld->draw.type = CUSTOM_DRAW_SPOT;
    cld->draw.sx   = x;
    cld->draw.sy   = y;
    cld->draw.size = size;
    cld->draw.color = color;
    list_add_tail(&(cld->entry), &custom_head);	/* 追加到链表 */
    print_list();

    if (redraw) {
        ui_core_redraw(elm->parent);
    }
    return 0;
}


// 自定义绘线段
// 参数：table = {x1, y1, x2, y2, size, color}
// (table)
// (table, redraw)
// 返回：无
static int obj_drawLine(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int x1, y1, x2, y2, size;	// 线条参数
    int color = 0;
    char *color_name = NULL;	// 画线颜色
    struct rect r = {0};
    struct element *elm = NULL;
    struct element_css *css;

    int n = lua_gettop(L);
    if (n == 3) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    lua_getfield(L, -1, "x1");
    if (lua_isnumber(L, -1)) {
        x1 = lua_tointeger(L, -1);
        if (x1 < 0) {
            x1 = 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "y1");
    if (lua_isnumber(L, -1)) {
        y1 = lua_tointeger(L, -1);
        if (y1 < 0) {
            y1 = 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "x2");
    if (lua_isnumber(L, -1)) {
        x2 = lua_tointeger(L, -1);
        if (x2 < 0) {
            x2 = 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "y2");
    if (lua_isnumber(L, -1)) {
        y2 = lua_tointeger(L, -1);
        if (y2 < 0) {
            y2 = 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "size");
    if (lua_isnumber(L, -1)) {
        size = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "color");
    int type = lua_type(L, -1);
    if (type == LUA_TSTRING) {
        color_name = lua_tostring(L, -1);
        color = get_color_value_by_name(color_name);
    } else if (type == LUA_TNUMBER) {
        color = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    id = lua_get_self_id(L);
    lua_db("%s, id:0x%x, start (%d, %d), end (%d, %d), size:%d, color:0x%x\n", __FUNCTION__, id, x1, y1, x2, y2, size, color);

    elm = ui_core_get_element_by_id(id);
    css = &elm->css;
    ui_core_get_element_abs_rect(elm->parent, &r);
    ui_css_get_abs_rect(css, &r);

#if 0
    /* 调整绘制坐标，让它落到控件内 */
    if ((x1 > (r.left + r.width)) || ((x1 + r.left) > (r.left + r.width))) {
        x1 = r.left + r.width;
    } else {
        x1 = x1 + r.left;
    }

    if ((y1 > (r.top + r.height)) || ((y1 + r.top) > (r.top + r.height))) {
        y1 = r.top + r.height;
    } else {
        y1 = y1 + r.top;
    }

    if ((x2 > (r.left + r.width)) || ((x2 + r.left) > (r.left + r.width))) {
        x2 = r.left + r.width;
    } else {
        x2 = x2 + r.left;
    }

    if ((y2 > (r.top + r.height)) || ((y2 + r.top) > (r.top + r.height))) {
        y2 = r.top + r.height;
    } else {
        y2 = y2 + r.top;
    }
#endif

    struct custom_list *cld = malloc(sizeof(struct custom_list));
    cld->draw.wait = true;
    cld->draw.id   = id;
    cld->draw.type = CUSTOM_DRAW_LINE;
    cld->draw.sx   = x1;
    cld->draw.sy   = y1;
    cld->draw.ex_w = x2;
    cld->draw.ey_h = y2;
    cld->draw.size = size;
    cld->draw.color = color;
    list_add_tail(&(cld->entry), &custom_head);
    print_list();

    if (redraw) {
        ui_core_redraw(elm->parent);
    }
    return 0;
}


// 自定义绘制矩形
// 参数：table = {x, y, width, height, size, color}
// (table)
// (table, redraw)
// 返回：无
static int obj_drawRect(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int x, y, w, h, size;		// 线条参数
    int color = 0;
    char *color_name = NULL;	// 画线颜色
    struct element *elm = NULL;
    struct rect r = {0};
    struct element_css *css;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    lua_getfield(L, -1, "x");
    if (lua_isnumber(L, -1)) {
        x = lua_tointeger(L, -1);
        if (x < 0) {
            x = 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "y");
    if (lua_isnumber(L, -1)) {
        y = lua_tointeger(L, -1);
        if (y = 0) {
            y = 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "width");
    if (lua_isnumber(L, -1)) {
        w = lua_tointeger(L, -1);
        if (w < 0) {
            printf("ERROR: width < 0!\n");
            return 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "height");
    if (lua_isnumber(L, -1)) {
        h = lua_tointeger(L, -1);
        if (h < 0) {
            printf("ERROR: height < 0!");
            return 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "size");
    if (lua_isnumber(L, -1)) {
        size = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "color");
    int type = lua_type(L, -1);
    if (type == LUA_TSTRING) {
        color_name = lua_tostring(L, -1);
        color = get_color_value_by_name(color_name);
    } else if (type == LUA_TNUMBER) {
        color = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    id = lua_get_self_id(L);
    lua_db("%s, id:0x%x, x:%d, y:%d, w:%d, h:%d, size:%d, color_name:%s\n", __FUNCTION__, id, x, y, w, h, size, color_name);

    elm = ui_core_get_element_by_id(id);
    css = &elm->css;
    ui_core_get_element_abs_rect(elm->parent, &r);
    ui_css_get_abs_rect(css, &r);

#if 0
    /* 调整绘制坐标，让它落到控件内 */
    if ((x > (r.left + r.width)) || ((x + r.left) > (r.left + r.width))) {
        /* 如果X坐标在控件外，或者偏移后x坐标落到控件外，则x设置为控件最边 */
        x = r.left + r.width;
    } else {
        x = x + r.left;
    }

    if ((y > (r.top + r.height)) || ((y + r.top) > (r.top + r.height))) {
        y = r.top + r.height;
    } else {
        y = y + r.top;
    }

    if ((w + x) > (r.left + r.width - x)) {
        w = r.width - x;
    }

    if ((h + y) > (r.top + r.height - y)) {
        h = r.height - y;
    }
#endif

    struct custom_list *cld = malloc(sizeof(struct custom_list));
    cld->draw.wait = true;
    cld->draw.id   = id;
    cld->draw.type = CUSTOM_DRAW_RECT;
    cld->draw.sx   = x;
    cld->draw.sy   = y;
    cld->draw.ex_w = w;
    cld->draw.ey_h = h;
    cld->draw.size = size;
    cld->draw.color = color;
    list_add_tail(&(cld->entry), &custom_head);
    print_list();

    if (redraw) {
        ui_core_redraw(elm->parent);
    }
    return 0;
}


// 自定义画圆
// 参数：table = {x, y, r, size, color}
// 参数：table = {x, y, r, size, color, angle_begin, angle_end}
// (table)
// (table, redraw)
// 返回：无
static int obj_drawCircle(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int x, y, r, size;			// 坐标及大小
    int color = 0;
    int angle_begin;
    int angle_end;
    char *color_name = NULL;	// 画线颜色
    struct element *elm = NULL;
    struct rect rect = {0};
    struct element_css *css;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    lua_getfield(L, -1, "x");
    if (lua_isnumber(L, -1)) {
        x = lua_tointeger(L, -1);
        if (x < 0) {
            x = 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "y");
    if (lua_isnumber(L, -1)) {
        y = lua_tointeger(L, -1);
        if (y < 0) {
            y = 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "r");
    if (lua_isnumber(L, -1)) {
        r = lua_tointeger(L, -1);
        if (r < 0) {
            printf("ERROR: r < 0!");
            return 0;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "size");
    if (lua_isnumber(L, -1)) {
        size = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    // 获取颜色
    lua_getfield(L, -1, "color");
    int type = lua_type(L, -1);
    if (type == LUA_TSTRING) {
        color_name = lua_tostring(L, -1);
        color = get_color_value_by_name(color_name);
    } else if (type == LUA_TNUMBER) {
        color = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    // 获取起始角度
    lua_getfield(L, -1, "angle_begin");
    type = lua_type(L, -1);
    if (type == LUA_TNIL) {
        angle_begin = 0;	/* 默认0°开始 */
    } else {
        angle_begin = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    // 获取结束角度
    lua_getfield(L, -1, "angle_end");
    type = lua_type(L, -1);
    if (type == LUA_TNIL) {
        angle_end = 360;	/* 默认360°结束 */
    } else {
        angle_end = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    id = lua_get_self_id(L);
    lua_db("%s: id:0x%x, x:%d, y:%d, r:%d, size:%d, color:%s, angle_begin:%d, angle_end:%d\n", \
           __FUNCTION__, id, x, y, r, size, color_name, angle_begin, angle_end);
    elm = ui_core_get_element_by_id(id);
    css = &elm->css;
    ui_core_get_element_abs_rect(elm->parent, &rect);
    ui_css_get_abs_rect(css, &rect);

#if 0
    /* 调整绘制坐标，让它落到控件内 */
    if ((x > (rect.left + rect.width)) || ((x + rect.left) > (rect.left + rect.width))) {
        /* 如果X坐标在控件外，或者偏移后x坐标落到控件外，则x设置为控件最边 */
        x = rect.left + rect.width;
    } else {
        x = x + rect.left;
    }

    if ((y > (rect.top + rect.height)) || ((y + rect.top) > (rect.top + rect.height))) {
        y = rect.top + rect.height;
    } else {
        y = y + rect.top;
    }
#endif

    struct custom_list *cld = malloc(sizeof(struct custom_list));
    cld->draw.wait = true;
    cld->draw.id   = id;
    cld->draw.type = CUSTOM_DRAW_CIRCLE;
    cld->draw.sx   = x;
    cld->draw.sy   = y;
    cld->draw.ex_w = angle_begin;
    cld->draw.ey_h = angle_end;
    cld->draw.r    = r;
    cld->draw.size = size;
    cld->draw.color = color;
    list_add_tail(&(cld->entry), &custom_head);
    print_list();

    if (redraw) {
        elm = ui_core_get_element_by_id(id);
        ui_core_redraw(elm->parent);
    }
    return 0;
}


// 清空链表中该控件自定义绘制内容
// 参数：redraw
// 返回：无
static int obj_clearDraw(lua_State *L)
{
    int id = 0;
    int redraw = true;
    struct custom_list *cld = NULL;
    struct list_head *pos, *note;

    int n = lua_gettop(L);
    if (2 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    print_list();

    id = lua_get_self_id(L);
    list_for_each_safe(pos, note, &custom_head) {
        cld = list_entry(pos, struct custom_list, entry);
        /* 删除当前id下链表内自定义绘图信息 */
        if (cld->draw.id == id) {
            list_del(&(cld->entry));
            free(cld);
        }
    }

    if (redraw) {
        struct element *elm = NULL;
        elm = ui_core_get_element_by_id(id);
        /* ui_remove_backcolor(elm);//移除控件背景颜色 */
        ui_core_redraw(elm->parent);
        /* ui_core_redraw(elm); */

        /* ui_hide(id); */
        /* ui_show(id); */
    }
    print_list();

    /* ui_remove_backcolor(elm);//移除控件背景颜色 */
    /* ui_remove_backimage(elm);//移除控件背景图像 */
    /* ui_remove_border(elm);//移除控件边界 */

    return 0;
}


// 设置成可移动的
// 参数：
// () --> 省略，设置成可移动
// (movable) --> true 可移动，false 不可移动
static int obj_setMovable(lua_State *L)
{
    int movable = true;
    int id = 0;
    struct layout *layout = NULL;

    int n = lua_gettop(L);
    if (2 == n) {
        movable = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    id = lua_get_self_id(L);
    layout = (struct layout *)ui_core_get_element_by_id(id);
    lua_db("%s, id:%d, movable:%d\n", __FUNCTION__, id, movable);
    layout->movable = movable;

    return 0;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~ 满天星菜单相关 ~~~~~~~~~~~~~~~~~~~~~~~~~*/
struct icon_list {
    struct list_head entry;
    int index;
    struct element *elm;

    int left;
    int top;
    int right;
    int bottom;

    int css_left;
    int css_top;
    int css_width;
    int css_height;

    int width;
    int height;
    int cx;
    int cy;
    int size;

    struct icon_list *icon_left_top;
    struct icon_list *icon_right_top;
    struct icon_list *icon_left;
    struct icon_list *icon_right;
    struct icon_list *icon_left_bottom;
    struct icon_list *icon_right_bottom;

};

struct scene_info {
    u32 saved;

    //layout info
    int css_left;
    int css_top;
    int css_width;
    int css_height;

    //icon_info
    int center_index;
};


struct icon_size_info {
    u16 icon_w;
    u16 icon_h;
};

struct variable_info {
    u16 init;

    //屏幕的宽和高
    u16 lcd_w;
    u16 lcd_h;

    //两点间距离的经验值，用于判断中心图标和某个图标间的距离属于哪个级别，
    //从而显示哪张尺寸的图片，这个需要调试来获取！！！
    u16 ref_distance;

    //图标的图片数和图标图片的实际宽高，用于分多级来显示
    //目前是4张图片每个图标, 每个图标尺寸一样的
    u16 icon_level;
    struct icon_size_info *ics;

};


LIST_HEAD(icon_root);
#define abs(x)  ((x)>0?(x):-(x) )
#define EFFECT_DEBUG 0

static struct scene_info slinfo = {0};
static struct variable_info svi = {0};

static int icon_step = 0;
/* static float ratio_tab[5] = {0.4, 0.5, 0.86, 0.91, 1}; */
static float ratio_tab[5] = {0.4, 0.5, 0.86, 0.91, 1};


static int startlight_svi_init()
{
    if (svi.init) {
        return 0;
    }

    svi.lcd_w = get_lcd_width_from_imd();
    svi.lcd_h = get_lcd_height_from_imd();

    svi.ref_distance = 3500 * 4;

    svi.icon_level = 5;
    svi.ics = malloc(svi.icon_level * sizeof(struct icon_size_info));
    if (svi.ics == NULL) {
        printf("err malloc ics \n");
        return -1;
    }

    svi.ics[0].icon_w = 45 * 2;
    svi.ics[0].icon_h = 45 * 2;

    svi.ics[1].icon_w = 41 * 2;
    svi.ics[1].icon_h = 41 * 2;

    svi.ics[2].icon_w = 39 * 2;
    svi.ics[2].icon_h = 39 * 2;

    svi.ics[3].icon_w = 22 * 2;
    svi.ics[3].icon_h = 22 * 2;

    svi.ics[4].icon_w = 16 * 2;
    svi.ics[4].icon_h = 16 * 2;
    svi.init = 1;

    return 0;
}



static int startlight_scene_save(struct layout *layout, struct list_head *root)
{
    struct icon_list *icon_p;

    slinfo.saved = 0;

    list_for_each_entry(icon_p, root, entry) {
        if (icon_p->size == 0) {
            slinfo.center_index = icon_p->index;
            slinfo.saved = 1;
        }
    }

    if (slinfo.saved == 0) {
        printf("scene_info save fail\n");
        return -1;
    }
    slinfo.css_left = layout->elm.css.left;
    slinfo.css_top = layout->elm.css.top;
    slinfo.css_width = layout->elm.css.width;
    slinfo.css_height = layout->elm.css.height;

    return 0;
}

static int startlight_scene_recover(struct layout *layout, struct list_head *root)
{
    struct icon_list *icon_p;

    if (slinfo.saved == 0) {
        return -1;
    }

    list_for_each_entry(icon_p, root, entry) {
        if (icon_p->index == slinfo.center_index) {
            icon_p->size = 0;
        }
    }

    layout->elm.css.left = slinfo.css_left;
    layout->elm.css.top = slinfo.css_top;
    layout->elm.css.width = slinfo.css_width;
    layout->elm.css.height = slinfo.css_height;

    slinfo.saved = 0;
    return 0;
}


static int icon_is_limit(struct list_head *root)
{
    struct icon_list *icon_p;
    struct rect rect;
    struct element elm;

    list_for_each_entry(icon_p, root, entry) {

        if (icon_p->size == 0) {
            memcpy(&elm, icon_p->elm, sizeof(struct element));

            elm.css.left = icon_p->css_left;
            elm.css.top = icon_p->css_top;
            elm.css.width = icon_p->css_width;
            elm.css.height = icon_p->css_height;

            ui_core_get_element_abs_rect(&elm, &rect);
            if ((rect.left < 60) ||
                ((rect.left + rect.width) > (svi.lcd_w - 60)) ||
                (rect.top < 60) ||
                ((rect.top + rect.height) > (svi.lcd_h - 60))) {
                return 1;
            }
        }
    }

    return 0;
}



int icon_change(struct list_head *root, struct element *elm, int draw)
{
    struct element *p;
    struct icon_list *icon_p;
    struct icon_list *n;
    struct rect rect;

    int center_x;
    int center_y;
    int center_change;
    int center_index;

    int s_icon, p_icon;
    struct element elm_tmp;
    struct element *elm_p;
    int cx, cy;
    int xoffset, yoffset;
    float ratio;
    int limit = svi.icon_level - 1;

    center_index = -1;
    list_for_each_entry(icon_p, root, entry) {
        if (icon_p->size) {
            icon_p->size = -1;
        } else {
            icon_p->size = -1;
            center_index = icon_p->index;
        }
    }

    center_change = false;
    list_for_each_entry(icon_p, root, entry) {
        /* struct element elm; */
        /* memcpy(&elm, icon_p->elm, sizeof(struct element)); */

        elm_tmp.parent = icon_p->elm->parent;
        elm_tmp.css.left = icon_p->css_left;
        elm_tmp.css.top = icon_p->css_top;
        elm_tmp.css.width = icon_p->css_width;
        elm_tmp.css.height = icon_p->css_height;

        ui_core_get_element_abs_rect(&elm_tmp, &rect);
        if ((rect.left < (svi.lcd_w / 2)) &&
            ((rect.left + rect.width) > (svi.lcd_w / 2)) &&
            (rect.top < (svi.lcd_h / 2)) &&
            ((rect.top + rect.height) > (svi.lcd_h / 2))) {
            /* icon_p->size = 0;  */
            center_change = true;
            break;
        }
    }

    if (!center_change) {
        if (center_index != (-1)) {
            list_for_each_entry(icon_p, root, entry) {
                if (icon_p->index == center_index) {
                    icon_p->size = 0;//记录中心图标的圆心位置 */
                    center_x = icon_p->cx;
                    center_y = icon_p->cy;
                    break;
                }
            }
        } else {
            lua_db("not find center!\n");
            return -1;
        }
    } else {
        icon_p->size = 0; //记录中心图标的圆心位置 */
        center_x = icon_p->cx;
        center_y = icon_p->cy;
    }
    /* printf("zero index %x\n", icon_p->index); */

    list_for_each_entry(icon_p, root, entry) {
        int distance = abs(icon_p->cx - center_x) * abs(icon_p->cx - center_x) + \
                       abs(icon_p->cy - center_y) * abs(icon_p->cy - center_y);

        if (icon_p->size == -1) {
            if (distance < svi.ref_distance) {
                icon_p->size = 1;
            } else {
                icon_p->size = 2;
            }

            struct element elm;
            elm.parent = icon_p->elm->parent;
            /* memcpy(&elm, icon_p->elm, sizeof(struct element)); */

            elm.css.left = icon_p->css_left;
            elm.css.top = icon_p->css_top;
            elm.css.width = icon_p->css_width;
            elm.css.height = icon_p->css_height;
            ui_core_get_element_abs_rect(&elm, &rect);

            if ((rect.left < 0) || (rect.top < 0) || ((rect.left + rect.width) > svi.lcd_w) || \
                ((rect.top + rect.height) > svi.lcd_h)) {

                xoffset = 0;
                yoffset = 0;
                icon_p->size = 3;

                cy = 10000;
                cx = 10000;
                if (rect.left < 0) {
                    cx = rect.left + rect.width;
                    xoffset = (abs(rect.left) / 2) * 10000 / svi.lcd_w;
                    if (cx > svi.ics[2].icon_w) {
                        cx = svi.ics[2].icon_w;
                    }
                }
                if (rect.top < 0) {
                    cy = rect.top + rect.height;
                    yoffset = (abs(rect.top) / 2) * 10000 / svi.lcd_h;
                    if (cy > svi.ics[2].icon_h) {
                        cy = svi.ics[2].icon_h;
                    }
                }
                if ((rect.left + rect.width) > svi.lcd_w) {
                    cx = svi.lcd_w - rect.left;
                    xoffset = -(abs(rect.width - cx) / 2) * 10000 / svi.lcd_w;
                    if (cx > svi.ics[2].icon_w) {
                        cx = svi.ics[2].icon_w;
                    }
                }
                if ((rect.top + rect.height) > svi.lcd_h) {
                    cy = svi.lcd_h - rect.top;
                    yoffset = -(abs(rect.height - cy) / 2) * 10000 / svi.lcd_h;
                    if (cy > svi.ics[2].icon_h) {
                        cy = svi.ics[2].icon_h;
                    }
                }

                if ((cx != 10000) && (cy != 10000)) {
                    cx = (cx > cy) ? cy : cx;
                } else {
                    if (cx == 10000) {
                        cx = cy;
                    } else {
                        cy = cx;
                    }
                }

                if (cx <= 0) {
                    xoffset = 0;
                    yoffset = 0;
                    cx = 0;
                    cy = 0;
                }
                if (icon_step != 0) {
                    s_icon = limit;
                    if (icon_step < 0) {
                        s_icon = s_icon - icon_step;
                        s_icon = (s_icon > limit) ? limit : s_icon;
                    } else {
                        s_icon = s_icon - icon_step;
                        s_icon = (s_icon < 0) ? 0 : s_icon;
                    }
                    cx = svi.ics[s_icon].icon_w;
                    cy = svi.ics[s_icon].icon_h;
                    xoffset = 0;
                    yoffset = 0;
                }

                /* if (((rect.left < 0) && ((rect.left + 10) > 0)) || ((rect.top < 0) && ((rect.top + 22) > 0)) || (((rect.left + rect.width) > svi.lcd_w) && (rect.left < 203)) || (((rect.top + rect.height) > svi.lcd_h) && (rect.top < 213))) { */
                /* icon_p->size = 3; */
                /* } else { */
                /* icon_p->size = 4; */
                /* } */

            }

        }

        /* printf("%d, %d, %d, %d, %d, %d\n", icon_p->size, size0, size1, size2, size3, distance); */

        if (icon_p->size == 0) {
            struct element *elm = icon_p->elm;
            elm->css.left = icon_p->css_left;
            elm->css.top = icon_p->css_top;
            elm->css.width = icon_p->css_width;
            elm->css.height = icon_p->css_height;

            s_icon = 0;
            p_icon = limit;
            if (icon_step < 0) {
                s_icon = s_icon - icon_step;
                s_icon = (s_icon > limit) ? limit : s_icon;
                p_icon = p_icon + icon_step;
                p_icon = (p_icon < 0) ? 0 : p_icon;

                elm->css.width = svi.ics[s_icon].icon_w * 10000 / svi.lcd_w;
                elm->css.height = svi.ics[s_icon].icon_h * 10000 / svi.lcd_h;
            } else if (icon_step > 0) {

            } else {

            }

            elm->css.left = icon_p->css_left + (icon_p->css_width - elm->css.width) / 2;
            elm->css.top = icon_p->css_top + (icon_p->css_height - elm->css.height) / 2;

            elm->css.ratio.en = 1;
            elm->css.ratio.ratio_w = ratio_tab[p_icon];
            elm->css.ratio.ratio_h = ratio_tab[p_icon];
        } else if (icon_p->size == 1) {
            struct element *elm = icon_p->elm;

            s_icon = 1;
            p_icon = 3;
            if (icon_step < 0) {
                s_icon = s_icon - icon_step;
                s_icon = (s_icon > limit) ? limit : s_icon;
                p_icon = p_icon + icon_step;
                p_icon = (p_icon < 0) ? 0 : p_icon;

            } else if (icon_step > 0) {
                s_icon = s_icon - icon_step;
                s_icon = (s_icon < 0) ? 0 : s_icon;
                p_icon = p_icon + icon_step;
                p_icon = (p_icon > limit) ? limit : p_icon;
            } else {

            }

            elm->css.width = svi.ics[s_icon].icon_w * 10000 / svi.lcd_w;
            elm->css.height = svi.ics[s_icon].icon_h * 10000 / svi.lcd_h;

            elm->css.left = icon_p->css_left + (icon_p->css_width - elm->css.width) / 2;
            elm->css.top = icon_p->css_top + (icon_p->css_height - elm->css.height) / 2;

            elm->css.ratio.en = 1;
            elm->css.ratio.ratio_w = ratio_tab[p_icon];
            elm->css.ratio.ratio_h = ratio_tab[p_icon];
        } else if (icon_p->size == 2) {
            struct element *elm = icon_p->elm;

            s_icon = 2;
            p_icon = 2;
            if (icon_step < 0) {
                s_icon = s_icon - icon_step;
                s_icon = (s_icon > limit) ? limit : s_icon;
                p_icon = p_icon + icon_step;
                p_icon = (p_icon < 0) ? 0 : p_icon;

            } else if (icon_step > 0) {
                s_icon = s_icon - icon_step;
                s_icon = (s_icon < 0) ? 0 : s_icon;
                p_icon = p_icon + icon_step;
                p_icon = (p_icon > limit) ? limit : p_icon;
            } else {

            }

            elm->css.width = svi.ics[s_icon].icon_w * 10000 / svi.lcd_w;
            elm->css.height = svi.ics[s_icon].icon_h * 10000 / svi.lcd_h;

            elm->css.left = icon_p->css_left + (icon_p->css_width - elm->css.width) / 2;
            elm->css.top = icon_p->css_top + (icon_p->css_height - elm->css.height) / 2;

            elm->css.ratio.en = 1;
            elm->css.ratio.ratio_w = ratio_tab[p_icon];
            elm->css.ratio.ratio_h = ratio_tab[p_icon];
        } else if (icon_p->size >= 3) {
            struct element *elm = icon_p->elm;

            elm->css.width = cx * 10000 / svi.lcd_w;
            elm->css.height = cy * 10000 / svi.lcd_h;

            elm->css.left = icon_p->css_left + (icon_p->css_width - elm->css.width) / 2 + xoffset;
            elm->css.top = icon_p->css_top + (icon_p->css_height - elm->css.height) / 2 + yoffset;

            ratio = (float)cx / rect.width;
            elm->css.ratio.en = 1;
            elm->css.ratio.ratio_w = ratio;
            elm->css.ratio.ratio_h = ratio;
        }

#if 0
        else if (icon_p->size == 3) {
            struct element *elm = icon_p->elm;

            s_icon = 3;
            p_icon = 1;
            if (icon_step < 0) {
                s_icon = s_icon - icon_step;
                s_icon = (s_icon > 4) ? 4 : s_icon;
                p_icon = p_icon + icon_step;
                p_icon = (p_icon < 0) ? 0 : p_icon;

            } else if (icon_step > 0) {
                s_icon = s_icon - icon_step;
                s_icon = (s_icon < 0) ? 0 : s_icon;
                p_icon = p_icon + icon_step;
                p_icon = (p_icon > 4) ? 4 : p_icon;
            } else {

            }
            /* printf("s_icon %d, p_icon %d\n", s_icon, p_icon); */

            elm->css.width = svi.ics[s_icon].icon_w * 10000 / svi.lcd_w;
            elm->css.height = svi.ics[s_icon].icon_h * 10000 / svi.lcd_h;

            elm->css.left = icon_p->css_left + (icon_p->css_width - elm->css.width) / 2 + xoffset;

            elm->css.top = icon_p->css_top + (icon_p->css_height - elm->css.height) / 2 + yoffset;

            /* ui_pic_set_image_index((struct ui_pic *)elm, p_icon); */
            /* ui_pic_set_image_index((struct ui_pic *)elm, 4); */
            elm->css.ratio.en = 1;
            elm->css.ratio.ratio_w = ratio_tab[p_icon];
            elm->css.ratio.ratio_h = ratio_tab[p_icon];
        } else if (icon_p->size == 4) {
            struct element *elm = icon_p->elm;

            s_icon = 4;
            p_icon = 0;
            if (icon_step < 0) {

            } else if (icon_step > 0) {
                s_icon = s_icon - icon_step;
                s_icon = (s_icon < 0) ? 0 : s_icon;
                p_icon = p_icon + icon_step;
                p_icon = (p_icon > 4) ? 4 : p_icon;
            } else {

            }
            /* printf("s_icon %d, p_icon %d\n", s_icon, p_icon); */

            elm->css.width = svi.ics[s_icon].icon_w * 10000 / svi.lcd_w;
            elm->css.height = svi.ics[s_icon].icon_h * 10000 / svi.lcd_h;

            elm->css.left = icon_p->css_left + (icon_p->css_width - elm->css.width) / 2 + xoffset;

            elm->css.top = icon_p->css_top + (icon_p->css_height - elm->css.height) / 2 + yoffset;

            /* ui_pic_set_image_index((struct ui_pic *)elm, p_icon); */
            /* ui_pic_set_image_index((struct ui_pic *)elm, 4); */
            elm->css.ratio.en = 1;
            elm->css.ratio.ratio_w = ratio_tab[p_icon];
            elm->css.ratio.ratio_h = ratio_tab[p_icon];
        }
#endif

    }

    int distance_x;
    int distance_y;
    int step;
    int cur_center_y;
    int tmp_y;
    struct icon_list *left = NULL;
    struct icon_list *right = NULL;
    struct icon_list *cur_left = NULL;
    struct icon_list *cur_right = NULL;
    int center_diffx;
    int center_diffy;

    if (icon_step < 0) {
        list_for_each_entry(icon_p, root, entry) {
            if (icon_p->index == 0) {
                n = icon_p;
                break;
            }
        }
        step = abs(icon_step);
        step = (step > 4) ? 4 : step;
        distance_x = (svi.ics[0].icon_w - svi.ics[step].icon_w) * 10000 / svi.lcd_w / 2;
        distance_y = (svi.ics[0].icon_h - svi.ics[step].icon_h) * 10000 / svi.lcd_h / 2;
        center_diffx = (n->cx - n->icon_left->cx) * 10000 / svi.lcd_w - distance_x * 2;
        center_diffy = (n->cy - n->icon_left_top->cy) * 10000 / svi.lcd_h - distance_y * 2;

        left = n;
        right = n;
        cur_left = n;
        cur_right = n;
        cur_center_y = n->elm->css.top + n->elm->css.height / 2;
        do {

            left = cur_left;
            right = cur_right;
            /* printf("now %x, %x\n", left->index, right->index); */

            do {
                if (left == NULL) {
                    break;
                }
                left = left->icon_left;
                if (left == NULL) {
                    break;
                }
                elm_p = left->elm;
                elm_p->css.left = left->icon_right->elm->css.left + left->icon_right->elm->css.width / 2 - center_diffx - elm_p->css.width / 2;
                elm_p->css.top = cur_center_y - elm_p->css.height / 2;

            } while (1);
            do {
                if (right == NULL) {
                    break;
                }
                right = right->icon_right;
                if (right == NULL) {
                    break;
                }

                elm_p = right->elm;
                elm_p->css.left = right->icon_left->elm->css.left + right->icon_left->elm->css.width / 2 + center_diffx - elm_p->css.width / 2;
                elm_p->css.top = cur_center_y - elm_p->css.height / 2;

            } while (1);

            if ((cur_left == NULL) && (cur_right == NULL)) {
                break;
            }
            if (cur_left == cur_right) {
                if ((cur_left->icon_left_top == NULL) && (cur_left->icon_right_top == NULL)) {
                    break;
                }

                if (cur_left->icon_left_top) {
                    left = cur_left->icon_left_top;
                    elm_p = left->elm;
                    elm_p->css.left = left->icon_right_bottom->elm->css.left + left->icon_right_bottom->elm->css.width / 2 - center_diffx / 2 - elm_p->css.width / 2;
                    elm_p->css.top = cur_center_y - center_diffy - elm_p->css.height / 2;
                    tmp_y = elm_p->css.top + elm_p->css.height / 2;
                }
                if (cur_left->icon_right_top) {
                    right = cur_left->icon_right_top;
                    elm_p = right->elm;
                    elm_p->css.left = right->icon_left_bottom->elm->css.left + right->icon_left_bottom->elm->css.width / 2 + center_diffx / 2 - elm_p->css.width / 2;
                    elm_p->css.top = cur_center_y - center_diffy - elm_p->css.height / 2;
                    tmp_y = elm_p->css.top + elm_p->css.height / 2;
                }

                cur_right = cur_left->icon_right_top;
                cur_left = cur_left->icon_left_top;
                cur_center_y = tmp_y;
            } else {
                if (cur_left) {
                    left = cur_left->icon_right_top;
                } else {
                    left = cur_right->icon_left_top;
                }
                elm_p = left->elm;
                elm_p->css.left = elm_p->css.left;
                elm_p->css.top = cur_center_y - center_diffy - elm_p->css.height / 2;

                cur_center_y = elm_p->css.top + elm_p->css.height / 2;
                cur_left = left;
                cur_right = left;
            }
        } while (1);

        left = n;
        right = n;
        cur_left = n;
        cur_right = n;
        cur_center_y = n->elm->css.top + n->elm->css.height / 2;
        do {
            if ((cur_left == NULL) && (cur_right == NULL)) {
                break;
            }
            if (cur_left == cur_right) {
                if ((cur_left->icon_left_bottom == NULL) && (cur_left->icon_right_bottom == NULL)) {
                    break;
                }

                if (cur_left->icon_left_bottom) {
                    left = cur_left->icon_left_bottom;
                    elm_p = left->elm;
                    elm_p->css.left = left->icon_right_top->elm->css.left + left->icon_right_top->elm->css.width / 2 - center_diffx / 2 - elm_p->css.width / 2;
                    elm_p->css.top = cur_center_y + center_diffy - elm_p->css.height / 2;
                    tmp_y = elm_p->css.top + elm_p->css.height / 2;
                }
                if (cur_left->icon_right_bottom) {
                    right = cur_left->icon_right_bottom;
                    elm_p = right->elm;
                    elm_p->css.left = right->icon_left_top->elm->css.left + right->icon_left_top->elm->css.width / 2 + center_diffx / 2 - elm_p->css.width / 2;
                    elm_p->css.top = cur_center_y + center_diffy - elm_p->css.height / 2;
                    tmp_y = elm_p->css.top + elm_p->css.height / 2;
                }

                cur_right = cur_left->icon_right_bottom;
                cur_left = cur_left->icon_left_bottom;
                cur_center_y = tmp_y;
            } else {
                if (cur_left) {
                    left = cur_left->icon_right_bottom;
                } else {
                    left = cur_right->icon_left_bottom;
                }
                elm_p = left->elm;
                elm_p->css.left = elm_p->css.left;
                elm_p->css.top = cur_center_y + center_diffy - elm_p->css.height / 2;

                cur_center_y = elm_p->css.top + elm_p->css.height / 2;
                cur_left = left;
                cur_right = left;
            }

            left = cur_left;
            right = cur_right;

            do {
                if (left == NULL) {
                    break;
                }
                left = left->icon_left;
                if (left == NULL) {
                    break;
                }
                elm_p = left->elm;
                elm_p->css.left = left->icon_right->elm->css.left + left->icon_right->elm->css.width / 2 - center_diffx - elm_p->css.width / 2;
                elm_p->css.top = cur_center_y - elm_p->css.height / 2;

            } while (1);
            do {
                if (right == NULL) {
                    break;
                }
                right = right->icon_right;
                if (right == NULL) {
                    break;
                }
                elm_p = right->elm;
                elm_p->css.left = right->icon_left->elm->css.left + right->icon_left->elm->css.width / 2 + center_diffx - elm_p->css.width / 2;
                elm_p->css.top = cur_center_y - elm_p->css.height / 2;

            } while (1);
        } while (1);
    }

#if 0
    /*修改验证*/
    list_for_each_child_element(p, elm) {
        ui_core_get_element_abs_rect(p, &rect);

#if EFFECT_DEBUG
        printf("%4d, %4d, %4d, %4d] [%3d, %3d, %3d, %3d]\n",
               p->css.left, p->css.top, p->css.width, p->css.height,
               rect.left, rect.top, rect.width, rect.height);
#endif
    }
#endif

    if (draw) {
        ui_core_redraw(elm);
    }

    return 0;
}




// 设置成满天星菜单
static int obj_starMenuInit(lua_State *L)
{
    int id = 0;
    struct layout *layout = NULL;

    id = lua_get_self_id(L);
    layout = (struct layout *)ui_core_get_element_by_id(id);
    layout->star_menu = true;

    struct element *p;
    struct rect rect;
    struct icon_list *icon_p;
    struct icon_list *n;
    int index;
    u32 i, j, k;

    startlight_svi_init();
    index = 0;

    list_for_each_child_element(p, &layout->elm) {
        ui_core_get_element_abs_rect(p, &rect);

        struct icon_list *icon = (struct icon_list *)zalloc(sizeof(struct icon_list));
        icon->index = index++;
        icon->size = -1;
        /* icon->id = p->id; */
        icon->elm = p;

        icon->css_left = p->css.left;
        icon->css_top = p->css.top;
        icon->css_width = p->css.width;
        icon->css_height = p->css.height;

        icon->left = rect.left;
        icon->right = rect.left + rect.width - 1;
        icon->top = rect.top;
        icon->bottom = rect.top + rect.height - 1;

        icon->width = rect.width;
        icon->height = rect.height;
        icon->cx = icon->left + rect.width / 2;
        icon->cy = icon->top + rect.height / 2;
        /* icon->radius = rect.width; */
        list_add_tail(&icon->entry, &icon_root);
    }

    for (i = 0; i < index; i++) {
        j = 0;
        list_for_each_entry(icon_p, &icon_root, entry) {
            if ((j++) == i) {
                break;
            }
        }

        k = 0;
        list_for_each_entry(n, &icon_root, entry) {
            /* printf("ccc %x, %x, %x, %x\n", icon_p->index, n->index, (u32)icon_p, (u32)n);     */
            if (n == icon_p) {
                continue;
            }

            if (k >= 6) {
                break;
            }
            if ((icon_p->cy == n->cy)
                && (abs(icon_p->left - n->right) < icon_p->width / 2)) {
                icon_p->icon_left = n;
                k++;
                continue;
            }
            if ((icon_p->cy == n->cy)
                && (abs(icon_p->right - n->left) < icon_p->width / 2)) {
                icon_p->icon_right = n;
                k++;
                continue;
            }
            if ((icon_p->cy != n->cy) && (abs(icon_p->top - n->bottom) < icon_p->height / 2)
                && (abs(icon_p->left - n->right) < icon_p->width / 2)) {
                icon_p->icon_left_top = n;
                k++;
                continue;
            }
            if ((icon_p->cy != n->cy) && (abs(icon_p->top - n->bottom) < icon_p->height / 2)
                && (abs(icon_p->right - n->left) < icon_p->width / 2)) {
                icon_p->icon_right_top = n;
                k++;
                continue;
            }
            if ((icon_p->cy != n->cy) && (abs(icon_p->bottom - n->top) < icon_p->height / 2)
                && (abs(icon_p->left - n->right) < icon_p->width / 2)) {
                icon_p->icon_left_bottom = n;
                k++;
                continue;
            }
            if ((icon_p->cy != n->cy) && (abs(icon_p->bottom - n->top) < icon_p->height / 2)
                && (abs(icon_p->right - n->left) < icon_p->width / 2)) {
                icon_p->icon_right_bottom = n;
                k++;
                continue;
            }
        }

        printf("cur icon info : %d\n", icon_p->index);
        if (icon_p->icon_left != NULL) {
            printf("icon_left : %d\n", icon_p->icon_left->index);
        }
        if (icon_p->icon_right != NULL) {
            printf("icon_right : %d\n", icon_p->icon_right->index);
        }
        if (icon_p->icon_left_top != NULL) {
            printf("icon_left_top : %d\n", icon_p->icon_left_top->index);
        }
        if (icon_p->icon_right_top != NULL) {
            printf("icon_right_top : %d\n", icon_p->icon_right_top->index);
        }
        if (icon_p->icon_left_bottom != NULL) {
            printf("icon_left_bottom : %d\n", icon_p->icon_left_bottom->index);
        }
        if (icon_p->icon_right_bottom != NULL) {
            printf("icon_right_bottom : %d\n", icon_p->icon_right_bottom->index);
        }
        printf("\n\n");
    }

    //先恢复布局坐标再调icon_change,调用顺序不能改
    startlight_scene_recover(layout, &icon_root);

    icon_change(&icon_root, &layout->elm, false);
    layout->icon_root = &icon_root;
    layout->lcd_w = svi.lcd_w;
    layout->lcd_h = svi.lcd_h;

    return 0;
}


static int obj_starMenuFree(lua_State *L)
{
    int id = 0;
    struct icon_list *n;
    struct icon_list *icon_p;
    struct layout *layout = NULL;

    id = lua_get_self_id(L);
    layout = (struct layout *)ui_core_get_element_by_id(id);
    layout->star_menu = false;

    startlight_scene_save(layout, &icon_root);

    list_for_each_entry_safe(icon_p, n, &icon_root, entry) {
        struct element *elm = icon_p->elm;
        elm->css.left = icon_p->css_left;
        elm->css.top = icon_p->css_top;
        elm->css.width = icon_p->css_width;
        elm->css.height = icon_p->css_height;
        list_del(&icon_p->entry);
        free(icon_p);
    }

    return 0;
}

// 描点画线，SDK接口只能在layout层绘制，因此只注册到layout方法
const luaL_Reg obj_DrawMethod[] = {
    // 描点画线
    {"drawSpot",	obj_drawSpot},
    {"drawLine",	obj_drawLine},
    {"drawRect",	obj_drawRect},
    {"drawCircle",	obj_drawCircle},
    {"clearDraw",	obj_clearDraw},

    // 设置成可移动
    {"setMovable",	obj_setMovable},

    // 初始化成满天星菜单
    {"starMenuInit",	obj_starMenuInit},
    {"starMenuFree",	obj_starMenuFree},

    {NULL, NULL}
};

#endif /* #if (TCFG_LUA_ENABLE) */


