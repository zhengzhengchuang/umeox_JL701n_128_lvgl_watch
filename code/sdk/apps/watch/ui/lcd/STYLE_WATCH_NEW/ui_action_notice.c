#include "app_config.h"
#include "ui/ui_api.h"
#include "ui/ui.h"
#include "ui/ui_style.h"
#include "app_task.h"
#include "system/timer.h"
#include "app_main.h"
#include "init.h"
#include "key_event_deal.h"
#include "ui_vm/ui_vm.h"
#include "message_vm_cfg.h"

/* #pragma bss_seg(".ui_not_bss") */
/* #pragma data_seg(".ui_not_data") */





#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"


#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_UI_ENABLE_NOTICE

/* #define STYLE_NAME  DIAL */
#define STYLE_NAME  JL

REGISTER_UI_STYLE(STYLE_NAME)

#define ui_grid_for_id(id) \
	({ \
		struct element *elm = ui_core_get_element_by_id(id); \
	 	elm ? container_of(elm, struct ui_grid, elm): NULL; \
	 })

#define MESSAGE_DETAIL_PAGE     PAGE_65

#define IOS_PACKAGE_NAME_SYS_MESSAGE    "com.apple.MobileSMS"
#define IOS_PACKAGE_NAME_WECHAT         "com.tencent.xin"
#define IOS_PACKAGE_NAME_QQ             "com.tencent.mqq"
#define IOS_PACKAGE_NAME_DING_DING      "com.laiwang.DingTalk"


#define Msg_Max_Num     10          /**< 支持存储的最大消息数量 */
#define MESSAGE_PACKAGENAME_LEN    32   /**< 通知消息包名数据长度 */
#define MESSAGE_APPIDENTIFIER_LEN   1   /**< 通知消息app标识符数据长度 0:通用 1:短信 2:微信 3:QQ 4:钉钉 其他:未定义 */
#define MESSAGE_TITLE_LEN   37          /**< 通知消息标题数据长度 */
#define Msg_Ctx_Len 441         /**< 通知消息内容数据长度 */
#define MESSAGE_TIMESTAMP_LEN  4        /**< 通知消息时间戳数据长度 */

struct __UI_MESSAGE {
    u32 TimeStamp;
    u8 packagename[MESSAGE_PACKAGENAME_LEN];
    u8 AppIdentifier;
    u8 title[MESSAGE_TITLE_LEN];
    u8 content[Msg_Ctx_Len];

    u8 title_encode;
    u8 title_flags;
    u16 title_icon_Xcoordinate;
    u16 title_text_Xcoordinate;
};

extern void ui_send_event(u16 event, u32 val);
extern u16 ui_get_text_width_and_height(u8 encode, u8 *str, u16 strlen, u16 elm_width, u16 elm_height, u8 flags, char *value_type);


static struct __UI_MESSAGE temp_message_buffer;
static u8 message_select_index = 0;
static u16 timestamp_len_for_ancs;
static u16 packagename_len_for_ancs;
static u16 AppIdentifier_len_for_ancs;
static u16 title_len_for_ancs;
static u16 content_len_for_ancs;
static u8 create_control_by_menu;


static u8 message_num = 0;


static u8 message_index[3];

#define MESSAGE_MALLOC_ENABLE       1

#define MESSAGE_STORE_PATH      "storage/sd1/C/DOWNLOAD/message.txt"

static struct __UI_MESSAGE *message_store_buffer[3];


static u32 message_offset_tab[Msg_Max_Num];
static u32 timestamp_tab[Msg_Max_Num];
static u16 message_data_len[Msg_Max_Num];


