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
#include "bt_emitter.h"
#include "soundbox.h"

#define STYLE_NAME  JL


#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_USER_EMITTER_ENABLE
#if TCFG_UI_ENABLE_SCAN_TOUCH

extern void bt_list_clean();
extern int bt_menu_list_count();
extern void bt_menu_list_add(u8 *name, u8 *mac, u8 rssi);
extern int page_list_add(u8 *mac, u8 *name);//添加收藏
extern u8 *bt_list_get_mac_by_number(u8 num);
extern u8 *bt_list_get_name_by_number(u8 num);
extern u8 *get_cur_connect_emitter_mac_addr(void);


static u8 scan_complete = 0;
static u16 timer_out = 0;

#define ID_WINDOW_SCAN PAGE_24
#define ID_WINDOW_PAGE PAGE_23
#define ID_WINDOW_REQUEST_PAGE PAGE_25
#define ID_WINDOW_END_PAGE PAGE_26

#define TEXT_NAME_LEN (32)

struct grid_set_info {
    u8 name[TEXT_NAME_LEN];
    u8 mac[6];
};

static struct grid_set_info *handler = NULL;
#define __this 	(handler)
#define sizeof_this     (sizeof(struct grid_set_info))


int  ui_set_page_info(u8 *name, u8 *mac)
{
    if (!__this) {
        __this = zalloc(sizeof_this);
    } else {
        memset(__this, 0, sizeof_this);
    }
    sprintf(__this->name, "%s", name);
    memcpy(__this->mac, mac, 6);
    return 0;
}


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
            u8 *name  = bt_list_get_name_by_number(index);

            if (name) {
                text->attrs.str    = (const char *)name;
                text->attrs.strlen = strlen((const char *)name);
                text->attrs.encode = FONT_ENCODE_UTF8;
                text->attrs.flags  = FONT_DEFAULT;
            }

        }

        if (!strcmp(text->source, "status")) {
            /* ui_text_set_index(text,bt_info->status); */
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
    struct element *k;
    if (!grid) {
        return 0;
    }

    int count = bt_menu_list_count();

    for (int i = 0; i < grid->avail_item_num; i++) {
        if (i < count) {
            list_for_each_child_element(k, &grid->item[i].elm) {
                grid_child_cb(k, k->id, ui_id2type(k->id), i);
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
    static u8 move_flag = 0;
    /* int item_sel = ui_grid_cur_item_dynamic(grid); // 虚拟被选择的项 */
    /* int cur_item = ui_grid_cur_item(grid); // 实际被点击的项 */
    /* printf("$$$$$$$$$$$$$ sel: %d, cur: %d\n", item_sel, cur_item); */

    switch (e->event) {
    case ELM_EVENT_TOUCH_MOVE:

        /* int x = e->pos.x - grid->pos.x; */
        /* int y = e->pos.y - grid->pos.y; */
        /* [> printf("y= %d Dy =%d ty=%d\n",grid->pos.y,y,e->pos.y); <] */

        /* if (ui_grid_dynamic_slide(grid, 1, y)) { */
        /* grid->pos.x = e->pos.x; */
        /* grid->pos.y = e->pos.y; */
        /* } */

        move_flag = 1;
        break;

    case ELM_EVENT_TOUCH_DOWN:
        move_flag = 0;
        return false;//不接管消息
        break;
    case ELM_EVENT_TOUCH_UP:

        if (move_flag) {
            move_flag = 0;
            return false;//不接管消息
        }

        /* sel_item =  ui_grid_dynamic_cur_item(grid); */
        sel_item = ui_grid_cur_item_dynamic(grid); // 虚拟被选择的项
        u8 *mac = bt_list_get_mac_by_number(sel_item);
        if (mac) {
            printf("bt connet start mac:\n");
            put_buf(mac, 6);
            memcpy(__this->mac, mac, 6);
            sprintf((char *)__this->name, "%s", bt_list_get_name_by_number(sel_item));
            /* bt_emitter_stop_search_device(); */

            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(ID_WINDOW_REQUEST_PAGE);
        }


        return false;//不接管消息
        break;
    default:
        return false;//不接管消息
        break;
    }

    return false;//true;//接管消息
}





static int browse_enter_onchane(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        scan_complete = 0;
        if (!__this) {
            __this = zalloc(sizeof_this);
        } else {
            memset(__this, 0, sizeof_this);
        }

        grid->elm.css.invisible = 1;
        ui_auto_shut_down_disable();
        bt_list_clean();
        g_printf("************   emitter \n");
        bt_emitter_start_search_device();
        g_printf("************   emitter start\n");
        /* ui_grid_dynamic_create(grid, 1, 2, grid_child_cb); */
        int row = bt_menu_list_count();
        int col = 1;
        ui_grid_init_dynamic(grid, &row, &col);
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        break;
    case ON_CHANGE_RELEASE:
        /* ui_grid_dynamic_release(grid); */
        if (!scan_complete) {
            bt_emitter_stop_search_device();
        }
        bt_list_clean();
        ui_auto_shut_down_enable();
        g_printf("************   emitter release\n");
        break;
    case ON_CHANGE_FIRST_SHOW:
        /* brows_children_init(grid); */
        break;
    default:
        return false;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(BT_SCAN_VLIST)
.onchange = browse_enter_onchane,
 .onkey = NULL,
  .ontouch = brow_switch_ontouch,
};

static int list_bt_name_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;
    int index;
    int count = bt_menu_list_count();

    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_HIGHLIGHT:
        break;
    case ON_CHANGE_UPDATE_ITEM:

        index = (u32)arg;
        if (index > count) {
            break;
        }
        switch (text->elm.id) {
        case BASEFORM_318:
            break;
        case BASEFORM_322:
            break;
        case BASEFORM_326:
            break;
        default:
            return FALSE;
        }

        u8 *name = bt_list_get_name_by_number(index);
        printf("name[%d] = %s\n", index, name);
        ui_text_set_utf8_str(text, "text", name, strlen(name), FONT_DEFAULT);
        break;
    }
    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(BASEFORM_318)
.onchange = list_bt_name_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(BASEFORM_322)
.onchange = list_bt_name_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(BASEFORM_326)
.onchange = list_bt_name_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};





static int bt_list_updata_handler(const char *type, u32 arg)
{
    /* if (bt_menu_list_count() >= 2) { */
    if (bt_menu_list_count() >= 1) {
        if (ui_core_get_disp_status_by_id(BT_SCAN_VLIST) != TRUE) {
            ui_hide(BT_SCAN_ING_PIC);
            ui_show(BT_SCAN_VLIST);
        }
        int row = 1;
        int col = 0;
        ui_grid_add_dynamic_by_id(BT_SCAN_VLIST, &row, &col, true);
    }
    return 0;
}


static int bt_status_handler(const char *type, u32 arg)
{
    printf("_func_  == %s %d\n", __func__, __LINE__);
    if (type && (!strcmp(type, "event"))) {
        switch (arg) {
        case BT_STATUS_SECOND_CONNECTED:
        case BT_STATUS_FIRST_CONNECTED:

            break;
        case BT_STATUS_FIRST_DISCONNECT:
        case BT_STATUS_SECOND_DISCONNECT:
            printf("%s %d\n", __func__, __LINE__);
            break;
        case BT_STATUS_A2DP_MEDIA_START:
            printf("%s %d\n", __func__, __LINE__);
            break;
        case BT_STATUS_A2DP_MEDIA_STOP:
            printf("%s %d\n", __func__, __LINE__);
            break;
        }
    }

    if (type && (!strcmp(type, "hci_event"))) {
        switch (arg) {
        case HCI_EVENT_INQUIRY_COMPLETE:
            /* ui_grid_dynamic_set_item_by_id(BT_SCAN_VLIST, bt_menu_list_count()); //修改动态列表数 */
            /* ui_grid_dynamic_reset((struct ui_grid *)ui_core_get_element_by_id(BT_SCAN_VLIST), 0); */
            if (ui_core_get_disp_status_by_id(BT_SCAN_VLIST) != TRUE) {
                ui_hide(BT_SCAN_ING_PIC);
                ui_show(BT_SCAN_VLIST);
            }
            ui_show(BT_SCAN_RETRY_TEXT);
            ui_text_show_index_by_id(BT_SCAN_TITLE_TEXT, 1);
            scan_complete = 1;
            break;
        }
    }
    return 0;
}






static const struct uimsg_handl ui_msg_handler[] = {
    { "bt_list",          bt_list_updata_handler     }, /* 更新蓝牙列表 */
    { "bt_status",        bt_status_handler     }, /* 蓝牙状态 */
    { NULL, NULL},      /* 必须以此结尾！ */
};

void bt_scan_timeout(void *priv)
{
    if (!timer_out) {
        return ;
    }
    printf("bt scan timeout.\n");
    UI_MSG_POST("bt_status:hci_event=%4", HCI_EVENT_INQUIRY_COMPLETE);
}

static int bt_scan_mode_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        puts("\n***bt_scan_mode_onchange***\n");
        {
            // 搜索前断开已经连接的蓝牙耳机
            u8 *current_mac = get_cur_connect_emitter_mac_addr();
            if (current_mac) {
                user_emitter_cmd_prepare(USER_CTRL_DISCONNECTION_HCI, 0, NULL);
            }
        }
        ui_register_msg_handler(window->elm.id, ui_msg_handler);//注册消息交互的回调
        if (!timer_out) {
            /* timer_out = sys_timeout_add(NULL, bt_scan_timeout, 10000); */
        }
        break;
    case ON_CHANGE_RELEASE:
        if (timer_out) {
            sys_timer_del(timer_out);
        }
        break;
    default:
        return false;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(ID_WINDOW_SCAN)
.onchange = bt_scan_mode_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};



