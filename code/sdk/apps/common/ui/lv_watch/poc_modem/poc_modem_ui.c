#include "../lv_watch.h"
#include "poc_modem_ui.h"

static bool lcd_is_sleep = false;

void ui_mode_handle(ui_act_id_t act_id)
{
    ui_mode_t *ui_mode = \
        &(p_ui_info_cache->ui_mode);

    if(act_id == ui_act_id_watchface)
        *ui_mode = ui_mode_watchface;
    else if(act_id == ui_act_id_menu)
        *ui_mode = ui_mode_menu_list;
    else if(act_id == ui_act_id_tool_box)
        *ui_mode = ui_mode_tool_box;

    return;
}

void clear_menu_return_level(void)
{
    ui_act_id_t *prev_act_id = \
        p_ui_info_cache->prev_act_id;
    uint8_t *menu_return_level = \
        &(p_ui_info_cache->menu_return_level);

    if(*menu_return_level == 0)
        return;

    *menu_return_level = 0;
    for(uint8_t i = 0; i < Menu_Return_Level; i++)
        prev_act_id[i] = ui_act_id_null;
    
    printf("%s\n", __func__);

    return;
}

ui_act_id_t read_menu_return_level_id(void)
{
    ui_act_id_t *prev_act_id = \
        p_ui_info_cache->prev_act_id;
    uint8_t menu_return_level = \
        p_ui_info_cache->menu_return_level;
    
    ui_act_id_t new_prev_act_id = \
        ui_act_id_watchface;

    if(menu_return_level == 0)
        return new_prev_act_id;

    ui_act_id_t cur_act_id = \
        p_ui_info_cache->cur_act_id;

    int8_t i;
    for(i = (menu_return_level - 1); \
        i >= 0; i--)
    {   
        if(prev_act_id[i] == cur_act_id)
            continue;
        else
            new_prev_act_id = \
                prev_act_id[i];

        break;
    }

    printf("new_prev_act_id = %d\n", new_prev_act_id);

    return new_prev_act_id;  
}

void push_menu_return_level(ui_act_id_t act_id)
{
    ui_act_id_t *prev_act_id = \
        p_ui_info_cache->prev_act_id;
    uint8_t *menu_return_level = \
        &(p_ui_info_cache->menu_return_level);

    if(*menu_return_level >= \
        Menu_Return_Level)
        return;

    if(menu_return_level_exist_handle(act_id))
        return;

    prev_act_id[*menu_return_level] = \
        act_id;
    (*menu_return_level) += 1;

    for(uint8_t i = 0; i < *menu_return_level; i++)
        printf("%s:act_id %d\n", __func__, \
            prev_act_id[i]);
    printf("%s:level deep %d\n", __func__, \
        *menu_return_level);

    return;
}

bool menu_return_level_exist_handle(ui_act_id_t act_id)
{
    ui_act_id_t *prev_act_id = \
        p_ui_info_cache->prev_act_id;
    uint8_t *menu_return_level = \
        &(p_ui_info_cache->menu_return_level);

    bool act_id_exist = false;

    if(*menu_return_level == 0)
        return act_id_exist;

    int8_t i;
    for(i = (*menu_return_level - 1); \
        i >= 0; i--)
    {
        if(prev_act_id[i] == act_id)
        {
            act_id_exist = true;
            break;
        }    
    }

    if(act_id_exist)
    {
        for(int8_t j = (i + 1); j < *menu_return_level; j++)
            prev_act_id[j] = ui_act_id_null;

        *menu_return_level = i + 1;
    }

    for(i = 0; i < *menu_return_level; i++)
        printf("%s:act_id %d\n", __func__, \
            prev_act_id[i]);
    printf("%s:level deep %d\n", __func__, \
        *menu_return_level);

    return act_id_exist;
}

void ui_menu_jump_post_msg(ui_act_id_t act_id)
{   
    int msg_buf[2] = {0};
    uint8_t msg_arg_len = 2;

    msg_buf[0] = ui_msg_menu_jump;
    msg_buf[1] = act_id;

    post_ui_msg(msg_buf, msg_arg_len);

    return;
}

void ui_menu_exit_prepare(ui_act_id_t act_id)
{
    common_refresh_timer_destroy();

    common_offscreen_timer_destroy();

    ui_menu_destory_func_cb destory_func_cb = \
        p_ui_info_cache->menu_load_info.destory_func_cb;
    if(destory_func_cb)
        destory_func_cb(NULL);

    if(p_ui_info_cache->ui_menu_container)
    {
        lv_obj_del(p_ui_info_cache->ui_menu_container);
        p_ui_info_cache->ui_menu_container = NULL;
    }

#if UI_USE_COVER
    cover_info_clear();
#endif

#if UI_USE_TILEVIEW
    uint8_t tileview_valid_cnt = \
        p_ui_info_cache->ui_tileview_info.tileview_valid_cnt;
    if(tileview_valid_cnt)
        tileview_info_clear();
#endif

#if UI_USE_TRANSLATE
    translate_info_clear();
#endif

    common_widget_para_init();

    if(!lcd_is_sleep)
        memcpy(&p_ui_info_cache->exit_menu_load_info, &p_ui_info_cache->menu_load_info, \
            sizeof(ui_menu_load_info_t));

    if(ui_act_id_validity(act_id))
    {
        //回到主表盘,清空返回级数列表
        if(act_id == ui_act_id_watchface)
            clear_menu_return_level();

        lcd_is_sleep = false;
    }else
    {
        lcd_is_sleep = true;
    }
        
    memset(&p_ui_info_cache->menu_load_info, 0, \
        sizeof(ui_menu_load_info_t));

    return;
}