void notice_set_info_from_ancs(void *name, void *data, u16 len)
{
    u32 copy_len;
    u8 title_set_flag = 0;

    if (!strcmp((char *)name, "UID")) {
        timestamp_len_for_ancs = len;
        memcpy(&temp_message_buffer.TimeStamp, (u8 *)data, sizeof(temp_message_buffer.TimeStamp));
        /* log_info("TimeStamp:%d", temp_message_buffer.TimeStamp); */
    } else if (!strcmp((char *)name, "AppIdentifier")) {
        copy_len = (len + 1) > MESSAGE_PACKAGENAME_LEN ? MESSAGE_PACKAGENAME_LEN : (len + 1);
        packagename_len_for_ancs = copy_len;
        memset(temp_message_buffer.packagename, 0, MESSAGE_PACKAGENAME_LEN);
        memcpy(temp_message_buffer.packagename, (u8 *)data, copy_len);
        /* log_info("packagename:%s", temp_message_buffer.packagename); */
        if (!strcmp((char *)data, IOS_PACKAGE_NAME_SYS_MESSAGE)) {
            temp_message_buffer.AppIdentifier = 1;
        } else if (!strcmp((char *)data, IOS_PACKAGE_NAME_WECHAT)) {
            temp_message_buffer.AppIdentifier = 2;
        } else if (!strcmp((char *)data, IOS_PACKAGE_NAME_QQ)) {
            temp_message_buffer.AppIdentifier = 3;
        } else if (!strcmp((char *)data, IOS_PACKAGE_NAME_DING_DING)) {
            temp_message_buffer.AppIdentifier = 4;
        } else {
            temp_message_buffer.AppIdentifier = 0;
        }
        AppIdentifier_len_for_ancs = 1;
        /* log_info("AppIdentifier:%d", temp_message_buffer.AppIdentifier); */
    } else if (!strcmp((char *)name, "IDTitle")) {
        copy_len = (len + 1) > MESSAGE_TITLE_LEN ? MESSAGE_TITLE_LEN : (len + 1);
        title_len_for_ancs = copy_len;
        memset(temp_message_buffer.title, 0, MESSAGE_TITLE_LEN);
        memcpy(temp_message_buffer.title, (u8 *)data, copy_len);
        log_info("%s %d %d\n", data, len, __LINE__);
        title_set_flag = 1;
    } else if (!strcmp((char *)name, "IDMessage")) {
        copy_len = (len + 1) > Msg_Ctx_Len ? Msg_Ctx_Len : (len + 1);
        content_len_for_ancs = copy_len;
        memset(temp_message_buffer.content, 0, Msg_Ctx_Len);
        memcpy(temp_message_buffer.content, (u8 *)data, copy_len);
        log_info("%s %d %d\n", data, len, __LINE__);
    } else if (!strcmp((char *)name, "IDDate")) {
    }

}


u8 message_data_analysis(int index, u8 store_buf_index)
{
    u16 L;
    u8 T;
    u8 *V;
    u8 seq;
    u8 num = flash_message_count();
    u16 offset = 0;
    u32 copy_len;
    int len;
    struct __UI_MESSAGE *temp_message_buf = 0;
    u8 *data;

    if (index >= flash_message_count()) {
        log_info("index over");
        return 1;
    }

    data = zalloc(512);
    if (!data) {
        log_info("data malloc fail");
        return 1;
    }

    len = flash_message_read_by_index(flash_message_count() - 1 - index, data, 512);

    if (!len) {
        log_info("len err is 0 < %s %d>\n", __FUNCTION__, __LINE__);
    }


    if (!message_store_buffer[store_buf_index]) {
        message_store_buffer[store_buf_index] = (struct __UI_MESSAGE *)zalloc(sizeof(struct __UI_MESSAGE));
        if (!message_store_buffer[store_buf_index]) {
            log_info("message_store_buffer malloc fail");
            return 1;
        }
    }
    temp_message_buf = message_store_buffer[store_buf_index];
    memset(temp_message_buf, 0, sizeof(struct __UI_MESSAGE));
    /* log_info("index:%d store_buf_index:%d",index,store_buf_index); */
    while (offset < len) {
        L = (data[offset] << 8) | data[offset + 1];
        T = data[offset + 2];
        V = &data[offset + 3];
        if (T == 0) {
            copy_len = (L - 1) > MESSAGE_TIMESTAMP_LEN ? MESSAGE_TIMESTAMP_LEN : (L - 1);
            memcpy(&temp_message_buf->TimeStamp, V, copy_len);
            /* log_info("TimeStamp:0x%x",temp_message_buf->TimeStamp); */
        } else if (T == 1) {
            copy_len = (L - 1) > MESSAGE_PACKAGENAME_LEN ? MESSAGE_PACKAGENAME_LEN : (L - 1);
            memcpy(temp_message_buf->packagename, V, copy_len);
            /* log_info("packagename:%s", temp_message_buf->packagename); */
        } else if (T == 2) {
            copy_len = (L - 1) > MESSAGE_APPIDENTIFIER_LEN ? MESSAGE_APPIDENTIFIER_LEN : (L - 1);
            memcpy(&temp_message_buf->AppIdentifier, V, copy_len);
            /* log_info("AppIdentifier:%d", temp_message_buf->AppIdentifier); */
        } else if (T == 3) {
            copy_len = (L - 1) > MESSAGE_TITLE_LEN ? MESSAGE_TITLE_LEN : (L - 1);
            memcpy(temp_message_buf->title, V, copy_len);
            /* log_info("title:%s", temp_message_buf->title); */
        } else if (T == 4) {
            copy_len = (L - 1) > Msg_Ctx_Len ? Msg_Ctx_Len : (L - 1);
            memcpy(temp_message_buf->content, V, copy_len);
            /* log_info("copy_len:%d",copy_len); */
            /* log_info("content:%s", temp_message_buf->content); */
        }
        offset += L + 2;
    }

    //图标和标题居中处理
    u16 title_width;
    u16 icon_width = 24;            //图标控件宽度
    u16 title_elm_width = 160;      //标题控件宽度
    u16 title_elm_height = 40;      //标题控件高度
    u16 total_width;
    temp_message_buf->title_encode = FONT_ENCODE_UTF8;
    temp_message_buf->title_flags = FONT_DEFAULT;
    title_width = ui_get_text_width_and_height(temp_message_buf->title_encode,
                  temp_message_buf->title,
                  strlen(temp_message_buf->title),
                  title_elm_width, title_elm_height,
                  temp_message_buf->title_flags,
                  "width");
    total_width = title_width + icon_width;
    temp_message_buf->title_icon_Xcoordinate = (240 - total_width) / 2;   //根据屏幕大小、标题宽度、图标宽度进行居中处理
    temp_message_buf->title_text_Xcoordinate = temp_message_buf->title_icon_Xcoordinate + icon_width;

    free(data);
    return 0;
}

