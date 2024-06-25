#include "../lv_watch.h"

widget_num_str_para_t num_str_para;

int16_t common_widget_num_str_create(\
    widget_num_str_para_t *num_str_para)
{
    if(!num_str_para)
        return 0;

    if(!num_str_para->parent)
        return 0;

    if(!num_str_para->p_num_str)
        return 0;

    if(!num_str_para->num_str_len)
        return 0;

    if(num_str_para->num_obj)
    {
        if(num_str_para->num_str_len > \
            num_str_para->num_obj_max)
            num_str_para->num_str_len = \
                num_str_para->num_obj_max;
    }

    lv_obj_t *num_obj;
    uint32_t file_index = 0;
    uint16_t total_width = 0;
    for(uint8_t i = 0; i < num_str_para->num_str_len; i++)
    {
        if(num_str_para->p_num_str[i] == ':')
            file_index = num_str_para->file_00_index + \
                Comm_Symbol_10;
        else if(num_str_para->p_num_str[i] == '-')
            file_index = num_str_para->file_00_index + \
                Comm_Symbol_11;
        else if(num_str_para->p_num_str[i] == '/')
            file_index = num_str_para->file_00_index + \
                Comm_Symbol_12;
        else if(num_str_para->p_num_str[i] == '%')
            file_index = num_str_para->file_00_index + \
                Comm_Symbol_13;
        else if(num_str_para->p_num_str[i] == '.')
            file_index = num_str_para->file_00_index + \
                Comm_Symbol_14;
        else if(num_str_para->p_num_str[i] == '~')
            file_index = num_str_para->file_00_index + \
                Comm_Symbol_15;
        else if(num_str_para->p_num_str[i] == 'C')
            file_index = num_str_para->file_00_index + \
                Comm_Symbol_16;
        else if(num_str_para->p_num_str[i] == 'F')
            file_index = num_str_para->file_00_index + \
                Comm_Symbol_17;
        else if(num_str_para->p_num_str[i] == '^')
            file_index = num_str_para->file_00_index + \
                Comm_Symbol_18;
        else if(num_str_para->p_num_str[i] == '_')
            file_index = num_str_para->file_00_index + \
                Comm_Symbol_19;
        else if(num_str_para->p_num_str[i] == '*')
            file_index = num_str_para->file_00_index + \
                Comm_Symbol_20;
        else if(num_str_para->p_num_str[i] == '+')
            file_index = num_str_para->file_00_index + \
                Comm_Symbol_21;
        else if(num_str_para->p_num_str[i] >= '0' && \
            num_str_para->p_num_str[i] <= '9')
            file_index = num_str_para->file_00_index + \
                (num_str_para->p_num_str[i] - '0');

        widget_img_para.img_parent = \
            num_str_para->parent;
        widget_img_para.img_x = \
            num_str_para->num_obj_x + \
                total_width;
        widget_img_para.img_y = \
            num_str_para->num_obj_y;
        widget_img_para.file_img_dat = \
            file_index;
        widget_img_para.img_click_attr = \
            false;
        widget_img_para.event_cb = \
            NULL;
        
        if(num_str_para->num_dsc_idx)
            num_obj = \
                common_widget_img_create(&widget_img_para, \
                    &num_str_para->num_dsc_idx[i]);
        else
            num_obj = \
                common_widget_img_create(&widget_img_para, \
                    NULL);

        if(num_str_para->num_obj)
            num_str_para->num_obj[i] = \
                num_obj;

        lv_img_dsc_t *img_dsc = \
            common_widget_img_open_res(\
                file_index);
        total_width += img_dsc->header.w;
    }

    return (num_str_para->num_obj_x + total_width);
}


void common_widget_num_str_refresh(lv_obj_t **num_obj, uint8_t obj_max, \
    uint16_t *num_dsc_idx, char *p_num_str, uint8_t str_len, uint32_t index)
{
    if(!num_obj || obj_max == 0 || !num_dsc_idx ||\
        !p_num_str || str_len == 0)
        return;

    if(str_len > obj_max)
        str_len = obj_max;

    uint32_t file_index;
    for(uint8_t i = 0; i < str_len; i++)
    {
        if(p_num_str[i] == ':')
            file_index = index + \
                Comm_Symbol_10;
        else if(p_num_str[i] == '-')
            file_index = index + \
                Comm_Symbol_11;
        else if(p_num_str[i] == '/')
            file_index = index + \
                Comm_Symbol_12;
        else if(p_num_str[i] == '%')
            file_index = index + \
                Comm_Symbol_13;
        else if(p_num_str[i] == '.')
            file_index = index + \
                Comm_Symbol_14;
        else if(p_num_str[i] == '~')
            file_index = index + \
                Comm_Symbol_15;
        else if(p_num_str[i] == 'C')
            file_index = index + \
                Comm_Symbol_16;
        else if(p_num_str[i] == 'F')
            file_index = index + \
                Comm_Symbol_17;
        else if(p_num_str[i] == '^')
            file_index = index + \
                Comm_Symbol_18;
        else if(p_num_str[i] == '_')
            file_index = index + \
                Comm_Symbol_19;
        else if(p_num_str[i] == '*')
            file_index = index + \
                Comm_Symbol_20;
        else if(p_num_str[i] == '+')
            file_index = index + \
                Comm_Symbol_21;
        else if(p_num_str[i] >= '0' && \
            p_num_str[i] <= '9')
            file_index = index + \
                (p_num_str[i] - '0');

        common_widget_img_replace_src(num_obj[i], \
            file_index, num_dsc_idx[i]);
    }

    return;  
}