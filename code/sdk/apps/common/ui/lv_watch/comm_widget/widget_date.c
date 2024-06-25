#include "../lv_watch.h"

widget_date_para_t widget_date_para = {0};

/*********************************************************************************
                                  日期参数                                       
*********************************************************************************/
static uint8_t date_group_num = 0;
static widget_date_para_t date_para_cache[\
    Date_Group_Max] = {0};
static uint16_t date_dsc_idx[Date_Group_Max*\
    Date_Obj_Max] = {0xffff};
static lv_obj_t *common_date_widget[Date_Group_Max*\
    Date_Obj_Max] = {NULL};


void common_date_widget_init(void)
{
    if(!date_group_num)
        return;

    date_group_num = 0;

    memset(date_dsc_idx, 0xffff, \
        sizeof(date_dsc_idx));

    memset(common_date_widget, 0, \
        sizeof(common_date_widget));

    return;
}

void common_date_widget_refresh(void)
{
    if(!date_group_num)
        return;

    struct sys_time utc_time;
    GetUtcTime(&utc_time);

    uint16_t date_year = \
        utc_time.year;
    uint8_t date_month = \
        utc_time.month;
    uint8_t date_day = \
        utc_time.day;

    char date_num_str[Date_Obj_Max + 1];
    memset(date_num_str, 0, \
        sizeof(date_num_str));
    sprintf(date_num_str, "%04d/%02d/%02d", \
        date_year, date_month, date_day);

    for(uint8_t i = 0; i < date_group_num; i++)
    {
        widget_date_para_t *p_date_para = \
            &date_para_cache[i];

        uint32_t num_addr_index = \
            p_date_para->num_addr_index;
        
        uint32_t date_addr_index[Date_Obj_Max];
        for(uint8_t j = 0; j < Date_Obj_Max; j++)
        {
            if(date_num_str[j] == '/')
            {
                date_addr_index[j] = \
                    num_addr_index + \
                        Comm_Symbol_12;
            }else
            {
                date_addr_index[j] = \
                    num_addr_index + \
                        (date_num_str[j] - '0');
            }

            if(!common_date_widget[j + i*Date_Obj_Max] || \
                date_dsc_idx[j + i*Date_Obj_Max] == 0xffff)
                continue;

            common_widget_img_replace_src(common_date_widget[j + i*Date_Obj_Max], \
                date_addr_index[j], date_dsc_idx[j + i*Date_Obj_Max]);
        }
    }

    return;
}

void common_date_widget_create(widget_date_para_t *date_para, \
    widget_date_mode_t date_mode)
{
    if(!date_para) return;

    if(date_group_num >= \
        Date_Group_Max)
        return;

    struct sys_time utc_time;
    GetUtcTime(&utc_time);

    uint16_t date_year = \
        utc_time.year;
    uint8_t date_month = \
        utc_time.month;
    uint8_t date_day = \
        utc_time.day;

    memcpy(&date_para_cache[date_group_num], \
        date_para, sizeof(widget_date_para_t));

    widget_date_para_t *p_date_para = \
        &date_para_cache[date_group_num];

    char date_num_str[Date_Obj_Max + 1];
    memset(date_num_str, 0, \
        sizeof(date_num_str));
    sprintf(date_num_str, "%04d/%02d/%02d", \
        date_year, date_month, date_day);

    uint32_t num_addr_index = \
        p_date_para->num_addr_index;

    lv_img_dsc_t *date_img_dsc = \
        common_widget_img_open_res(\
            num_addr_index);
    uint16_t num_width = \
        date_img_dsc->header.w;

    uint32_t symb_12_addr_index = \
        num_addr_index + Comm_Symbol_12;
    date_img_dsc = \
        common_widget_img_open_res(\
            symb_12_addr_index);
    uint16_t symb_12_width = \
        date_img_dsc->header.w;

    uint32_t date_addr_index[Date_Obj_Max];
    uint16_t date_char_width[Date_Obj_Max];
    for(uint8_t i = 0; i < Date_Obj_Max; i++)
    {
        if(date_num_str[i] == '/')
        {
            date_char_width[i] = \
                symb_12_width;
            date_addr_index[i] = \
                symb_12_addr_index;
        }else
        {
            date_char_width[i] = \
                num_width;
            date_addr_index[i] = \
                num_addr_index + \
                    (date_num_str[i] - '0');
        }
    }

    uint8_t obj_idx = 0;
    uint8_t obj_num = 0;

    switch(date_mode)
    {
        case widget_date_mode_yyyy:
            obj_num = 4;
            obj_idx = 0 + \
                date_group_num*Date_Obj_Max;
            break;

        case widget_date_mode_mm:
            obj_num = 2;
            obj_idx = 5 + \
                date_group_num*Date_Obj_Max;
            break;

        case widget_date_mode_dd:
            obj_num = 2;
            obj_idx = 8 + \
                date_group_num*Date_Obj_Max;
            break;

        case widget_date_mode_yyyy_mm:
            obj_num = 7;
            obj_idx = 0 + \
                date_group_num*Date_Obj_Max;
            break;

        case widget_date_mode_mm_dd:
            obj_num = 5;
            obj_idx = 5 + \
                date_group_num*Date_Obj_Max;
            break;

        case widget_date_mode_yyyy_mm_dd:
            obj_num = Date_Obj_Max;
            obj_idx = 0 + \
                date_group_num*Date_Obj_Max;
            break;
        
        default:
            obj_num = Date_Obj_Max;
            obj_idx = 0 + \
                date_group_num*Date_Obj_Max;
            break;
    }

    if(obj_idx%Date_Obj_Max + obj_num > \
        Date_Obj_Max)
        return;

    widget_img_para.event_cb = NULL;
    widget_img_para.user_data = NULL;
    widget_img_para.img_click_attr = false;
    widget_img_para.img_parent = \
        p_date_para->date_parent;

    uint32_t width_sum = 0;
    for(uint8_t i = 0; i < obj_num; i++)
    {
        if(i)
            width_sum += \
                date_char_width[obj_idx%Date_Obj_Max + i - 1];

        widget_img_para.img_x = \
            p_date_para->date_x + \
                width_sum + p_date_para->num_inv*i;
        widget_img_para.img_y = \
            p_date_para->date_y;
        widget_img_para.file_img_dat = \
            date_addr_index[obj_idx%Date_Obj_Max + i];
        common_date_widget[obj_idx + i] = \
            common_widget_img_create(&widget_img_para, &date_dsc_idx[obj_idx + i]);
    }

    date_group_num++;

    return;
}