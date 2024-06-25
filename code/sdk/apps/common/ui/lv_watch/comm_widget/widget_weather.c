#include "../lv_watch.h"

/*********************************************************************************
                                  天气数据参数                                       
*********************************************************************************/
static uint8_t weather_group_num = 0;
static widget_data_para_t weather_para_cache[\
    Weather_Group_Max] = {0};
static uint16_t weather_dsc_idx[Weather_Group_Max\
    *Weather_Obj_Max] = {0xffff};
static lv_obj_t *common_widget_weather[Weather_Group_Max*\
    Weather_Obj_Max] = {NULL};
static widget_data_type_t weather_data_type[Weather_Group_Max] \
    = {widget_data_type___};

static int16_t common_weather_widget_sx(uint8_t idx, \
    uint8_t data_bit_num, uint16_t num_width, uint16_t symb_11_width, \
        uint16_t symb_16_width, uint16_t *weather_data_width)
{
    int16_t weather_data_sx = 0;

    widget_data_para_t *p_data_para = \
        &weather_para_cache[idx];

    if(!weather_data_width)
        return (p_data_para->data_x);

    uint16_t width_sum = 0;
    for(uint8_t i = 0; i < data_bit_num; i++)
        width_sum += weather_data_width[i];

    widget_data_align_t data_align = \
        p_data_para->data_align;

    if(data_align == widget_data_align_left)
        weather_data_sx = \
            p_data_para->data_x;
    else if(data_align == widget_data_align_right)
        weather_data_sx = \
            p_data_para->data_x + \
                (((Weather_Obj_Max-2)*num_width) + \
                    symb_11_width + symb_16_width - width_sum);
    else if(data_align == widget_data_align_center)
        weather_data_sx = \
            p_data_para->data_x + \
                (((Weather_Obj_Max-2)*num_width) + \
                    symb_11_width + symb_16_width - width_sum)/2;
    else
        weather_data_sx = \
            p_data_para->data_x;

    return weather_data_sx;
}

void common_weather_widget_init(void)
{
    if(!weather_group_num)
        return;

    memset(weather_dsc_idx, 0xffff, \
        sizeof(weather_dsc_idx));

    memset(common_widget_weather, 0, \
        sizeof(common_widget_weather));

    memset(weather_para_cache, 0, \
        sizeof(widget_data_para_t)* \
            weather_group_num);

    weather_group_num = 0;

    return;
}

