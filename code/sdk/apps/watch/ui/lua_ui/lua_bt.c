#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_bt.h"

#include "generic/typedef.h"
#include "app_config.h"
#include "app_main.h"
#include "app_task.h"
#include "key_event_deal.h"
#include "audio_config.h"
#include "btstack/avctp_user.h"
#include "audio_dec.h"
#include "audio_dec_file.h"
#include "bt/bt.h"
#include "ui/ui_api.h"
#include "bt_common.h"

#if (TCFG_LUA_ENABLE)


static int bt_getBluetoothConnectState(lua_State *L)
{
    int device = lua_tointeger(L, -1);
    int status = BT_STATUS_INITING;

    if (device == BT_EMITTER_CONNECT_STATUS) {
        status = get_emitter_connect_status();
    } else if (device == BT_PHONE_CONNECT_STATUS) {
        status = get_bt_connect_status();
    } else {
        printf("ERROR! unknow device :%d\n", device);
        return 0;
    }
    lua_db(">>> %s, %d. status: %d\n", __FUNCTION__, __LINE__, status);
    lua_pushinteger(L, status);
    return 1;

#if 0
    int state = 0;	/* 蓝牙连接状态，0 未连接； 1 已连接*/

    if (get_curr_channel_state() ||
        get_emitter_curr_channel_state()) {
        state = 1;
    }
    lua_db(">> bt << %s, state:%d\n", __FUNCTION__, state);
    lua_pushboolean(L, state);
    return 1;
#endif
}


static int bt_getCallState(lua_State *L)
{
    int state = 0;	/* 手机通话状态，0 未通话；1 去电；2 来电；3 通话中*/

    if ((get_call_status() == BT_CALL_OUTGOING) ||
        (get_call_status() == BT_CALL_ALERT)) {
        state = 1;
    } else if (get_call_status() == BT_CALL_INCOMING) {
        state = 2;
    } else if (get_call_status() == BT_CALL_ACTIVE) {
        state = 3;
    }

    lua_db(">> bt << %s, state:%d\n", __FUNCTION__, state);
    lua_pushinteger(L, state);

    return 1;
}


// 接听电话
static int bt_pickUpThePhone(lua_State *L)
{
    int state = 0;	/* 接听结果：1 成功；0 失败 */

    if ((get_call_status() == BT_CALL_OUTGOING) ||
        (get_call_status() == BT_CALL_ALERT)) {
        /* user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL); */
    } else if (get_call_status() == BT_CALL_INCOMING) {
        user_send_cmd_prepare(USER_CTRL_HFP_CALL_ANSWER, 0, NULL);
        state = 1;
    } else if (get_call_status() == BT_CALL_ACTIVE) {
        /* user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL); */
    }

    lua_db(">> bt << %s, state:%d\n", __FUNCTION__, state);
    lua_pushboolean(L, state);

    return 1;
}


// 挂断电话
static int bt_hangUpThePhone(lua_State *L)
{
    int state = 0;	/* 挂断结果：1 成功；0 失败 */

    if ((get_call_status() == BT_CALL_OUTGOING) ||
        (get_call_status() == BT_CALL_ALERT)) {
        user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
        state = 1;
    } else if (get_call_status() == BT_CALL_INCOMING) {
        user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
        state = 1;
    } else if (get_call_status() == BT_CALL_ACTIVE) {
        user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
        state = 1;
    }

    lua_db(">> bt << %s, state:%d\n", __FUNCTION__, state);
    lua_pushboolean(L, state);

    return 1;
}


