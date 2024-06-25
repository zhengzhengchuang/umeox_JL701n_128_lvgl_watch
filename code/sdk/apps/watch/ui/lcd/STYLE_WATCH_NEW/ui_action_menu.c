#include "app_config.h"
#include "system/includes.h"
#include "ui/ui_sys_param.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "app_task.h"
#include "system/timer.h"
#include "device/device.h"
#include "key_event_deal.h"
#include "res/resfile.h"
#include "ui/res_config.h"
#include "music/music.h"

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


#define STYLE_NAME  JL

#define MENU_SEL_ID_VERTICAL		0
#define MENU_SEL_ID_MATRIX			1
#define MENU_SEL_ID_STAR			2
#define MENU_SEL_ID_MAX				3

typedef struct menu_callback {
    /* int vertical_sel_id; */
    /* int matrix_sel_id; */
    /* int star_sel_id; */
    int sel_id[MENU_SEL_ID_MAX];
    int (*exit_check)();
    int window;
    int (*enter_check)(int window);
} MENU_CB;

#ifndef	S_MENU_ALARM_LAYOUT
#define S_MENU_ALARM_LAYOUT 0
#endif
#ifndef S_MENU_FIND_PHONE_LAYOUT
#define S_MENU_FIND_PHONE_LAYOUT 0
#endif
#ifndef S_MENU_TIME_CLOCK_LAYOUT
#define S_MENU_TIME_CLOCK_LAYOUT 0
#endif
#ifndef S_MENU_PC_LAYOUT
#define S_MENU_PC_LAYOUT 0
#endif
#ifndef S_MENU_FLASHLIGHT_LAYOUT
#define S_MENU_FLASHLIGHT_LAYOUT 0
#endif
#ifndef S_MENU_CARD_BAG_LAYOUT
#define S_MENU_CARD_BAG_LAYOUT 0
#endif
#ifndef S_MENU_RECORD_LAYOUT
#define S_MENU_RECORD_LAYOUT	0
#endif
#ifndef S_MENU_CALCULATOR_LAYOUT
#define S_MENU_CALCULATOR_LAYOUT 0
#endif
#ifndef S_MENU_CALENDAR_LAYOUT
#define S_MENU_CALENDAR_LAYOUT 0
#endif

extern int setpage_enter_check(int *window);
extern int alipay_enter_check(int *window);