static int page_retry_button_ontouch(void *ctr, struct element_touch_event *e)
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


REGISTER_UI_EVENT_HANDLER(BT_SCAN_RETRY_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = page_retry_button_ontouch,
};





static int bt_page_name_title_text_onchane(void *ctr, enum element_change_event e, void *arg)
{

    struct ui_text *text = (struct ui_text *)ctr;

    if (!__this) {
        return false;
    }

    switch (e) {
    case ON_CHANGE_INIT:
        text->attrs.offset = 0;
        text->attrs.format = "text";
        text->attrs.str    = (const char *)__this->name;
        text->attrs.strlen = strlen((const char *)__this->name);
        text->attrs.encode = FONT_ENCODE_UTF8;
        text->attrs.flags  = FONT_DEFAULT;
        break;
    default:
        break;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(BT_PAGE_NAME_TITLE_TEXT)
.onchange = bt_page_name_title_text_onchane,
 .onkey = NULL,
  .ontouch = NULL,
};



static int page_yes_button_ontouch(void *ctr, struct element_touch_event *e)
{
    if (!__this) {
        return true;
    }
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_END_PAGE);
        UI_WINDOW_BACK_DEL(ID_WINDOW_REQUEST_PAGE);
        break;
    default:
        return false;
        break;
    }
    return true;
}


