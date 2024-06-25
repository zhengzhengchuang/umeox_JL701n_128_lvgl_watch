#include "../lv_watch.h"

/*********************************************************************************
                                  卡路里数据参数                                       
*********************************************************************************/
static uint8_t calorie_group_num = 0;
static widget_data_para_t calorie_para_cache[\
    Calorie_Group_Max] = {0};
static uint16_t calorie_dsc_idx[Calorie_Group_Max*\
    Calorie_Obj_Max] = {0xffff};
static lv_obj_t *common_calorie_widget[Calorie_Group_Max*\
    Calorie_Obj_Max] = {NULL};
static widget_data_type_t calorie_data_type[Calorie_Group_Max] \
    = {widget_data_type___};


static int16_t common_calorie_widget_sx(uint8_t idx, \
    uint8_t data_bit_num, uint16_t num_width, 
        uint16_t *calorie_data_width)
{
    int16_t calorie_data_sx = 0;

    widget_data_para_t *p_calorie_para = \
        &calorie_para_cache[idx];

    if(!calorie_data_width)
        return (p_calorie_para->data_x);

    uint16_t width_sum = 0;
    for(uint8_t i = 0; i < data_bit_num; i++)
        width_sum += calorie_data_width[i];

    widget_data_align_t data_align = \
        p_calorie_para->data_align;

    if(data_align == widget_data_align_left)
        calorie_data_sx = \
            p_calorie_para->data_x;
    else if(data_align == widget_data_align_right)
        calorie_data_sx = \
            p_calorie_para->data_x + \
                ((Calorie_Obj_Max*num_width) - \
                    width_sum);
    else if(data_align == widget_data_align_center)
        calorie_data_sx = \
            p_calorie_para->data_x + \
                ((Calorie_Obj_Max*num_width) - \
                    width_sum)/2;
    else
        calorie_data_sx = \
            p_calorie_para->data_x;

    return calorie_data_sx;
}

void common_calorie_widget_init(void)
{
    if(!calorie_group_num)
        return;

    memset(calorie_dsc_idx, 0xffff, \
        sizeof(calorie_dsc_idx));

    memset(common_calorie_widget, 0, \
        sizeof(common_calorie_widget));

    memset(calorie_para_cache, 0, \
        sizeof(widget_data_para_t)* \
            calorie_group_num);

    calorie_group_num = 0;

    return;
}

void common_calorie_widget_refresh(void)
{
    if(!calorie_group_num)
        return;

    for(uint8_t i = 0; i < calorie_group_num; i++)
    {
        widget_data_para_t *p_calorie_para = \
            &calorie_para_cache[i];

        uint16_t vm_label_type = \
            vm_label_none;

        if(calorie_data_type[i] == \
            widget_data_type_calorie)
            vm_label_type = vm_label_daily_calorie;
        else
            continue;

        int __data = \
            GetVmParaCacheByLabel(\
                vm_label_type); 

        if(__data > Calorie_Disp_Max)
            __data = Calorie_Disp_Max;

        uint32_t num_addr_index = \
            p_calorie_para->num_addr_index;

        lv_img_dsc_t *data_img_dsc = \
            common_widget_img_open_res(\
                num_addr_index);
        uint16_t num_width = \
            data_img_dsc->header.w;

        uint32_t symb_19_addr_index = \
            num_addr_index + \
                Comm_Symbol_19;
        data_img_dsc = \
            common_widget_img_open_res(\
                symb_19_addr_index);
        uint16_t symb_19_width = \
            data_img_dsc->header.w;

        char calorie_num_str[Calorie_Obj_Max + 1];
        memset(calorie_num_str, 0, \
            sizeof(calorie_num_str));

        if(!__data)
            memcpy(calorie_num_str, (void *)"__", 2);
        else
            sprintf(calorie_num_str, "%d", __data);

        uint8_t data_bit_num = \
            strlen((const char *)calorie_num_str);

        uint32_t calorie_num_index[Calorie_Obj_Max];
        uint16_t calorie_data_width[Calorie_Obj_Max];
        for(uint8_t j = 0; j < data_bit_num; j++)
        {
            if(calorie_num_str[j] == '_')
            {
                calorie_data_width[j] = \
                    symb_19_width;
                calorie_num_index[j] = \
                    symb_19_addr_index;
            }else
            {
                calorie_data_width[j] = \
                    num_width;
                calorie_num_index[j] = \
                    num_addr_index + \
                        (calorie_num_str[j] - '0');
            }
        }

        int16_t calorie_data_sx = 0;
        calorie_data_sx = common_calorie_widget_sx(i, \
            data_bit_num, num_width, \
                calorie_data_width);

        uint8_t obj_idx = \
        (Calorie_Obj_Max - data_bit_num) + \
            Calorie_Obj_Max*i;

        uint16_t width_sum = 0;
        int16_t refresh_x, refresh_y;
        for(uint8_t j = 0; j < data_bit_num; j++)
        {
            if(j)
               width_sum += \
                calorie_data_width[j - 1];

            if(!common_calorie_widget[j + obj_idx] || \
                calorie_dsc_idx[j + obj_idx] == 0xffff)
                continue;

            refresh_x = calorie_data_sx + width_sum + \
                p_calorie_para->num_inv*j;
            refresh_y = p_calorie_para->data_y;
            lv_obj_set_pos(common_calorie_widget[j + obj_idx], \
                refresh_x, refresh_y);
            
            common_widget_img_replace_src(common_calorie_widget[j + obj_idx], \
                calorie_num_index[j], calorie_dsc_idx[j + obj_idx]);
            lv_obj_clear_flag(common_calorie_widget[j + obj_idx], \
                LV_OBJ_FLAG_HIDDEN);
        }

        width_sum = 0;

        obj_idx = Calorie_Obj_Max*i;
        for(uint8_t j = 0; j < (Calorie_Obj_Max - data_bit_num); j++)
           lv_obj_add_flag(common_calorie_widget[j + obj_idx], \
                LV_OBJ_FLAG_HIDDEN); 
    }

    return;
}

