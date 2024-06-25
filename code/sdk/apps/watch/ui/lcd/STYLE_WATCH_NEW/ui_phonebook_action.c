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
#include "message_vm_cfg.h"

#define STYLE_NAME  JL



#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#if TCFG_UI_ENABLE_PHONEBOOK

extern void __set_bt_esco_by_watch(bool flag);
/*----------------------------------------------------------------------------*/
/**@brief    蓝牙电话本获取回调函数
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
int ui_phonebook_packet_handler(u8 type, const u8 *name, const u8 *number, const u8 *date)
{
    static u16 number_cnt = 0;
    printf("NO.%d:", number_cnt);
    number_cnt++;
    printf("type:%d ", type);

    if (name) {
        printf(" NAME:%s  ", name);
    }
    if (number) {
        /* printf("number:%s  ", number); */
    }
    if (date) {
        printf("date:%s ", date);
    }
    /* putchar('\n'); */
    if (!type || type == 255) { //只同步通讯录，不同步通话记录

    }

    if (type == 0xff) {
        number_cnt = 0;
        /* if (UI_GET_WINDOW_ID() == ID_WINDOW_PHONEBOOK_SYNC) { */
        /*     UI_HIDE_CURR_WINDOW(); */
        /*     UI_SHOW_WINDOW(ID_WINDOW_PHONEBOOK); */
        /* } */
    }

    return true;
}


#if (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE

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
        /* printf("y= %d Dy =%d ty=%d\n",grid->pos.y,y,e->pos.y); */

        /* if (ui_grid_dynamic_slide(grid, 1, y)) { */
        /*     grid->pos.x = e->pos.x; */
        /*     grid->pos.y = e->pos.y; */
        /* } */
        move_flag = 1;
        return false;//不接管消息
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


        sel_item = ui_grid_cur_item_dynamic(grid);
        /* sel_item = ui_grid_dynamic_cur_item(grid); */

        if (!sel_item) {
            return false;    //不接管消息
        }

        struct phonebook *b_list  = book_list_read_by_index(0, sel_item);
        if (!b_list) {
            break;
        }

        struct phonebook message;
        memcpy(&message, b_list, CALL_LOGS_LEN);
        g_printf("name:%s", message.name);
        /* g_printf("number:%s", message.number); */
        set_call_log_message(1, message.name, message.number, NULL);
        /* update_call_log_message(); */
        /* g_printf("pbap1_vaild_cnt:%d  pbap1_total_cnt:%d", pbap1_vaild_cnt, pbap1_total_cnt); */
        __set_bt_esco_by_watch(1);
        user_send_cmd_prepare(USER_CTRL_DIAL_NUMBER, strlen(b_list->number), (u8 *)b_list->number);

        return false;//不接管消息

        break;
    default:
        return false;
        break;
    }
    return true;//接管消息
}