void notice_add_info_from_ancs()
{
    s8 i;
    u32 offset = 0;
    int len;
    u8 *temp_ptr = 0;
    u32 mess_data_len = timestamp_len_for_ancs + packagename_len_for_ancs +
                        AppIdentifier_len_for_ancs + title_len_for_ancs +
                        content_len_for_ancs + 3 * 5;

    temp_ptr = zalloc(mess_data_len);
    if (!temp_ptr) {
        log_info("temp_ptr malloc fail");
        return;
    }

    temp_ptr[offset] = (timestamp_len_for_ancs + 1) >> 8;
    offset++;
    temp_ptr[offset] = (timestamp_len_for_ancs + 1) & 0xff;
    offset++;
    temp_ptr[offset] = 0;
    offset++;
    memcpy(&temp_ptr[offset], &temp_message_buffer.TimeStamp, timestamp_len_for_ancs);
    offset += timestamp_len_for_ancs;

    temp_ptr[offset] = (packagename_len_for_ancs + 1) >> 8;
    offset++;
    temp_ptr[offset] = (packagename_len_for_ancs + 1) & 0xff;
    offset++;
    temp_ptr[offset] = 1;
    offset++;
    memcpy(&temp_ptr[offset], temp_message_buffer.packagename, packagename_len_for_ancs);
    offset += packagename_len_for_ancs;

    temp_ptr[offset] = (AppIdentifier_len_for_ancs + 1) >> 8;
    offset++;
    temp_ptr[offset] = (AppIdentifier_len_for_ancs + 1) & 0xff;
    offset++;
    temp_ptr[offset] = 2;
    offset++;
    temp_ptr[offset] = temp_message_buffer.AppIdentifier;
    offset += AppIdentifier_len_for_ancs;

    temp_ptr[offset] = (title_len_for_ancs + 1) >> 8;
    offset++;
    temp_ptr[offset] = (title_len_for_ancs + 1) & 0xff;
    offset++;
    temp_ptr[offset] = 3;
    offset++;
    memcpy(&temp_ptr[offset], temp_message_buffer.title, title_len_for_ancs);
    offset += title_len_for_ancs;

    temp_ptr[offset] = (content_len_for_ancs + 1) >> 8;
    offset++;
    temp_ptr[offset] = (content_len_for_ancs + 1) & 0xff;
    offset++;
    temp_ptr[offset] = 4;
    offset++;
    memcpy(&temp_ptr[offset], temp_message_buffer.content, content_len_for_ancs);

    //////////////////////////////////////////////
    flash_message_write(temp_ptr, mess_data_len);
    ///////////////////////////////////////
    free(temp_ptr);
    if (flash_message_count() < 10) {
        UI_MSG_POST("message_status:event=%4", 1);
    } else {
        UI_MSG_POST("message_status:event=%4", 3);
    }
}

void notice_remove_info_from_ancs(u32 uid)
{
    s8 i;

    if (flash_message_count() == 0) {
        log_info("message null!!!");
        return;
    }

    flash_message_delete_by_mask(uid);

    UI_MSG_POST("message_status:event=%4", 2);
}

void func_attr_notice_add_info(void *priv, u8 attr, u8 *data, u16 len)
{
    u8 seq;
    /* put_buf(data, len); */
    seq = data[0];
    log_info("seq:%d", seq);
    static u8 *message_buf;
    static u16 message_offset = 0;
    if (!seq) {
        if (message_buf) {
            //分包
            ASSERT(message_offset);
            ASSERT(message_offset + len - 1 <= 512);
            memcpy(message_buf + message_offset, &data[1], len - 1);
            message_offset += len - 1;
            flash_message_write(message_buf, message_offset);
            message_offset = 0;
            free(message_buf);
            message_buf = NULL;

        } else {
            //不分包
            flash_message_write(&data[1], len - 1);
        }
    } else { //分包情况
        if (!message_buf) {
            message_buf = malloc(512);
        }
        memcpy(message_buf + message_offset, &data[1], len - 1);
        message_offset += len - 1;
    }

    if (flash_message_count() < 10) {
        UI_MSG_POST("message_status:event=%4", 1);
    } else {
        UI_MSG_POST("message_status:event=%4", 3);
    }
}

