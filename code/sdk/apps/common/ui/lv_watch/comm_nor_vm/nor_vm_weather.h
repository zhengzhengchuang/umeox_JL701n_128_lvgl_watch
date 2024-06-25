#ifndef __NOR_VM_WEATHER_H__
#define __NOR_VM_WEATHER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

/*********************************************************************************
                                  同步最大天数                                       
*********************************************************************************/
#define Weather_Sync_Days (7)

/*********************************************************************************
                                  天气类型(不要改枚举的顺序)                                    
*********************************************************************************/
enum
{
    weather_type_sunny,            //晴
    weather_type_snow,             //雪
    weather_type_thunderstorm,     //雷阵雨
    weather_type_wind,             //风
    weather_type_misthaze,         //雾霾
    weather_type_sandstorm,        //沙尘暴
    weather_type_cloudy,           //多云
    weather_type_rain_snow,        //雨夹雪
    weather_type_shower,           //阵雨
    weather_type_overcast,         //阴天
    weather_type_pouring,          //大雨
    weather_type_light_rain,       //小雨

    weather_type_unknown,          //未知
};
typedef uint8_t weather_type_t;

typedef struct
{
    weather_type_t \
        type;
    int8_t real_temper;
    int8_t min_temper;
    int8_t max_temper;
}vm_weather_data_ctx_t;

typedef struct
{
    uint16_t check_code;

    struct sys_time time;

    vm_weather_data_ctx_t \
        vm_ctx[Weather_Sync_Days];
}vm_weather_ctx_t;
extern vm_weather_ctx_t w_weather;
extern vm_weather_ctx_t r_weather;

void VmWeatherCtxClear(void);
uint8_t VmWeatherItemNum(void);
bool VmWeatherCtxFlashRead(void);
void VmWeatherCtxFlashSave(void *p);
#ifdef __cplusplus
}
#endif

#endif