// 回拨最后一个号码
static int bt_callbackTheLastNumber(lua_State *L)
{
    char *phone_number = NULL;	/* 待回拨的号码 */

    /* bt_key_call_last_on(); */
    if ((get_call_status() == BT_CALL_ACTIVE) ||
        (get_call_status() == BT_CALL_OUTGOING) ||
        (get_call_status() == BT_CALL_ALERT) ||
        (get_call_status() == BT_CALL_INCOMING)) {
        ;//通话过程不允许回拨
    } else {
        if (bt_user_priv_var.last_call_type ==  BT_STATUS_PHONE_INCOME) {
            user_send_cmd_prepare(USER_CTRL_DIAL_NUMBER, bt_user_priv_var.income_phone_len,
                                  bt_user_priv_var.income_phone_num);
            phone_number = bt_user_priv_var.income_phone_num;
        } else {
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_LAST_NO, 0, NULL);
        }
    }

    lua_db(">> bt << %s, phone_number:%s\n", __FUNCTION__, phone_number);
    lua_pushstring(L, phone_number);
    return 1;
}

/* 从flash读取通话记录和通讯录 */
#if (TCFG_NOR_FS && WATCH_FILE_TO_FLASH)
#if TCFG_NOR_FAT
#define CALL_LOGS_PATH      "storage/fat_nor/C/calllog.txt"
#define PHONE_BOOK_PATH      "storage/fat_nor/C/call.txt"
#else // TCFG_NOR_FAT
#define CALL_LOGS_PATH      "storage/virfat_flash/C/calllog.txt"
#define PHONE_BOOK_PATH      "storage/virfat_flash/C/call.txt"
#endif // TCFG_NOR_FAT
#else // TCFG_NOR_FS
#define CALL_LOGS_PATH      "storage/sd1/C/DOWNLOAD/calllog.txt"
#define PHONE_BOOK_PATH      "storage/sd1/C/DOWNLOAD/call.txt"
#endif // TCFG_NOR_FS

#define BOOK_MASK (0x1234)
//结构体新成员在最后加
static struct _phoneBook {
    char name[20];
    char number[20];
    char date[20];
    u8  type;
    u32 mask;
};

#define PHONE_BOOK_LEN            (40)
#define CALL_LOGS_LEN             (sizeof(struct _phoneBook))
#define PBAP_MAX_NUM              (10)
#define MAX_CALL_LOG_NUM          (30)


// 获取通讯录
static int bt_getPhoneMailList(lua_State *L)
{
    int cnt = 0;
    int number = 0;	/* 要获取的个数 */
    FILE *phoneBook_fp = NULL; /* 通讯录文件句柄 */
    struct vfs_attr attr = {0};	/* 文件属性 */
    struct _phoneBook pB = {0};

    number = lua_tointeger(L, -1);

    phoneBook_fp = fopen(PHONE_BOOK_PATH, "r");
    if (!phoneBook_fp) {
        printf("LUA ERROR: open phone book fail!\n");
        return 0;
    }
    fget_attrs(phoneBook_fp, &attr);

    int fsize = attr.fsize;
    lua_db("phone book file size:%d\n", fsize);
    if (fsize < PHONE_BOOK_LEN) {
        printf("Not mail list in flash!\n");
        fclose(phoneBook_fp);
        return 0;
    }

    /* int bsize = sizeof(struct _phoneBook) * number; */
    lua_newtable(L);	/* new tabt use return */
    // 读number个
    for (cnt = 0; cnt < number; cnt ++) {
        int len;
        lua_pushnumber(L, cnt + 1);
        fseek(phoneBook_fp, cnt * PHONE_BOOK_LEN, SEEK_SET);
        len = fread(phoneBook_fp, &pB, PHONE_BOOK_LEN);

        lua_db("name:%s, number:%s, cnt:%d\n", pB.name, pB.number, cnt);
        // 把当前读到的数据存到一个tabt中
        lua_newtable(L);
        lua_pushstring(L, "name");	// save name
        lua_pushstring(L, pB.name);
        lua_settable(L, -3);

        lua_pushstring(L, "number");	// save number
        lua_pushstring(L, pB.number);
        lua_settable(L, -3);

        /* lua_pushstring(L, "date");	// save date */
        /* lua_pushstring(L, pB.date); */
        /* lua_settable(L, -3); */

        /* lua_pushstring(L, "type");	// save type */
        /* lua_pushinteger(L, pB.type); */
        /* lua_settable(L, -3); */

        /* lua_pushstring(L, "mask");	// save mask */
        /* lua_pushinteger(L, pB.mask); */
        /* lua_settable(L, -3); */

        lua_settable(L, -3);	// save cell tabt
        fsize -= len;	/* 如果不足number时文件已读完，提前退出 */
        if (fsize <= 0) {
            break;
        }
    }
    fclose(phoneBook_fp);	/* close phone book file */
    lua_db(">> bt << %s, read count:%d\n", __FUNCTION__, cnt);

    return 1;
}


