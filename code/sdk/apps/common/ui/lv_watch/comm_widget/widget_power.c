#include "../lv_watch.h"

PowerPara_t PowerPara;

static uint8_t group_num = 0;
static PowerPara_t para_cache[Power_Group_Max] = {0};
static uint16_t dsc_idx[Power_Group_Max*Power_Obj_Max] = {0xffff};
static lv_obj_t *power_widget[Power_Group_Max*Power_Obj_Max] = {NULL};

static int16_t widget_sx(uint8_t idx, uint8_t str_num, \
    uint16_t num_w, uint16_t *w_buf)
{
    int16_t sx = 0;

    uint16_t w_sum = 0;
    for(uint8_t i = 0; i < str_num; i++)
        w_sum += w_buf[i];

    PowerPara_t *p_para_cache = &para_cache[idx];
    widget_data_align_t align = p_para_cache->align;

    if(align == widget_data_align_left)
    {
        sx = p_para_cache->x;
    }else if(align == widget_data_align_right)
    {
        if(p_para_cache->symb_disp == true)
            sx = p_para_cache->x + ((Power_Obj_Max*num_w) - w_sum);
        else
            sx = p_para_cache->x + (((Power_Obj_Max-1)*num_w) - w_sum);
    }else if(align == widget_data_align_center)
    {
        if(p_para_cache->symb_disp == true)
            sx = p_para_cache->x + ((Power_Obj_Max*num_w) - w_sum)/2;
        else
            sx = p_para_cache->x + (((Power_Obj_Max-1)*num_w) - w_sum)/2;
    }else
    {
        sx = p_para_cache->x;
    }
        
    return sx;
}

void WidgetPowerInit(void)
{
    if(!group_num) return;

    memset(dsc_idx, 0xff, sizeof(dsc_idx));

    memset(para_cache, 0, sizeof(PowerPara_t)*group_num);

    group_num = 0;

    return;
}

void WidgetPowerRefresh(void)
{
    if(group_num == 0)
        return;

    int __data = GetBatPower();
    char str[Power_Obj_Max + 1];
    for(uint8_t g = 0; g < group_num; g++)
    {
        PowerPara_t *p_para_cache = &para_cache[g];

        memset(str, 0, sizeof(str));
        if(p_para_cache->symb_disp == true)
            sprintf(str, "%d%%", __data);
        else
            sprintf(str, "%d", __data);

        uint8_t str_num = strlen((const char *)str);

        uint32_t file_idx = p_para_cache->file_idx;

        lv_img_dsc_t *data_img_dsc = common_widget_img_open_res(file_idx);
        uint16_t num_width = data_img_dsc->header.w;

        uint32_t symb_13_addr_index = file_idx + Comm_Symbol_13;
        data_img_dsc = common_widget_img_open_res(symb_13_addr_index);
        uint16_t symb_13_width = data_img_dsc->header.w;
        
        uint16_t img_w_buf[Power_Obj_Max]; 
        uint32_t file_idx_buf[Power_Obj_Max];
        for(uint8_t i = 0; i < str_num; i++)
        {
            if(str[i] == '%')
            {
                img_w_buf[i] = symb_13_width;
                file_idx_buf[i] = symb_13_addr_index;
            }else
            {
                img_w_buf[i] = num_width;
                file_idx_buf[i] = file_idx + (str[i] - '0');
            }
        }

         int16_t sx = widget_sx(g, str_num, num_width, img_w_buf);

         uint8_t obj_idx = (Power_Obj_Max - str_num) + Power_Obj_Max*g;

        uint16_t w_sum = 0;
        int16_t refr_x, refr_y;
        for(uint8_t i = 0; i < str_num; i++)
        {
            refr_x = sx + w_sum;
            refr_y = p_para_cache->y;
            lv_obj_set_pos(power_widget[i + obj_idx], refr_x, refr_y);
            
            common_widget_img_replace_src(power_widget[i + obj_idx], file_idx_buf[i], dsc_idx[i + obj_idx]);
            lv_obj_clear_flag(power_widget[i + obj_idx], LV_OBJ_FLAG_HIDDEN);

            w_sum += (img_w_buf[i] + p_para_cache->inv*i);
        }

        w_sum = 0;

        obj_idx = Power_Obj_Max*g;
        for(uint8_t i = 0; i < (Power_Obj_Max - str_num); i++)
           lv_obj_add_flag(power_widget[i + obj_idx], LV_OBJ_FLAG_HIDDEN); 
    }

    return;
}

int16_t WidgetPowerCreate(PowerPara_t *ppower)
{
    int16_t end_x = 0;

    if(ppower == NULL) 
        return end_x;

    if(group_num >= Power_Group_Max)
        return end_x;

    int __data = GetBatPower();

    uint8_t idx = group_num;

    memcpy(&para_cache[idx], ppower, sizeof(PowerPara_t));
    PowerPara_t *p_para_cache = &para_cache[idx];

    char str[Power_Obj_Max + 1];
    memset(str, 0, sizeof(str));
    if(p_para_cache->symb_disp == true)
        sprintf(str, "%d%%", __data);
    else
       sprintf(str, "%d", __data); 

    uint8_t str_num = strlen((const char *)str);

    uint32_t file_idx = p_para_cache->file_idx;

    lv_img_dsc_t *data_img_dsc = common_widget_img_open_res(file_idx);
    uint16_t num_width = data_img_dsc->header.w;

    uint32_t symb_13_addr_index = file_idx + Comm_Symbol_13;
    data_img_dsc = common_widget_img_open_res(symb_13_addr_index);
    uint16_t symb_13_width = data_img_dsc->header.w;
    
    uint16_t img_w_buf[Power_Obj_Max]; 
    uint32_t file_idx_buf[Power_Obj_Max];
    for(uint8_t i = 0; i < str_num; i++)
    {
        if(str[i] == '%')
        {
            img_w_buf[i] = symb_13_width;
            file_idx_buf[i] = symb_13_addr_index;
        }else
        {
            img_w_buf[i] = num_width;
            file_idx_buf[i] = file_idx + (str[i] - '0');
        }
    }

    int16_t sx = widget_sx(idx, str_num, num_width, img_w_buf);

    uint8_t obj_idx = (Power_Obj_Max - str_num) + Power_Obj_Max*idx;

    widget_img_para.img_parent = p_para_cache->parent;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;

    uint16_t w_sum = 0;
    for(uint8_t i = 0; i < str_num; i++)
    {
        widget_img_para.img_x = sx + w_sum;
        widget_img_para.img_y = p_para_cache->y;
        widget_img_para.file_img_dat = file_idx_buf[i];
        power_widget[i + obj_idx] = common_widget_img_create(&widget_img_para, &dsc_idx[i + obj_idx]);
        lv_obj_clear_flag(power_widget[i + obj_idx], LV_OBJ_FLAG_HIDDEN);
        w_sum += (img_w_buf[i] + p_para_cache->inv*i);
        end_x = widget_img_para.img_x + w_sum;
    }

    obj_idx = Power_Obj_Max*idx;
    for(uint8_t i = 0; i < (Power_Obj_Max - str_num); i++)
    {
        widget_img_para.file_img_dat = file_idx;
        power_widget[i + obj_idx] = common_widget_img_create(&widget_img_para, &dsc_idx[i + obj_idx]);
        lv_obj_add_flag(power_widget[i + obj_idx], LV_OBJ_FLAG_HIDDEN);
    }

    memset(&widget_img_para, 0, sizeof(common_widget_img_para_t));

    group_num += 1;

    return end_x;
}