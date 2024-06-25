#include "app_config.h"
#include "ui/ui_api.h"
#include "ui/ui.h"
#include "ui/ui_style.h"
#include "app_task.h"
#include "system/timer.h"
#include "btstack/avctp_user.h"
#include "app_main.h"
#include "init.h"
#include "key_event_deal.h"
#include "bt_emitter.h"



#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_UI_ENABLE_BT_SCAN

extern void key_ui_takeover(u8 on);

#define STYLE_NAME  JL


#if TCFG_USER_EMITTER_ENABLE//带有屏幕的方案根据UI选项连接

extern u8 get_bt_connect_status(void);
extern u8 bt_search_status();
extern void bt_search_device(void);
extern void bt_list_clean();
extern int bt_menu_list_count();
extern void bt_menu_list_add(u8 *name, u8 *mac, u8 rssi);
extern int page_list_add(u8 *mac, u8 *name);//添加收藏
extern u8 *bt_list_get_mac_by_number(u8 num);


struct bt_info {
    u8 name[32];
    u8 mac[6];
    u8 rssi;
    u8 num;
};


struct bt_list {
    struct list_head entry;
    struct bt_info info;
};


struct bt_list_hd {
    struct list_head *head;
    int count;
};

struct bt_list_hd *hd = NULL;

static struct list_head head = LIST_HEAD_INIT(head);


static const int TEXT_LIST[] = {
    BT_SCAN_TEXT_0,
    BT_SCAN_TEXT_1,
    BT_SCAN_TEXT_2,
    BT_SCAN_TEXT_3,
    BT_SCAN_TEXT_4,
    BT_SCAN_TEXT_5,
};

static u32 LAYOUT_FNAME_LIST_ID[] = {
    BT_SCAN_LIST_0,
    BT_SCAN_LIST_1,
    BT_SCAN_LIST_2,
    BT_SCAN_LIST_3,
    BT_SCAN_LIST_4,
    BT_SCAN_LIST_5,
};

#define NAME_1 "小米蓝牙1"
#define NAME_2 "小米蓝牙2"
#define NAME_3 "小米蓝牙3"
#define NAME_4 "小米蓝牙4"
#define NAME_5 "小米蓝牙5"
#define NAME_6 "小米蓝牙6"
#define NAME_7 "小米蓝牙7"
#define NAME_8 "小米蓝牙8"
#define NAME_9 "小米蓝牙9"
#define NAME_10 "小米蓝牙10"
#define NAME_11 "小米蓝牙11"
#define NAME_12 "小米蓝牙12"
#define NAME_13 "小米蓝牙13"

#define TEXT_NAME_LEN (32)
#define TEXT_PAGE     (sizeof(TEXT_LIST)/sizeof(TEXT_LIST[0]))
/* #define BT_SCAN_GRID_ID BASEFORM_15 */



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



int bt_menu_list_init()
{
    if (hd) {
        return 0;
    }
    hd = zalloc(sizeof(struct bt_list_hd));
    hd->head = &head;
    hd->count = 0;
#if 0
    u8 mac[6] = {0x01, 0x02, 0x03, 0x4, 0x05, 0x06};
    bt_menu_list_add(NAME_1, mac, 88);
    bt_menu_list_add(NAME_2, mac, 88);
    bt_menu_list_add(NAME_3, mac, 88);
    bt_menu_list_add(NAME_4, mac, 88);
    bt_menu_list_add(NAME_5, mac, 88);
    bt_menu_list_add(NAME_6, mac, 88);
    bt_menu_list_add(NAME_7, mac, 88);
    bt_menu_list_add(NAME_8, mac, 88);
    bt_menu_list_add(NAME_9, mac, 88);
    bt_menu_list_add(NAME_10, mac, 88);
    bt_menu_list_add(NAME_11, mac, 88);
    bt_menu_list_add(NAME_12, mac, 88);
    bt_menu_list_add(NAME_13, mac, 88);
#endif

    return 0;
}

late_initcall(bt_menu_list_init);


