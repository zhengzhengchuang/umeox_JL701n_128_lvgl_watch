#include "../lv_watch.h"


static uint8_t clk_p_num = 0;
static uint16_t clk_p_timer_id = 0;
static int16_t clock_hour_angle = 0;
static int16_t clock_minute_angle = 0;
static int16_t clock_second_angle = 0;
static int16_t clock_hour_angle_l = \
    Clock_Pointer_Inv_Angle;
static int16_t clock_minute_angle_l = \
    Clock_Pointer_Inv_Angle;
static int16_t clock_second_angle_l = \
    Clock_Pointer_Inv_Angle;
static lv_obj_t *clk_p_obj[3] = {NULL};
static struct sys_time clk_p_utc_time = {0};
static const uint16_t clock_refresh_inv = 100;
static common_clock_pointer_para_t *clk_p_para = NULL;


void common_clock_pointer_para_init(void)
{
    if(!clk_p_timer_id) 
        return;

    clk_p_num = 0;

    clk_p_para = NULL; 

    for(uint8_t i = 0; i < 3; i++)
       clk_p_obj[i] = NULL;

    sys_hi_timer_del(clk_p_timer_id);

    clk_p_timer_id = 0;

    if(!lcd_sleep_status())
    {
        clock_hour_angle_l = \
            clock_hour_angle;
        clock_minute_angle_l = \
            clock_minute_angle;
        clock_second_angle_l = \
            clock_second_angle;
    }else
    {   
        clock_hour_angle_l = \
            Clock_Pointer_Inv_Angle;
        clock_minute_angle_l = \
            Clock_Pointer_Inv_Angle;
        clock_second_angle_l = \
            Clock_Pointer_Inv_Angle;
    }

    return;
}

void common_clock_pointer_refresh(void)
{
#if (Clock_Pointer_Rot == 2)
    static uint8_t cnt = 0;
    uint8_t clock_sec_div = \
        1000/clock_refresh_inv; 

    clock_second_angle += \
        (60/clock_sec_div);
    clock_second_angle %= 3600;

    if(++cnt >= clock_sec_div)
    {
        cnt = 0;

        /*******Rotate once per second minute********/
        clock_minute_angle += 1;
        clock_minute_angle %= 3600;

        if(!(clock_minute_angle%12))
            clock_hour_angle += 1;
    }
#elif (Clock_Pointer_Rot == 1)
    GetUtcTime(&clk_p_utc_time);

    uint8_t utc_time_hour = clk_p_utc_time.hour;
    uint8_t utc_time_minute = clk_p_utc_time.min;
    uint8_t utc_time_second = clk_p_utc_time.sec;

    clock_hour_angle = (utc_time_hour%12)*300 + (utc_time_minute%60)*5;
    clock_minute_angle = (utc_time_minute%60)*60 + utc_time_second%60;
    clock_second_angle = (utc_time_second%60)*60;
#endif

    for(uint8_t i = 0; i < clk_p_num; i++)
    {
        if(!clk_p_obj[i]) continue;

        if(clk_p_para[i].clk_p_type == clk_pointer_type_hour)
            lv_img_set_angle(clk_p_obj[i], clock_hour_angle);
        else if(clk_p_para[i].clk_p_type == clk_pointer_type_minute)
            lv_img_set_angle(clk_p_obj[i], clock_minute_angle);
        else if(clk_p_para[i].clk_p_type == clk_pointer_type_second)
            lv_img_set_angle(clk_p_obj[i], clock_second_angle);
    }

    return;
}

void common_clock_pointer_angle_update(void)
{
    GetUtcTime(&clk_p_utc_time);

    uint8_t utc_time_hour = clk_p_utc_time.hour;
    uint8_t utc_time_minute = clk_p_utc_time.min;
    uint8_t utc_time_second = clk_p_utc_time.sec;

    clock_hour_angle = (utc_time_hour%12)*300 + \
        (utc_time_minute%60)*5;
    clock_minute_angle = (utc_time_minute%60)*60 + \
        utc_time_second%60;
    clock_second_angle = (utc_time_second%60)*60;

    if(clk_p_timer_id)
        sys_hi_timer_modify(clk_p_timer_id, \
            clock_refresh_inv);

    return;
}

static void common_clock_pointer_timer_cb(void *priv)
{
    int clk_p_refr_img[1];
    clk_p_refr_img[0] = \
        ui_msg_clp_refresh;
    post_ui_msg(clk_p_refr_img, 1);

    return;
}

void common_clock_pointer_create(lv_obj_t *obj, \
    const common_clock_pointer_para_t *para, uint8_t num)
{
    if(!obj || !para || !num) 
        return;

    clk_p_num = num;
    clk_p_para = para;
    clk_p_timer_id = 0; 
    for(uint8_t i = 0; i < 3; i++)
       clk_p_obj[i] = NULL;

    GetUtcTime(&clk_p_utc_time);

    uint8_t utc_time_hour = clk_p_utc_time.hour;
    uint8_t utc_time_minute = clk_p_utc_time.min;
    uint8_t utc_time_second = clk_p_utc_time.sec;

    clock_hour_angle = (utc_time_hour%12)*300 + \
        (utc_time_minute%60)*5;
    clock_minute_angle = (utc_time_minute%60)*60 + \
        utc_time_second%60;
    clock_second_angle = (utc_time_second%60)*60;

    if(clock_hour_angle_l != Clock_Pointer_Inv_Angle && \
        clock_minute_angle_l != Clock_Pointer_Inv_Angle && \
            clock_second_angle_l != Clock_Pointer_Inv_Angle)
    {
        int16_t s_angle_0 = \
            clock_second_angle;
        int16_t s_angle_1 = \
            clock_second_angle_l;
        if((s_angle_1 - s_angle_0) > 0)
        {
            if((s_angle_1 - s_angle_0) < 3600/2)
                clock_second_angle = \
                    clock_second_angle_l;
        }else
        {
            if((s_angle_1 - s_angle_0) < -3600/2)
               clock_second_angle = \
                    clock_second_angle_l; 
        }
    }

    widget_img_para.img_x = 0;
    widget_img_para.img_y = 0;
    widget_img_para.img_parent = obj;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    
    for(uint8_t i = 0; i < clk_p_num; i++)
    {
        int16_t align_offset = \
            clk_p_para[i].clk_p_center - clk_p_para[i].clk_p_height/2;

        widget_img_para.file_img_dat = clk_p_para[i].clk_p_file_dat;
        clk_p_obj[i] = common_widget_img_create(&widget_img_para, NULL);
        lv_obj_align(clk_p_obj[i], LV_ALIGN_CENTER, 0, -align_offset + \
            Clock_Pointer_Ver_Offset);
        lv_img_set_pivot(clk_p_obj[i], clk_p_para[i].clk_p_width/2, \
            clk_p_para[i].clk_p_center);
        if(clk_p_para[i].clk_p_type == clk_pointer_type_hour)
            lv_img_set_angle(clk_p_obj[i], clock_hour_angle);
        else if(clk_p_para[i].clk_p_type == clk_pointer_type_minute)
            lv_img_set_angle(clk_p_obj[i], clock_minute_angle);
        else if(clk_p_para[i].clk_p_type == clk_pointer_type_second)
            lv_img_set_angle(clk_p_obj[i], clock_second_angle);
    }

    if(!clk_p_timer_id)
        clk_p_timer_id = (uint32_t)sys_hi_timer_add(NULL, \
            common_clock_pointer_timer_cb, clock_refresh_inv);

    return;
}