void func_attr_notice_remove_info(void *priv, u8 attr, u8 *data, u16 len)
{
    u8 packagename[31];
    u32 timestamp;
    u32 copy_len;
    u16 offset = 0;
    s8 i;
    u16 L;
    u8 T;
    u8 *V;

    if (flash_message_count() == 0) {
        log_info("message null!!!");
        return;
    }

    while (offset < len) {
        L = (data[offset] << 8) | data[offset + 1];
        T = data[offset + 2];
        V = &data[offset + 3];
        offset += L + 2;
        if (T == 0) {
            copy_len = (L - 1) > MESSAGE_TIMESTAMP_LEN ? MESSAGE_TIMESTAMP_LEN : (L - 1);
            memcpy(&timestamp, V, copy_len);
        } else if (T == 1) {
            copy_len = (L - 1) > MESSAGE_PACKAGENAME_LEN ? MESSAGE_PACKAGENAME_LEN : (L - 1);
            memcpy(packagename, V, copy_len);
        }
    }
    flash_message_delete_by_mask(timestamp);
    UI_MSG_POST("message_status:event=%4", 2);
}




int notice_status_handler(const char *type, u32 arg)
{
    int i;
    int row, col;
    struct element_css *css;
    struct element *elm;
    struct ui_grid *grid = ui_grid_for_id(SIDEBAR_VLIST_NOTICE);

    if (type && (!strcmp(type, "event"))) {
        switch (arg) {
        case 1: /* add */
            log_info("add message.....");
            log_info("message_num:%d", flash_message_count());
            if (flash_message_count() == 1) {
                row = 2;
                col = 1;
            } else {
                row = 1;
                col = 0;
            }
            css = ui_core_get_element_css(&grid->elm);
            if (css->invisible == true) {
                css->invisible = false;
            }
            ui_grid_add_dynamic_by_id(SIDEBAR_VLIST_NOTICE, &row, &col, create_control_by_menu);
            break;
        case 2: /* del */
            log_info("del message.....");
            log_info("message_num:%d", flash_message_count());
            if (flash_message_count() == 0) {
                row = 2;
                col = 1;
            } else {
                row = 1;
                col = 0;
            }
            if (flash_message_count() == 0) {
                css = ui_core_get_element_css(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT));
                css->top = css->height;
                css = ui_core_get_element_css(&grid->elm);
                css->invisible = true;
            }
            ui_grid_del_dynamic_by_id(SIDEBAR_VLIST_NOTICE, &row, &col, create_control_by_menu);
            break;
        case 3:
            /* log_info("redraw grid:%d.....",grid->avail_item_num); */
            for (u8 index = 0; index < grid->avail_item_num; index++) {
                list_for_each_child_element(elm, &grid->item[index].elm) {
                    if (elm->handler && elm->handler->onchange) {
                        elm->handler->onchange(elm, ON_CHANGE_UPDATE_ITEM, (void *)message_index[index]);
                    }
                }
            }
            break;
        }
    }

    return 0;
}

#if 0
static struct __UI_MESSAGE test1_message = {
    .packagename = "com.tencent.mm",
    .AppIdentifier = 2,
    .title = "微信",
    .content = "微信：helloworld",
    .TimeStamp = 1,
    .title_encode = FONT_ENCODE_UTF8,
    .title_flags = FONT_DEFAULT,
    .title_icon_Xcoordinate = 92,
    .title_text_Xcoordinate = 116,
};

static struct __UI_MESSAGE test2_message = {
    .packagename = "com.tencent.mobileqq",
    .AppIdentifier = 3,
    .title = "QQ",
    .content = "QQ：helloworld",
    .TimeStamp = 2,
    .title_encode = FONT_ENCODE_UTF8,
    .title_flags = FONT_DEFAULT,
    .title_icon_Xcoordinate = 92,
    .title_text_Xcoordinate = 116,
};

static struct __UI_MESSAGE test3_message = {
    .packagename = "com.alibaba.android.rimet",
    .AppIdentifier = 4,
    .title = "钉钉",
    .content = "钉钉：helloworld",
    .TimeStamp = 3,
    .title_encode = FONT_ENCODE_UTF8,
    .title_flags = FONT_DEFAULT,
    .title_icon_Xcoordinate = 92,
    .title_text_Xcoordinate = 116,
};

void test_message_add(u8 index)
{
    s8 i;
    u8 num = flash_message_count();
    struct __UI_MESSAGE *test_message;

    if (index == 1) {
        test_message = &test1_message;
    } else if (index == 2) {
        test_message = &test2_message;
    } else if (index == 3) {
        test_message = &test3_message;
    } else {
        return;
    }
    UI_MSG_POST("message_status:event=%4", 1);
}