static int grid_child_cb(void *_ctrl, int id, int type, int index)
{
    struct phonebook *b_list ;
    switch (type) {
    case CTRL_TYPE_PROGRESS:
        break;
    case CTRL_TYPE_MULTIPROGRESS:
        break;
    case CTRL_TYPE_TEXT:
        struct ui_text *text = (struct ui_text *)_ctrl;
        if (!strcmp(text->source, "title")) {
            text->elm.css.invisible = !!index;
            break;
        }
        if (!index) {
            text->elm.css.invisible = !index;
            break;
        }

        b_list  = book_list_read_by_index(0, index);
        if (!b_list) {
            return 0;
        }
        if (!strcmp(text->source, "name")) {
            text->attrs.offset = 0;
            text->attrs.format = "text";
            text->attrs.str    = b_list->name;
            text->attrs.strlen = strlen(b_list->name);
            text->attrs.encode = FONT_ENCODE_UTF8;
            text->attrs.flags  = FONT_DEFAULT;
            text->elm.css.invisible = 0;//
        }


        break;
    case CTRL_TYPE_NUMBER:
        struct ui_number *number = (struct ui_number *)_ctrl;
        if (!index) {
            number->text.elm.css.invisible = !index;
            break;
        }

        b_list  = book_list_read_by_index(0, index);
        if (!b_list) {
            return 0;
        }
        number->text.elm.css.invisible = !index;
        number->type = TYPE_STRING;
        number->num_str = b_list->number;
        break;
    case CTRL_TYPE_PIC:
        struct ui_pic *pic = (struct ui_pic *)_ctrl;

        if (!index) {
            pic->elm.css.invisible = !index;
            break;
        }

        b_list  = book_list_read_by_index(0, index);
        if (!b_list) {
            return 0;
        }
        ui_pic_set_image_index(pic, (index - 1) % 4);
        pic->elm.css.invisible = !index;
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
    int count = book_list_get_count(0) + 1;
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

static int phonebook_sync_start()
{
    if (get_bt_connect_status() == BT_STATUS_WAITINT_CONN) {
        return -1;
    }
#if PHONEBOOK_SAVE_INSIDE_FLASH
    book_list_clean_in_flash(0);
    book_list_clean_in_flash(1);
#endif
    user_send_cmd_prepare(USER_CTRL_PBAP_READ_ALL, 0, NULL);
    return 0;
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
        /* ui_auto_shut_down_disable(); */
        open_file_handler(0, grid->avail_item_num);

        int row = book_list_get_count(0) + 1;
        int col = 1;

        ui_set_default_handler(grid, NULL, NULL, browse_enter_child_onchange);
        ui_grid_init_dynamic(grid, &row, &col);

        /* ui_grid_dynamic_create(grid, 1, book_list_get_count(0) + 1, grid_child_cb); */
        break;
    case ON_CHANGE_RELEASE:
        /* ui_grid_dynamic_release(grid); */
        ui_set_default_handler(grid, NULL, NULL, NULL);
        /* ui_auto_shut_down_enable(); */
        close_file_handler(0);
        break;
    case ON_CHANGE_FIRST_SHOW:
        /* brows_children_init(grid); */
        break;
    default:
        return false;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(VERTLIST_1)
.onchange = browse_enter_onchane,
 .onkey = NULL,
  .ontouch = brow_switch_ontouch,
};





/*
 * 通讯录同步布局以及子控件ontouch响应
 * */
static int phonebook_sync_child_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct element *elm = (struct element *)_ctrl;
    int type = ui_id2type(elm->id);

    switch (type) {
    case CTRL_TYPE_PROGRESS:
        break;
    case CTRL_TYPE_MULTIPROGRESS:
        break;
    case CTRL_TYPE_TEXT:
        struct ui_text *text = (struct ui_text *)_ctrl;

        /* printf(">>>>>>>>>>>>>%s %d %d %x\n", __FUNCTION__, __LINE__, e->event, text->elm.id); */
        switch (e->event) {
        case ELM_EVENT_TOUCH_DOWN:
            if (!strcmp(text->source, "yes")) {
                UI_HIDE_CURR_WINDOW();
                UI_SHOW_WINDOW(ID_WINDOW_PHONEBOOK);
                /* if (phonebook_sync_start()) { */
                /*  */
                /*     g_printf("line:%d", __LINE__); */
                /*     ui_send_event(KEY_CHANGE_PAGE, 1); */
                /* } */
            }

            if (!strcmp(text->source, "no")) {
                UI_HIDE_CURR_WINDOW();
                UI_SHOW_WINDOW(ID_WINDOW_PHONEBOOK);
            }

            break;
        }
        break;
    case CTRL_TYPE_NUMBER:
        break;
    case CTRL_TYPE_TIME:
        break;
    }

    return 0;
}



static int phonebook_sync_onchange(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_INIT:
        ui_set_default_handler(ctr, phonebook_sync_child_ontouch, NULL, NULL);
        break;
    case ON_CHANGE_RELEASE:
        ui_set_default_handler(ctr, NULL, NULL, NULL);
        break;
    default:
        return FALSE;
    }
    return FALSE;
}

REGISTER_UI_EVENT_HANDLER(PHONEBOOK_SYNC_LAYOUT)
.onchange = phonebook_sync_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};



static int record_grid_child_cb(void *_ctrl, int id, int type, int index)
{
    struct phonebook *b_list = book_list_read_by_index(1, index);
    switch (type) {
    case CTRL_TYPE_PROGRESS:
        break;
    case CTRL_TYPE_MULTIPROGRESS:
        break;
    case CTRL_TYPE_TEXT:
        struct ui_text *text = (struct ui_text *)_ctrl;

        if (!strcmp(text->source, "title")) {
            text->elm.css.invisible = !!index;
            break;
        }

        if (!index) {
            text->elm.css.invisible = !index;
            break;
        } else if (index == book_list_get_count(1) + 2 - 1) {
            if (!strcmp(text->source, "last")) {
                text->elm.css.invisible = 0;
            } else {
                text->elm.css.invisible = 1;
            }
            break;
        } else {
            if (!strcmp(text->source, "last")) {
                text->elm.css.invisible = 1;
                break;
            }
        }

        if (!b_list) {
            return 0;
        }

        text->attrs.offset = 0;
        text->attrs.format = "text";

        if (strcmp(b_list->name, "unknow")) {
            text->attrs.str    = b_list->name;
            text->attrs.strlen = strlen(b_list->name);
        } else {
            text->attrs.str    = b_list->number;
            text->attrs.strlen = strlen(b_list->number);
        }

        text->attrs.encode = FONT_ENCODE_UTF8;
        text->attrs.flags  = FONT_DEFAULT;
        text->elm.css.invisible = 0;
        break;
    case CTRL_TYPE_NUMBER:
        struct ui_number *number = (struct ui_number *)_ctrl;
        if (!index  || (index == book_list_get_count(1) + 2 - 1)) {
            number->text.elm.css.invisible = 1;
            break;
        }
        if (!b_list) {
            return 0;
        }
        number->type = TYPE_STRING;
        number->num_str = b_list->date + 11;
        number->text.elm.css.invisible = !index;
        break;
    case CTRL_TYPE_TIME:
        break;
    case CTRL_TYPE_PIC:
        struct ui_pic *pic = (struct ui_pic *)_ctrl;
        if (!index || (index == book_list_get_count(1) + 2 - 1)) {
            pic->elm.css.invisible = 1;
            break;
        }
        if (!b_list) {
            return 0;
        }
        ui_pic_set_image_index(pic, b_list->type - 1);
        pic->elm.css.invisible = !index;
        break;
    }
    return 0;
}