void common_weather_widget_refresh(void)
{
    if(!weather_group_num)
        return;

#if 0
    int16_t __data = 0;

    for(uint8_t i = 0; i < weather_group_num; i++)
    {
        if(weather_data_type[i] == widget_data_type_weather)
        {
            __data = weather_real_temper();
        }else if(weather_data_type[i] >= widget_data_type_min0_weather && \
            weather_data_type[i] <= widget_data_type_max6_weather)
        {
            uint8_t idx = \
                (weather_data_type[i] - widget_data_type_min0_weather);
            
            if(!(idx % 2))
                __data = \
                    weather_min_temper(idx/2);
            else
                __data = \
                    weather_max_temper(idx/2);
        }
   
        widget_data_para_t *p_data_para = \
            &weather_para_cache[i];

        uint32_t num_addr_index = \
            p_data_para->num_addr_index;

        lv_img_dsc_t *data_img_dsc = \
            common_widget_img_open_res(\
                num_addr_index);
        uint16_t num_width = \
            data_img_dsc->header.w;

        uint32_t symb_11_addr_index = \
            num_addr_index + \
                Comm_Symbol_11;
        data_img_dsc = \
            common_widget_img_open_res(\
                symb_11_addr_index);
        uint16_t symb_11_width = \
            data_img_dsc->header.w;

        uint32_t symb_16_addr_index = \
            num_addr_index + \
                Comm_Symbol_16;
        data_img_dsc = \
            common_widget_img_open_res(\
                symb_16_addr_index);
        uint16_t symb_16_width = \
            data_img_dsc->header.w;

        uint32_t symb_17_addr_index = \
            num_addr_index + \
                Comm_Symbol_17;
        data_img_dsc = \
            common_widget_img_open_res(\
                symb_17_addr_index);
        uint16_t symb_17_width = \
            data_img_dsc->header.w;

        char __data_str[Weather_Obj_Max + 1];
        memset(__data_str, 0, \
            sizeof(__data_str));

        int unit_temperature = \
            GetVmParaCacheByLabel(\
                vm_label_unit_temperature);

        if(unit_temperature == unit_temperature_F)
            sprintf(__data_str, "%dF", __data);
        else
            sprintf(__data_str, "%dC", __data);

        uint8_t data_bit_num = \
            strlen((const char *)__data_str);

        uint32_t weather_num_index[\
            Weather_Obj_Max];
        uint16_t weather_data_width[\
            Weather_Obj_Max] = {0};
        for(uint8_t j = 0; j < data_bit_num; j++)
        {
            if(__data_str[j] == '-')
            {
                weather_data_width[j] = \
                    symb_11_width;
                weather_num_index[j] = \
                    symb_11_addr_index;
            }else if(__data_str[j] == 'C')
            {
                weather_data_width[j] = \
                    symb_16_width;
                weather_num_index[j] = \
                    symb_16_addr_index;
            }else if(__data_str[j] == 'F')
            {
                weather_data_width[j] = \
                    symb_17_width;
                weather_num_index[j] = \
                    symb_17_addr_index;
            }else
            {
                weather_data_width[j] = \
                    num_width;
                weather_num_index[j] = \
                    num_addr_index + \
                        (__data_str[j] - '0');
            }
        }

        int16_t weather_data_sx = \
            common_weather_widget_sx(i, \
                data_bit_num, num_width, symb_11_width, \
                    symb_16_width, weather_data_width);

        uint8_t obj_idx = \
            (Weather_Obj_Max - data_bit_num) + \
                Weather_Obj_Max*i;

        uint16_t width_sum = 0;
        int16_t refresh_x, refresh_y;
        for(uint8_t j = 0; j < data_bit_num; j++)
        {
            if(j)
                width_sum += \
                    weather_data_width[j - 1];

            if(!common_widget_weather[j + obj_idx] || \
                weather_dsc_idx[j + obj_idx] == 0xffff)
                    continue;

            refresh_x = weather_data_sx + width_sum + \
                p_data_para->num_inv*j;
            refresh_y = p_data_para->data_y;
            lv_obj_set_pos(common_widget_weather[j + obj_idx], \
                refresh_x, refresh_y);
            
            common_widget_img_replace_src(common_widget_weather[j + obj_idx], \
                weather_num_index[j], weather_dsc_idx[j + obj_idx]);
            lv_obj_clear_flag(common_widget_weather[j + obj_idx], \
                LV_OBJ_FLAG_HIDDEN);
        }

        width_sum = 0;

        obj_idx = Weather_Obj_Max*i;
        for(uint8_t j = 0; j < (Weather_Obj_Max - data_bit_num); j++)
           lv_obj_add_flag(common_widget_weather[j + obj_idx], \
                LV_OBJ_FLAG_HIDDEN);
    }
#endif

    return;
}