void test_message_del(u8 timestamp)
{
    s8 i;
    UI_MSG_POST("message_status:event=%4", 2);
}
#endif

#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
static int vlist_notice_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    static struct scroll_area area = {0, 0, 10000, 10000};
    struct ui_grid *grid = (struct ui_grid *)_ctrl;
    struct element_css *css;
    int index;
    int row, col;

    switch (event) {
    case ON_CHANGE_INIT:
#if 0
        if (message_num == 0) {
            memset(message_store_buffer, 0, sizeof(struct __UI_MESSAGE) * Msg_Max_Num);
            for (index = 0; index < 3; index++) {
                if (index == 0) {
                    memcpy(&message_store_buffer[index], &test1_message, sizeof(struct __UI_MESSAGE));
                } else if (index == 1) {
                    memcpy(&message_store_buffer[index], &test2_message, sizeof(struct __UI_MESSAGE));
                } else if (index == 2) {
                    memcpy(&message_store_buffer[index], &test3_message, sizeof(struct __UI_MESSAGE));
                }
            }
            message_num = index;
        }
#endif
        if (flash_message_count() == 0) {
            row = flash_message_count();
            col = 0;
        } else {
            row = flash_message_count() + 1;
            col = 1;
        }
        ui_grid_init_dynamic(grid, &row, &col);
        log_info("%s %d:dynamic_grid %d X %d\n", __FUNCTION__, __LINE__, row, col);
        /* for (u8 i = 0; i < grid->avail_item_num; i++) { */
        /*     css = ui_core_get_element_css(&grid->item[i].elm); */
        /*     y_log_info("css[%d]->invisible:%d", i, css->invisible); */
        /* } */

        ui_grid_set_scroll_area(grid, &area);
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        if (flash_message_count() == 0) {
            css = ui_core_get_element_css(&grid->elm);
            css->invisible = true;
        }
        break;
    case ON_CHANGE_RELEASE:
        for (u8 i = 0; i < 3; i++) {
            if (message_store_buffer[i]) {
                free(message_store_buffer[i]);
                message_store_buffer[i] = 0;
            }
        }
        break;
    }
    return 0;
}
static int vlist_notice_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctrl;
    struct element_css *css;
    static u8 touch_action = 0;
    static u8 hide_flag = 0;
    static int first_y_offset = 0;

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        if (touch_action == 1) {
            int sel_item = ui_grid_cur_item(grid);
            message_select_index = message_index[sel_item];
            log_info("sel_item is:%d, index:%d, message_num:%d", sel_item, message_select_index, flash_message_count());

            if (message_select_index < flash_message_count()) {
                ui_send_event(KEY_CHANGE_PAGE, BIT(31) | MESSAGE_DETAIL_PAGE);
            }
        }
        break;
    case ELM_EVENT_TOUCH_DOWN:
        touch_action = 1;
        first_y_offset = e->pos.y;
        css = ui_core_get_element_css(&grid->item[0].elm);
        int top_half_scr = (grid->dynamic->grid_yval - css->height) / 2;
        int top = grid->area ? grid->area->top : top_half_scr;
        if (css->top == top) {
            hide_flag = 1;
        } else {
            hide_flag = 0;
        }
        break;
    case ELM_EVENT_TOUCH_MOVE:
        touch_action = 2;
        if (e->pos.y > first_y_offset) {
            if ((hide_flag == 1) && (create_control_by_menu == 0)) {
                css = ui_core_get_element_css(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT));
                css->top = css->height;
                /* ui_core_redraw(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT)); */
            }
        } else {
            hide_flag = 0;
        }
        break;
    }

    return false;
}
REGISTER_UI_EVENT_HANDLER(SIDEBAR_VLIST_NOTICE)
.onchange = vlist_notice_onchange,
 .onkey = NULL,
  .ontouch = vlist_notice_ontouch,
};

