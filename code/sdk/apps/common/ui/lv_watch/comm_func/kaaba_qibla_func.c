#include "math.h"
#include "../lv_watch.h"

#define M_PI (3.14159f)

static float deg2rad(float deg)
{
    return (deg * (M_PI / 180.0f));
}

static float rad2deg(float rad)
{
    return (rad * (180.0f / M_PI));
}

static s16 KaabaQiblaAzimuthCalc(void)
{   
    float X = 0.0f;
    float Y = 0.0f;
    float B = 0.0f;
    float latitude_Kaaba = 21.422530f;
    float longitude_Kaaba = 39.826181f;
    float latitude_temp = ll_info.vm_latitude;
    float longitude_temp = ll_info.vm_longitude;


    Y = cosf(deg2rad(latitude_Kaaba)) * sinf(deg2rad(longitude_Kaaba - longitude_temp));

    X = cosf(deg2rad(latitude_temp)) * sinf(deg2rad(latitude_Kaaba)) - \
        sinf(deg2rad(latitude_temp)) * cosf(deg2rad(latitude_Kaaba)) * \
            cosf(deg2rad(longitude_Kaaba - longitude_temp));

    B = atan2f(Y, X);

    float angle = rad2deg(B);

    s16 azimuth = (s16)(angle);

    if(azimuth < 0)
        azimuth = 360 + azimuth;

    return azimuth;
}

static s16 azimuth;
s16 GetKaabaQiblaAzimuth(void)
{
    return azimuth;
}

void SetKaabaQiblaAzimuth(s16 a)
{
    azimuth = a;

    return;
}

void KaabaQiblaParaUpdate(void)
{
    s16 azimuth = \
        KaabaQiblaAzimuthCalc();
    SetKaabaQiblaAzimuth(azimuth);

    printf("azimuth = %d\n", azimuth);

    return;
}