int16_t common_weather_widget_create(widget_data_para_t *data_para, \
    widget_data_type_t type, void *data_val)
{
    int16_t data_end_x = 0;

    if(weather_group_num >= \
        Weather_Group_Max)
        return data_end_x;

    int16_t __data = *(int16_t *)data_val;

    weather_data_type[weather_group_num] = type;

    memcpy(&weather_para_cache[weather_group_num], \
        data_para, sizeof(widget_data_para_t));

    widget_data_para_t *p_data_para = \
        &weather_para_cache[weather_group_num];

    uint32_t num_addr_index = \
        p_data_para->num_addr_index;

    lv_img_dsc_t *data_img_dsc = \
        common_widget_img_open_res(\
            num_addr_index);
    uint16_t num_width = \
        data_img_dsc->header.w;

    uint32_t symb_11_addr_index = \
        num_addr_index + \
            Comm_Symbol_11;
    data_img_dsc = \
        common_widget_img_open_res(\
            symb_11_addr_index);
    uint16_t symb_11_width = \
        data_img_dsc->header.w;

    uint32_t symb_16_addr_index = \
        num_addr_index + \
            Comm_Symbol_16;
    data_img_dsc = \
        common_widget_img_open_res(\
            symb_16_addr_index);
    uint16_t symb_16_width = \
        data_img_dsc->header.w;

    uint32_t symb_17_addr_index = \
        num_addr_index + \
            Comm_Symbol_17;
    data_img_dsc = \
        common_widget_img_open_res(\
            symb_17_addr_index);
    uint16_t symb_17_width = \
        data_img_dsc->header.w;

    uint32_t symb_19_addr_index = \
        num_addr_index + \
            Comm_Symbol_19;
    data_img_dsc = \
        common_widget_img_open_res(\
            symb_19_addr_index);
    uint16_t symb_19_width = \
        data_img_dsc->header.w;

    char __data_str[Weather_Obj_Max + 1];
    memset(__data_str, 0, sizeof(__data_str));

    int unit_temperature = \
        GetVmParaCacheByLabel(vm_label_unit_temperature);

    if(unit_temperature == unit_temperature_F)
    {
        if(__data == No_Weather)
            memcpy(__data_str, "__F", 3);
        else
            sprintf(__data_str, "%dF", __data);
    }else
    {
        if(__data == No_Weather)
            memcpy(__data_str, "__C", 3);
        else
            sprintf(__data_str, "%dC", __data);
    }

    uint8_t data_bit_num = \
        strlen((const char *)__data_str);

    uint32_t weather_num_index[\
        Weather_Obj_Max];
    uint16_t weather_data_width[\
        Weather_Obj_Max] = {0};
    for(uint8_t i = 0; i < data_bit_num; i++)
    {
        if(__data_str[i] == '-')
        {
            weather_data_width[i] = \
                symb_11_width;
            weather_num_index[i] = \
                symb_11_addr_index;
        }else if(__data_str[i] == 'C')
        {
            weather_data_width[i] = \
                symb_16_width;
            weather_num_index[i] = \
                symb_16_addr_index;
        }else if(__data_str[i] == 'F')
        {
            weather_data_width[i] = \
                symb_17_width;
            weather_num_index[i] = \
                symb_17_addr_index;
        }else if(__data_str[i] == '_')
        {
            weather_data_width[i] = \
                symb_19_width;
            weather_num_index[i] = \
                symb_19_addr_index;
        }else
        {
            weather_data_width[i] = \
                num_width;
            weather_num_index[i] = \
                num_addr_index + (__data_str[i] - '0');
        }
    }

    int16_t weather_data_sx = \
        common_weather_widget_sx(weather_group_num, \
            data_bit_num, num_width, symb_11_width, \
                symb_16_width, weather_data_width);

    uint8_t obj_idx = \
        (Weather_Obj_Max - data_bit_num) + \
            Weather_Obj_Max*weather_group_num;

    widget_img_para.event_cb = NULL;
    widget_img_para.user_data = NULL;
    widget_img_para.img_click_attr = false;
    widget_img_para.img_parent = \
        p_data_para->data_parent;

    uint16_t width_sum = 0;
    for(uint8_t i = 0; i < data_bit_num; i++)
    {
        if(i)
           width_sum += \
                weather_data_width[i - 1];

        widget_img_para.img_x = \
            weather_data_sx + width_sum + \
                p_data_para->num_inv*i;
        widget_img_para.img_y = \
            p_data_para->data_y;
        widget_img_para.file_img_dat = \
            weather_num_index[i];

        common_widget_weather[i + obj_idx] = \
            common_widget_img_create(&widget_img_para, \
                &weather_dsc_idx[i + obj_idx]);
        lv_obj_clear_flag(common_widget_weather[i + obj_idx], \
            LV_OBJ_FLAG_HIDDEN);
    }

    data_end_x = widget_img_para.img_x + \
        weather_data_width[data_bit_num - 1];

    obj_idx = Weather_Obj_Max*weather_group_num;
    for(uint8_t i = 0; i < (Weather_Obj_Max - data_bit_num); i++)
    {
        widget_img_para.file_img_dat = \
            num_addr_index;
        common_widget_weather[i + obj_idx] = \
            common_widget_img_create(&widget_img_para, \
                &weather_dsc_idx[i + obj_idx]);
        lv_obj_add_flag(common_widget_weather[i + obj_idx], \
            LV_OBJ_FLAG_HIDDEN);
    }

    weather_group_num++;

    return data_end_x;
}