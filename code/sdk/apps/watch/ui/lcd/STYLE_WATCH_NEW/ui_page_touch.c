#include "ui/ui.h"
#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui_api.h"
#include "app_action.h"
#include "system/timer.h"
#include "key_event_deal.h"
#include "audio_config.h"
#include "jiffies.h"
#include "audio_dec.h"
#include "audio_dec_file.h"
#include "music/music_player.h"
#include "btstack/avctp_user.h"

#define STYLE_NAME  JL


#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_USER_EMITTER_ENABLE
#if TCFG_UI_ENABLE_PAGE_TOUCH


#define ID_WINDOW_PAGE PAGE_23
#define ID_WINDOW_SCAN PAGE_24
#define ID_WINDOW_END_PAGE PAGE_26
#define ID_WINDOW_PAGE_MANAGE  PAGE_27

extern u8 *page_list_read_name(u8 *mac, int index); //index从1开始,获取收藏的蓝牙mac
extern int page_list_get_count();//获得收藏数
extern int page_list_del_by_mac(u8 *mac);

extern u8 *get_cur_connect_emitter_mac_addr(void);
extern void delete_link_key(bd_addr_t *bd_addr, u8 id);

#define TEXT_NAME_LEN (32)

struct grid_set_info {
    u8 name[TEXT_NAME_LEN];
    u8 mac[6];
};

static struct grid_set_info handler;// = NULL;
#define __this 	(&handler)
#define sizeof_this     (sizeof(struct grid_set_info))



static int grid_child_cb(void *_ctrl, int id, int type, int index)
{
    switch (type) {
    case CTRL_TYPE_PROGRESS:
        break;
    case CTRL_TYPE_MULTIPROGRESS:
        break;
    case CTRL_TYPE_TEXT:
        struct ui_text *text = (struct ui_text *)_ctrl;
        if (!strcmp(text->source, "name")) {
            text->attrs.offset = 0;
            text->attrs.format = "text";
            u8 mac[6];
            u8 *name = page_list_read_name(mac, index + 1);  //index从1开始,获取收藏的蓝牙mac
            if (name) {
                text->attrs.str    = (const char *)name;
                text->attrs.strlen = strlen((const char *)name);
                text->attrs.encode = FONT_ENCODE_UTF8;
                text->attrs.flags  = FONT_DEFAULT;
            }
        }

        if (!strcmp(text->source, "status")) {
            u8 mac[6] = {0};
            u8 *name = page_list_read_name(mac, index + 1);  //index从1开始,获取收藏的蓝牙mac
            u8 *current_mac = get_cur_connect_emitter_mac_addr();
            if (current_mac && !memcmp(current_mac, mac, 6)) {
                ui_text_set_index(text, 1);
            } else {
                ui_text_set_index(text, 0);
            }
        }
        break;
    case CTRL_TYPE_NUMBER:
        struct ui_number *number = (struct ui_number *)_ctrl;
        number->type = TYPE_STRING;
        /* number->num_str = b_list->number; */
        break;
    case CTRL_TYPE_TIME:
        break;
    }
    return 0;
}

static int brows_children_init(struct ui_grid *grid)
{
    struct element_css *css;
    struct element *k;
    if (!grid) {
        return 0;
    }


    int count = page_list_get_count();


    for (int i = 0; i < grid->avail_item_num; i++) {
        css = ui_core_get_element_css(&grid->item[i].elm);

        if (i < count) {
            list_for_each_child_element(k, &grid->item[i].elm) {
                grid_child_cb(k, k->id, ui_id2type(k->id), i);
            }

            if ((css->left < 0) || ((css->left + css->width) > 10000) || (css->top < 0)  || ((css->top + css->height) > 10000)) {
                css->invisible = 1;
            } else {
                css->invisible = 0;
            }
        } else {
            grid->item[i].elm.css.invisible = 1;
        }
    }
    return 0;
}


