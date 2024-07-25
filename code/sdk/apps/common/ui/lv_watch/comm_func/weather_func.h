#ifndef __WEATHER_FUNC_H__
#define __WEATHER_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define No_Weather (0x7f)

typedef struct
{
    bool valid;        //数据是否有效

    struct sys_time time;
    vm_weather_data_ctx_t \
        data_ctx[Weather_Sync_Days];
}WeatherInfoPara_t;
extern WeatherInfoPara_t Weather_Info;

int16_t weather_real_temper(void);
int16_t weather_min_temper(uint8_t day);
int16_t weather_max_temper(uint8_t day);
weather_type_t weather_weather_type(uint8_t day);

void SetWeatherInfoPara(void);
void WeatherInfoParaUpdate(void);
void WeatherProcess(struct sys_time *ptime);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