// 获取通话记录
static int bt_getPhoneCallLog(lua_State *L)
{
    int cnt = 0;
    int number = 0;	/* 要获取的个数 */
    FILE *phoneBook_fp = NULL; /* 通讯录文件句柄 */
    struct vfs_attr attr = {0};	/* 文件属性 */
    struct _phoneBook pB = {0};

    number = lua_tointeger(L, -1);

    phoneBook_fp = fopen(CALL_LOGS_PATH, "r");
    if (!phoneBook_fp) {
        printf("LUA ERROR: open phone book fail!\n");
        return 0;
    }
    fget_attrs(phoneBook_fp, &attr);
    int fsize = attr.fsize;
    lua_db("phone book file size:%d\n", fsize);
    if (fsize < PHONE_BOOK_LEN) {
        printf("Not mail list in flash!\n");
        fclose(phoneBook_fp);
        return 0;
    }

    /* int bsize = sizeof(struct _phoneBook) * number; */
    lua_newtable(L);	/* new tabt use return */
    // 读number个
    for (cnt = 0; cnt < number; cnt ++) {
        int len;
        lua_pushnumber(L, cnt + 1);
        fseek(phoneBook_fp, cnt * CALL_LOGS_LEN, SEEK_SET);
        len = fread(phoneBook_fp, &pB, CALL_LOGS_LEN);
        if (BOOK_MASK == pB.mask) {
        } else {
            printf("phonebook file need delete\n");
            break;
        }
        lua_db("name:%s, number:%s, date:%s, cnt:%d\n", pB.name, pB.number, pB.date, cnt);

        // 把当前读到的数据存到一个tabt中
        lua_newtable(L);
        lua_pushstring(L, "name");	// save name
        lua_pushstring(L, pB.name);
        lua_settable(L, -3);

        lua_pushstring(L, "number");	// save number
        lua_pushstring(L, pB.number);
        lua_settable(L, -3);

        lua_pushstring(L, "date");	// save date
        lua_pushstring(L, pB.date);
        lua_settable(L, -3);

        lua_pushstring(L, "type");	// save type
        lua_pushinteger(L, pB.type);
        lua_settable(L, -3);

        lua_pushstring(L, "mask");	// save mask
        lua_pushinteger(L, pB.mask);
        lua_settable(L, -3);

        lua_settable(L, -3);	// save cell tabt
        fsize -= len;	/* 如果不足number时文件已读完，提前退出 */
        if (fsize <= 0) {
            break;
        }
    }
    fclose(phoneBook_fp);	/* close phone book file */
    lua_db(">> bt << %s, read count:%d\n", __FUNCTION__, cnt);

    return 1;	// return tabt
}

static int bt_getLastPhoneNum(lua_State *L)
{
    lua_pushstring(L, bt_user_priv_var.income_phone_num);
    return 1;	// return table
}

extern void set_call_log_message(u8 type, const u8 *name, const u8 *number, const u8 *date);
static int bt_dialPhoneNumber(lua_State *L)
{
    char *phone_name = lua_tostring(L, -2);
    char *phone_number = lua_tostring(L, -1);
    set_call_log_message(1, (u8 *)phone_name, (u8 *)phone_number, NULL);
    user_send_cmd_prepare(USER_CTRL_DIAL_NUMBER, strlen(phone_number), (u8 *)phone_number);
    return 0;
}

