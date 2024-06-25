#ifndef __WEATHER_H__
#define __WEATHER_H__

#include "generic/typedef.h"

struct __WEATHER_INFO {
    u8 province_name_len;
    u8 *province;
    u8 city_name_len;
    u8 *city;
    u8 weather;
    s8 temperature;
    u8 humidity;
    u8 wind_direction;
    u8 wind_power;
    u32 update_time;
};

#endif
