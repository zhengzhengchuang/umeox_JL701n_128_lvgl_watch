#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "app_task.h"
#include "system/timer.h"
#include "device/device.h"
#include "key_event_deal.h"
#include "res/resfile.h"
#include "ui/res_config.h"
#include "system/includes.h"
#include "syscfg_id.h"
#include "cJSON.h"

#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE

#define STYLE_NAME  JL

extern char *watch_get_item(int style);
extern int watch_get_style();

extern const u8 *watch_ex_item[];
//最大8字节包括‘\0'
const u8 *watch_ex_item[] = {
    "heart",//心率
    "battrty",//电量
    "weather",//天气
    "press",//压力
    "step",//步数
    "streng",//强度
    "active",//活动时间
    "calorie",//热量
    "baro",//气压
    "alti",//海拔
    "oxygen",//血氧
    "time",//时间
    "zone",//时区
    "none",//没有
};



#define EX_VM_COUNT_MAX (8)

struct watch_ex_vm {
    u8  id;
    u8  source[EX_VM_COUNT_MAX][8];//
    struct position pos[EX_VM_COUNT_MAX];
};


static struct watch_ex_vm watch_ex = {0};
static u8 watch_source_button  = 0;//记录触发的序号

extern void ui_send_event(u16 event, u32 val);


static void vm_watch_ex_reset(struct watch_ex_vm *ex, int id)
{
    ex->id = id;
    for (int i = 0; i < EX_VM_COUNT_MAX; i++) {
        sprintf((u8 *)&ex->source[i], "none");
    }
}

static const u8 *get_watch_ex_item_by_index(int index)
{
    if (index >= 0 && index < (sizeof(watch_ex_item) / sizeof(watch_ex_item[0]))) {
        return watch_ex_item[index];
    }
    return NULL;
}

static const int get_watch_ex_index_by_item(u8 *name)
{
    int index ;
    for (index = 0; index < (sizeof(watch_ex_item) / sizeof(watch_ex_item[0])); index ++) {
        printf("%s %s %d\n", name, watch_ex_item[index], index);
        if (!strcmp(name, watch_ex_item[index])) {
            return index;
        }
    }
    return -1;
}




static int text_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct element *elm = (struct element *)_ctrl;
    struct ui_text *text = (struct ui_text *)_ctrl;
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        printf("<<<<< __FUNCTION__ = %s __LINE__ =%d >>>>>\n", __FUNCTION__, __LINE__);
        move_flag = 0;
        if (!strncmp(text->source, "excfg", strlen("excfg"))) {
            //判断是是配置的字符 就是return true 设置为焦点
            return TRUE;
        }
        break;
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_UP:
        printf("<<<<< __FUNCTION__ = %s __LINE__ =%d >>>>>\n", __FUNCTION__, __LINE__);
        if (move_flag) {
            move_flag = 0;
            break;
        }
        struct rect r;
        struct element *p, *n;
        struct element *parent_elm = (struct element *)elm->parent;
        list_for_each_child_element_reverse(p, n, parent_elm) {
            if (!p->css.invisible  && (ui_id2type(p->id)) == CTRL_TYPE_PIC) {//查找重叠的字符图片控件

                ui_core_get_element_abs_rect(p, &r);
                if (in_rect(&r, &e->pos)) {

                    int style = watch_get_style() + VM_WATCH_EX_BEGIN;
                    if (sizeof(struct watch_ex_vm) != syscfg_read(style, &watch_ex, sizeof(struct watch_ex_vm))) {
                        vm_watch_ex_reset(&watch_ex, watch_get_style());
                    }
                    syscfg_write(style, &watch_ex, sizeof(struct watch_ex_vm));
                    watch_source_button = text->source[5] - '0';
                    printf("<<<<< __FUNCTION__ = %s __LINE__ =%d  %x %d %d %d>>>>>\n", __FUNCTION__, __LINE__, p->id, watch_source_button, p->css.left, p->css.top);
                    watch_ex.pos[watch_source_button].x = p->css.left;
                    watch_ex.pos[watch_source_button].y = p->css.top;

                    ui_send_event(KEY_CHANGE_PAGE, BIT(31) | PAGE_38);
                    return true;
                    /* break; */
                }
            }
        }
        break;
    default:
        break;
    }
    return 0;
}