static int record_brows_children_init(struct ui_grid *grid)
{
    struct element *k;
    if (!grid) {
        return 0;

    }
    int count = book_list_get_count(1) + 2;
    for (int i = 0; i < grid->avail_item_num; i++) {
        if (i < count) { //最后一下是删
            list_for_each_child_element(k, &grid->item[i].elm) {
                record_grid_child_cb(k, k->id, ui_id2type(k->id), i);
            }
        } else {
            grid->item[i].elm.css.invisible = 1;
        }
    }
    return 0;
}



static int record_browse_enter_child_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    int type = ui_id2type(elm->id);
    if (event == ON_CHANGE_UPDATE_ITEM) {
        int index = (u32)arg;
        record_grid_child_cb(elm, elm->id, type, index);
    }
    return 0;
}


static int record_browse_enter_onchane(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        open_file_handler(1, grid->avail_item_num);

        if (!book_list_get_count(1)) {
            /* ui_send_event(KEY_CHANGE_PAGE, 0); */
            /* break; */
        }
        /* ui_auto_shut_down_disable(); */

        int row = book_list_get_count(1) + 2;
        int col = 1;


        ui_set_default_handler(grid, NULL, NULL, record_browse_enter_child_onchange);
        ui_grid_init_dynamic(grid, &row, &col);


        /* ui_grid_dynamic_create(grid, 1, book_list_get_count(1) + 2, record_grid_child_cb); */
        break;
    case ON_CHANGE_RELEASE:
        /* ui_grid_dynamic_release(grid); */
        ui_set_default_handler(grid, NULL, NULL, NULL);
        /* ui_auto_shut_down_enable(); */
        close_file_handler(1);
        break;
    case ON_CHANGE_FIRST_SHOW:
        /* record_brows_children_init(grid); */
        break;
    default:
        return false;
    }
    return false;
}



static int record_brow_switch_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    int sel_item;
    /* sel_item = ui_grid_cur_item(grid); */
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_MOVE:
        /* int x = e->pos.x - grid->pos.x; */
        /* int y = e->pos.y - grid->pos.y; */
        /*  */
        /* if (ui_grid_dynamic_slide(grid, 1, y)) { */
        /*     grid->pos.x = e->pos.x; */
        /*     grid->pos.y = e->pos.y; */
        /* } */
        move_flag = 1;
        return false;//不接管消息
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


        sel_item = ui_grid_cur_item_dynamic(grid);
        /* sel_item = ui_grid_dynamic_cur_item(grid); */

        if (!sel_item) {
            return false;    //不接管消息
        }

        if (sel_item == book_list_get_count(1) + 2 - 1) {
            printf("add_del_func_in_here\n");
            delete_call_file();
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(0);
            return true;
        }

        struct phonebook *b_list  = book_list_read_by_index(1, sel_item);
        if (!b_list) {
            break;
        }

        struct phonebook message;
        memcpy(&message, b_list, CALL_LOGS_LEN);
        g_printf("name:%s", message.name);
        /* g_printf("number:%s", message.number); */
        set_call_log_message(1, message.name, message.number, NULL);
        /* update_call_log_message(); */
        /* g_printf("pbap1_vaild_cnt:%d  pbap1_total_cnt:%d", pbap1_vaild_cnt, pbap1_total_cnt); */
        __set_bt_esco_by_watch(1);
        user_send_cmd_prepare(USER_CTRL_DIAL_NUMBER, strlen(b_list->number), (u8 *)b_list->number);


        return false;//不接管消息

        break;
    default:
        return false;
        break;
    }
    return true;//接管消息
}



REGISTER_UI_EVENT_HANDLER(VERTLIST_2)
.onchange = record_browse_enter_onchane,
 .onkey = NULL,
  .ontouch = record_brow_switch_ontouch,
};





#endif
#endif /* #if (!TCFG_LUA_ENABLE) */
#else
/* int phonebook_get_name_by_number(u8 *number, u8 *name) */
/* { */
/* return 0; */
/* } */

/* void set_call_log_type(u8 type) */
/* { */

/* } */
#endif
#endif
