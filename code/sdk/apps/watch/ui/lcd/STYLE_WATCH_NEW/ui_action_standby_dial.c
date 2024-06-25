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
#include "ui/ui_resource.h"


#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"


#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_UI_ENABLE_STANDBY_DIAL

#define STYLE_NAME  JL

REGISTER_UI_STYLE(STYLE_NAME)

#define MAX_GRID_ITEM       (6)
extern int watch_set_style(int style);
extern int watch_get_items_num();
extern char *watch_get_item(int style);
extern int watch_version_juge(char *watch_item);
extern int watch_get_style();
extern int standby_watch_set_style(int style);

//判断是否当前表盘是否支持多功能拓展 由用户进行判断依据
//这里先默认表盘0是支持的
static int IS_watch_support_expand(int style)
{
    return false;
}

static int watch_list_children_init(struct ui_grid *grid)
{
    struct element *k;
    if (!grid) {
        return 0;
    }

    for (int i = 0; i < grid->avail_item_num; i++) {
        list_for_each_child_element(k, &grid->item[i].elm) {
            switch (ui_id2type(k->id)) {
            case CTRL_TYPE_TEXT:
                struct ui_text *text = (struct ui_text *)k;
                if (!strcmp(text->source, "expand")) {
                    if (IS_watch_support_expand(i)) {
                        text->elm.css.invisible = 0;
                    } else {
                        text->elm.css.invisible = 1;
                    }
                }
                break;
            }
        }
    }
    return 0;
}

static int standby_dial_vertlist_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *)_ctrl;
    int items_num = 0;

    switch (event) {
    case ON_CHANGE_INIT_PROBE:
        items_num = watch_get_items_num();
        items_num = (items_num >= MAX_GRID_ITEM) ? MAX_GRID_ITEM : items_num;

        printf("grid num %d\n", items_num);
        ui_grid_set_item_num(grid, items_num + 1);//修改列表条目数量,不能超过列表的最大数量
        break;
    case ON_CHANGE_FIRST_SHOW:
        watch_list_children_init(grid);//初始化一些功能表盘按钮
        break;
    default:
        break;
    }
    return 0;
}

static int standby_dial_vertlist_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    static u8 touch_action = 0;
    int sel_item;
    int ret;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:

        if (touch_action != 1) {
            break;
        }

        sel_item = ui_grid_cur_item(grid);
        printf("standby_watch_set_style %d\n", sel_item);

        if (sel_item > 0) {
            ret = watch_version_juge(watch_get_item(sel_item - 1));
            if (ret != 0) {
                break;
            }
            sel_item--;
        } else {
            sel_item = -1;
        }

        standby_watch_set_style(sel_item);

        ui_send_event(KEY_CHANGE_PAGE, BIT(31) | PAGE_42);

        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        touch_action = 2;
        break;
    case ELM_EVENT_TOUCH_DOWN:
        touch_action = 1;
        break;
    case ELM_EVENT_TOUCH_L_MOVE:
        return true;
    case ELM_EVENT_TOUCH_R_MOVE:
        sel_item = ui_grid_cur_item(grid);
        printf("tem:%d \n", sel_item);
        if (sel_item == 0) { // 在第一项位置右滑默认处理（返回上一页面）
            struct element_css *css;
            css = ui_core_get_element_css(&grid->item[sel_item].elm);
            printf("css:%d,%d,%d,%d \n", css->left, css->top, css->width, css->height);
            if (css->left > 0) {
                break;
            }
        }
        return true;
    }

    return false;
}

REGISTER_UI_EVENT_HANDLER(STANDBY_DIAL_VLIST)
.onchange = standby_dial_vertlist_onchange,
 .onkey = NULL,
  .ontouch = standby_dial_vertlist_ontouch,
};

typedef struct viewfile {
    RESFILE *file;
    struct flash_file_info info;
} VIEWFILE;