// 获取短消息
static int bt_getShortMessage(lua_State *L)
{
    char *msg = NULL;	// 短信
    lua_db(">> bt << %s\n", __FUNCTION__);


    lua_pushstring(L, msg);
    return 1;
}


// 获取MAC地址
static int bt_GetAddr(lua_State *L)
{
    u8 len = 0;
    u8 addr[6] = {0};
    u8 addr_str[18] = {0};

    char *name = lua_tostring(L, -1);
    if (!strcmp(name, "mac")) {
        memcpy(addr, bt_get_mac_addr(), 6);
    } else if (!strcmp(name, "ble")) {
        extern int le_controller_get_mac(void *addr);
        le_controller_get_mac(addr);
    } else {
        return 0;
    }

    /* printf_buf((void *)addr, 6); */
    for (s8 i = 5; i >= 0; i--) {
        if ((addr[i] / 16) >= 10) {
            addr_str[len] = 'A' + addr[i] / 16 - 10;
        } else {
            addr_str[len] = '0' + addr[i] / 16;
        }
        if ((addr[i] % 16) >= 10) {
            addr_str[len + 1] = 'A' + addr[i] % 16 - 10;
        } else {
            addr_str[len + 1] = '0' + addr[i] % 16;
        }
        len += 2;
        addr_str[len] = ':';
        len += 1;
    }
    addr_str[len - 1] = '\0';
    lua_pushstring(L, addr_str);
    /* lua_newtable(L); */
    /* set_integer_array_to_table(L, bt_get_mac_addr(), 6); */

    return 1;
}


// 获取名字
static int bt_GetName(lua_State *L)
{
    u8 *name = (u8 *)bt_get_local_name();

    if (name) {
        lua_pushstring(L, name);
        g_printf("%s:%s", __FUNCTION__, name);
        return 1;
    }

    return 0;
}

#if TCFG_USER_EMITTER_ENABLE

#include "bt_emitter.h"

// 蓝牙发射
#define BT_EMITTER_NAME_LEN		32
#define BT_EMITTER_MAC_LEN		6

enum {
    LUA_BT_EMITTER_STATUS_SCAN_DOING = 1,	// 正在扫描
    LUA_BT_EMITTER_STATUS_SCAN_END,	// 扫描结束

    LUA_BT_EMITTER_STATUS_CONNECT_NULL = 10,// 没有连接
    LUA_BT_EMITTER_STATUS_CONNECT_OK,		// 连接成功
    /* LUA_BT_EMITTER_STATUS_CONNECT_ERR,		// 连接失败 */
    /* LUA_BT_EMITTER_STATUS_CONNECT_DOING,	// 正在连接 */
};

// 蓝牙发射搜索列表
struct bt_info {
    u8 name[BT_EMITTER_NAME_LEN];
    u8 mac[BT_EMITTER_MAC_LEN];
    u8 rssi;
    u8 num;
};
struct bt_list {
    struct list_head entry;
    struct bt_info info;
};
struct bt_list_hd {
    struct list_head head;
    int count;
};
static struct bt_list_hd *bt_emitter_list_hdl = NULL;

// 蓝牙发射收藏夹
struct bt_emitter_save {
    u8 cnt;
    u8 start;
    u8 end;
    u8 name_total;
    u8 name_cnt;
    u8 mac_total;
    u8 mac_cnt;
    u8 *name[20];
    u8 *mac[20];
};

struct bt_emitter_lua_get_save {
    u8 cnt;
    u8 user_cnt;
    u8 start;
    u8 end;
    u8 check_conn; // 1-only conn, 2-ext conn
    u8 *connect_mac;
    lua_State *L;
};

// 蓝牙发射临时数据
struct bt_tmp {
    u8 name[BT_EMITTER_NAME_LEN];
    u8 mac[BT_EMITTER_MAC_LEN];
    u8 connect;
};
static struct bt_tmp *bt_tmp_info = NULL;