ui_act_id_t ui_menu_jump_prepare(ui_act_id_t act_id)
{
    if(ui_act_id_validity(act_id))
        p_ui_info_cache->cur_act_id = act_id;

    return act_id;
}

void ui_menu_container_create(void)
{
    int16_t lcd_w = lv_disp_get_hor_res(NULL);
    int16_t lcd_h = lv_disp_get_ver_res(NULL);

    lv_obj_t *ui_menu_container = lv_obj_create(lv_scr_act());
    lv_obj_remove_style(ui_menu_container, NULL, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_remove_style(ui_menu_container, NULL, LV_PART_SCROLLBAR|LV_STATE_PRESSED);
    lv_obj_set_size(ui_menu_container, lcd_w, lcd_h);
    lv_obj_set_pos(ui_menu_container, 0, 0);
    lv_obj_set_style_border_width(ui_menu_container, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_menu_container, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_pad_all(ui_menu_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(ui_menu_container, LV_OBJ_FLAG_SCROLLABLE);

    p_ui_info_cache->ui_menu_container = ui_menu_container;

    printf("%s:%p\n", __func__, ui_menu_container);
 
    return;
}

void ui_menu_jump_handle(ui_act_id_t act_id)
{
    ui_mode_handle(act_id);

    ui_menu_exit_prepare(act_id);
    ui_act_id_t load_act_id = ui_menu_jump_prepare(act_id);

    ui_menu_load_info_t *menu_load_info = \
        ui_menu_load_info(load_act_id);
    if(menu_load_info)
    {
        memcpy(&p_ui_info_cache->menu_load_info, \
            menu_load_info, sizeof(ui_menu_load_info_t));

        ui_mode_t ui_mode = \
            p_ui_info_cache->ui_mode;
        bool return_flag = \
            p_ui_info_cache->menu_load_info.return_flag;
        if(return_flag/* && ui_mode != ui_mode_watchface*/)
            push_menu_return_level(act_id);
    }else
    {
        printf("%s:menu_load_info is NULL!!!\n", __func__);

        return;
    }
   
    ui_menu_container_create();

    ui_menu_create_func_cb create_func_cb = \
        p_ui_info_cache->menu_load_info.create_func_cb;
    if(create_func_cb)
        create_func_cb(p_ui_info_cache->ui_menu_container);
 
    common_refresh_timer_create();

    common_offscreen_timer_create();

    widget_refresh_timer_create();

    return;
}

ui_menu_load_info_t *ui_menu_load_info(ui_act_id_t act_id)
{
    bool list_find_ret = false;
    ui_menu_load_info_t *menu_load_info = NULL;

    if(!ui_act_id_validity(act_id))
        return NULL;

    list_for_ui_menu_load(menu_load_info)
    {
        if(menu_load_info->menu_id == act_id)
        {
            list_find_ret = true;
            break;
        }    
    }

    if(!list_find_ret)
        return NULL;


    /*对于多页面使用同个id，需要单独拎出来做处理，如：表盘、多风格菜单*/
    if(act_id == ui_act_id_watchface)
    {
        ui_watchface_id_t cur_watchface_id = \
            GetVmParaCacheByLabel(vm_label_watchface_sel);

        ui_watchface_id_t i;
        list_for_ui_watchface_load(i)
        {
            if(cur_watchface_id == i)
            {
                menu_load_info = watchface_load_info[i];
                break;
            }    
        }
    }else if(act_id == ui_act_id_menu)
    {
        ui_menu_view_t menu_style = \
            GetVmParaCacheByLabel(vm_label_menu_view);

        ui_menu_view_t i;
        list_for_ui_menu_style(i)
        {
            if(menu_style == i)
            {
                menu_load_info = menu_style_load_info[i];
                break;
            }    
        }
    }else if(act_id == ui_act_id_al_name_list)
    {
        int al_name_list_mode = \
            GetVmParaCacheByLabel(\
                vm_label_al_name_list_mode);
        
        al_name_list_mode_t i;
        list_for_ui_al_name_list(i)
        {
            if(i == al_name_list_mode)
            {
                menu_load_info = \
                    al_name_load_info[i];
                break;
            }
        }
    }

    return menu_load_info;
}