static int watch_expand_child_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct element *elm = (struct element *)_ctrl;
    int type = ui_id2type(elm->id);
    switch (type) {
    case CTRL_TYPE_GRID:
        break;
    case CTRL_TYPE_TEXT:
        return text_ontouch(_ctrl, e);
        break;
    }
    return 0;
}


static int watch_expand_children_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    switch (event) {

    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_FIRST_SHOW:
        struct rect r;
        struct rect b;
        struct rect c;
        struct element *p, *n;
        struct element *parent_elm = (struct element *)elm->parent;
        struct ui_text *text = (struct ui_text *)_ctrl;
        if ((ui_id2type(elm->id) == CTRL_TYPE_TEXT) && !strncmp(text->source, "excfg", strlen("excfg"))) {
            printf("<<<<< __FUNCTION__ = %s __LINE__ =%d  %x>>>>>\n", __FUNCTION__, __LINE__, elm->id);
            ui_core_get_element_abs_rect(elm, &b);
            list_for_each_child_element_reverse(p, n, parent_elm) {
                if (!p->css.invisible  && (ui_id2type(p->id)) == CTRL_TYPE_PIC) {//查找重叠的字符图片控件
                    ui_core_get_element_abs_rect(p, &r);
                    printf("<<<<< __FUNCTION__ = %s __LINE__ =%d  %x>>>>>\n", __FUNCTION__, __LINE__, p->id);
                    if (get_rect_cover(&r, &b, &c)) {
                        printf("<<<<< __FUNCTION__ = %s __LINE__ =%d  %x>>>>>\n", __FUNCTION__, __LINE__, p->id);
                        int index = text->source[5] - '0';
                        int style = watch_get_style() + VM_WATCH_EX_BEGIN;
                        p->css.invisible = 1;
                        if (style <= VM_WATCH_EX_END) {

                            if (sizeof(struct watch_ex_vm) != syscfg_read(style, &watch_ex, sizeof(struct watch_ex_vm))) {
                                vm_watch_ex_reset(&watch_ex, watch_get_style());
                            }

                            printf("<<<<< __FUNCTION__ = %s __LINE__ =%d  %x %s>>>>>\n", __FUNCTION__, __LINE__, p->id, watch_ex.source[index]);
                            int ui_index =  get_watch_ex_index_by_item(&watch_ex.source[index][0]);
                            if (ui_index >= 0) {
                                printf("<<<<< __FUNCTION__ = %s __LINE__ =%d  %x>>>>>\n", __FUNCTION__, __LINE__, p->id);
                                p->css.invisible = 0;
                                ui_pic_set_image_index((struct ui_pic *)p, ui_index);
                            }

                        }
                        break;
                    }
                }
            }
        }
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }
    return 0;
}

static int WATCH_EXPAND_SET_LAYOUT_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct layout *layout = (struct layout *)_ctrl;
    switch (event) {

    case ON_CHANGE_INIT:
        ui_set_default_handler(layout, watch_expand_child_ontouch, NULL, watch_expand_children_onchange);
        void *hdl;
        char *root_path = watch_get_item(watch_get_style());
        char *watch;//
        watch = zalloc(128);
        strcpy(watch, root_path);
        strcpy(watch + strlen(watch) - 4, ".tab");
        hdl = load_control_info_by_id(watch, 0X20820002, 0X208334A1);
        if (hdl)  {
            struct element *elm;
            elm = (struct element *)ui_control_new(hdl, _ctrl);
            if (elm) {
                elm->css.invisible = 0;
                elm->css.top = 0;
                elm->css.left = 0;
                printf("create new widget succ!\n");
            }
        }
        if (watch) {
            free(watch);
            watch = NULL;

        }
    case ON_CHANGE_FIRST_SHOW:
        break;
    case ON_CHANGE_RELEASE:
        ui_set_default_handler(layout, NULL, NULL, NULL);
        break;
    default:
        break;
    }
    return 0;
}