static int standby_dial_preview_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)_ctrl;
    struct draw_context *dc = (struct draw_context *)arg;
    static VIEWFILE view_file[MAX_GRID_ITEM] = {0};
    static u32 open_flag = 0;
    int sel_item;
    char tmp_name[100];
    char *sty_suffix = ".sty";
    char *view_suffix = ".view";
    char *watch_item;
    u32 tmp_strlen;
    u32 sty_strlen;

    switch (event) {
    case ON_CHANGE_INIT:
        int items_num = 0;
        if (open_flag) {
            break;
        }
        items_num = watch_get_items_num();
        items_num = (items_num >= MAX_GRID_ITEM) ? MAX_GRID_ITEM : items_num;
        sty_strlen = strlen(sty_suffix);
        for (sel_item = 0; sel_item < items_num; sel_item++) {
            watch_item = watch_get_item(sel_item);
            if (watch_item == NULL) {
                printf("prew get item err %d\n", sel_item);
                return FALSE;
            }

            tmp_strlen = strlen(watch_item);
            strcpy(tmp_name, watch_item);
            strcpy(&tmp_name[tmp_strlen - sty_strlen], view_suffix);
            tmp_name[tmp_strlen - sty_strlen + strlen(view_suffix)] = '\0';
            printf("prew name %s\n", tmp_name);
            view_file[sel_item].file = res_fopen(tmp_name, "r");
            if (!view_file[sel_item].file) {
                printf("open_prewfile fail %s\n", tmp_name);
                return FALSE;
            }
            if (ui_res_flash_info_get(&view_file[sel_item].info, tmp_name, "res")) {
                printf("get_prewfile tab fail %s\n", tmp_name);
                return false;
            }
            printf("prew out\n");

        }
        open_flag = 1;

        break;
    case ON_CHANGE_SHOW:
        if (pic->elm.id == STANDBY_DIAL0_PREVIEW) {
            sel_item = 0;
        } else if (pic->elm.id == STANDBY_DIAL1_PREVIEW) {
            sel_item = 1;
        } else if (pic->elm.id == STANDBY_DIAL2_PREVIEW) {
            sel_item = 2;
        } else if (pic->elm.id == STANDBY_DIAL3_PREVIEW) {
            sel_item = 3;
        } else if (pic->elm.id == STANDBY_DIAL4_PREVIEW) {
            sel_item = 4;
        } else if (pic->elm.id == STANDBY_DIAL5_PREVIEW) {
            sel_item = 5;
        } else {
            printf("select preview err0 %x\n", pic->elm.id);
            return FALSE;
        }

        if ((sel_item >= watch_get_items_num()) || (view_file[sel_item].file == NULL)) {
            printf("select preview err1 %x, %d\n", pic->elm.id, sel_item);
            return FALSE;
        }
        pic->elm.css.background_image = 1;
        dc->preview.file = view_file[sel_item].file;
        dc->preview.file_info = &view_file[sel_item].info;
        dc->preview.id = 1;
        dc->preview.page = 0;
        break;
    case ON_CHANGE_RELEASE:

        for (sel_item = 0; sel_item < MAX_GRID_ITEM; sel_item++) {
            if (view_file[sel_item].file) {
                res_fclose(view_file[sel_item].file);
                ui_res_flash_info_free(&view_file[sel_item].info, "res");
                view_file[sel_item].file = NULL;
            }
        }
        open_flag = 0;

        break;
    }

    return 0;
}

REGISTER_UI_EVENT_HANDLER(STANDBY_DIAL0_PREVIEW)
.onchange = standby_dial_preview_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(STANDBY_DIAL1_PREVIEW)
.onchange = standby_dial_preview_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(STANDBY_DIAL2_PREVIEW)
.onchange = standby_dial_preview_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(STANDBY_DIAL3_PREVIEW)
.onchange = standby_dial_preview_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(STANDBY_DIAL4_PREVIEW)
.onchange = standby_dial_preview_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(STANDBY_DIAL5_PREVIEW)
.onchange = standby_dial_preview_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#endif /* #if TCFG_UI_ENABLE_STANDBY_DIAL */
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