REGISTER_UI_EVENT_HANDLER(BT_PAGE_YES_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = page_yes_button_ontouch,
};



static int page_no_button_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_PAGE);
        UI_WINDOW_BACK_DEL(ID_WINDOW_SCAN);
        UI_WINDOW_BACK_DEL(ID_WINDOW_REQUEST_PAGE);
        break;
    default:
        return false;
        break;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(BT_PAGE_NO_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = page_no_button_ontouch,
};



static int page_end_button_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_PAGE);
        UI_WINDOW_BACK_DEL(ID_WINDOW_SCAN);
        UI_WINDOW_BACK_DEL(ID_WINDOW_REQUEST_PAGE);
        UI_WINDOW_BACK_DEL(ID_WINDOW_END_PAGE);
        break;
    default:
        return false;
        break;
    }
    return true;
}


REGISTER_UI_EVENT_HANDLER(BT_PAGE_END_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = page_end_button_ontouch,
};




static int bt_connect_info_handler(const char *type, u32 arg)
{
    printf("_func_  == %s %d\n", __func__, __LINE__);
    if (!__this) {
        return 0;
    }
    if (type && (!strcmp(type, "mac"))) {
        printf("connet mac :\n");
        put_buf((const u8 *)arg, 6);
        if (!memcmp(__this->mac, (u8 *)arg, 6)) {
            ui_text_show_index_by_id(BT_PAGE_END_TITLE_TEXT, 1);
            if (ui_core_get_disp_status_by_id(BT_PAGE_END_TEXT) != TRUE) {
                ui_show(BT_PAGE_END_TEXT);
            }
            ui_pic_show_image_by_id(BT_PAGE_END_PIC, 1);
        }
    }
    return 0;
}




static int bt_wait_status_handler(const char *type, u32 arg)
{
    printf("_func_  == %s %d\n", __func__, __LINE__);
    u8 err_flag = 0;
    if (type) {
        if (!strcmp(type, "hci_value")) {
            switch (arg) {
            case ERROR_CODE_PIN_OR_KEY_MISSING:
            case ERROR_CODE_PAGE_TIMEOUT:
                err_flag = 1;
                break;
            }
        } else if (!strcmp(type, "hci_event")) {
            switch (arg) {
            case HCI_EVENT_DISCONNECTION_COMPLETE:
                err_flag = 1;
                break;
            }
        }
    }
    if (err_flag) {
        ui_text_show_index_by_id(BT_PAGE_END_TITLE_TEXT, 2);
        if (ui_core_get_disp_status_by_id(BT_PAGE_END_TEXT) != TRUE) {
            ui_show(BT_PAGE_END_TEXT);
        }
        ui_pic_show_image_by_id(BT_PAGE_END_PIC, 2);
    }
    return 0;
}





static const struct uimsg_handl ui_wait_msg_handler[] = {
    { "bt_connect_info",  bt_connect_info_handler     }, /* 蓝牙状态 */
    { "bt_status",        bt_wait_status_handler     }, /* 蓝牙状态 */
    { NULL, NULL},      /* 必须以此结尾！ */
};



static int bt_scan_wait_mode_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:

        puts("\n***bt_scan_wait_mode_onchange***\n");
        ui_auto_shut_down_disable();
        if (!__this) {
            return false;
        }
        ui_register_msg_handler(window->elm.id, ui_wait_msg_handler);//注册消息交互的回调
        emitter_bt_connect(__this->mac);
        page_list_add(__this->mac, __this->name);
        break;
    case ON_CHANGE_RELEASE:
        ui_auto_shut_down_enable();
        break;
    default:
        return false;
    }
    return false;
}


static int bt_scan_wait_mode_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_R_MOVE:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_PAGE);
        UI_WINDOW_BACK_DEL(ID_WINDOW_SCAN);
        UI_WINDOW_BACK_DEL(ID_WINDOW_REQUEST_PAGE);
        UI_WINDOW_BACK_DEL(ID_WINDOW_END_PAGE);
        return true;
        break;
    default:
        break;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(ID_WINDOW_END_PAGE)
.onchange = bt_scan_wait_mode_onchange,
 .onkey = NULL,
  .ontouch = bt_scan_wait_mode_ontouch,
};




#endif
#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