/* REGISTER_UI_EVENT_HANDLER(WATCH_EXPAND_SET_LAYOUT) */
/* .onchange = WATCH_EXPAND_SET_LAYOUT_onchange, */
/*  .onkey = NULL,// */
/*   .ontouch = NULL,// */
/* }; */
/*  */

static int preview_grid_children_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    struct ui_grid *grid = (struct ui_grid *)_ctrl;
    switch (event) {
    case ON_CHANGE_INIT:
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        break;
    case ON_CHANGE_FIRST_SHOW:
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }
    return 0;
}

static int preview_text_children_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    struct ui_text *text = (struct ui_text *)_ctrl;
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_FIRST_SHOW:
        struct element *p, *n;
        struct element *parent_elm = (struct element *)elm->parent;
        int style = watch_get_style() + VM_WATCH_EX_BEGIN;
        if (style <= VM_WATCH_EX_END) {
            if (sizeof(struct watch_ex_vm) != syscfg_read(style, &watch_ex, sizeof(struct watch_ex_vm))) {
                vm_watch_ex_reset(&watch_ex, watch_get_style());
            }
            printf("<<<<< __FUNCTION__ = %s __LINE__ =%d %d %s %s>>>>>\n", __FUNCTION__, __LINE__, watch_source_button, &watch_ex.source[watch_source_button][0], text->source);


            if (!strcmp(text->source, &watch_ex.source[watch_source_button][0])) {
                list_for_each_child_element_reverse(p, n, parent_elm) {
                    if (!p->css.invisible  && (ui_id2type(p->id)) == CTRL_TYPE_PIC) {
                        ui_pic_set_image_index((struct ui_pic *)p, 1);
                        break;
                    }

                }
            }
        }
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }
    return 0;
}
static int watch_expand_preview_child_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    int type = ui_id2type(elm->id);
    switch (type) {
    case CTRL_TYPE_GRID:
        preview_grid_children_onchange(_ctrl, event, arg);
        break;
    case CTRL_TYPE_TEXT:
        preview_text_children_onchange(_ctrl, event, arg);
        break;
    default:
        break;
    }
    return 0;
}

static int preview_grid_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct element *elm = (struct element *)_ctrl;
    struct ui_grid *grid = (struct ui_grid *)_ctrl;
    struct element *k;
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        move_flag = 0;
        break;
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_UP:
        if (move_flag) {
            move_flag = 0;
            break;
        }
        int sel_item = ui_grid_cur_item(grid);

        if (!sel_item) {
            break;
        }
        struct rect r;
        struct element *p, *n;
        list_for_each_child_element_reverse(p, n, &grid->item[sel_item].elm) {

            if (!p->css.invisible  && (ui_id2type(p->id)) == CTRL_TYPE_TEXT) {//查找重叠的字符图片控件

                ui_core_get_element_abs_rect(p, &r);
                if (in_rect(&r, &e->pos)) {
                    printf("<<<<< __FUNCTION__ = %s __LINE__ =%d  %x>>>>>\n", __FUNCTION__, __LINE__, p->id);
                    struct ui_text *text = (struct ui_text *)p;
                    int style = watch_get_style() + VM_WATCH_EX_BEGIN;
                    if (style <= VM_WATCH_EX_END) {
                        watch_ex.id =  watch_get_style();
                        sprintf(watch_ex.source[watch_source_button], text->source);
                        syscfg_write(style, &watch_ex, sizeof(struct watch_ex_vm));
                    }
                    ui_send_event(KEY_CHANGE_PAGE, BIT(31) | PAGE_37);
                    return true;
                    /* break; */
                }
            }
        }

        break;
    default:
        break;
    }
    return 0;
}

