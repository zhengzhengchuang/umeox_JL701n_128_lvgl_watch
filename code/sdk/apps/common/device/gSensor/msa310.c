#include "math.h"
#include "gSensor/msa310.h"
#include "gSensor/gSensor_manage.h"
#include "gSensor/msa310_function.h"


#if TCFG_MSA310_EN

#define LOG_TAG             "[msa310]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"



static u8 msa310_is_idle = 1;



#define MSA310_W_ADDR          (0x62 << 1 | 0x0)
#define MSA310_R_ADDR          (0x62 << 1 | 0x1)

uint8_t msa_id = 0;

static unsigned int   MSA_STEP                  = 0;//计步值
static unsigned int   MSA_LIGHT                 = 0;//抬手亮屏
static unsigned char  MSA_SWAY_STATUS           = 0;//摇一摇拍照
static unsigned char  MSA_SLEEP           		  = 0;//摇一摇拍照
static uint8_t slee_cnt = 0;//睡眠变量


#if 0
#define abs(x) (((x) < 0) ? (-(x)) : (x))
int16_t msa_sqrt(int32_t x)
{
    double a = 1.0;
    while (abs(a * a - x) > 100) {
        a = (a + x / a) / 2;
    }
    return (int16_t)a;
}
#endif
/*return value: 0: is ok    -1:read is failed*/
int32_t msa_register_read(uint8_t addr, uint8_t *data)
{
    int32_t     res = 0;

    res = _gravity_sensor_get_ndata(MSA310_R_ADDR, addr, data, 1);//return:0:err,  1:ok
    if (res) {
        return 0;
    }
    return -1;
}

/*return value: 0: is ok    -2:write is failed*/
int32_t msa_register_write(uint8_t addr, uint8_t data)
{
    int32_t     res = 0;

    // ret = msa_WriteBytes(addr,data);
    res = gravity_sensor_command(MSA310_W_ADDR, addr, data);//return:0:err,  1:ok
    if (res) {
        return 0;
    }
    return -2;
}

//return:0:ok,  -1:err
int32_t msa_register_read_continuously(uint8_t addr, uint8_t count, uint8_t *data)
{
    int32_t     res = 0;
    int32_t i = 0;

    for (i = 0; i < count; i++) {
        res = msa_register_read(addr + i, data + i);
        if (res) {
            return res;

        }
    }
    return res;//ok
}

//return:0:ok,  -1/-2:err
int32_t msa_register_mask_write(uint8_t addr, uint8_t mask, uint8_t data)
{
    int32_t     res = 0;
    uint8_t      tmp_data;

    res = msa_register_read(addr, &tmp_data);
    if (res) {
        return res;
    }

    tmp_data &= ~mask;
    tmp_data |= data & mask;
    res = msa_register_write(addr, tmp_data);

    return res;
}

/*return value: 0: is ok    other: is failed*/
int32_t msa_read_data(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t    tmp_data[6] = {0};

    if (msa_register_read_continuously(MSA_REG_ACC_X_LSB, 6, tmp_data) != 0) {
        return -1;
    }

    *x = ((short)(tmp_data[1] << 8 | tmp_data[0])) >> 4;
    *y = ((short)(tmp_data[3] << 8 | tmp_data[2])) >> 4;
    *z = ((short)(tmp_data[5] << 8 | tmp_data[4])) >> 4;

    return 0;
}


#if 1

int32_t msa_read_fifo(axis_info_t *raw_accel)
{
    int i = 0;
    unsigned char data_count = 0;
    uint8_t temp_buf[192];//32*6
    msa_register_read(0x0d, &data_count);
    data_count = (data_count & 0x7F);
    /* printf("data_count1 = %d \r\n",data_count); */
    if (data_count > 32) {
        data_count = 32;
    }
    /* printf("data_count = %d \r\n",data_count); */
    _gravity_sensor_get_ndata(MSA310_R_ADDR, 0xff, temp_buf, data_count * 6); //return:0:err,  acc_count*6:ok

    for (u8 i = 0; i < data_count; i++) {
        raw_accel[i].x = ((short)(temp_buf[i * 6 + 1] << 8 | temp_buf[i * 6 + 0])) >> 4;
        raw_accel[i].y = ((short)(temp_buf[i * 6 + 3] << 8 | temp_buf[i * 6 + 2])) >> 4;
        raw_accel[i].z = ((short)(temp_buf[i * 6 + 5] << 8 | temp_buf[i * 6 + 4])) >> 4;
        /* printf("%d:    %d     %d    %d\r\n", data_count-i,raw_accel[i].x,raw_accel[i].y,raw_accel[i].z); */
    }
    return data_count;
}
#else
int32_t msa_read_fifo(int16_t *xBuf, int16_t *yBuf, int16_t *zBuf, int len)
{
    int i = 0;
    uint8_t temp_buf[384];
#if 0

//#else
    _gravity_sensor_get_ndata(MSA310_R_ADDR, 0xff, data, len * 6);//return:0:err,  len * 6:ok

    for (i = 0; i < len; i++) {
        xBuf[i] = ((short)(temp_buf[i * 6 + 1] << 8 | temp_buf[i * 6 + 0])) >> 4;
        yBuf[i] = ((short)(temp_buf[i * 6 + 3] << 8 | temp_buf[i * 6 + 2])) >> 4;
        zBuf[i] = ((short)(temp_buf[i * 6 + 5] << 8 | temp_buf[i * 6 + 4])) >> 4;
    }
#endif
    return TRUE;
}

/* u8 MSA310_GetAccAxesRaw(axis_info_t* buff) */
/* { */
/* 	uint8_t    tmp_data[6] = {0}; */
/*   if (msa_register_read_continuously(MSA_REG_ACC_X_LSB, 6, tmp_data) != 0) { */
/*   			return -1; */
/*   } */
/*  */
/* 	buff->y = ((short)(tmp_data[3] << 8 | tmp_data[2]))>> 4; */
/* 	buff->x = ((short)(tmp_data[1] << 8 | tmp_data[0]))>> 4; */
/* 	buff->z = ((short)(tmp_data[5] << 8 | tmp_data[4]))>> 4; */
/*  */
/* 	return 0; */
/* } */
#endif


/*return value: 0: is ok    other: is failed*/
uint8_t msa310_init(void)
{
    int32_t             res = 0;
    uint8_t data = 0;

    msa_register_read(MSA_REG_WHO_AM_I, &msa_id);
    if (msa_id != 0x13) {
        log_error("read msa310 id error");
        return -1;
    }
    log_info("msa310 id:0x%x", msa_id);

    //reset
    res =  msa_register_mask_write(MSA_REG_SOFT_RESET, 0x24, 0x24);

    mdelay(10);//10ms

    res |= msa_register_mask_write(MSA_REG_G_RANGE, 0x03, 0x02);//0:2g, 1:4g, 2:8g, 3:16g
    res |= msa_register_mask_write(MSA_REG_POWERMODE_BW, 0xFF, 0x5e);//low power mode, BW:500hz
    /* res |= msa_register_mask_write(MSA_REG_ODR_AXIS_DISABLE, 0xFF, 0x04);//xyz enable, odr:15.63hz */
    /* res |= msa_register_mask_write(MSA_REG_POWERMODE_BW, 0xFF, 0x50);//low power mode, BW:125hz */
    /* res |= msa_register_mask_write(MSA_REG_POWERMODE_BW, 0xFF, 0x4e);//low power mode, BW:62.5hz */
    res |= msa_register_mask_write(MSA_REG_ODR_AXIS_DISABLE, 0xFF, 0x07);//xyz enable, odr:125hz

//fifo config
    // bit67:00: bypass mode, 01: FIFO mode, 10: stream mode, 11: trigger mode, WATERMARK_SAMPLES[5:0]:0x20(32)
    res |= msa_register_mask_write(MSA_REG_FIFO_CTRL, 0xFF, 0xa0);
    res |= msa_register_mask_write(MSA_REG_FIFO_CTRL1, 0x03, 0x00);//fifo enable xyz data

    //int config
    res |= msa_register_mask_write(MSA_REG_INTERRUPT_SETTINGS2, 0x78, 0x60);//bit6:OVERRUN_INT_EN, bit5:WATERMARK_INT_EN, bit4:NEW_DATA_INT_EN, bit3:FREEFALL_INT_EN
    res |= msa_register_mask_write(MSA_REG_INTERRUPT_MAPPING2, 0x07, 0x06);//bit2:OVERRUN_INT1, bit1:WATERMARK_INT1, bit0:NEW_DATA_INT1
    res |= msa_register_mask_write(MSA_REG_INT_PIN_CONFIG, 0x03, 0x00);//bit1:INT1 OD mode, bit0:INT1 LEVEL
    res |= msa_register_mask_write(MSA_REG_INT_LATCH, 0x8f, 0x00);//non-latched

    msa_param_init();

    return res;
}
/*return value: 0: is ok    other: is failed*/
uint8_t msa310_stop(void)
{
    int32_t             res = 0;

    res |= msa_register_mask_write(MSA_REG_POWERMODE_BW, 0xFF, 0xDE);//lkk modify 0x5E
    res |= msa_register_mask_write(MSA_REG_FIFO_CTRL, 0xFF, 0x00);

    return res;
}