static int brow_switch_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    int sel_item;
    /* sel_item = ui_grid_cur_item(grid); */
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_MOVE:
        /* int x = e->pos.x - grid->pos.x; */
        /* int y = e->pos.y - grid->pos.y; */
        /* #<{(| printf("y= %d Dy =%d ty=%d\n",grid->pos.y,y,e->pos.y); |)}># */
        /*  */
        /* if (ui_grid_dynamic_slide(grid, 1, y)) { */
        /*     grid->pos.x = e->pos.x; */
        /*     grid->pos.y = e->pos.y; */
        /* } */
        /*  */
        move_flag  = 1;
        return false;
        break;
    case ELM_EVENT_TOUCH_DOWN:
        move_flag  = 0;
        return FALSE;
    case ELM_EVENT_TOUCH_UP:
        if (move_flag) {
            move_flag = 0;
            return false;
        }
        /* sel_item =  ui_grid_dynamic_cur_item(grid); */
        sel_item = ui_grid_cur_item_dynamic(grid);
        u8   mac[6];
        u8 *name = page_list_read_name(mac, sel_item + 1);  //index从1开始,获取收藏的蓝牙mac
        if (name) {
            memcpy(__this->mac, mac, 6);
            sprintf((char *)__this->name, "%s", name);
        }
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_PAGE_MANAGE);
        break;
    default:
        return false;
        break;
    }
    return true;
}

static int browse_enter_child_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    int type = ui_id2type(elm->id);
    if (event == ON_CHANGE_UPDATE_ITEM) {
        int index = (u32)arg;
        grid_child_cb(elm, elm->id, type, index);
    }
    return 0;
}


static int browse_enter_onchane(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        if (page_list_get_count()) {
            grid->elm.css.invisible = 0;
        }
        /* ui_auto_shut_down_disable(); */
        int row = page_list_get_count();
        int col = 1;


        ui_set_default_handler(grid, NULL, NULL, browse_enter_child_onchange);
        ui_grid_init_dynamic(grid, &row, &col);
        /* ui_grid_dynamic_create(grid, 1, page_list_get_count(), grid_child_cb); */
        break;
    case ON_CHANGE_RELEASE:
        /* ui_grid_dynamic_release(grid); */
        ui_set_default_handler(grid, NULL, NULL, NULL);
        /* ui_auto_shut_down_enable(); */
        break;
    case ON_CHANGE_FIRST_SHOW:
        /* brows_children_init(grid); */
        break;
    default:
        return false;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(BT_PAGE_VLIST)
.onchange = browse_enter_onchane,
 .onkey = NULL,
  .ontouch = brow_switch_ontouch,
};


static int text_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_SCAN);
        break;
    default:
        return false;
        break;
    }
    return true;
}


REGISTER_UI_EVENT_HANDLER(BT_PAGE_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = text_ontouch,
};







static int page_manage_child_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    int type = ui_id2type(elm->id);

    if (!__this) {
        return 0;
    }

    switch (type) {
    case CTRL_TYPE_PROGRESS:
        break;
    case CTRL_TYPE_MULTIPROGRESS:
        break;
    case CTRL_TYPE_TEXT:
        struct ui_text *text = (struct ui_text *)_ctrl;
        if (!strcmp(text->source, "name")) {
            text->attrs.offset = 0;
            text->attrs.format = "text";
            u8 mac[6];
            u8 *name = __this->name;  //index从1开始,获取收藏的蓝牙mac
            if (name) {
                text->attrs.str    = (const char *)name;
                text->attrs.strlen = strlen((const char *)name);
                text->attrs.encode = FONT_ENCODE_UTF8;
                text->attrs.flags  = FONT_DEFAULT;
            }
        }

        if (!strcmp(text->source, "status")) {
            u8 *current_mac = get_cur_connect_emitter_mac_addr();
            if (current_mac && (!memcmp(current_mac, __this->mac, 6))) {
                ui_text_set_index(text, 1);
            } else {
                ui_text_set_index(text, 0);
            }
        }

        if (!strcmp(text->source, "connect")) {
            u8 *current_mac = get_cur_connect_emitter_mac_addr();
            if (current_mac && (!memcmp(current_mac, __this->mac, 6))) {
                ui_text_set_index(text, 1);
            } else {
                ui_text_set_index(text, 0);
            }
        }


        break;
    case CTRL_TYPE_NUMBER:
        break;
    case CTRL_TYPE_TIME:
        break;
    }

    return 0;
}






