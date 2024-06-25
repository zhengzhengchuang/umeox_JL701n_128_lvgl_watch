#include "key_event_deal.h"
#include "key_driver.h"
#include "app_config.h"
#include "board_config.h"
#include "app_task.h"

#ifdef CONFIG_BOARD_AD2802E_DEMO
/***********************************************************
 *				bt 模式的 iokey table
 ***********************************************************/
#if TCFG_APP_BT_EN
const u16 bt_key_io_table[KEY_IO_NUM_MAX][KEY_EVENT_MAX] = {
    //单击             //长按          //hold         //抬起            //双击                //三击
    [0] = {
        KEY_UI_HOME, KEY_UI_POWEROFF
    },
    [1] = {
        KEY_UI_SHORTCUT
    },
    [2] = {
        KEY_NULL, KEY_NULL
    },
    [3] = {
        KEY_CHANGE_PAGE
    },
};
#endif

/***********************************************************
 *				fm 模式的 iokey table
 ***********************************************************/
#if TCFG_APP_FM_EN
const u16 fm_key_io_table[KEY_IO_NUM_MAX][KEY_EVENT_MAX] = {
    //单击             //长按          //hold         //抬起            //双击                //三击
    [0] = {
        KEY_UI_HOME, KEY_UI_POWEROFF
    },
    [1] = {
        KEY_UI_SHORTCUT
    },
    [2] = {
        KEY_NULL, KEY_NULL
    },
    [3] = {
        KEY_CHANGE_PAGE
    },
};
#endif

/***********************************************************
 *				linein 模式的 iokey table
 ***********************************************************/
#if TCFG_APP_LINEIN_EN
const u16 linein_key_io_table[KEY_IO_NUM_MAX][KEY_EVENT_MAX] = {
    //单击             //长按          //hold         //抬起            //双击                //三击
    [0] = {
        KEY_UI_HOME, KEY_UI_POWEROFF
    },
    [1] = {
        KEY_UI_SHORTCUT
    },
    [2] = {
        KEY_NULL, KEY_NULL
    },
    [3] = {
        KEY_CHANGE_PAGE
    },
};
#endif

/***********************************************************
 *				music 模式的 iokey table
 ***********************************************************/
#if TCFG_APP_MUSIC_EN
const u16 music_key_io_table[KEY_IO_NUM_MAX][KEY_EVENT_MAX] = {
    //单击             //长按          //hold         //抬起            //双击                //三击
    [0] = {
        KEY_UI_HOME, KEY_UI_POWEROFF
    },
    [1] = {
        KEY_UI_SHORTCUT
    },
    [2] = {
        KEY_NULL, KEY_NULL
    },
    [3] = {
        KEY_CHANGE_PAGE
    },
};
#endif

/***********************************************************
 *				pc 模式的 iokey table
 ***********************************************************/
#if TCFG_APP_PC_EN
const u16 pc_key_io_table[KEY_IO_NUM_MAX][KEY_EVENT_MAX] = {
    //单击             //长按          //hold         //抬起            //双击                //三击
    [0] = {
        KEY_UI_HOME, KEY_UI_POWEROFF
    },
    [1] = {
        KEY_UI_SHORTCUT
    },
    [2] = {
        KEY_NULL, KEY_NULL
    },
    [3] = {
        KEY_CHANGE_PAGE
    },
};
#endif

/***********************************************************
 *				record 模式的 iokey table
 ***********************************************************/
#if TCFG_APP_RECORD_EN
const u16 record_key_io_table[KEY_IO_NUM_MAX][KEY_EVENT_MAX] = {
    //单击             //长按          //hold         //抬起            //双击                //三击
    [0] = {
        KEY_UI_HOME, KEY_UI_POWEROFF
    },
    [1] = {
        KEY_UI_SHORTCUT
    },
    [2] = {
        KEY_NULL, KEY_NULL
    },
    [3] = {
        KEY_CHANGE_PAGE
    },
};
#endif

/***********************************************************
 *				rtc 模式的 iokey table
 ***********************************************************/
#if TCFG_APP_RTC_EN
const u16 rtc_key_io_table[KEY_IO_NUM_MAX][KEY_EVENT_MAX] = {
    //单击             //长按          //hold         //抬起            //双击                //三击
    [0] = {
        KEY_UI_HOME, KEY_UI_POWEROFF
    },
    [1] = {
        KEY_UI_SHORTCUT
    },
    [2] = {
        KEY_NULL, KEY_NULL
    },
    [3] = {
        KEY_CHANGE_PAGE
    },
};
#endif

/***********************************************************
 *				spdif 模式的 iokey table
 ***********************************************************/
#if TCFG_APP_SPDIF_EN
const u16 spdif_key_io_table[KEY_IO_NUM_MAX][KEY_EVENT_MAX] = {
    //单击             //长按          //hold         //抬起            //双击                //三击
    [0] = {
        KEY_UI_HOME, KEY_UI_POWEROFF
    },
    [1] = {
        KEY_UI_SHORTCUT
    },
    [2] = {
        KEY_NULL, KEY_NULL
    },
    [3] = {
        KEY_CHANGE_PAGE
    },
};
#endif

/***********************************************************
 *				idle 模式的 iokey table
 ***********************************************************/
const u16 idle_key_io_table[KEY_IO_NUM_MAX][KEY_EVENT_MAX] = {
    //单击             //长按          //hold         //抬起            //双击                //三击
    [0] = {
        KEY_NULL/*KEY_UI_HOME*/, KEY_UI_POWEROFF
    },
    [1] = {
        KEY_NULL/*KEY_UI_SHORTCUT*/
    },
    [2] = {
        KEY_NULL, KEY_NULL
    },
    [3] = {
        KEY_CHANGE_PAGE
    },
};
#endif