static int text_title_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;
    struct element_css *css;
    int index;
    int len;
    u8 err;

    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        break;
    case ON_CHANGE_HIGHLIGHT:
        break;
    case ON_CHANGE_UPDATE_ITEM:

        index = (u32)arg;
        if (index > flash_message_count()) {
            break;
        }

        if (index == flash_message_count()) {
            ui_text_set_hide_by_id(text->elm.id, true);
        } else {
            ui_text_set_hide_by_id(text->elm.id, false);
        }

        switch (text->elm.id) {
        case SIDEBAR_TEXT_TITLE0:
            err = message_data_analysis(index, 0);
            if (err) {
                return FALSE;
            }
            index = 0;
            break;
        case SIDEBAR_TEXT_TITLE1:
            err = message_data_analysis(index, 1);
            if (err) {
                return FALSE;
            }
            index = 1;
            break;
        case SIDEBAR_TEXT_TITLE2:
            err = message_data_analysis(index, 2);
            if (err) {
                return FALSE;
            }
            index = 2;
            break;
        default:
            return FALSE;
        }

        /* log_info("title:%s\n",message_store_buffer[index]->title); */
        /* log_info("%s %d:index %d, message_num:%d", __FUNCTION__, __LINE__, index, message_num); */
        css = ui_core_get_element_css(&text->elm);
        css->left = message_store_buffer[index]->title_text_Xcoordinate * 10000 / 240;
        ui_text_set_text_attrs(text, message_store_buffer[index]->title,
                               strlen(message_store_buffer[index]->title),
                               message_store_buffer[index]->title_encode, 0,
                               message_store_buffer[index]->title_flags);
        break;
    }
    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(SIDEBAR_TEXT_TITLE0)
.onchange = text_title_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(SIDEBAR_TEXT_TITLE1)
.onchange = text_title_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(SIDEBAR_TEXT_TITLE2)
.onchange = text_title_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int text_content_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;
    int index;
    int len;

    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        break;
    case ON_CHANGE_HIGHLIGHT:
        break;
    case ON_CHANGE_UPDATE_ITEM:

        index = (u32)arg;
        if (index > flash_message_count()) {
            break;
        }

        if (index == flash_message_count()) {
            ui_text_set_hide_by_id(text->elm.id, true);
        } else {
            ui_text_set_hide_by_id(text->elm.id, false);
        }

        switch (text->elm.id) {
        case SIDEBAR_TEXT_CONTENT0:
            /* log_info("b0\n"); */
            message_index[0] = index;
            index = 0;
            break;
        case SIDEBAR_TEXT_CONTENT1:
            /* log_info("b1\n"); */
            message_index[1] = index;
            index = 1;
            break;
        case SIDEBAR_TEXT_CONTENT2:
            /* log_info("b2\n"); */
            message_index[2] = index;
            index = 2;
            break;
        default:
            return FALSE;
        }

        /* log_info("content:%s\n",message_store_buffer[index]->content); */
        /* log_info("%s %d:index %d, message_num:%d", __FUNCTION__, __LINE__, index, message_num); */
        ui_text_set_text_attrs(text, message_store_buffer[index]->content,
                               strlen(message_store_buffer[index]->content),
                               FONT_ENCODE_UTF8, 0, FONT_DEFAULT | FONT_SHOW_MULTI_LINE);
        break;
    }

    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(SIDEBAR_TEXT_CONTENT0)
.onchange = text_content_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(SIDEBAR_TEXT_CONTENT1)
.onchange = text_content_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(SIDEBAR_TEXT_CONTENT2)
.onchange = text_content_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int pic_icon_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)_ctrl;
    struct element_css *css;
    int index;
    int len;

    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        break;
    case ON_CHANGE_HIGHLIGHT:
        break;
    case ON_CHANGE_UPDATE_ITEM:

        index = (u32)arg;
        if (index > flash_message_count()) {
            break;
        }

        if (index == flash_message_count()) {
            ui_pic_set_hide_by_id(pic->elm.id, true);
        } else {
            ui_pic_set_hide_by_id(pic->elm.id, false);
        }

        switch (pic->elm.id) {
        case SIDEBAR_PIC_APP_ICON0:
            index = 0;
            break;
        case SIDEBAR_PIC_APP_ICON1:
            index = 1;
            break;
        case SIDEBAR_PIC_APP_ICON2:
            index = 2;
            break;
        default:
            return FALSE;
        }

        if (index != flash_message_count()) {
            css = ui_core_get_element_css(&pic->elm);
            css->left = message_store_buffer[index]->title_icon_Xcoordinate * 10000 / 240;
        }

        if (message_store_buffer[index]->AppIdentifier >= 4) {
            ui_pic_set_image_index(pic, 0);
        } else {
            ui_pic_set_image_index(pic, message_store_buffer[index]->AppIdentifier);
        }
        break;
    }
    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(SIDEBAR_PIC_APP_ICON0)
.onchange = pic_icon_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(SIDEBAR_PIC_APP_ICON1)
.onchange = pic_icon_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(SIDEBAR_PIC_APP_ICON2)
.onchange = pic_icon_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int text_clear_all_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;
    int index;

    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        break;
    case ON_CHANGE_HIGHLIGHT:
        break;
    case ON_CHANGE_UPDATE_ITEM:

        index = (u32)arg;
        if (index > flash_message_count()) {
            break;
        }
        switch (text->elm.id) {
        case SIDEBAR_TEXT_CLEAR_ALL1:
            if ((index == flash_message_count()) && (index > 0)) {
                if (flash_message_count() <= 1) {
                    ui_text_set_hide_by_id(SIDEBAR_TEXT_CLEAR_ALL1, false);
                }
            } else {
                ui_text_set_hide_by_id(SIDEBAR_TEXT_CLEAR_ALL1, true);
            }
            break;
        case SIDEBAR_TEXT_CLEAR_ALL2:
            if ((index == flash_message_count()) && (index > 0)) {
                if (flash_message_count() > 1) {
                    ui_text_set_hide_by_id(SIDEBAR_TEXT_CLEAR_ALL2, false);
                }
            } else {
                ui_text_set_hide_by_id(SIDEBAR_TEXT_CLEAR_ALL2, true);
            }
            break;
        default:
            return FALSE;
        }

        break;
    }

    return FALSE;
}