/****************************************** 蓝牙发射 ******************************************/
// 快速mac转字符串
static inline void mac_to_string(char *str, u8 mac[BT_EMITTER_MAC_LEN])
{
    /* for (int i = 0; i < BT_EMITTER_MAC_LEN; i++) { */

    /* } */
}

// 快速字符串转mac
static inline void string_to_mac(char *str, u8 mac[BT_EMITTER_MAC_LEN])
{

}

static void lua_bt_name_parse(lua_State *L, u8 name[BT_EMITTER_NAME_LEN])
{
    strncpy(name, lua_tostring(L, -1), BT_EMITTER_NAME_LEN);
    lua_pop(L, 1);
}

static void lua_bt_mac_parse(lua_State *L, u8 mac[BT_EMITTER_MAC_LEN])
{
    for (int i = 0; i < BT_EMITTER_MAC_LEN; i++) {
        lua_pushinteger(L, i + 1);
        lua_gettable(L, -2);
        mac[i] = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

// 连接
static int bt_emitterConnect(lua_State *L)
{
    u8 name[BT_EMITTER_NAME_LEN];
    u8 mac[BT_EMITTER_MAC_LEN];

    lua_bt_mac_parse(L, mac);
    lua_bt_name_parse(L, name);

    emitter_bt_connect(mac);
    emitter_save_remote_name(mac, name);

    return 0;
}

// 断开连接
static int bt_emitterDisConnect(lua_State *L)
{
    u8 name[BT_EMITTER_NAME_LEN];
    u8 mac[BT_EMITTER_MAC_LEN];

    lua_bt_mac_parse(L, mac);
    lua_bt_name_parse(L, name);

    extern u8 *get_cur_connect_emitter_mac_addr(void);
    u8 *current_mac = get_cur_connect_emitter_mac_addr();
    if (current_mac && (!memcmp(mac, current_mac, BT_EMITTER_MAC_LEN))) {
        user_emitter_cmd_prepare(USER_CTRL_DISCONNECTION_HCI, 0, NULL);
    }

    return 0;
}

// 解除连接
static int bt_emitterReleaseConnect(lua_State *L)
{
    u8 name[BT_EMITTER_NAME_LEN];
    u8 mac[BT_EMITTER_MAC_LEN];

    lua_bt_mac_parse(L, mac);
    lua_bt_name_parse(L, name);

    // 断开
    u8 *current_mac = get_cur_connect_emitter_mac_addr();
    if (current_mac && (!memcmp(mac, current_mac, BT_EMITTER_MAC_LEN))) {
        user_emitter_cmd_prepare(USER_CTRL_DISCONNECTION_HCI, 0, NULL);
    }
    // 清除记录
    emitter_delete_remote_name(mac);

    return 0;
}

// 连接状态
static int bt_emitterConnectStatus(lua_State *L)
{
    u8 status = LUA_BT_EMITTER_STATUS_CONNECT_NULL;
    u8 *current_mac = get_cur_connect_emitter_mac_addr();
    if (current_mac) {
        status = LUA_BT_EMITTER_STATUS_CONNECT_OK;
    }
    lua_pushinteger(L, status);
    return 1;
}

// 开始扫描设备
static int bt_emitterScanStart(lua_State *L)
{
    /* bt_list_clean(); */
    g_printf("************   emitter \n");
    bt_emitter_start_search_device();
    g_printf("************   emitter start\n");
    return 0;
}

// 停止扫描设备
static int bt_emitterScanStop(lua_State *L)
{
    bt_emitter_stop_search_device();
    /* bt_list_clean(); */
    return 0;
}

// 扫描设备状态
static int bt_emitterScanStatus(lua_State *L)
{
    u8 status = LUA_BT_EMITTER_STATUS_SCAN_END;
    if (bt_search_status()) {
        status = LUA_BT_EMITTER_STATUS_SCAN_DOING;
    }
    lua_pushinteger(L, status);
    return 1;
}

/****************************************** 蓝牙发射搜索列表 ******************************************/
extern void bt_menu_list_add(u8 *name, u8 *mac, u8 rssi);
extern int bt_menu_list_count();
extern void bt_list_clean();
extern u8 *bt_list_get_name_by_number(u8 num);
extern u8 *bt_list_get_mac_by_number(u8 num);


// 清除列表
static int bt_emitterListClean(lua_State *L)
{
    bt_list_clean();
    return 0;
}

// 获取列表中的总数
static int bt_emitterGetTotal(lua_State *L)
{
    u8 num = bt_menu_list_count();
    lua_pushinteger(L, num);
    return 1;
}

// 获取名字
static int bt_emitterGetName(lua_State *L)
{
    int index = lua_tointeger(L, -1);
    u8 *name = bt_list_get_name_by_number(index);
    if (name) {
        lua_pushstring(L, name);
        return 1;
    }
    return 0;
}

// 获取地址
static int bt_emitterGetMac(lua_State *L)
{
    int index = lua_tointeger(L, -1);
    u8 *mac = bt_list_get_mac_by_number(index);
    if (mac) {
        lua_newtable(L);
        set_integer_array_to_table(L, mac, BT_EMITTER_MAC_LEN);
        return 1;
    }
    return 0;
}

// 列表遍历
static int bt_emitterGetList(lua_State *L)
{
    int cnt = 0;
    int number = 20;	/* 要获取的个数 */
    struct bt_list *p, *n;

    /* number = lua_tointeger(L, -1); */
    lua_newtable(L);	/* new tabt use return */

    // 读number个
    list_for_each_entry_safe(p, n, &bt_emitter_list_hdl->head, entry) {
        if (cnt >= number) {
            break;
        }
        cnt ++;
        lua_pushnumber(L, cnt);

        lua_db("name:%s, mac:%s, cnt:%d\n", p->info.name, p->info.mac, cnt);

        // 把当前读到的数据存到一个tabt中
        lua_newtable(L);

        lua_pushstring(L, "name");	// save mac
        lua_pushstring(L, p->info.name);
        lua_settable(L, -3);

        lua_pushstring(L, "mac");	// save mac
        lua_newtable(L);
        set_integer_array_to_table(L, p->info.mac, BT_EMITTER_MAC_LEN);
        lua_settable(L, -3);

        lua_settable(L, -3);	// save
    }
    lua_db(">> bt << %s, read count:%d\n", __FUNCTION__, cnt);

    return 1;	// return tabt
}

/****************************************** 蓝牙发射收藏夹 ******************************************/

static int bt_emitter_save_cb(void *priv, u8 *mac, u8 *name, u8 *out_del, u8 *out_exit)
{
    struct bt_emitter_save *hdl = priv;
    hdl->cnt++;
    if (hdl->cnt <= hdl->start) {
        return 0;
    }
    if (hdl->name_cnt < hdl->name_total) {
        memcpy(hdl->name[hdl->name_cnt++], name, BT_EMITTER_NAME_LEN);
    }
    if (hdl->mac_cnt < hdl->mac_total) {
        memcpy(hdl->mac[hdl->mac_cnt++], mac, BT_EMITTER_MAC_LEN);
    }
    if (hdl->end && (hdl->cnt >= hdl->end)) {
        *out_exit = 1;
    }
    return 1;
}

static int bt_emitter_lua_get_save_cb(void *priv, u8 *mac, u8 *name, u8 *out_del, u8 *out_exit)
{
    struct bt_emitter_lua_get_save *hdl = priv;
    hdl->cnt++;
    if (hdl->cnt <= hdl->start) {
        return 0;
    }
    if (hdl->L) {
        u8 connect_flag = 0;
        if (hdl->connect_mac) {
            if (!memcmp(mac, hdl->connect_mac, BT_EMITTER_MAC_LEN)) {
                if (hdl->check_conn == 1) {
                    connect_flag = 1;
                    *out_exit = 1;
                } else if (hdl->check_conn == 2) {
                    return 0;
                }
            } else {
                if (hdl->check_conn == 1) {
                    return 0;
                }
            }
        }

        hdl->user_cnt++;
        lua_pushnumber(L, hdl->user_cnt);

        lua_db("name:%s, mac:%s, cnt:%d\n", name, mac, hdl->user_cnt);

        // 把当前读到的数据存到一个tabt中
        lua_newtable(L);

        lua_pushstring(L, "name");	// save name
        lua_pushstring(L, name);
        lua_settable(L, -3);

        lua_pushstring(L, "mac");	// save mac
        lua_newtable(L);
        set_integer_array_to_table(L, mac, BT_EMITTER_MAC_LEN);
        lua_settable(L, -3);

        lua_pushstring(L, "connect");	// save connect
        lua_pushinteger(L, connect_flag);
        lua_settable(L, -3);

        lua_settable(L, -3);	// save
    }
    if (hdl->end && (hdl->cnt >= hdl->end)) {
        *out_exit = 1;
    }
    return 1;
}

// 清除收藏夹
static int bt_emitterSaveListClean(lua_State *L)
{
    emitter_delete_remote_all();
    return 0;
}

// 获取收藏夹中的总数
static int bt_emitterSaveGetTotal(lua_State *L)
{
    struct bt_emitter_save *p_save = zalloc(sizeof(struct bt_emitter_save));
    ASSERT(p_save);
    emitter_search_remote(p_save, bt_emitter_save_cb);
    u8 num = p_save->cnt;
    free(p_save);
    lua_pushinteger(L, num);
    return 1;
}

// 获取收藏夹中的名字
static int bt_emitterSaveGetName(lua_State *L)
{
    int index = lua_tointeger(L, -1);
    u8 name[BT_EMITTER_NAME_LEN] = {0};
    struct bt_emitter_save *p_save = zalloc(sizeof(struct bt_emitter_save));
    ASSERT(p_save);
    p_save->start = index;
    p_save->end = index + 1;
    p_save->name[0] = name;
    p_save->name_total = 1;
    emitter_search_remote(p_save, bt_emitter_save_cb);
    free(p_save);
    lua_pushstring(L, name);
    return 1;
}

// 获取收藏夹中的地址
static int bt_emitterSaveGetMac(lua_State *L)
{
    int index = lua_tointeger(L, -1);
    u8 mac[BT_EMITTER_MAC_LEN] = {0};
    struct bt_emitter_save *p_save = zalloc(sizeof(struct bt_emitter_save));
    ASSERT(p_save);
    p_save->start = index;
    p_save->end = index + 1;
    p_save->mac[0] = mac;
    p_save->mac_total = 1;
    emitter_search_remote(p_save, bt_emitter_save_cb);
    free(p_save);
    lua_newtable(L);
    set_integer_array_to_table(L, mac, BT_EMITTER_MAC_LEN);
    return 1;
}

// 收藏夹列表遍历
static int bt_emitterSaveGetList(lua_State *L)
{
    int cnt = 0;
    int number = 20;	/* 要获取的个数 */

    /* number = lua_tointeger(L, -1); */

    lua_newtable(L);	/* new tabt use return */

    struct bt_emitter_lua_get_save *p_save = zalloc(sizeof(struct bt_emitter_lua_get_save));
    ASSERT(p_save);

    u8 *current_mac = get_cur_connect_emitter_mac_addr();

    p_save->start = 0;
    p_save->end = number;
    p_save->connect_mac = current_mac;
    p_save->L = L;

    if (current_mac) {
        // 有连接的
        p_save->check_conn = 1;
        emitter_search_remote(p_save, bt_emitter_lua_get_save_cb);
    }

    p_save->cnt = 0;
    p_save->check_conn = 0;
    emitter_search_remote(p_save, bt_emitter_lua_get_save_cb);

    lua_db(">> bt << %s, read count:%d\n", __FUNCTION__, p_save->user_cnt);

    free(p_save);

    return 1;	// return tabt
}


/****************************************** 蓝牙发射收藏夹 ******************************************/
// 保存临时数据
static int bt_emitterTempSet(lua_State *L)
{
    if (bt_tmp_info == NULL) {
        bt_tmp_info = zalloc(sizeof(struct bt_info));
        ASSERT(bt_tmp_info);
    }

    bt_tmp_info->connect = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_bt_mac_parse(L, bt_tmp_info->mac);

    lua_bt_name_parse(L, bt_tmp_info->name);

    return 0;
}

// 获取临时数据
static int bt_emitterTempGet(lua_State *L)
{
    if (bt_tmp_info) {
        // 把当前读到的数据存到一个tabt中
        lua_newtable(L);

        lua_pushstring(L, "name");	// save name
        lua_pushstring(L, bt_tmp_info->name);
        lua_settable(L, -3);

        lua_pushstring(L, "mac");	// save mac
        lua_newtable(L);
        set_integer_array_to_table(L, bt_tmp_info->mac, BT_EMITTER_MAC_LEN);
        lua_settable(L, -3);

        lua_pushstring(L, "connect");	// save connect
        lua_pushinteger(L, bt_tmp_info->connect);
        lua_settable(L, -3);

        return 1;
    }
    return 0;
}

// 清除临时数据
static int bt_emitterTempClean(lua_State *L)
{
    if (bt_tmp_info) {
        free(bt_tmp_info);
        bt_tmp_info = NULL;
    }
    return 0;
}

#endif /* #if TCFG_USER_EMITTER_ENABLE */

// bt module
const luaL_Reg bt_method[] = {
    {"getBluetoothConnectState",	bt_getBluetoothConnectState},
    {"getCallState", 				bt_getCallState},

    {"pickUpThePhone",				bt_pickUpThePhone},
    {"hangUpThePhone",				bt_hangUpThePhone},
    {"callbackTheLastNumber",		bt_callbackTheLastNumber},
    {"getPhoneMailList",			bt_getPhoneMailList},
    {"getPhoneCallLog",				bt_getPhoneCallLog},
    {"getLastPhoneNum",				bt_getLastPhoneNum},
    {"dialPhoneNumber",				bt_dialPhoneNumber},
    /* {"getShortMessage",				bt_getShortMessage}, */
    {"GetBtAddr",				    bt_GetAddr},
    {"GetBtName",				    bt_GetName},

#if TCFG_USER_EMITTER_ENABLE
    // 蓝牙发射
    {"emitterConnect",				bt_emitterConnect},
    {"emitterDisConnect",			bt_emitterDisConnect},
    {"emitterReleaseConnect",		bt_emitterReleaseConnect},
    {"emitterConnectStatus",		bt_emitterConnectStatus},
    {"emitterScanStart",			bt_emitterScanStart},
    {"emitterScanStop",				bt_emitterScanStop},
    {"emitterScanStatus",			bt_emitterScanStatus},
    // 搜索链表
    {"emitterListClean",			bt_emitterListClean},
    {"emitterGetTotal",				bt_emitterGetTotal},
    {"emitterGetName",				bt_emitterGetName},
    {"emitterGetMac",				bt_emitterGetMac},
    {"emitterGetList",				bt_emitterGetList},
    // 收藏
    {"emitterSaveListClean",		bt_emitterSaveListClean},
    {"emitterSaveGetTotal",			bt_emitterSaveGetTotal},
    {"emitterSaveGetName",			bt_emitterSaveGetName},
    {"emitterSaveGetMac",			bt_emitterSaveGetMac},
    {"emitterSaveGetList",			bt_emitterSaveGetList},
    // 临时数据
    {"emitterTempSet",				bt_emitterTempSet},
    {"emitterTempGet",				bt_emitterTempGet},
    {"emitterTempClean",			bt_emitterTempClean},
#endif /* #if TCFG_USER_EMITTER_ENABLE */

    {NULL, NULL}
};


// 注册bt模块
LUALIB_API int luaopen_bt(lua_State *L)
{
    luaL_newlib(L, bt_method);
    return 1;
}

#endif /* #if (TCFG_LUA_ENABLE) */