//摇一摇拍照接口
bool msa310_shake(short x, short y, short z)
{
#if 1
    static u8 cnt = 0;
    static axis_info_t xyz_axis[4];
    static short max_x, max_y, max_z;
    static short min_x, min_y, min_z;

    xyz_axis[cnt].x = x;
    xyz_axis[cnt].y = y;
    xyz_axis[cnt].z = z;

    cnt++;
    if (cnt == 4) {
        cnt = 0;
        max_x = min_x = xyz_axis[cnt].x;
        max_y = min_y = xyz_axis[cnt].y;
        max_z = min_z = xyz_axis[cnt].z;
        for (u8 i = 0; i < 4; i++) {
            if (xyz_axis[i].x > max_x) {
                max_x = xyz_axis[i].x;
            }

            if (xyz_axis[i].x < min_x) {
                min_x = xyz_axis[i].x;
            }

            if (xyz_axis[i].y > max_y) {
                max_y = xyz_axis[i].y;
            }

            if (xyz_axis[i].y < min_y) {
                min_y = xyz_axis[i].y;
            }

            if (xyz_axis[i].z > max_z) {
                max_z = xyz_axis[i].z;
            }

            if (xyz_axis[i].z < min_z) {
                min_z = xyz_axis[i].z;
            }
        }
        //co_printf("max_x:%d,min_x:%d\r\n",max_x,min_x);
        //co_printf("max_y:%d,min_y:%d\r\n",max_y,min_y);
        //co_printf("max_z:%d,min_z:%d\r\n",max_z,min_z);

        if (((max_x - min_x) > 400) || ((max_y - min_y) > 400) || ((max_z - min_z) > 400)) {
            return true;
        } else {
            return false;
        }
    }
    return false;
#endif
}


