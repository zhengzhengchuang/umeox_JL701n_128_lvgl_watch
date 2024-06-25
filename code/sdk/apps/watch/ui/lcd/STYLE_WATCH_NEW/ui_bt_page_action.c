#include "ui/ui.h"
#include "app_config.h"
#include "ui/ui_api.h"
#include "ui/ui_style.h"
#include "app_action.h"
#include "btstack/avctp_user.h"
#include "system/timer.h"
#include "init.h"
#include "bt_emitter.h"


#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#if TCFG_UI_ENABLE_BT_PAGE_ACTION


extern void key_ui_takeover(u8 on);

#define STYLE_NAME  JL

#if TCFG_USER_EMITTER_ENABLE//带有屏幕的方案根据UI选项连接

extern int page_list_get_count();//获得收藏数
extern u8 *page_list_read_name(u8 *mac, int index); //index从1开始,获取收藏的蓝牙信息
extern int page_list_del(int index);//index从1开始,删除收藏

static const int TEXT_LIST[] = {
    BT_PAGE_TEXT0,
    BT_PAGE_TEXT1,
    BT_PAGE_TEXT2,
    BT_PAGE_TEXT3,
    BT_PAGE_TEXT4,
    BT_PAGE_TEXT5,
};



static u32 LAYOUT_FNAME_LIST_ID[] = {
    BT_PAGE_LAYOUT0,
    BT_PAGE_LAYOUT1,
    BT_PAGE_LAYOUT2,
    BT_PAGE_LAYOUT3,
    BT_PAGE_LAYOUT4,
    BT_PAGE_LAYOUT5,
};


#define TEXT_PAGE     (sizeof(TEXT_LIST)/sizeof(TEXT_LIST[0]))
#define TEXT_NAME_LEN (32)

struct text_name_t {
    u8 len;
    u8 fname[TEXT_NAME_LEN];
    u8 mac[6];
};

struct grid_set_info {
    struct text_name_t text_list[TEXT_PAGE];
    u16 flist_index;
    u16 cur_total;
    u8 name[TEXT_NAME_LEN];
    u8 mac[6];
};

static struct grid_set_info *handler = NULL;
#define __this 	handler
#define sizeof_this     (sizeof(struct grid_set_info))





static int __select_enter(int from_index)
{
    int one_page_num = sizeof(TEXT_LIST) / sizeof(TEXT_LIST[0]);
    int i = from_index % one_page_num;
    if (from_index) {
        u8 *mac = __this->text_list[i].mac;
        if (mac) {
            printf("bt connet start mac:\n");
            put_buf(mac, 6);
            memcpy(__this->mac, __this->text_list[i].mac, 6);
            sprintf((char *)__this->name, "%s", __this->text_list[i].fname);
            emitter_bt_connect(mac);
        }
    } else {
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(PAGE_9);
    }

    /* ui_hide(BT_PAGE_LAYOUT); */
    return 0;
}


static int __list_flush(int from_index)
{
    int one_page_num = sizeof(TEXT_LIST) / sizeof(TEXT_LIST[0]);
    int i = 0;
    int end_index = from_index + one_page_num;
    u8 *name;


    for (i = 0; i < sizeof(TEXT_LIST) / sizeof(TEXT_LIST[0]); i++) {
        memset(__this->text_list[i].fname, 0, TEXT_NAME_LEN);
        __this->text_list[i].len = TEXT_NAME_LEN;
    }

    if (!from_index) {
        sprintf((char *)__this->text_list[0].fname, "%s", "back...");
        __this->text_list[0].len = strlen((const char *)__this->text_list[0].fname);
        i = 1 ;
    } else {
        i = 0 ;
    }

    for (int j = from_index; j < end_index; j++) {
        name =  page_list_read_name(__this->text_list[i].mac, j);
        if (name) {
            __this->text_list[i].len = strlen((const char *)name);
            memcpy(__this->text_list[i].fname, name, __this->text_list[i].len);
            i++;
        }
    }



    for (i = 0; i < sizeof(TEXT_LIST) / sizeof(TEXT_LIST[0]); i++) {
        ui_text_set_textu_by_id(TEXT_LIST[i], (const char *)__this->text_list[i].fname, __this->text_list[i].len, FONT_DEFAULT);
    }
    return 0;
}