const static MENU_CB menu_cb[] = {
    {
        .sel_id		= {MENU_ALARM_LAYOUT,			M_MENU_ALARM_LAYOUT,		S_MENU_ALARM_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_ALARM,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_BARO_LAYOUT,			M_MENU_BARO_LAYOUT,  	  	S_MENU_BARO_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_BARO,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_BLOOD_LAYOUT, 			M_MENU_BLOOD_LAYOUT, 		S_MENU_BLOOD_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_BLOOD_OXYGEN,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_BRAETHE_LAYOUT, 		M_MENU_BRAETHE_LAYOUT, 		S_MENU_BRAETHE_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_BREATH_TRAIN,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_CALL_RECOED_LAYOUT,		M_MENU_CALL_RECOED_LAYOUT,	S_MENU_CALL_RECOED_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_CALLRECORD,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_ALIPAY_LAYOUT,        	M_MENU_ALIPAY_LAYOUT,       S_MENU_ALIPAY_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_ALIPAY,
        .enter_check	= alipay_enter_check
    },
    {
        .sel_id		= {MENU_CARD_BAG_LAYOUT,		M_MENU_CARD_BAG_LAYOUT,  	S_MENU_CARD_BAG_LAYOUT},
        .exit_check 	= NULL,
        .window 		= 0,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_COMPASS_LAYOUT,			M_MENU_COMPASS_LAYOUT, 		S_MENU_COMPASS_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_COMPASS,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_FIND_PHONE_LAYOUT,		M_MENU_FIND_PHONE_LAYOUT,  	S_MENU_FIND_PHONE_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_FINDPHONE,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_FLASHLIGHT_LAYOUT,		M_MENU_FLASHLIGHT_LAYOUT, 	S_MENU_FLASHLIGHT_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_FLASHLIGHT,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_HEART_LAYOUT,			M_MENU_HEART_LAYOUT,  		S_MENU_HEART_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_HEART,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_MESS_LAYOUT,			M_MENU_MESS_LAYOUT,    		S_MENU_MESS_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_MESS,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_MUSIC_LAYOUT,			M_MENU_MUSIC_LAYOUT,  		S_MENU_MUSIC_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_MUSIC,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_PHONEBOOK_LAYOUT,		M_MENU_PHONEBOOK_LAYOUT,  	S_MENU_PHONEBOOK_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_PHONEBOOK,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_PRESSURE_LAYOUT, 		M_MENU_PRESSURE_LAYOUT,		S_MENU_PRESSURE_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_PRESSURE,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_SET_LAYOUT,				M_MENU_SET_LAYOUT,  		S_MENU_SET_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_SET,
        .enter_check	= setpage_enter_check
    },
    {
        .sel_id		= {MENU_SLEEP_LAYOUT,			M_MENU_SLEEP_LAYOUT, 		S_MENU_SLEEP_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_SLEEP,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_SPORT_RECORD_LAYOUT,	M_MENU_SPORT_RECORD_LAYOUT, S_MENU_SPORT_RECORD_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_ACTIVERECORD,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_STOP_WATCH_LAYOUT,		M_MENU_STOP_WATCH_LAYOUT, 	S_MENU_STOP_WATCH_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_STOPWATCH,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_TIME_CLOCK_LAYOUT,		M_MENU_TIME_CLOCK_LAYOUT,  	S_MENU_TIME_CLOCK_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_CALCULAGRAPH,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_TRAIN_LAYOUT,			M_MENU_TRAIN_LAYOUT,  		S_MENU_TRAIN_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_TRAIN,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_TRAIN_RECORD_LAYOUT,	M_MENU_TRAIN_RECORD_LAYOUT, S_MENU_TRAIN_RECORD_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_SPORT_RECORD,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_TRAIN_STATUS_LAYOUT,	M_MENU_TRAIN_STATUS_LAYOUT, S_MENU_TRAIN_STATUS_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_TRAIN_STATUS,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_WEATHER_LAYOUT,			M_MENU_WEATHER_LAYOUT,  	S_MENU_WEATHER_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_WEATHER,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_PC_LAYOUT,				M_MENU_PC_LAYOUT,  			S_MENU_PC_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_PC,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_RECORD_LAYOUT,			M_MENU_RECORD_LAYOUT,  		S_MENU_RECORD_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_RECORD,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_CALCULATOR_LAYOUT,	M_MENU_CALCULATOR_LAYOUT, S_MENU_CALCULATOR_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_CALCULATOR,
        .enter_check	= NULL
    },
    {
        .sel_id		= {MENU_CALENDAR_LAYOUT,	M_MENU_CALENDAR_LAYOUR, S_MENU_CALENDAR_LAYOUT},
        .exit_check 	= NULL,
        .window 		= ID_WINDOW_CALENDAR,
        .enter_check	= NULL
    },

};

static menu_sel_enter(u8 menu_style, int id)
{
#if TCFG_PSRAM_UI_EFFECT
    static struct effect_sca_alpha sal = {0};
    sal.alpha = 100;
    sal.cnt = 5;
#endif
    if (menu_style >= MENU_SEL_ID_MAX) {
        log_error("menu_style err:%d \n", menu_style);
        return false;
    }
    for (int i = 0; i < (sizeof(menu_cb) / sizeof(menu_cb[0])); i++) {
        if (menu_cb[i].sel_id[menu_style] == id) {
            log_info("menu layout id = %x, window = %x\n", id, menu_cb[i].window);
            if (menu_cb[i].window) {
                int window = menu_cb[i].window;
                if (menu_cb[i].enter_check && (!menu_cb[i].enter_check(&window))) {
                    return false;
                }
                if (ui_page_get_auto_sw_effect() == 0) {
                    UI_HIDE_CURR_WINDOW();
                    UI_SHOW_WINDOW(window);
                } else {
#if TCFG_PSRAM_UI_EFFECT
                    ui_auto_sw_page_effect_post(UI_GET_WINDOW_ID(), window, EFFECT_MODE_AUTO_SCA_WITH_BG, NULL, &sal);
#endif
                }
            }
            return true;
            break;
        }
    }
    return false;
}

static int menu_in_sel(struct ui_grid *grid)
{
    u32 sel_item = ui_grid_cur_item(grid);

    if (sel_item >= grid->avail_item_num) {
        return false;
    }
    u8 menu_style = get_ui_sys_param(MenuStyle);
    int id = grid->item[sel_item].elm.id;
    log_info(">>>>>>>>>>>>>>sel_item=%d,id=%x", sel_item, id);
    return menu_sel_enter(menu_style, id);
}




static u32 item_memory;
static void menu_item_get(struct ui_grid *grid)
{
    item_memory = ui_grid_get_hindex(grid);
    if (item_memory >= grid->avail_item_num) {
        item_memory = 0;
    }
}
static void menu_item_set(struct ui_grid *grid, int item)
{
    ui_grid_set_hi_index(grid, item);
}

static int menu_switch_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    int sel_item;
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_MOVE:
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
        return menu_in_sel(grid);
        break;
    default:
        return false;
        break;
    }
    return true;//接管消息
}