void MSA310_GetFiFOAccAxesRaw(void)
{
    uint8_t temp_buf[384] = {0};
    short x = 0, y = 0, z = 0;
    int xyz = 0;
    unsigned char data_count = 0;
    char i;
    char id;

    msa_register_read(0x0d, &data_count);
    //printf("data_count1 = %d \r\n",data_count);
    data_count = (data_count & 0x7F);
    //printf("data_count = %d \r\n",data_count);
    if (data_count > 32) {
        data_count = 32;
    }

    _gravity_sensor_get_ndata(MSA310_R_ADDR, 0xff, temp_buf, data_count * 6); //return:0:err,  acc_count*6:ok

    for (i = 0; i < data_count; i++) {
        x = ((short)(temp_buf[i * 6 + 1] << 8 | temp_buf[i * 6 + 0])) >> 4;
        y = ((short)(temp_buf[i * 6 + 3] << 8 | temp_buf[i * 6 + 2])) >> 4;
        z = ((short)(temp_buf[i * 6 + 5] << 8 | temp_buf[i * 6 + 4])) >> 4;
        printf("     %d     %d     %d  %d\r\n", x, y, z, data_count - i);


        //计步
        MSA_STEP = msa_step(x, y, z);
        //APP_LOG("MSA_STEP=%d\n",MSA_STEP);

        //抬手亮屏
        if ((MSA_LIGHT == 1) || (MSA_LIGHT == 2)) { //取到状态后就不用计算抬手亮屏了
            //APP_LOG("MSA_LIGHT=%d\n",MSA_LIGHT);
        } else {
            MSA_LIGHT = msa_light(x, y, z); //函数返回值: 1 : 亮屏   2:灭屏  0:无动作
        }

        if (i == (data_count - 1)) {
            MSA_LIGHT = 0;
        }



        //摇一摇拍照
        static u8 SWAY_STATUS_TEMP = 0;
        if (1) { //(SmartBand_GetShakePicture())//摇一摇拍照
            /**this function will disable pedo function**/
            MSA_SWAY_STATUS = msa310_shake(x, y, z); //get sway value
            /* if(MSA_SWAY_STATUS== 1 && MSA_SWAY_STATUS!= SWAY_STATUS_TEMP && !motor_is_running() && motor_delay_lock()) */
            if (MSA_SWAY_STATUS == 1 && MSA_SWAY_STATUS != SWAY_STATUS_TEMP) {
                //APP_LOG("MSA motor_is_running : %d\r\n", motor_is_running());
                //SmartBand_ControlShakePicture();//拍照
                SWAY_STATUS_TEMP = 1;
            } else if (MSA_SWAY_STATUS == 0 && MSA_SWAY_STATUS != SWAY_STATUS_TEMP) {
                SWAY_STATUS_TEMP = 0;
            }
            //APP_LOG("MSA_SWAY_STATUS = %d\r\n",MSA_SWAY_STATUS);
        }


        //睡眠
        //手环睡眠状态:SmartBand_SleepStatus
        //充电状态:!get_charging_state()
        /* if(SmartBand_SleepStatus && !get_charging_state()) */
        if (1) {
            int sleep_status = msa_sleep(x, y, z); //3S采样一次数据
#if 0
            if (sleep_status == MSA_RESTLESS) {
                MSA_SLEEP = SLEEP_STATUS_AWAKE;
            } else if (sleep_status == MSA_ASLEEP) {
                MSA_SLEEP = SLEEP_STATUS_LIGHT;
            } else if (sleep_status == MSA_ONTABLE) {
                MSA_SLEEP = SLEEP_STATUS_DEEP;
            } else if (sleep_status == MSA_AWAKE) {
                MSA_SLEEP = SLEEP_STATUS_GETUP;
            }
#endif
            //APP_LOG("SL_SLEEP:%d\n",SL_SLEEP);
        }
    }
}
int msa_get_step(void)
{
    return MSA_STEP;
}
int msa_get_light(void)
{
    return MSA_LIGHT;
}
bool  is_msa310_chipid(void)
{
    if (msa_id == 0x13) {
        return true;
    } else {
        return false;
    }
}

bool msa310_check(void)
{
    u8 msa310_d = 0;
    msa_register_read(MSA_REG_WHO_AM_I, &msa310_d);
    if (msa310_d == 0x13) {
        return true;
    } else {
        return false;
    }
}





int msa310_ctl(u8 cmd, void *arg)
{
    int res = -1;
    switch (cmd) {
    case GSENSOR_DISABLE:
        res = msa310_stop();
        memcpy(arg, &res, 1);
        break;
    case GSENSOR_RESET_INT:
        res = msa310_init();
        memcpy(arg, &res, 1);
        break;
    case GSENSOR_RESUME_INT:
        break;
    case GSENSOR_INT_DET:
        /* msa310_int_io_detect(*(u8 *)arg); */
        res = 0;
        break;
    case READ_GSENSOR_DATA:
        res = msa_read_fifo((axis_info_t *)arg);
        break;
    case SEARCH_SENSOR:
        res = msa310_check();
        memcpy(arg, &res, 1);
        break;
    default:

        break;
    }
    return res;//>=0:ok,,<0:err
}

static u8 msa310_idle_query(void)
{
    return msa310_is_idle;
}

REGISTER_GRAVITY_SENSOR(gSensor) = {
    .logo = "msa310",
    .gravity_sensor_init  = msa310_init,
    .gravity_sensor_check = NULL,//msa310_click_status,
    .gravity_sensor_ctl   = msa310_ctl,
};

REGISTER_LP_TARGET(msa310_lp_target) = {
    .name = "msa310",
    .is_idle = msa310_idle_query,
};

#endif