static int __list_flush(int from_index);
void bt_menu_list_add(u8 *name, u8 *mac, u8 rssi)
{
    if (!hd) {
        return;
    }
    /* 这里过滤搜索到的蓝牙设备 */
    /* 如果mac地址已经在链表中，就不需要添加到链表里了 */
    /* 但如果新搜索到的设备有设备名称，就需更新设备名 */
    struct bt_list *p, *n;
    /* list_for_each_entry_safe(p, n, hd->head, entry) { */
    /* printf("bt name:%s\n", p->info.name); */
    /* put_buf(p->info.mac, 6); */
    /* } */
    local_irq_disable();
    list_for_each_entry_safe(p, n, hd->head, entry) {
        if (!memcmp(p->info.mac, mac, 6)) {
            printf("get new bt device is in list, throw it:");
            put_buf(mac, 6);
            if (name) {
                printf("device rename:%s\n", name);
                sprintf((char *)p->info.name, "%s", name);
            }
            local_irq_enable();
            return;
        }
    }
    local_irq_enable();
    /*--------------------------*/

    struct bt_list *b_list = zalloc(sizeof(struct bt_list));
    ASSERT(b_list, "bt list malloc err");



    if (name) {
        sprintf((char *)b_list->info.name, "%s", name);
    } else {
        sprintf((char *)b_list->info.name, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
    }

    memcpy(b_list->info.mac, mac, 6);
    b_list->info.num = hd->count;
    b_list->info.rssi = rssi;
    local_irq_disable();
    list_add_tail(&b_list->entry, hd->head);
    hd->count++;
    local_irq_enable();
    UI_MSG_POST("bt_list:n=%4", b_list->info.num);
}


int bt_list_updata_handler(const char *type, u32 arg)
{
    if (bt_menu_list_count() <= TEXT_PAGE) {
        __list_flush(0);
    }
    return 0;
}
int bt_menu_list_count()
{
    if (!hd) {
        return 0;
    }
    return  hd->count;
}


void bt_list_clean()
{
    if (!hd || !(hd->count)) {
        return ;
    }
    local_irq_disable();
    struct bt_list *p, *n;
    list_for_each_entry_safe(p, n, hd->head, entry) {
        list_del(&p->entry);
        free(p);
    }
    hd->count  = 0;
    local_irq_enable();
}

u8 *bt_list_get_name_by_number(u8 num)
{
    if (!hd || !(hd->count)) {
        return NULL;
    }
    struct bt_list *p, *n;
    list_for_each_entry_safe(p, n, hd->head, entry) {
        if (p->info.num == num) {
            return p->info.name;
        }

    }
    return NULL;
}

u8 *bt_list_get_mac_by_number(u8 num)
{
    if (!hd || !(hd->count)) {
        return NULL;
    }
    struct bt_list *p, *n;
    list_for_each_entry_safe(p, n, hd->head, entry) {
        if (p->info.num == num) {
            return p->info.mac;
        }

    }
    return NULL;
}



#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)



static int __select_enter(int from_index)
{
    int one_page_num = sizeof(TEXT_LIST) / sizeof(TEXT_LIST[0]);
    int i = 0;
    i = from_index % one_page_num;
    u8 *mac = bt_list_get_mac_by_number(from_index);
    if (mac) {
        printf("bt connet start mac:\n");
        put_buf(mac, 6);
        memcpy(__this->mac, mac, 6);
        sprintf((char *)__this->name, "%s", bt_list_get_name_by_number(from_index));
        /* bt_emitter_stop_search_device(); */
        emitter_bt_connect(mac);
    }
    return 0;
}


static int __list_flush(int from_index)
{
    int one_page_num = sizeof(TEXT_LIST) / sizeof(TEXT_LIST[0]);
    int i = 0;
    int end_index = from_index + one_page_num;

    for (i = 0; i < sizeof(TEXT_LIST) / sizeof(TEXT_LIST[0]); i++) {
        memset(__this->text_list[i].fname, 0, TEXT_NAME_LEN);
        __this->text_list[i].len = TEXT_NAME_LEN;
    }

    if (!hd || !(hd->count)) {
        return  0;
    }

    i = 0 ;
    struct bt_list *p, *n;
    list_for_each_entry_safe(p, n, hd->head, entry) {
        if (p->info.num >= from_index && p->info.num < end_index) {
            __this->text_list[i].len =  strlen((const char *)p->info.name);
            memcpy(__this->text_list[i].fname, p->info.name, __this->text_list[i].len);
            memcpy(__this->text_list[i].mac, p->info.mac, 6);
            i++;
        }
    }

    for (i = 0; i < sizeof(TEXT_LIST) / sizeof(TEXT_LIST[0]); i++) {
        ui_text_set_textu_by_id(TEXT_LIST[i], (const char *)__this->text_list[i].fname, __this->text_list[i].len, FONT_DEFAULT);
    }
    return 0;
}

static int list_browse_enter_onchane(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    int fnum = 0;

    switch (e) {
    case ON_CHANGE_INIT:
        ui_auto_shut_down_disable();
        bt_list_clean();
        g_printf("************   emitter \n");
        /* emitter_or_receiver_switch(BT_EMITTER_EN); */
        bt_emitter_start_search_device();
        g_printf("************   emitter start\n");
        break;
    case ON_CHANGE_RELEASE:
        /* g_printf("************   reveive  \n"); */
        /* emitter_or_receiver_switch(BT_RECEIVER_EN); */
        bt_emitter_stop_search_device();
        bt_list_clean();
        ui_auto_shut_down_enable();
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
    __this->cur_total = bt_menu_list_count();
    if (!__this->cur_total) {
        __this->cur_total = 1;
    }
    switch (e->value) {
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

REGISTER_UI_EVENT_HANDLER(BT_SCAN_GRID_ID)
.onchange = list_browse_enter_onchane,
 .onkey = list_browse_onkey,
  .ontouch = NULL,
};




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
    return 0;
}



static int bt_connect_info_handler(const char *type, u32 arg)
{
    printf("_func_  == %s %d\n", __func__, __LINE__);
    if (type && (!strcmp(type, "mac"))) {
        printf("connet mac :\n");
        put_buf((const u8 *)arg, 6);
        if (!memcmp(__this->mac, (u8 *)arg, 6)) {
            page_list_add(__this->mac, __this->name);
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(PAGE_9);
        }
    }
    return 0;


}

static const struct uimsg_handl ui_msg_handler[] = {
    { "bt_list",          bt_list_updata_handler     }, /* 更新蓝牙列表 */
    { "bt_status",        bt_status_handler     }, /* 蓝牙状态 */
    { "bt_connect_info",  bt_connect_info_handler     }, /* 蓝牙状态 */
    { NULL, NULL},      /* 必须以此结尾！ */
};


static int bt_scan_mode_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        puts("\n***bt_scan_mode_onchange***\n");
        /*
         * 注册APP消息响应
         */
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
        break;
    default:
        return false;
    }
    return false;
}



REGISTER_UI_EVENT_HANDLER(PAGE_8)
.onchange = bt_scan_mode_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};




#endif
#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */
