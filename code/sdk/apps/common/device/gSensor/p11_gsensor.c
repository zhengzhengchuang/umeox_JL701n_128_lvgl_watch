#include "app_config.h"
#include "gSensor/gSensor_manage.h"

#if TCFG_P11GSENSOR_EN

#include "user_p11_cmd.h"

int p11_gsensor_cbuf_read(u8 *data, u16 len)
{
    int ret = 0;
    ret =  master_p11cbuf_read(0, data, len);
    if (ret == 0) {
        printf("%s %d ret =%d,len=%d\n", __FUNCTION__, __LINE__, ret, len);
    }
    return ret;
}


static int sensor_ctl(u8 cmd, void *arg)
{
    int res = -1;
    switch (cmd) {
    case GSENSOR_DISABLE:
        res = 0;//SC7A20_disable();
        user_main_post_to_p11_system(P11_SYS_GSENSOR_STOP, 0, NULL, 1);
        memcpy(arg, &res, 1);
        break;
    case GSENSOR_RESET_INT:
        /* res = SC7A20_Config(); */
        user_main_post_to_p11_system(P11_SYS_GSENSOR_START, 0, NULL, 1);
        res = 0;
        memcpy(arg, &res, 1);
        break;
    case GSENSOR_RESUME_INT:
        break;
    case READ_GSENSOR_DATA:
        printf("%s %d\n", __FUNCTION__, __LINE__);
        res = 0;
        break;
    case SEARCH_SENSOR:
        int valid = 0;
        user_main_post_to_p11_system(P11_SYS_GSENSOR_SEARCH, sizeof(int), &valid, 1);
        /* printf("%s vaild:0x%x\n",__func__,valid); */
        memcpy(arg, &valid, 1);//有效值值判断0/1，如需获取id号可以拓展到4
        res = 0;
        break;
    default:
        break;
    }
    return res;//>=0:ok,,<0:err
}


static u8 gsensor_init(void)
{
    printf("p11 gsensor init\n");
    //返回0 成功
    return 0;
}


REGISTER_GRAVITY_SENSOR(gSensor) = {
    .logo = "p11gsensor",
    .gravity_sensor_init  = gsensor_init,
    .gravity_sensor_check = NULL,
    .gravity_sensor_ctl   = sensor_ctl,
};

#endif