static int page_manage_onchane(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_INIT:
        ui_auto_shut_down_disable();
        ui_set_default_handler(ctr, NULL, NULL, page_manage_child_onchange);
        break;
    case ON_CHANGE_RELEASE:
        ui_auto_shut_down_enable();
        ui_set_default_handler(ctr, NULL, NULL, NULL);
        break;
    case ON_CHANGE_FIRST_SHOW:
        break;
    default:
        return false;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(BT_PAGE_MANAGE_LAYOUT)
.onchange = page_manage_onchane,
 .onkey = NULL,
  .ontouch = NULL,
};



extern int ui_set_page_info(u8 *name, u8 *mac);

static int page_button_connect_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        u8 *current_mac = get_cur_connect_emitter_mac_addr();
        if (__this && current_mac) {
            /* page_list_del_by_mac(__this->mac); */
            user_emitter_cmd_prepare(USER_CTRL_DISCONNECTION_HCI, 0, NULL);
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(ID_WINDOW_PAGE);

        }

        if (__this && !current_mac) {
            ui_set_page_info(__this->name, __this->mac);
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(ID_WINDOW_END_PAGE);
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}






static int page_button_cancel_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        if (__this) {
            u8 *current_mac = get_cur_connect_emitter_mac_addr();
            if (__this && current_mac && (!memcmp(__this->mac, current_mac, 6))) {
                user_emitter_cmd_prepare(USER_CTRL_DISCONNECTION_HCI, 0, NULL);
            }


            delete_link_key((bd_addr_t *)__this->mac, 1);
            page_list_del_by_mac(__this->mac);
            /* ui_set_page_info(__this->name ,__this->mac); */
        }
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_PAGE);
        break;
    default:
        return false;
        break;
    }
    return true;
}


REGISTER_UI_EVENT_HANDLER(BT_PAGE_MANAGE_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = page_button_connect_ontouch,
};



REGISTER_UI_EVENT_HANDLER(BT_PAGE_MANAGE_CANCEL_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = page_button_cancel_ontouch,
};



static int page_brows_children_redraw(int id)
{
    struct ui_grid *grid = (struct ui_grid *)ui_core_get_element_by_id(id);
    struct element *k;
    if (!grid) {
        return 0;
    }
    for (int i = 0; i < grid->avail_item_num; i++) {
        list_for_each_child_element(k, &grid->item[i].elm) {
            grid_child_cb(k, k->id, ui_id2type(k->id), i);
        }
    }
    ui_core_redraw(&grid->elm);
    return 0;
}

static int bt_status_handler(const char *type, u32 arg)
{
    printf("_func_  == %s %d\n", __func__, __LINE__);
    if (type && (!strcmp(type, "event"))) {
        switch (arg) {
        case BT_STATUS_SECOND_CONNECTED:
        case BT_STATUS_FIRST_CONNECTED:
        case BT_STATUS_CONN_A2DP_CH:
            page_brows_children_redraw(BT_PAGE_VLIST);
            break;
        case BT_STATUS_FIRST_DISCONNECT:
        case BT_STATUS_SECOND_DISCONNECT:
            printf("%s %d\n", __func__, __LINE__);
            page_brows_children_redraw(BT_PAGE_VLIST);
            break;
        case BT_STATUS_A2DP_MEDIA_START:
            printf("%s %d\n", __func__, __LINE__);
            break;
        case BT_STATUS_A2DP_MEDIA_STOP:
            printf("%s %d\n", __func__, __LINE__);
            break;
        }
    }

    return 0;
}






static const struct uimsg_handl ui_page_msg_handler[] = {
    { "bt_status",        bt_status_handler     }, /* 蓝牙状态 */
    { NULL, NULL},      /* 必须以此结尾！ */
};



static int bt_page_mode_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        ui_register_msg_handler(window->elm.id, ui_page_msg_handler);//注册消息交互的回调
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        return false;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(ID_WINDOW_PAGE)
.onchange = bt_page_mode_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};





#endif
#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

