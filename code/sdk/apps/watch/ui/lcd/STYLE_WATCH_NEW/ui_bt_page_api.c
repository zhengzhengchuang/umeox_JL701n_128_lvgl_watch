#include "ui/ui.h"
#include "app_config.h"
#include "ui/ui_style.h"
#include "app_action.h"
#include "system/timer.h"
#include "key_event_deal.h"
#include "audio_config.h"
#include "jiffies.h"
#include "audio_dec.h"
#include "audio_dec_file.h"
#include "music/music_player.h"
#include "ui/watch_syscfg_manage.h"

#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_USER_EMITTER_ENABLE//带有屏幕的方案根据UI选项连接
#if TCFG_SPI_LCD_ENABLE

struct BT_PAGE {
    u8 used;
    u8 mac[6];
    u8 name[32];
};

struct BT_PAGE_VM {
    int total;
    int data[0];
};

#define BT_LIST_MAX (10)

static struct BT_PAGE_VM *hd = NULL;


static int page_list_check(u8 *mac, u8 *name) //判断是否已经保存有
{
    if (!hd) {
        return -1;
    }
    struct BT_PAGE *fav = (struct BT_PAGE *)hd->data;
    for (int i = 0; i < BT_LIST_MAX; i++, fav++) {
        /* if (fav->used && !memcmp(mac, fav->mac, 6) && !memcmp(name, fav->name, strlen((const char *)name))) { */
        if (fav->used && !memcmp(mac, fav->mac, 6)) {
            return TRUE;
        }
    }
    return FALSE;
}

int page_list_init()
{
    int len = sizeof(struct BT_PAGE_VM) + sizeof(struct BT_PAGE) * BT_LIST_MAX;
    hd = (struct BT_PAGE_VM *)zalloc(len);
    struct BT_PAGE *fav = (struct BT_PAGE *)hd->data;
    int ret = 0;
    int count = 0;
    ret = syscfg_read(CFG_BT_PAGE_LIST, hd, len);
    for (int i = 0; i < BT_LIST_MAX; i++, fav++) {
        if (fav->used) {
            count++;
        }
    }
    if (len != ret || hd->total != count) {
        memset(hd, 0x00, len);
        syscfg_write(CFG_BT_PAGE_LIST, hd, len);
    }
    return 0;
}

late_initcall(page_list_init);


static int page_list_find_idle()//查找idle存储
{
    if (!hd) {
        return -1;
    }
    struct BT_PAGE *fav = (struct BT_PAGE *)hd->data;
    for (int i = 0; i < BT_LIST_MAX; i++, fav++) {
        if (!fav->used) {
            return i;
        }
    }
    return -1;
}

int page_list_add(u8 *mac, u8 *name)//添加收藏
{
    int ret = 0;
    int index = 0;
    if (!hd) {
        return -1;
    }
    struct BT_PAGE *fav = (struct BT_PAGE *)hd->data;
    if (hd->total >= BT_LIST_MAX) {
        return -1;
    }

    ret = page_list_check(mac, name);
    if (ret) {
        return ret;
    }
    index = page_list_find_idle();
    if (index == -1) {
        return -1;
    }
    local_irq_disable();
    memcpy(fav[index].mac, mac, 6);
    sprintf((char *)fav[index].name, "%s", name);
    fav[index].used = 1;
    hd->total++;
    local_irq_enable();
    printf("page_list_add success \n");

    /* syscfg_write(CFG_BT_PAGE_LIST, hd, sizeof(struct BT_PAGE_VM) + sizeof(struct BT_PAGE)*BT_LIST_MAX); //保存vm */
    return 0;
}


u8 *page_list_read_name(u8 *mac, int index) //index从1开始,获取收藏的蓝牙mac
{
    if (!hd) {
        return 0;
    }
    int find = 1;
    u8 *name = NULL;
    struct BT_PAGE *fav = (struct BT_PAGE *)hd->data;
    if (!index || index > BT_LIST_MAX) {
        return NULL;
    }

    for (int i = 0; i < BT_LIST_MAX; i++, fav++) {
        if (fav->used) {
            if (find++ == index) {
                if (mac) {
                    memcpy(mac, fav->mac, 6);
                }
                name = fav->name;
                return name;
            }
        }
    }
    return NULL;
}

int page_list_del(int index)//index从1开始,删除收藏
{
    if (!hd) {
        return -1;
    }

    int find = 1;
    struct BT_PAGE *fav = (struct BT_PAGE *)hd->data;
    if (index > BT_LIST_MAX) {
        return -1;
    }


    local_irq_disable();
    for (int i = 0; i < BT_LIST_MAX; i++, fav++) {
        if (fav->used) {
            if (find++ == index) {
                fav->used = 0;
                hd->total--;
                local_irq_enable();
                return 0;
            }
        }
    }
    local_irq_enable();
    return -1;
}

int page_list_del_by_mac(u8 *mac)
{
    if (!hd || !mac) {
        return -1;
    }

    struct BT_PAGE *fav = (struct BT_PAGE *)hd->data;
    local_irq_disable();
    for (int i = 0; i < BT_LIST_MAX; i++, fav++) {
        if (fav->used) {
            if (!memcmp(mac, fav->mac, 6)) {
                fav->used = 0;
                hd->total--;
                local_irq_enable();
                /* syscfg_write(CFG_BT_PAGE_LIST, hd, sizeof(struct BT_PAGE_VM) + sizeof(struct BT_PAGE)*BT_LIST_MAX); //保存vm */
                return 0;
            }
        }
    }
    local_irq_enable();
    return -1;
}


int page_list_get_count()//获得收藏数
{
    if (!hd) {
        return 0;
    }

    return  hd->total;
}


static int watch_bt_page_list_wtrite_vm(void *priv)
{
    int ret = 0;
    int len = sizeof(struct BT_PAGE_VM) + sizeof(struct BT_PAGE) * BT_LIST_MAX;
    if ((int)priv == (int)SYSCFG_WRITE_ERASE_STATUS) {
        memset(hd, 0x00, len);
    }
    ret =  syscfg_write(CFG_BT_PAGE_LIST, hd, len);
    if (ret != len) {
        printf("watch_bt_page_list_wtrite_vm err\n");
        return -1;
    }
    return ret;
}

REGISTER_WATCH_SYSCFG(bt_page_ops) = {
    .name = "bt_page_list",
    .read = NULL,
    .write = watch_bt_page_list_wtrite_vm,
};




#endif
#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

