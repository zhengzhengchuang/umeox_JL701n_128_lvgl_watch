#include "../lv_watch.h"

widget_time_para_t widget_time_para = {0};

/*********************************************************************************
                                  时间参数                                       
*********************************************************************************/
static uint8_t time_group_num = 0;
static widget_time_para_t time_para_cache[\
    Time_Group_Max] = {0};
static uint16_t time_dsc_idx[Time_Group_Max*\
    Time_Obj_Max] = {0xffff};
static lv_obj_t *common_time_widget[Time_Group_Max*\
    Time_Obj_Max] = {NULL};


void common_time_widget_init(void)
{
    if(!time_group_num)
        return;
        
    time_group_num = 0;

    memset(time_dsc_idx, 0xffff, \
        sizeof(time_dsc_idx));

    memset(common_time_widget, 0, \
        sizeof(common_time_widget));

    return;
}

void common_time_widget_refresh(void)
{
    if(!time_group_num)
        return;

    struct sys_time utc_time;
    GetUtcTime(&utc_time);

    uint8_t time_hour = \
        utc_time.hour;
    uint8_t time_minute = \
        utc_time.min;
    uint8_t time_second = \
        utc_time.sec;

    int time_format = \
        GetVmParaCacheByLabel(\
            vm_label_time_format);
    if(time_format == time_format_12)
    {
        if(time_hour > 12)
            time_hour -= 12;
        else if(time_hour == 0)
           time_hour = 12; 
    }

    char time_num_str[Time_Obj_Max + 1];
    memset(time_num_str, 0, \
        sizeof(time_num_str));
    sprintf(time_num_str, "%02d:%02d:%02d", \
        time_hour, time_minute, time_second);

    for(uint8_t i = 0; i < time_group_num; i++)
    {
        widget_time_para_t *p_time_para = \
            &time_para_cache[i];

        uint32_t num_addr_index = \
            p_time_para->num_addr_index;
        
        uint32_t time_addr_index[Time_Obj_Max];
        for(uint8_t j = 0; j < Time_Obj_Max; j++)
        {
            if(time_num_str[j] == ':')
            {
                time_addr_index[j] = \
                    num_addr_index + \
                        Comm_Symbol_10;
            }else
            {
                time_addr_index[j] = \
                    num_addr_index + \
                        (time_num_str[j] - '0');
            }

            if(!common_time_widget[j + i*Time_Obj_Max] || \
                time_dsc_idx[j + i*Time_Obj_Max] == 0xffff)
                continue;

            common_widget_img_replace_src(common_time_widget[j + i*Time_Obj_Max], \
                time_addr_index[j], time_dsc_idx[j + i*Time_Obj_Max]);
        }
    }

    return;
}


void common_time_widget_create(widget_time_para_t *time_para, \
    widget_time_mode_t time_mode)
{
    if(!time_para) return;

    if(time_group_num >= \
        Time_Group_Max)
        return;

    struct sys_time utc_time;
    GetUtcTime(&utc_time);

    uint8_t time_hour = \
        utc_time.hour;
    uint8_t time_minute = \
        utc_time.min;
    uint8_t time_second = \
        utc_time.sec;

    int time_format = \
        GetVmParaCacheByLabel(\
            vm_label_time_format);
    if(time_format == time_format_12)
    {
        if(time_hour > 12)
            time_hour -= 12;
        else if(time_hour == 0)
           time_hour = 12; 
    }

    memcpy(&time_para_cache[time_group_num], \
        time_para, sizeof(widget_time_para_t));

    widget_time_para_t *p_time_para = \
        &time_para_cache[time_group_num];

    char time_num_str[Time_Obj_Max + 1];
    memset(time_num_str, 0, \
        sizeof(time_num_str));
    sprintf(time_num_str, "%02d:%02d:%02d", \
        time_hour, time_minute, time_second);

    uint32_t num_addr_index = \
        p_time_para->num_addr_index;

    lv_img_dsc_t *time_img_dsc = \
        common_widget_img_open_res(\
            num_addr_index);
    uint16_t num_width = \
        time_img_dsc->header.w;

    uint32_t symb_10_addr_index = \
        num_addr_index + Comm_Symbol_10;
    time_img_dsc = \
        common_widget_img_open_res(\
            symb_10_addr_index);
    uint16_t symb_10_width = \
        time_img_dsc->header.w;

    uint32_t time_addr_index[Time_Obj_Max];
    uint16_t time_char_width[Time_Obj_Max];
    for(uint8_t i = 0; i < Time_Obj_Max; i++)
    {
        if(time_num_str[i] == ':')
        {
            time_char_width[i] = \
                symb_10_width;
            time_addr_index[i] = \
                symb_10_addr_index;
        }else
        {
            time_char_width[i] = \
                num_width;
            time_addr_index[i] = \
                num_addr_index + \
                    (time_num_str[i] - '0');
        }
    }

    uint8_t obj_idx = 0;
    uint8_t obj_num = 0;
    switch(time_mode)
    {
        case widget_time_mode_hh:
            obj_num = 2;
            obj_idx = 0 + \
                time_group_num*Time_Obj_Max;
            break;

        case widget_time_mode_mm:
            obj_num = 2;
            obj_idx = 3 + \
                time_group_num*Time_Obj_Max;
            break;

        case widget_time_mode_ss:
            obj_num = 2;
            obj_idx = 6 + \
                time_group_num*Time_Obj_Max;
            break;

        case widget_time_mode_hh_mm:
            obj_num = 5;
            obj_idx = 0 + \
                time_group_num*Time_Obj_Max;
            break;

        case widget_time_mode_mm_ss:
            obj_num = 5;
            obj_idx = 3 + \
                time_group_num*Time_Obj_Max;
            break;

        case widget_time_mode_hh_mm_ss:
            obj_num = Time_Obj_Max;
            obj_idx = 0 + \
                time_group_num*Time_Obj_Max;
            break;
        
        default:
            obj_num = Time_Obj_Max;
            obj_idx = 0 + \
                time_group_num*Time_Obj_Max;
            break;
    }

    if(obj_idx%Time_Obj_Max + obj_num > \
        Time_Obj_Max)
        return;

    widget_img_para.event_cb = NULL;
    widget_img_para.user_data = NULL;
    widget_img_para.img_click_attr = false;
    widget_img_para.img_parent = \
        p_time_para->time_parent;
    
    uint32_t width_sum = 0;
    for(uint8_t i = 0; i < obj_num; i++)
    {
        if(i)
            width_sum += \
                time_char_width[obj_idx%Time_Obj_Max + i - 1];

        widget_img_para.img_x = \
            p_time_para->time_x + \
                width_sum + p_time_para->num_inv*i;
        widget_img_para.img_y = \
            p_time_para->time_y;
        widget_img_para.file_img_dat = \
            time_addr_index[obj_idx%Time_Obj_Max + i];
        common_time_widget[obj_idx + i] = \
            common_widget_img_create(&widget_img_para, &time_dsc_idx[obj_idx + i]);
    }

    time_group_num++;

    return;
}

