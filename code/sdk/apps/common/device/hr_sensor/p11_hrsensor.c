#include "app_config.h"
#include "os/os_api.h"
#include "asm/clock.h"
#include "system/timer.h"
#include "asm/cpu.h"
#include "generic/typedef.h"
#include "generic/gpio.h"
#include "debug.h"
#include "hrSensor_manage.h"
#include "system/os/os_api.h"
#include "printf.h"
#include <stdint.h>
#include <stdlib.h>

#if TCFG_P11HR_SENSOR_ENABLE

#include "user_p11_cmd.h"
int READLEN = 0;
static u8 hrsensor_init()
{
    printf("p11 hrsensor init\n");
    //返回true 为成功
    return 1;
}

int p11_hrsensor_cbuf_read(u8 *data, u16 len)
{
    int ret = 0;
    ret =  master_p11cbuf_read(1, data, len);
    if (ret == 0) {
        printf("%s %d ret =%d,len=%d\n", __FUNCTION__, __LINE__, ret, len);
    }
    return ret;
}


int p11_spo2sensor_cbuf_read(u8 *data, u16 len)
{
    int ret = 0;
    ret =  master_p11cbuf_read(2, data, len);
    if (ret == 0) {
        printf("%s %d ret =%d,len=%d\n", __FUNCTION__, __LINE__, ret, len);
    }
    return ret;
}



static int Hrsensor_io_ctl(u8 cmd, void *arg)
{
    /* printf("%s %d", __func__, cmd); */
    switch (cmd) {
    case HR_SENSOR_ENABLE:
        user_main_post_to_p11_system(P11_SYS_HRSENSOR_START, 0, NULL, 1);
        memcpy(&READLEN, arg, sizeof(int));
        break;
    case HR_SENSOR_DISABLE:
        user_main_post_to_p11_system(P11_SYS_HRSENSOR_STOP, 0, NULL, 1);
        break;
    case SPO2_SENSOR_ENABLE:
        user_main_post_to_p11_system(P11_SYS_SPO2_START, 0, NULL, 1);
        memcpy(&READLEN, arg, sizeof(int));
        break;
    case SPO2_SENSOR_DISABLE:
        user_main_post_to_p11_system(P11_SYS_SPO2_STOP, 0, NULL, 1);
        break;
    case HR_SENSOR_READ_DATA:
        return p11_hrsensor_cbuf_read((u8 *)arg, sizeof(int) * READLEN);
        break;
    case SPO2_SENSOR_READ_DATA:
        return p11_spo2sensor_cbuf_read((u8 *)arg, sizeof(int) * READLEN);
        break;
    case HR_SEARCH_SENSOR:	//心率血氧共用一个传感器，可拆分成2组分别check
        int valid = 0;
        user_main_post_to_p11_system(P11_SYS_HRSENSOR_SEARCH, sizeof(int), &valid, 1);
        /* printf("%s vaild:0x%x\n",__func__,valid); */
        memcpy(arg, &valid, 1);//有效值值判断0/1，如需获取id号可以拓展到4
        break;
    default:
        break;
    }
    return 0;
}


REGISTER_HR_SENSOR(hrSensor) = {
    .logo = "p11hrsensor",
    .heart_rate_sensor_init  = hrsensor_init,
    .heart_rate_sensor_check = NULL,
    .heart_rate_sensor_ctl   = Hrsensor_io_ctl,
};

#endif
