#include "../lv_watch.h"

/*********************************************************************************
                                  血氧数据参数                                       
*********************************************************************************/
static uint8_t bo_group_num = 0;
static widget_data_para_t bo_para_cache[\
    Bo_Group_Max] = {0};
static uint16_t bo_dsc_idx[Bo_Group_Max*\
    Bo_Obj_Max] = {0xffff};
static lv_obj_t *common_bo_widget[Bo_Group_Max*\
    Bo_Obj_Max] = {NULL};
static widget_data_type_t bo_data_type[Bo_Group_Max] \
    = {widget_data_type___};


static int16_t common_bo_widget_sx(uint8_t idx, \
    uint8_t data_bit_num, uint16_t num_width, 
        uint16_t symb_13_width, uint16_t *bo_data_width)
{
    int16_t bo_data_sx = 0;

    widget_data_para_t *p_bo_para = \
        &bo_para_cache[idx];

    if(!bo_data_width)
        return (p_bo_para->data_x);

    uint16_t width_sum = 0;
    for(uint8_t i = 0; i < data_bit_num; i++)
        width_sum += bo_data_width[i];

    widget_data_align_t data_align = \
        p_bo_para->data_align;

    if(data_align == widget_data_align_left)
        bo_data_sx = \
            p_bo_para->data_x;
    else if(data_align == widget_data_align_right)
        bo_data_sx = \
            p_bo_para->data_x + \
                (((Bo_Obj_Max-1)*num_width + symb_13_width) - \
                    width_sum);
    else if(data_align == widget_data_align_center)
        bo_data_sx = \
            p_bo_para->data_x + \
                (((Bo_Obj_Max-1)*num_width + symb_13_width) - \
                    width_sum)/2;
    else
        bo_data_sx = \
            p_bo_para->data_x;

    return bo_data_sx;
}

void common_bo_widget_init(void)
{
    if(!bo_group_num)
        return;

    memset(bo_dsc_idx, 0xffff, \
        sizeof(bo_dsc_idx));

    memset(common_bo_widget, 0, \
        sizeof(common_bo_widget));

    memset(bo_para_cache, 0, \
        sizeof(widget_data_para_t)*\
            bo_group_num);

    bo_group_num = 0;

    return;
}

void common_bo_widget_refresh(void)
{
    if(!bo_group_num)
        return;

    for(uint8_t i = 0; i < bo_group_num; i++)
    {
        widget_data_para_t *p_bo_para = \
            &bo_para_cache[i];

        u8 __data = \
            GetBoRealVal();

        uint32_t num_addr_index = \
            p_bo_para->num_addr_index;

        lv_img_dsc_t *data_img_dsc = \
            common_widget_img_open_res(\
                num_addr_index);
        uint16_t num_width = \
            data_img_dsc->header.w;

        uint32_t symb_13_addr_index = \
            num_addr_index + \
                Comm_Symbol_13;
        data_img_dsc = \
            common_widget_img_open_res(\
                symb_13_addr_index);
        uint16_t symb_13_width = \
            data_img_dsc->header.w;

        uint32_t symb_19_addr_index = \
            num_addr_index + \
                Comm_Symbol_19;
        data_img_dsc = \
            common_widget_img_open_res(\
                symb_19_addr_index);
        uint16_t symb_19_width = \
            data_img_dsc->header.w;

        char bo_num_str[Bo_Obj_Max + 1];
        memset(bo_num_str, 0, \
            sizeof(bo_num_str));

        if(!__data)
            memcpy(bo_num_str, (void *)"__%", 3);
        else
            sprintf(bo_num_str, "%d%%", __data);

        uint8_t data_bit_num = \
            strlen((const char *)bo_num_str); 

        uint32_t bo_num_index[Bo_Obj_Max];
        uint16_t bo_data_width[Bo_Obj_Max] = {0};
        for(uint8_t j = 0; j < data_bit_num; j++)
        {
            if(bo_num_str[j] == '_')
            {
                bo_data_width[j] = \
                    symb_19_width;
                bo_num_index[j] = \
                    symb_19_addr_index;
            }else if(bo_num_str[j] == '%')
            {
                bo_data_width[j] = \
                    symb_13_width;
                bo_num_index[j] = \
                    symb_13_addr_index;    
            }else
            {
                bo_data_width[j] = \
                    num_width;
                bo_num_index[j] = \
                    num_addr_index + \
                        (bo_num_str[j] - '0');
            }
        }

        int16_t bo_data_sx = 0;
        bo_data_sx = common_bo_widget_sx(i, \
            data_bit_num, num_width, symb_13_width, \
                bo_data_width);
                
        uint8_t obj_idx = \
            (Bo_Obj_Max - data_bit_num) + \
                Bo_Obj_Max*i;

        uint16_t width_sum = 0;
        int16_t refresh_x, refresh_y;
        for(uint8_t j = 0; j < data_bit_num; j++)
        {
            if(j)
                width_sum += \
                    bo_data_width[j - 1];

            if(!common_bo_widget[j + obj_idx] || \
                bo_dsc_idx[j + obj_idx] == 0xffff)
                continue;

            refresh_x = bo_data_sx + width_sum + \
                p_bo_para->num_inv*j;
            refresh_y = p_bo_para->data_y;
            lv_obj_set_pos(common_bo_widget[j + obj_idx], \
                refresh_x, refresh_y);
            
            common_widget_img_replace_src(common_bo_widget[j + obj_idx], \
                bo_num_index[j], bo_dsc_idx[j + obj_idx]);
            lv_obj_clear_flag(common_bo_widget[j + obj_idx], \
                LV_OBJ_FLAG_HIDDEN);
        }

        width_sum = 0;

        obj_idx = Bo_Obj_Max*i;
        for(uint8_t j = 0; j < (Bo_Obj_Max - data_bit_num); j++)
           lv_obj_add_flag(common_bo_widget[j + obj_idx], \
                LV_OBJ_FLAG_HIDDEN);
    }

    return;
}