static int menu_enter_onchane(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    /* static struct scroll_area area = {0, 0, 10000, 10000}; */

    switch (e) {
    case ON_CHANGE_INIT:
        /* ui_grid_set_scroll_area(grid, &area); */
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        menu_item_set(grid, item_memory);
        /* ui_grid_set_hi_move(grid, 1, NULL); */
        break;
    case ON_CHANGE_RELEASE:
        menu_item_get(grid);
        break;
    case ON_CHANGE_FIRST_SHOW:
        break;
    default:
        return false;
    }
    return false;
}



REGISTER_UI_EVENT_HANDLER(VERTLIST)
.onchange = menu_enter_onchane,
 .onkey = NULL,
  .ontouch = menu_switch_ontouch,
};



REGISTER_UI_EVENT_HANDLER(MATRIX_GRID)
.onchange = menu_enter_onchane,
 .onkey = NULL,
  .ontouch = menu_switch_ontouch, // NULL,//

};

//***********star_menu**************************//
static int star_menu_in_sel(struct element *elm)
{
    /* u8 menu_style = get_ui_sys_param(MenuStyle); */
    u8 menu_style = MENU_SEL_ID_STAR;
    int id = elm->id;
    printf(">>>>>>>>>>>>>>menu_style=%d,id=%x", menu_style, id);
    return menu_sel_enter(menu_style, id);
}

extern int ui_star_move_check(void);
static int star_menu_switch_ontouch(void *ctr, struct element_touch_event *e)
{
    struct element *elm = (struct element *)ctr;
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_MOVE:
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
            /* return false;//不接管消息 */
        }
        if (ui_star_move_check()) {
            return false;
        }
        return star_menu_in_sel(elm);
        break;
    default:
        return false;
        break;
    }
    return true;//接管消息
}

REGISTER_UI_EVENT_HANDLER(S_MENU_TRAIN_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_TRAIN_RECORD_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_TRAIN_STATUS_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_HEART_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_BLOOD_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_SPORT_RECORD_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_SLEEP_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_PRESSURE_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_BRAETHE_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_CALL_RECOED_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_PHONEBOOK_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_MUSIC_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_BARO_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_COMPASS_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_WEATHER_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_MESS_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_CARD_BAG_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_SET_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};
REGISTER_UI_EVENT_HANDLER(S_MENU_STOP_WATCH_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = star_menu_switch_ontouch,
};



#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