int16_t common_calorie_widget_create(widget_data_para_t *data_para, \
    widget_data_type_t type, void *data_val)
{
    int16_t data_end_x = 0;

    if(calorie_group_num >= \
        Calorie_Group_Max)
        return data_end_x;

    int __data = *(int *)data_val;

    if(__data > Calorie_Disp_Max)
        __data = Calorie_Disp_Max;

    calorie_data_type[calorie_group_num] = type;

    memcpy(&calorie_para_cache[calorie_group_num], \
        data_para, sizeof(widget_data_para_t));

    widget_data_para_t *p_calorie_para = \
        &calorie_para_cache[calorie_group_num];

    uint32_t num_addr_index = \
        p_calorie_para->num_addr_index;

    lv_img_dsc_t *data_img_dsc = \
        common_widget_img_open_res(\
            num_addr_index);
    uint16_t num_width = \
        data_img_dsc->header.w;

    uint32_t symb_19_addr_index = \
        num_addr_index + \
            Comm_Symbol_19;
    data_img_dsc = \
        common_widget_img_open_res(\
            symb_19_addr_index);
    uint16_t symb_19_width = \
        data_img_dsc->header.w;

    char calorie_num_str[Calorie_Obj_Max + 1];
    memset(calorie_num_str, 0, \
        sizeof(calorie_num_str));

    if(!__data)
        memcpy(calorie_num_str, (void *)"__", 2);
    else
        sprintf(calorie_num_str, "%d", __data);

    uint8_t data_bit_num = \
        strlen((const char *)calorie_num_str);

    uint32_t calorie_num_index[Calorie_Obj_Max];
    uint16_t calorie_data_width[Calorie_Obj_Max];
    for(uint8_t i = 0; i < data_bit_num; i++)
    {
        if(calorie_num_str[i] == '_')
        {
            calorie_data_width[i] = \
                symb_19_width;
            calorie_num_index[i] = \
                symb_19_addr_index;
        }else
        {
            calorie_data_width[i] = \
                num_width;
            calorie_num_index[i] = \
                num_addr_index + \
                    (calorie_num_str[i] - '0');
        }
    }

    int16_t calorie_data_sx = 0;
    calorie_data_sx = common_calorie_widget_sx(calorie_group_num, \
        data_bit_num, num_width, calorie_data_width);

    uint8_t obj_idx = \
        (Calorie_Obj_Max - data_bit_num) + \
            Calorie_Obj_Max*calorie_group_num;
    
    widget_img_para.event_cb = NULL;
    widget_img_para.user_data = NULL;
    widget_img_para.img_click_attr = false;
    widget_img_para.img_parent = \
        p_calorie_para->data_parent;

    uint16_t width_sum = 0;
    for(uint8_t i = 0; i < data_bit_num; i++)
    {
        if(i)
           width_sum += \
                calorie_data_width[i - 1];

        widget_img_para.img_x = \
            calorie_data_sx + width_sum + \
                p_calorie_para->num_inv*i;
        widget_img_para.img_y = \
            p_calorie_para->data_y;
        widget_img_para.file_img_dat = \
            calorie_num_index[i];

        common_calorie_widget[i + obj_idx] = \
            common_widget_img_create(&widget_img_para, \
                &calorie_dsc_idx[i + obj_idx]);
        lv_obj_clear_flag(common_calorie_widget[i + obj_idx], \
            LV_OBJ_FLAG_HIDDEN);
    }

    data_end_x = widget_img_para.img_x + \
        calorie_data_width[data_bit_num - 1];

    obj_idx = Calorie_Obj_Max*calorie_group_num;
    for(uint8_t i = 0; i < (Calorie_Obj_Max - data_bit_num); i++)
    {
        widget_img_para.file_img_dat = \
            num_addr_index;
        common_calorie_widget[i + obj_idx] = \
            common_widget_img_create(&widget_img_para, \
                &calorie_dsc_idx[i + obj_idx]);
        lv_obj_add_flag(common_calorie_widget[i + obj_idx], \
            LV_OBJ_FLAG_HIDDEN);
    }

    calorie_group_num++;

    return data_end_x;
}