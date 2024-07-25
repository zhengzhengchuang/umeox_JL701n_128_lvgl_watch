#include "../lv_watch.h"

WeatherInfoPara_t Weather_Info;

static int16_t temper_ctof(int16_t c_temper)
{
	float f_temper = 0.0f;
	float f_temper_dec = 0.0f;
	
	f_temper = (((c_temper*(1.0f)) * 9 / 5) + 32); 
	f_temper_dec = (float)(f_temper - (int16_t)f_temper);
	if(f_temper_dec < 0.0f)
	{
		if(f_temper_dec + ((-1)*(0.5f)) <= ((-1)*(1.0f)))
            f_temper = f_temper + ((-1)*(0.5f));
	}else
	{
		if(f_temper_dec + (0.5f) >= 1.0f)
            f_temper = f_temper + (0.5f);
	}
	
	return (int16_t)f_temper;
}

int16_t weather_real_temper(void)
{
    int16_t real_temper = \
        Weather_Info.data_ctx[0].real_temper;

    int cur_unit_temperature = \
        GetVmParaCacheByLabel(vm_label_unit_temperature);

    if(cur_unit_temperature == unit_temperature_F)
        real_temper = \
            temper_ctof(real_temper);

    return real_temper;
}

int16_t weather_min_temper(uint8_t day)
{
    if(day >= Weather_Sync_Days)
        return 0;

    int16_t min_temper = \
        Weather_Info.data_ctx[day].min_temper;

    int cur_unit_temperature = \
        GetVmParaCacheByLabel(\
            vm_label_unit_temperature);

    if(cur_unit_temperature == unit_temperature_F)
        min_temper = \
            temper_ctof(min_temper);

    return min_temper;
}

int16_t weather_max_temper(uint8_t day)
{
    if(day >= Weather_Sync_Days)
        return 0;

    int16_t max_temper = \
        Weather_Info.data_ctx[day].max_temper;

    int cur_unit_temperature = \
        GetVmParaCacheByLabel(\
            vm_label_unit_temperature);

    if(cur_unit_temperature == unit_temperature_F)
        max_temper = \
            temper_ctof(max_temper);

    return max_temper;
}

weather_type_t weather_weather_type(uint8_t day)
{
    if(day >= Weather_Sync_Days)
        return weather_type_unknown;

    weather_type_t type = \
        Weather_Info.data_ctx[day].type;

    return type;
}

//false 没过期  true 过期
static bool WeatherVmDataIsPast(struct sys_time *weather_time)
{
    if(!weather_time) 
        return true;

    //今天
    struct sys_time utc_time1;
    GetUtcTime(&utc_time1);
    struct sys_time *utc_time2 = weather_time;

    if(utc_time1.year != utc_time2->year || \
        utc_time1.month != utc_time2->month || \
            utc_time1.day != utc_time2->day)
        goto __end;

    if(utc_time1.hour != utc_time2->hour)
        goto __end;

    return false;

__end:
    return true;
}

static bool GetWeatherData(void)
{
    bool ret = \
        VmWeatherCtxFlashRead();

    return ret;
}

static void ClearWeatherInfoPara(void)
{
    memset(&Weather_Info, 0, \
        sizeof(WeatherInfoPara_t));

    return;
}

void SetWeatherInfoPara(void)
{
    ClearWeatherInfoPara();

    bool r_ret = \
        GetWeatherData();
    if(r_ret == false) return;

    bool IsPast = \
        WeatherVmDataIsPast(&(r_weather.time));
    if(IsPast == true) return;

    memcpy(&(Weather_Info.time), &(r_weather.time), \
        sizeof(struct sys_time));

    memcpy(Weather_Info.data_ctx, r_weather.vm_ctx, \
        sizeof(vm_weather_data_ctx_t)*Weather_Sync_Days);

    bool *weather_valid = &(Weather_Info.valid);
    *weather_valid = true;

    return;
}

void WeatherInfoParaUpdate(void)
{
    if(w_weather.check_code != 0)
    {
        VmWeatherCtxFlashSave(&w_weather);
        SetWeatherInfoPara();
    }

    memset(&w_weather, 0, sizeof(vm_weather_ctx_t));

    return;
}

void WeatherProcess(struct sys_time *ptime)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    if(ptime->min == 0)
    {
        /*清空数据*/
        ClearWeatherInfoPara();
        VmWeatherCtxClear();

        /*请求天气数据*/
        RemoteGetDevEvents(Le_Event_Req_Weather);
    }

    return;
}