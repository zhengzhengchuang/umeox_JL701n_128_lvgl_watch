#include "ui/ui.h"
#include "ui/ui_slider.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_sys.h"
#include "lua_gui.h"

#include "weather/weather.h"

#if (TCFG_LUA_ENABLE)
#if TCFG_UI_ENABLE_WEATHER

struct __WEATHER_INFO *get_weather_info();
static int obj_getWeatherHandle(lua_State *L)
{
    struct __WEATHER_INFO *weather = get_weather_info();
    lua_pushlightuserdata(L, weather);

    printf("weather: 0x%x\n", weather);
    printf("weather->temperature : %d\n", weather->temperature);
    printf("weather->weather : %d\n", weather->weather);
    printf("weather->province_name_len : %d\n", weather->province_name_len);
    printf("weather->province : %s\n", weather->province);

    return 1;
}

static int obj_getWeather(lua_State *L)
{
    struct __WEATHER_INFO *weather = (struct __WEATHER_INFO *)lua_touserdata(L, -1);
    lua_pushinteger(L, weather->weather);
    printf("weather: 0x%x\n", weather);
    printf("weather->weather : %d\n", weather->weather);

    return 1;
}

static int obj_getTemperature(lua_State *L)
{
    struct __WEATHER_INFO *weather = (struct __WEATHER_INFO *)lua_touserdata(L, -1);
    lua_pushinteger(L, weather->temperature);
    printf("weather->temperature : %d\n", weather->temperature);

    return 1;
}

static int obj_getHumidity(lua_State *L)
{
    struct __WEATHER_INFO *weather = (struct __WEATHER_INFO *)lua_touserdata(L, -1);
    lua_pushinteger(L, weather->humidity);

    return 1;
}
static int obj_getWindDirection(lua_State *L)
{
    struct __WEATHER_INFO *weather = (struct __WEATHER_INFO *)lua_touserdata(L, -1);
    lua_pushinteger(L, weather->wind_direction);

    return 1;
}
static int obj_getWindPower(lua_State *L)
{
    struct __WEATHER_INFO *weather = (struct __WEATHER_INFO *)lua_touserdata(L, -1);
    lua_pushinteger(L, weather->wind_power);

    return 1;
}
static int obj_getUpdateTime(lua_State *L)
{
    struct __WEATHER_INFO *weather = (struct __WEATHER_INFO *)lua_touserdata(L, -1);
    lua_pushinteger(L, weather->update_time);

    lua_newtable(L);

    lua_pushinteger(L, ((weather->update_time >> 26) & 0x3f) + 2010);
    lua_setfield(L, -2, "year");

    lua_pushinteger(L, (weather->update_time >> 22) & 0xf);
    lua_setfield(L, -2, "month");

    lua_pushinteger(L, (weather->update_time >> 17) & 0x1f);
    lua_setfield(L, -2, "day");

    lua_pushinteger(L, (weather->update_time >> 12) & 0x1f);
    lua_setfield(L, -2, "hour");

    lua_pushinteger(L, (weather->update_time >> 6) & 0x3f);
    lua_setfield(L, -2, "minute");

    lua_pushinteger(L, weather->update_time & 0x3f);
    lua_setfield(L, -2, "second");

    /* lua_settable(L, -2); */

    stackDump(L);

    return 2;
}
static int obj_getProvinceData(lua_State *L)
{
    struct __WEATHER_INFO *weather = (struct __WEATHER_INFO *)lua_touserdata(L, -1);
    lua_pushinteger(L, weather->province_name_len);
    lua_pushinteger(L, weather->province);

    /* printf("weather->province_name_len : %d\n", weather->province_name_len); */
    /* printf("weather->province : 0x%x, %s\n", weather->province, weather->province); */

    return 2;
}
static int obj_getCityData(lua_State *L)
{
    struct __WEATHER_INFO *weather = (struct __WEATHER_INFO *)lua_touserdata(L, -1);
    lua_pushinteger(L, weather->city_name_len);
    lua_pushinteger(L, weather->city);

    /* printf("weather->city_name_len : %d\n", weather->city_name_len); */
    /* printf("weather->city : 0x%x, %s\n", weather->city, weather->city); */

    return 2;
}

const luaL_Reg obj_WeatherMethod[] = {
    {"getWeatherHandle",	obj_getWeatherHandle},
    {"getWeather",			obj_getWeather},
    {"getTemperature",		obj_getTemperature},
    {"getHumidity",			obj_getHumidity},
    {"getWindDirection",	obj_getWindDirection},
    {"getWindPower",		obj_getWindPower},
    {"getUpdateTime",		obj_getUpdateTime},
    {"getProvinceData",		obj_getProvinceData},
    {"getCityData",			obj_getCityData},

    {NULL, NULL}
};

#endif /* #if TCFG_UI_ENABLE_WEATHER */
#endif
