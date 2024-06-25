#include "../lv_watch.h"


/*********************************************************************************
                                  数据参数                                       
*********************************************************************************/
widget_data_para_t widget_data_para = {0};


void common_data_widget_init(void)
{
    common_hr_widget_init();
    common_bo_widget_init();
    common_step_widget_init();
    common_pace_widget_init();
    common_calorie_widget_init();
    common_weather_widget_init();
    common_distance_widget_init();
    
    return;
}

void common_data_refresh(void)
{
    common_hr_widget_refresh();
    common_bo_widget_refresh();
    common_step_widget_refresh();
    common_pace_widget_refresh();
    common_calorie_widget_refresh();
    //common_weather_widget_refresh();
    common_distance_widget_refresh();
    
    return;
}

int16_t common_data_widget_create(widget_data_para_t *data_para, \
    widget_data_type_t type, void *data_val)
{
    int16_t data_end_x = 0;

    if(!data_para || !data_val)
        return data_end_x;

    if(type == widget_data_type_hr)
    {
        data_end_x = \
            common_hr_widget_create(data_para, \
                type, data_val);
    }else if(type == widget_data_type_bo)
    {
        data_end_x = \
            common_bo_widget_create(data_para, \
                type, data_val);
    }else if(type == widget_data_type_step)
    {
        data_end_x = \
            common_step_widget_create(data_para, \
                type, data_val);
    }else if(type == widget_data_type_calorie)
    {
        data_end_x = \
            common_calorie_widget_create(data_para, \
                type, data_val);
    }else if(type == widget_data_type_pace)
    {
        data_end_x = \
            common_pace_widget_create(data_para, \
                type, data_val);
    }else if(type == Widget_Data_type_distance)
    {
        data_end_x = \
            common_distance_widget_create(data_para, \
                type, data_val);
    }else if(type >= widget_data_type_weather && \
        type <= widget_data_type_max6_weather)
    {
        data_end_x = \
            common_weather_widget_create(data_para, \
                type, data_val);
    }

    return data_end_x;
}