static int text_clear_all_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct ui_text *text = (struct ui_text *)ctrl;
    struct ui_grid *grid = ui_grid_for_id(SIDEBAR_VLIST_NOTICE);
    struct element_css *css;
    int row, col;

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        row = 1;//message_num + 1;
        col = 1;
        while (flash_message_count()) {
            flash_message_delete_by_index(0);
        }
        /* if (message_num == 0)  */
        {
            css = ui_core_get_element_css(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT));
            css->top = css->height;
            css = ui_core_get_element_css(&grid->elm);
            css->invisible = true;
        }
        ui_grid_del_dynamic_by_id(SIDEBAR_VLIST_NOTICE, &row, &col, 1);
        break;
    case ELM_EVENT_TOUCH_DOWN:
        return true;
    }

    return false;
}
REGISTER_UI_EVENT_HANDLER(SIDEBAR_TEXT_CLEAR_ALL1)
.onchange = text_clear_all_onchange,
 .onkey = NULL,
  .ontouch = text_clear_all_ontouch,
};
REGISTER_UI_EVENT_HANDLER(SIDEBAR_TEXT_CLEAR_ALL2)
.onchange = text_clear_all_onchange,
 .onkey = NULL,
  .ontouch = text_clear_all_ontouch,
};

//===============================================================================================================//

static int text_message_detail_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct layout *layout = (struct layout *)_ctrl;
    struct element_css *css;

    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        break;
    }

    return FALSE;
}

static int text_message_detail_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct layout *layout = (struct layout *)ctrl;
    static int last_pos_y = 0;
    int y_offset;
    struct rect rect;
    struct rect parent_rect;
    struct element_css *parent_css;
    int i;

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        last_pos_y = e->pos.y;
        break;
    case ELM_EVENT_TOUCH_MOVE:
        ui_core_get_element_abs_rect(layout->elm.parent, &parent_rect);
        ui_core_get_element_abs_rect(&layout->elm, &rect);
        if (rect.height > parent_rect.height) {
            y_offset = e->pos.y - last_pos_y;
            last_pos_y = e->pos.y;
            parent_css = ui_core_get_element_css(layout->elm.parent);
            rect.top += y_offset;
            layout->elm.css.top = rect.top * 10000 / parent_rect.height;
            if (layout->elm.css.top > 0) {
                layout->elm.css.top = 0;
            } else if (layout->elm.css.top < (parent_css->height - layout->elm.css.height)) {
                layout->elm.css.top = parent_css->height - layout->elm.css.height;
            }
            ui_core_redraw(layout->elm.parent);
        }
        break;
    case ELM_EVENT_TOUCH_R_MOVE:
        for (i = message_select_index; i < (flash_message_count() - 1); i++) {
        }
        log_info(">>>>>>>>>%s %d %d\n", __FUNCTION__, __LINE__, message_select_index);
        /* message_num--; */
        /* if (message_num == 0) { */
        /* message_total_len = 0; */
        /* css = ui_core_get_element_css(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT)); */
        /* css->top = css->height; */
        /* css = ui_core_get_element_css(&grid->elm); */
        /* css->invisible = true; */
        /* } */
        return false;
        break;
    }

    return true;
}
__REGISTER_UI_EVENT_HANDLER(JL, LAYOUT_MESSAGE_DETAIL)
.onchange = text_message_detail_onchange,
 .onkey = NULL,
  .ontouch = text_message_detail_ontouch,
};