int16_t common_bo_widget_create(widget_data_para_t *data_para, \
    widget_data_type_t type, void *data_val)
{
    int16_t data_end_x = 0;

    if(bo_group_num >= \
        Bo_Group_Max)
        return data_end_x;

    uint8_t __data = *(uint8_t *)data_val;

    bo_data_type[bo_group_num] = type;

    memcpy(&bo_para_cache[bo_group_num], \
        data_para, sizeof(widget_data_para_t));

    widget_data_para_t *p_bo_para = \
        &bo_para_cache[bo_group_num];

    uint32_t num_addr_index = \
        p_bo_para->num_addr_index;

    lv_img_dsc_t *data_img_dsc = \
        common_widget_img_open_res(\
            num_addr_index);
    uint16_t num_width = \
        data_img_dsc->header.w;

    uint32_t symb_13_addr_index = \
        num_addr_index + \
            Comm_Symbol_13;
    data_img_dsc = \
        common_widget_img_open_res(\
            symb_13_addr_index);
    uint16_t symb_13_width = \
        data_img_dsc->header.w;

    uint32_t symb_19_addr_index = \
        num_addr_index + \
            Comm_Symbol_19;
    data_img_dsc = \
        common_widget_img_open_res(\
            symb_19_addr_index);
    uint16_t symb_19_width = \
        data_img_dsc->header.w;


    char bo_num_str[Bo_Obj_Max + 1];
    memset(bo_num_str, 0, \
        sizeof(bo_num_str));

    if(!__data)
        memcpy(bo_num_str, (void *)"__%", 3);
    else
        sprintf(bo_num_str, "%d%%", __data);

    uint8_t data_bit_num = \
        strlen((const char *)bo_num_str);

    uint32_t bo_num_index[Bo_Obj_Max];
    uint16_t bo_data_width[Bo_Obj_Max] = {0};
    for(uint8_t i = 0; i < data_bit_num; i++)
    {
        if(bo_num_str[i] == '_')
        {
            bo_data_width[i] = \
                symb_19_width;
            bo_num_index[i] = \
                symb_19_addr_index;
        }else if(bo_num_str[i] == '%')
        {
            bo_data_width[i] = \
                symb_13_width;
            bo_num_index[i] = \
                symb_13_addr_index;    
        }else
        {
            bo_data_width[i] = \
                num_width;
            bo_num_index[i] = \
                num_addr_index + \
                    (bo_num_str[i] - '0');
        }      
    }

    int16_t bo_data_sx = 0;
    bo_data_sx = common_bo_widget_sx(bo_group_num, \
        data_bit_num, num_width, symb_13_width, \
            bo_data_width);

    uint8_t obj_idx = \
        (Bo_Obj_Max - data_bit_num) + \
            Bo_Obj_Max*bo_group_num;
    
    widget_img_para.event_cb = NULL;
    widget_img_para.user_data = NULL;
    widget_img_para.img_click_attr = false;
    widget_img_para.img_parent = \
        p_bo_para->data_parent;

    uint16_t width_sum = 0; 
    for(uint8_t i = 0; i < data_bit_num; i++)
    {
        if(i)
           width_sum += \
                bo_data_width[i - 1];

        widget_img_para.img_x = \
            bo_data_sx + width_sum + \
                p_bo_para->num_inv*i;
        widget_img_para.img_y = \
            p_bo_para->data_y;
        widget_img_para.file_img_dat = \
            bo_num_index[i];

        common_bo_widget[i + obj_idx] = \
            common_widget_img_create(&widget_img_para, \
                &bo_dsc_idx[i + obj_idx]);
        lv_obj_clear_flag(common_bo_widget[i + obj_idx], \
            LV_OBJ_FLAG_HIDDEN);
    }

    data_end_x = widget_img_para.img_x + \
        bo_data_width[data_bit_num - 1];

    obj_idx = Bo_Obj_Max*bo_group_num;
    for(uint8_t i = 0; i < (Bo_Obj_Max - data_bit_num); i++)
    {
        widget_img_para.file_img_dat = \
            num_addr_index;
        common_bo_widget[i + obj_idx] = \
            common_widget_img_create(&widget_img_para, \
                &bo_dsc_idx[i + obj_idx]);
        lv_obj_add_flag(common_bo_widget[i + obj_idx], \
            LV_OBJ_FLAG_HIDDEN);
    }

    bo_group_num++;

    return data_end_x;
}