static void ui_page_list_del(int from_index)
{
    if (from_index) {
        page_list_del(from_index);
        __this->cur_total = page_list_get_count() + 1;
    }
    /* ui_hide(BT_PAGE_LAYOUT); */
}
static int list_browse_enter_onchane(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    int fnum = 0;

    switch (e) {
    case ON_CHANGE_INIT:
        //断开连接
        __this->cur_total = page_list_get_count() + 1;

        /* user_send_cmd_prepare(USER_CTRL_POWER_OFF, 0, NULL);// */
        /* user_send_cmd_prepare(USER_CTRL_WRITE_SCAN_DISABLE, 0, NULL); */
        /* user_send_cmd_prepare(USER_CTRL_WRITE_CONN_DISABLE, 0, NULL); */
        break;
    case ON_CHANGE_RELEASE:
        break;
    case ON_CHANGE_FIRST_SHOW:
        __this->flist_index = 0;
        ui_set_call(__list_flush, __this->flist_index);
        break;
    default:
        return false;
    }
    return false;
}


static int list_browse_onkey(void *ctr, struct element_key_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    printf("ui key %s %d\n", __FUNCTION__, e->value);
    int sel_item;
    sel_item = ui_grid_cur_item(grid);
    switch (e->value) {
    case  KEY_MENU:
        if (sel_item) {
            ui_page_list_del(__this->flist_index);
            if (__this->flist_index >= __this->cur_total) { //判断最后一行
                ui_no_highlight_element_by_id(LAYOUT_FNAME_LIST_ID[sel_item]);
                __this->flist_index = __this->cur_total - 1;
                __list_flush(__this->flist_index / TEXT_PAGE * TEXT_PAGE);
                ui_grid_set_item(grid, sel_item - 1);
                ui_highlight_element_by_id(LAYOUT_FNAME_LIST_ID[sel_item - 1]);
            } else {
                __list_flush(__this->flist_index / TEXT_PAGE * TEXT_PAGE);
            }
        }
        break;

    case KEY_OK:
        __select_enter(__this->flist_index);
        break;
    case KEY_DOWN:
        sel_item++;
        __this->flist_index += 1;
        if (sel_item >= __this->cur_total || __this->flist_index >= __this->cur_total) {
            //大于文件数
            ui_no_highlight_element_by_id(LAYOUT_FNAME_LIST_ID[sel_item - 1]);
            __this->flist_index = 0;
            __list_flush(__this->flist_index);
            ui_grid_set_item(grid, 0);
            ui_highlight_element_by_id(LAYOUT_FNAME_LIST_ID[0]);
            return TRUE;
        }

        if (sel_item >= TEXT_PAGE) {
            ui_no_highlight_element_by_id(LAYOUT_FNAME_LIST_ID[sel_item - 1]);
            __list_flush(__this->flist_index);
            ui_grid_set_item(grid, 0);
            ui_highlight_element_by_id(LAYOUT_FNAME_LIST_ID[0]);
            return true; //不返回到首项
        }

        return FALSE;

        break;
    case KEY_UP:
        if (sel_item == 0) {
            __this->flist_index = __this->flist_index ? __this->flist_index - 1 : __this->cur_total - 1;
            ui_no_highlight_element_by_id(LAYOUT_FNAME_LIST_ID[sel_item]);
            __list_flush(__this->flist_index / TEXT_PAGE * TEXT_PAGE);
            ui_grid_set_item(grid, __this->flist_index % TEXT_PAGE);
            ui_highlight_element_by_id(LAYOUT_FNAME_LIST_ID[__this->flist_index % TEXT_PAGE]);
            return true; //不跳转到最后一项
        }
        __this->flist_index--;
        return FALSE;
        break;
    default:
        return false;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(BT_PAGE_LIST)
.onchange = list_browse_enter_onchane,
 .onkey = list_browse_onkey,
  .ontouch = NULL,
};



static int bt_connect_info_handler(const char *type, u32 arg)
{
    printf("_func_  == %s %d\n", __func__, __LINE__);
    if (type && (!strcmp(type, "mac"))) {
        printf("connet mac :\n");
        put_buf((const u8 *)arg, 6);
        if (!memcmp(__this->mac, (u8 *)arg, 6)) {
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(PAGE_9);
        }
    }
    return 0;


}

static const struct uimsg_handl ui_msg_handler[] = {
    { "bt_connect_info",  bt_connect_info_handler     }, /* 蓝牙状态 */
    { NULL, NULL},      /* 必须以此结尾！ */
};


static int bt_page__mode_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        puts("\n***bt_page__mode_onchange***\n");
        /*
         * 注册APP消息响应
         */
        ui_auto_shut_down_disable();
        if (!__this) {
            __this = zalloc(sizeof_this);
        }
        ui_register_msg_handler(window->elm.id, ui_msg_handler);//注册消息交互的回调

        break;
    case ON_CHANGE_RELEASE:
        if (__this) {
            free(__this);
            __this = NULL;
        }
        ui_auto_shut_down_enable();
        break;
    default:
        return false;
    }
    return false;
}



REGISTER_UI_EVENT_HANDLER(PAGE_11)
.onchange = bt_page__mode_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};




#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