static int text_title_detail_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;
    struct element_css *css;
    struct element_css *parent_css;
    u8 index = message_select_index;
    int len;

    switch (event) {
    case ON_CHANGE_INIT:
        //根据布局实际高度重新调整控件大小
        parent_css = ui_core_get_element_css(text->elm.parent);
        css = ui_core_get_element_css(&text->elm);
        css->top = css->top * 10000 / parent_css->height;
        css->height = css->height * 10000 / parent_css->height;
        index = 0;
        ASSERT(message_store_buffer[index]);
        ui_text_set_text_attrs(text, message_store_buffer[index]->title,
                               strlen(message_store_buffer[index]->title),
                               message_store_buffer[index]->title_encode,
                               0, message_store_buffer[index]->title_flags);
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return FALSE;
}
__REGISTER_UI_EVENT_HANDLER(JL, TEXT_TITLE_DETAIL)
.onchange = text_title_detail_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int text_content_detail_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;
    struct element_css *css;
    struct element_css *parent_css;
    struct rect rect;
    struct rect parent_rect;
    u8 content_encode = FONT_ENCODE_UTF8;
    u8 content_flags = FONT_DEFAULT | FONT_SHOW_MULTI_LINE;
    u16 content_elm_width;
    u16 content_elm_height;
    u16 message_content_height;
    u8 index = message_select_index;
    int len;

    switch (event) {
    case ON_CHANGE_INIT:
        ui_core_get_element_abs_rect(&text->elm, &rect);
        content_elm_width = rect.width;
        content_elm_height = 240;               //预先设置最大高度，方便下面计算文本高度


        message_data_analysis(message_select_index, 0);
        index = 0;

        message_content_height = ui_get_text_width_and_height(content_encode, message_store_buffer[index]->content,
                                 strlen(message_store_buffer[index]->content),
                                 content_elm_width, content_elm_height, content_flags, "height");
        /* log_info("message_content_height:%d",message_content_height); */
        if (message_content_height > rect.height) {                 //根据文本实际高度调整文本控件大小
            struct element *elm = ui_core_get_element_by_id(LAYOUT_MESSAGE_DETAIL);
            ui_core_get_element_abs_rect(elm->parent, &parent_rect);
            parent_css = ui_core_get_element_css(text->elm.parent);
            parent_css->height += (message_content_height - rect.height) * 10000 / parent_rect.height;
            ui_core_get_element_abs_rect(text->elm.parent, &parent_rect);
            css = ui_core_get_element_css(&text->elm);
            css->top = css->top * 10000 / parent_css->height;
            css->height = message_content_height * 10000 / parent_rect.height;
        }
        ui_text_set_text_attrs(text, message_store_buffer[index]->content,
                               strlen(message_store_buffer[index]->content),
                               content_encode, 0, content_flags);
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return FALSE;
}
__REGISTER_UI_EVENT_HANDLER(JL, TEXT_CONTENT_DETAIL)
.onchange = text_content_detail_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int pic_app_detail_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)_ctrl;
    struct element_css *css;
    struct element_css *parent_css;
    u8 index = message_select_index;
    int len;

    switch (event) {
    case ON_CHANGE_INIT:
        //根据布局实际高度重新调整控件大小
        parent_css = ui_core_get_element_css(pic->elm.parent);
        css = ui_core_get_element_css(&pic->elm);
        css->top = css->top * 10000 / parent_css->height;
        css->height = css->height * 10000 / parent_css->height;
        index = 0;

        ASSERT(message_store_buffer[index]);
        if (message_store_buffer[index]->AppIdentifier >= 4) {
            ui_pic_set_image_index(pic, 0);
        } else {
            ui_pic_set_image_index(pic, message_store_buffer[index]->AppIdentifier);
        }
        break;
    case ON_CHANGE_RELEASE:
        for (u8 i = 0; i < 3; i++) {
            if (message_store_buffer[i]) {
                free(message_store_buffer[i]);
                message_store_buffer[i] = 0;
            }
        }
        break;
    }
    return FALSE;
}
__REGISTER_UI_EVENT_HANDLER(JL, PIC_APP_ICON)
.onchange = pic_app_detail_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

//===============================================================================================================//

static const struct uimsg_handl ui_menu_notice_msg_handler[] = {
    { "message_status",   notice_status_handler     },
    { NULL, NULL},      /* 必须以此结尾！ */
};

u8 is_create_control_by_menu()
{
    return create_control_by_menu;
}

static int menu_notice_onchange(void *_ctrl, enum element_change_event event, void *arg)
{

    switch (event) {
    case ON_CHANGE_INIT:
        int root = LAYER_NOTICE;

        create_control_by_menu = 1;
        if (root && create_control_by_id("storage/virfat_flash/C/sidebar/sidebar.tab", 0X40820002, SIDEBAR_NOTICE_LAYOUT, root)) {
            log_info("crteate notice control succ! %x\n", root, __LINE__);
        } else {
            log_info("crteate notice control fail! %x\n", root, __LINE__);
        }

        ui_register_msg_handler(ID_WINDOW_MESS, ui_menu_notice_msg_handler);//注册消息交互的回调
        break;
    case ON_CHANGE_RELEASE:
        if (ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT)) {
            delete_control_by_id(SIDEBAR_NOTICE_LAYOUT);
        }
        create_control_by_menu = 0;
        break;
    }

    return FALSE;
}

__REGISTER_UI_EVENT_HANDLER(JL, LAYOUT_NOTICE)
.onchange = menu_notice_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#endif
#endif
#endif
