#ifndef _UI_SYS_PARAM__H_
#define _UI_SYS_PARAM__H_

#define     MAX_CARD_SELECT					(6)
#define     MAX_CARD_BACKUP_SELECT			(14)
#define     LANGUAGE_NUM					(4)
#define     SYS_PARAM_NUM                   (19)

enum {
    UI_PAGE_LIST_DEL,
    UI_PAGE_LIST_ADD,
    UI_PAGE_LIST_MOVE,
};

enum {
    CardSetNum,
    LightLevel,
    DarkTime,
    ShortcutKey,
    LastSysVol,
    LightAlwayEn,
    LightTime,
    SysVoiceMute,
    AllDayUndisturbEn,
    TimeUndisturbEn,
    UndisturbStimeH,
    UndisturbStimeM,
    UndisturbEtimeH,
    UndisturbEtimeM,
    TrainAutoEn,
    ConnNewPhone,
    Language,
    MenuStyle,
    MotoMode,
};

struct sys_param {
    u8 label;
    int value;
};

struct sys_vm_param {
    u8 valid;
    u8 card_select[MAX_CARD_SELECT];
    struct sys_param sys_param_table[SYS_PARAM_NUM];
};

struct set_info {
    int show_layout;

    u8 last_card_set_num;
    u8 card[MAX_CARD_BACKUP_SELECT];
    u8 vlist_card_index[4];

    u8 vslider_percent;

    /* u8 last_shake_level_sel; */

    u8 make_sure;
};

extern struct set_info set;
extern struct sys_vm_param ui_sys_param;

int write_UIInfo_to_vm(void *info);
int read_UIInfo_from_vm();
void erase_UIInfo_in_vm();
void restore_sys_settings();
void set_ui_sys_param(u8 label, int value);
int get_ui_sys_param(u8 label);
void ui_set_dark_time(u8 sel);
void screen_light_alway_switch(u8 on);
void ui_ajust_light(u8 level);
void ui_set_voice(int precent);
int ui_voice_to_percent(s8 volume);
void ui_volume_up(u8 step);
void ui_volume_down(u8 step);
void ui_voice_mute(u8 en);
void ui_page_list_update(u32 *id_list, u8 num);
void ui_moto_init(u8 gpio);
void ui_moto_set_H_L(u8 mode);
void ui_moto_run(u8 run_mode);
void watch_reboot_or_shutdown(u8 flag, u8 erase);

#endif