static int watch_expand_preview_child_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct element *elm = (struct element *)_ctrl;
    int type = ui_id2type(elm->id);
    switch (type) {
    case CTRL_TYPE_GRID:
        preview_grid_ontouch(_ctrl, e);
        break;
    case CTRL_TYPE_TEXT:
        break;
    default:
        break;
    }
    return 0;
}

static int watch_expand_preview_load_cfg_file(void *arg)
{
    FILE *file;

    int window = 0;
    int control = 0;

    char *root_path = watch_get_item(watch_get_style());
    char *watch;//[64];// = {0};
    watch = zalloc(128);
    strcpy(watch, root_path);
    strcpy(watch + strlen(watch) - 4, ".cfg");

    file = fopen(watch, "r");
    if (!file) {
        printf("open_cfg fail %s\n", watch);
        free(watch);
        return -1;
    }
    int len = 512;
    printf("cfg size = %x \n", len);
    u8 *buf = zalloc(len);
    fread(file, buf, len);
    printf("%s \n", buf);
    cJSON *root = NULL;
    cJSON *item = NULL;
    root = cJSON_Parse(buf);
    if (!root) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
    } else {
        item = cJSON_GetObjectItem(root, "watch_pr_window");
        if (item) {

        }
        item = cJSON_GetObjectItem(root, "watch_pr_layout");
        if (item) {

        }
        cJSON_Delete(root);
    }
    if (file) {
        fclose(file);
        file = NULL;
    }
    if (buf) {
        free(buf);
        buf = NULL;
    }

    return 0;
}

static int WATCH_EXPAND_PREVIEW_LAYOUT_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct layout *layout = (struct layout *)_ctrl;
    switch (event) {

    case ON_CHANGE_INIT:
        ui_set_default_handler(layout, watch_expand_preview_child_ontouch, NULL, watch_expand_preview_child_onchange);
        void *hdl;
        char *root_path = watch_get_item(watch_get_style());
        char *watch;//[64];// = {0};
        watch = zalloc(128);
        strcpy(watch, root_path);
        strcpy(watch + strlen(watch) - 4, ".tab");
        hdl = load_control_info_by_id(watch, 0X20420001, 0X2045EC0B);
        if (hdl)  {
            struct element *elm;
            elm = (struct element *)ui_control_new(hdl, _ctrl);
            if (elm) {
                elm->css.invisible = 0;
                elm->css.top = 0;
                elm->css.left = 0;
                printf("create new widget succ!\n");
            }
        }
        if (watch) {
            free(watch);
            watch = NULL;

        }
    case ON_CHANGE_FIRST_SHOW:
        break;
    case ON_CHANGE_RELEASE:
        ui_set_default_handler(layout, NULL, NULL, NULL);
        break;
    default:
        break;
    }
    return 0;
}

static int WATCH_EXPAND_PREVIEW_LAYOUT_ontouch(void *ctr, struct element_touch_event *e)
{

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        printf("<<<<< __FUNCTION__ = %s __LINE__ =%d >>>>>\n", __FUNCTION__, __LINE__);
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        printf("<<<<< __FUNCTION__ = %s __LINE__ =%d >>>>>\n", __FUNCTION__, __LINE__);
        break;
    case ELM_EVENT_TOUCH_U_MOVE:
        break;
    case ELM_EVENT_TOUCH_D_MOVE:
        break;
    default:
        break;
    }
    return false;
}

/* REGISTER_UI_EVENT_HANDLER(WATCH_EX_PREVIEW_LAYOUT) */
/* .onchange = WATCH_EXPAND_PREVIEW_LAYOUT_onchange, */
/*  .onkey = NULL,// */
/*   .ontouch = NULL,// */
/* }; */
/*  */








#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

