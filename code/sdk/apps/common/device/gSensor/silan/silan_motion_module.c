
#include "silan_motion_algo.h"
#include "silan_data.h"
#include "gSensor/gSensor_manage.h"
#include "app_config.h"
#include "math.h"
#include "imuSensor_manage.h"
#include "smartbox/cmd_user.h"
/* #include "le_smartbox_multi_trans.h" */



#ifndef TCFG_GSENSOR_ALGO_SILAN
#define TCFG_GSENSOR_ALGO_SILAN 0
#endif


#ifndef TCFG_QMC6310_ENABLE
#define TCFG_QMC6310_ENABLE 0
#endif



#ifndef TCFG_GSENSOR_RESULE_FROM_MCU
#define TCFG_GSENSOR_RESULE_FROM_MCU 0
#endif



#if TCFG_GSENSOR_ALGO_SILAN
#define test 0
/******** logset ********/
#define GSENSOR_LOG_INFO_ENABLE 0
#if GSENSOR_LOG_INFO_ENABLE
#include "debug.h"
#define LOG_TAG             "[SC7A20]"
#define gslog_info       log_info
#define gslog_e          log_error
#else
#define gslog_info(...)
#define gslog_e(...)
#endif


/***使用驱动前请根据实际接线情况配置******/
/**SC7A20的SDO 脚接地：  0****************/
/**SC7A20的SDO 脚接电源：1****************/
#if 0 //def TCFG_SC7A20_I2C_ADDR_7BIT
#define SL_SC7A20_SDO_VDD_GND            1//1
#else
#define SL_SC7A20_SDO_VDD_GND            0//1
#endif
/*****************************************/

/***使用驱动前请根据实际IIC情况进行配置***/
/**SC7A20的IIC 接口地址类型 7bits：  0****/
/**SC7A20的IIC 接口地址类型 8bits：  1****/
#define SL_SC7A20_IIC_7BITS_8BITS        0//1
/*****************************************/

#if SL_SC7A20_SDO_VDD_GND==0
#define SL_SC7A20_IIC_7BITS_ADDR        0x18
#define SL_SC7A20_IIC_8BITS_WRITE_ADDR  0x30
#define SL_SC7A20_IIC_8BITS_READ_ADDR   0x31
#else
#define SL_SC7A20_IIC_7BITS_ADDR        0x19
#define SL_SC7A20_IIC_8BITS_WRITE_ADDR  0x32
#define SL_SC7A20_IIC_8BITS_READ_ADDR   0x33
#endif

#if SL_SC7A20_IIC_7BITS_8BITS==0
#define SL_SC7A20_IIC_ADDRESS        SL_SC7A20_IIC_7BITS_ADDR
#if SL_SC7A20_SDO_VDD_GND==0
#define SL_SC7A20_IIC_W_ADDR         (0x19U << 1 | 0x0)//  (SL_SC7A20_IIC_ADDRESS << 1 | 0x0)
#define SL_SC7A20_IIC_R_ADDR         (0x19U << 1 | 0x1)  //  (SL_SC7A20_IIC_ADDRESS << 1 | 0x1)
#else
#define SL_SC7A20_IIC_W_ADDR         (0x19U << 1 | 0x0)//  (SL_SC7A20_IIC_ADDRESS << 1 | 0x0)
#define SL_SC7A20_IIC_R_ADDR         (0x19U << 1 | 0x1)  //  (SL_SC7A20_IIC_ADDRESS << 1 | 0x1)
#endif
#else
#define SL_SC7A20_IIC_WRITE_ADDRESS  SL_SC7A20_IIC_8BITS_WRITE_ADDR
#define SL_SC7A20_IIC_READ_ADDRESS   SL_SC7A20_IIC_8BITS_READ_ADDR
#endif

#if SL_Sensor_Algo_Release_Enable==0x00
#include "usart.h"
extern signed short          SL_DEBUG_DATA[10][128];
extern unsigned char         SL_DEBUG_DATA_LEN;
#endif

#if SL_Sensor_Algo_Release_Enable==0x00
extern signed short          SL_ACCEL_DATA_Buf[3][16];
extern unsigned char         SL_FIFO_ACCEL_PEDO_NUM;
signed short                 hp_buf[16];
#endif

#define SL_SC7A20_SPI_IIC_MODE  1
/***SL_SC7A20_SPI_IIC_MODE==0  :SPI*******/
/***SL_SC7A20_SPI_IIC_MODE==1  :IIC*******/

#define SL_SC7A20_CALIBRATION_EN  0
/***SL_SC7A20_CALIBRATION_EN==0  :DISABLE*******/
/***SL_SC7A20_CALIBRATION_EN==1  :ENABLE*******/

unsigned char sl_sleep_sys_hour;
extern int sensor_motion_info_refresh(unsigned int *sport_data);
static u32 Sc7a20_Timer_timeout_tid = 0;
static unsigned char  sl_person_para[4] = {167, 75, 36, 1};
static unsigned char  Turn_Wrist_Para[3] = {3, 3, 0}; //para[2]为水平抬腕开关，与翻腕无关，翻腕为常开。
static signed char SL_WRIST_EN = 1;//亮屏UI功能状态，需与NV初始化值设置一样
static signed char sl_init_status = 0x00;
static unsigned int accel_data[16];
static unsigned int noise_status = 0;
static unsigned char step_level = 1;
static bool accel_sw_reset = 0;
bool watch_level_flag = 0;

/***休眠唤醒时，中断服务函数中，需要关闭传感器中断功能***/
void  SL_MCU_WAKEUP_IN_INT_SERVICE_FUNCTION(void)
{
    gslog_e("SL_MCU_WAKE_CLOSE_SC7A20_INT 111");
    SL_MCU_WAKE_CLOSE_SC7A20_INT();//close sensor int function
    SL_Open_Sc7a20_Timer();        //open sensor timer function
}


#define LOG(fmt,...)  printf("[SL] %s " fmt "\n", __func__, ##__VA_ARGS__)

unsigned char SL_SC7A20_I2c_Spi_Write(bool sl_spi_iic, unsigned char reg, unsigned char dat)
{
    return sl_i2c_write(reg, dat);
}
unsigned char SL_SC7A20_I2c_Spi_Read(bool sl_spi_iic, unsigned char reg, unsigned char len, unsigned char *buf)
{
    return sl_i2c_read(reg, len, buf);
}


/**Please modify the following parameters according to the actual situation**/
signed char SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_INT_SWAY_INIT(void)
{
#if SL_Sensor_Algo_Release_Enable==0x00
    unsigned char sl_version_value = 0x00;
#endif
    unsigned char sl_address_value = 0x00;
    bool sl_cal_xy_flag = 0;
    unsigned char sl_cal_fifo = 12;
    signed char sl_cal_value_xy[2] = {0, 0};

//	bool  fifo_status       =0;
    unsigned char  sl_int_para[4]    = {1, 0, 2, 1}; //refer pdf

    SL_SC7A20_I2c_Spi_Read(1, 0x0F, 1, &sl_address_value);
    printf("############### 0x0F = 0x%x ###########\n", sl_address_value);
    SL_SC7A20_I2c_Spi_Read(1, 0x70, 1, &sl_address_value);
    printf("############### 0x70 = 0x%x ###########\n", sl_address_value);

#if SL_Sensor_Algo_Release_Enable==0x00
    //sl_version_value==0x11  SC7A20
    //sl_version_value==0x26  SC7A20E
    SL_SC7A20_I2c_Spi_Read(1, SL_SC7A20_VERSION_ADDR, 1, &sl_version_value);
    gslog_info("Version ID=0x%x!\r\n", sl_version_value);
    if (sl_version_value == SL_SC7A20_VERSION_VALUE) {
        gslog_info("SC7A20!\r\n");
    } else if (sl_version_value == SL_SC7A20E_VERSION_VALUE) {
        gslog_info("SC7A20E!\r\n");
    }
#endif

    /*****if use spi mode please config first******/
#if SL_SC7A20_SPI_IIC_MODE == 0//spi
    //init sensor
    sl_init_status = SL_SC7A20_Driver_Init(0, 0x04, 1); //spi or iic select
    //0x04-->close i2c pull up resister (must)
#else//i2c
    //init sensor
    sl_init_status = SL_SC7A20_Driver_Init(1, 0x08, 1); //spi or iic select
    //0x08-->close sdo pull up resister
#endif
    /**********sc7a20 fifo test***************/
    //fifo_status=SL_SC7A20_FIFO_TEST();

    /**********set pedo sensitivity***********/
    SL_PEDO_TH_SET(32, 10, step_level, 30, 55); //36 12 0 30 50for motor
    /*which axis used to pedo depend on algo**/
    SL_PEDO_SET_AXIS(3);
    /**********set int para*******************/
    SL_Sc7a20_Int_Config(&sl_int_para[0]);
    /**********set motion para****************/
    SL_Pedo_Person_Inf_Init(&sl_person_para[0]);//personal para init
    /**********set turn wrist para************/
    SL_Turn_Wrist_Init(&Turn_Wrist_Para[0]);
    /**********set sleep sensitivity**********/
    if (!accel_sw_reset) {
        SL_Sleep_Para(120, 10, 1);
    }

#if SL_SC7A20_CALIBRATION_EN==1
    sl_cal_xy_flag = SL_SC7A20_CAL_FUN(&sl_cal_fifo, &sl_cal_value_xy[0]);
#if SL_Sensor_Algo_Release_Enable==0x00
    gslog_info("cal_flag=%d sl_cal_fifo=%d cal_x=%d cal_y=%d!\r\n", sl_cal_xy_flag, sl_cal_fifo, sl_cal_value_xy[0], sl_cal_value_xy[1]);
#endif
    //SL_SC7A20_CAL_WRITE(sl_cal_value_xy[0],sl_cal_value_xy[1]);
    SL_SC7A20_CAL_WRITE(0, 0);
#endif

// #if 1
// 		if((sl_init_status == SL_SC7A20_VERSION_VALUE)	|| (sl_init_status == SL_SC7A20E_VERSION_VALUE)){
// 			gslog_e("SL_MCU_WAKE_CLOSE_SC7A20_INT 222");
// 			SL_MCU_WAKE_CLOSE_SC7A20_INT();
// 			SL_Open_Sc7a20_Timer();
// 		}
// #else
// 		SL_MCU_SLEEP_OPEN_SC7A20_INT();//等待系统完成后，中断再转timer
// #endif

    return sl_init_status;
}

void sl_fifo_delay(void)
{
    /*Add the delay function of the project to meet the slow axis requirements of FIFO length*/
    /*FIFO LEN==12*/

}

static signed char   SL_DEGREE                 = 0;
static unsigned int   SL_STEP                  = 0;
static unsigned int   SL_STEP_TEMP             = 0;
static unsigned char  SL_STEP_MOTION           = 0;
static unsigned int	  SL_DISTANCE              = 0;
static unsigned int	  SL_KCAL                  = 0;
static unsigned char  SL_CADENCE_STEP          = 0;//step per minutes
static unsigned short SL_CADENCE_AMP           = 0;//Equal scale scaling
static unsigned char  SL_CADENCE_DEGREE        = 0;//degree
static signed char    SL_WRIST                 = 0;
static unsigned char  SL_INT_CNT               = 0;//counter
static unsigned char  SL_SLEEP_ACTIVE          = 0;
static unsigned char  SL_SLEEP_STATUS          = 0;
static unsigned char  SL_SLEEP_ADOM            = 0;
static bool           SL_CLOCK_STATUS          = 0;
static bool           SL_SWAY_STATUS           = 0;
static bool           SL_SWAY_ENABLE           = 0;
static unsigned char  SL_DIRECTION_STATUS      = 0;

signed char SL_SC7A20_FIFO_To_Degree(signed short *sl_x_buf, signed short *sl_y_buf, signed short *sl_z_buf, unsigned char filter_en)
{
    signed short SC7A20_XYZ_Buf[3][14];
    if (filter_en > 14) {
        gslog_info("SC7A20_FIFO_NUM error ,return !!! \r\n", filter_en);
        return 0;
    }
    //仅分析平面
    memcpy(&SC7A20_XYZ_Buf[0][0], sl_x_buf, filter_en * sizeof(signed short));
    memcpy(&SC7A20_XYZ_Buf[1][0], sl_y_buf, filter_en * sizeof(signed short));
#if 0
    for (int i = 0; i < filter_en;) {
        gslog_info("filter_en-B: %d = X:%d  Y: %d  \r\n", i, SC7A20_XYZ_Buf[0][i], SC7A20_XYZ_Buf[1][i]);
        i++;
    }
#endif
    return 5;
}
int SC7A20_qmc6310_read(float *buf)
{
    signed short data[3] = {0};
    float i = 0, j = 0;
    static u8 k = 0;

    SL_SC7A20_I2c_Spi_Read(1, 0x63, 1, &data[0]);
    SL_SC7A20_I2c_Spi_Read(1, 0x61, 1, &data[1]);
    SL_SC7A20_I2c_Spi_Read(1, 0x62, 1, &data[2]);

    for (u8 k = 0; k < 3; k++) {
        if (data[k] > 127) {
            //buf[i] = -(float)((256-data[i]) << 8);
            buf[k] = -(float)(256 - data[k]);
        } else {
            //buf[i] = (float)(data[i] << 8);
            buf[k] = (float)(data[k]);
        }
        //printf("buf[%d] = %f",k,buf[k]);
    }
    if (buf[0] < 0) {
        i = -buf[0];
    } else {
        i = buf[0];
    }
    if (buf[1] < 0) {
        j = -buf[1];
    } else {
        j = buf[1];
    }
    if (i + j <= 4) {
        k = 0;
        watch_level_flag = 1;
    } else {
        k++;
        if (k > 5) {
            k = 0;
            watch_level_flag = 0;
        }
    }

    if (buf[0] + buf[1] + buf[2] == 0) {
        return -1;
    } else {
        return 1;
    }
}

int SC7A20_imusensor_read(signed short *buf)
{
    u32 data[3] = {0};
    char i = 0, j = 0, SC7A20_FIFO_NUM;
    signed short SC7A20_XYZ_Buf[3][14];

    SL_SC7A20_GET_DATA(&SC7A20_XYZ_Buf[0][0], &SC7A20_XYZ_Buf[1][0], &SC7A20_XYZ_Buf[2][0]);
    buf[0] = SC7A20_XYZ_Buf[0][0];
    buf[1] = SC7A20_XYZ_Buf[1][0];
    buf[2] = SC7A20_XYZ_Buf[2][0];
#if 0//test
    /**************************************/
    gslog_info("imu group:%2d,SC7A20_XYZ_Buf[0]:%d,  SC7A20_XYZ_Buf[1]:%d,  SC7A20_XYZ_Buf[2]:%d\n", i, SC7A20_XYZ_Buf[0][0], SC7A20_XYZ_Buf[1][0], SC7A20_XYZ_Buf[2][0]);
    /**************************************/
#endif
    return 0;
}


static u8 sc7a20_data_err_flag = 0; // 上报异常数据
int SC7A20_data_check(signed short *buf)
{
    signed short data[3] = {0};
    signed short data_buf[3] = {0};
    static u8 k = 0, j = 0;

    SL_SC7A20_I2c_Spi_Read(1, 0x63, 1, &data[0]);
    SL_SC7A20_I2c_Spi_Read(1, 0x61, 1, &data[1]);
    SL_SC7A20_I2c_Spi_Read(1, 0x62, 1, &data[2]);

    for (u8 k = 0; k < 3; k++) {
        if (data[k] > 127) {
            data[k] = -(256 - data[k]);
            data_buf[k] = -((256 - data[k]) << 8);
        } else {
            data[k] = (data[k]);
            data_buf[k] = (data[k] << 8);
        }
        //printf("buf[%d] = %d,%d",k,data_buf[k],data[k]);
    }

    sc7a20_data_err_flag = 0;
    if ((data[2] > 29) || ((data[2] > 0) && (data[2] <= 29) && (data[0] > 10))) {
        sc7a20_data_err_flag = 1;
    }

    if (data[0] < 0) {
        data[0] = -data[0];
    }
    if (data[1] < 0) {
        data[1] = -data[1];
    }
    if (data[2] < 0) {
        data[2] = -data[2];
    }

    if (data[0] + data[1] + data[2] == 0) {
        return -1;
    } else {
        if (data_buf[2] == buf[2]) {
            j = 0;
            //printf("ok\n");
            return 1;
        } else {
            j++;
            if (j > 1) {
                printf("###sc7a20_false\n");
                j = 0;
                return -1;
            }
        }
    }
    return 0;
}


signed short sc7a20[10];
u8 motion_state = 0;
u32 today_motion_time = 0; // 30s
// motion_relation mr;
// void app_get_realtime_data(int val0,int val1,u8 i)
// {
// 	if (val0 == -1){
// 		mr.motion_data_buf[i]   = (val1 >> 8)&0xff;
// 		mr.motion_data_buf[i+1] = val1&0xff;
// 	}else {
// 		mr.motion_data_buf[i]   = val0&0xff;
// 		mr.motion_data_buf[i+1] = val1&0xff;
// 	}
// 	mcu_send_data_to_app(mr.motion_data_buf);
// }

unsigned int  SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_INT_SWAY_ALGO(void)
{
#if SL_Sensor_Algo_Release_Enable==0x00
    unsigned char         sl_i;
#endif
    unsigned char         reg_20h_buff, reg_25h_buff;
    signed short          SC7A20_XYZ_Buf[3][14];
    signed short          SC7A20_XYZ_Dat[3];
    unsigned char         SC7A20_FIFO_NUM;
    bool                  SL_ACTION;
    static unsigned int   SL_STEP_PRE = 0, SL_KCAL_PRE = 0, SL_DISTANCE_PRE = 0;

    // if((sl_init_status!=SL_SC7A20_VERSION_VALUE)&&(sl_init_status!=SL_SC7A20E_VERSION_VALUE))
    // 	return 0;//initial fail

    // /*******有中断产生，启动500mS扫描模式******/
    // bool SL_TIMER_TO_INT_MODE = FALSE;
    // if((Sc7a20_Timer_timeout_tid == 0) && !SL_TIMER_TO_INT_MODE)
    //     SL_MCU_WAKEUP_IN_INT_SERVICE_FUNCTION();


    //SC7A20_qmc6310_read(sc7a20);

    /*******get sc7a20 FIFO data******/
    SC7A20_FIFO_NUM = SL_SC7A20_Read_FIFO();
    /*******get pedo value************/
    SL_STEP = SL_Watch_Kcal_Pedo_Algo(noise_status); //打开马达或音乐时调用该函数
    //SL_STEP= SL_Watch_Kcal_Pedo_Algo(1);//???????????????ú???

    /*********get sleep active degree value*********/
    SL_SLEEP_ACTIVE = SL_Sleep_Active_Degree(0);


    /*******get accel orginal data and length*******/
    SC7A20_FIFO_NUM = SL_SC7A20_GET_FIFO_Buf(&SC7A20_XYZ_Buf[0][0], &SC7A20_XYZ_Buf[1][0], &SC7A20_XYZ_Buf[2][0], 0);
#if test
    /**************************************/
    gslog_info("SC7A20 read_data, SC7A20_FIFO_NUM= %d , SL_STEP = %d \n", SC7A20_FIFO_NUM, SL_STEP);
    for (u8 i = 0; i < SC7A20_FIFO_NUM; ++i) {
        gslog_info("group:%2d,SC7A20_XYZ_Buf[0]:%d,  SC7A20_XYZ_Buf[1]:%d,  SC7A20_XYZ_Buf[2]:%d\n", i, SC7A20_XYZ_Buf[0][i], SC7A20_XYZ_Buf[1][i], SC7A20_XYZ_Buf[2][i]);
    }
    /**************************************/
#endif
    gslog_info("SC7A20_FIFO_NUM-B:%d!\r\n", SC7A20_FIFO_NUM);

#if TCFG_QMC6310_ENABLE
    acc.qmc6310_data[0] = (float)SC7A20_XYZ_Buf[0][0];
    acc.qmc6310_data[1] = (float)SC7A20_XYZ_Buf[1][0];
    acc.qmc6310_data[2] = (float)SC7A20_XYZ_Buf[2][0];
#endif

#if 0
    for (u8 sl_i = 0; sl_i < SC7A20_FIFO_NUM; sl_i++) {
        gslog_info("FIFO dat_x=%d dat_y=%d dat_z=%d!", SC7A20_XYZ_Buf[0][sl_i], SC7A20_XYZ_Buf[1][sl_i], SC7A20_XYZ_Buf[2][sl_i]);
    }
    gslog_info("\n");
#endif
// 	SL_SC7A20_I2c_Spi_Read(1, 0x20,1, &reg_20h_buff);
// 	SL_SC7A20_I2c_Spi_Read(1, 0x25,1, &reg_25h_buff);
    SL_SC7A20_GET_DATA(&SC7A20_XYZ_Dat[0], &SC7A20_XYZ_Dat[1], &SC7A20_XYZ_Dat[2]);
//  //异常检查,发生数据错位之后fifo数据减少，此时重启ic
// 	if ((-1 == SC7A20_data_check(SC7A20_XYZ_Dat)) || (SC7A20_FIFO_NUM < 5)||(reg_20h_buff != 0x37)){
// 		gslog_e("SC7A20E FIFO NUM = %d,reg_20h =%x,reg_25h = %x",SC7A20_FIFO_NUM,reg_20h_buff,reg_25h_buff);
// 		SL_SC7A20_I2c_Spi_Write(1,0x68, 0xa5);
// 		//os_time_dly(2);
// 		SL_SC7A20_Driver_Init(1,0x08,1);
// 		return 0;
// 	}

    gslog_info("### dat_x=%d dat_y=%d dat_z=%d!", SC7A20_XYZ_Dat[0], SC7A20_XYZ_Dat[1], SC7A20_XYZ_Dat[2]);

    //SL_DEGREE = SL_SC7A20_FIFO_To_Degree(&SC7A20_XYZ_Buf[0][0],&SC7A20_XYZ_Buf[1][0],&SC7A20_XYZ_Buf[2][0],SC7A20_FIFO_NUM);
    /* SL_Pedo_GetMotion_Status */
    SL_STEP_MOTION       = SL_Pedo_GetMotion_Status();
    /* SL_Pedo_Step_Get_Distance */
    SL_DISTANCE          = (SL_Pedo_Step_Get_Distance() * 4) / 50;
    motion_state = SL_STEP_MOTION;
    if (SL_DISTANCE == 0) {
        today_motion_time = 0;
    }
    /* SL_Pedo_Step_Get_KCal */
    SL_KCAL              = SL_Pedo_Step_Get_KCal() / 10;
    /*****average step per minutes****/
    SL_CADENCE_STEP      = SL_Pedo_Step_Get_Step_Per_Min();
    /*****average amp per step****/
    SL_CADENCE_AMP       = SL_Pedo_Step_Get_Avg_Amp();
    /*****motion degree****/
    SL_CADENCE_DEGREE    = SL_Pedo_Step_Get_Motion_Degree();
#if 0
    printf("SL_STEP:%d!\r\n", SL_STEP);
    printf("SL_STEP_MOTION:%d!\r\n", SL_STEP_MOTION);
    printf("SL_DISTANCE:%d!\r\n", SL_DISTANCE);
    printf("SL_KCAL:%d!\r\n", SL_KCAL);
    printf("SL_CADENCE_STEP:%d!\r\n", SL_CADENCE_STEP);
    printf("SL_CADENCE_AMP:%d!\r\n", SL_CADENCE_AMP);
    printf("SL_CADENCE_DEGREE:%d!\r\n", SL_CADENCE_DEGREE);
#else
    gslog_info("SL_STEP_MOTION:%d!\r\n", SL_STEP_MOTION);
    gslog_info("SL_DISTANCE:%d!\r\n", SL_DISTANCE);
    gslog_info("SL_KCAL:%d!\r\n", SL_KCAL);
    gslog_info("SL_CADENCE_STEP:%d!\r\n", SL_CADENCE_STEP);
    gslog_info("SL_CADENCE_AMP:%d!\r\n", SL_CADENCE_AMP);
    gslog_info("SL_CADENCE_DEGREE:%d!\r\n", SL_CADENCE_DEGREE);
#endif
//	    if(SL_STEP>200)
//	    {
    /**reset pedo value**/
//	        SL_Pedo_Kcal_ResetStepCount();
//	    }
#if 1
    SL_WRIST = SL_Watch_Wrist_Algo();//需要每次执行，防止库内逻辑错误。

    if (!SL_WRIST_EN) { //sensor会输出，按sensor记录再一次check
        SL_WRIST = 0;
    }

    gslog_info("000### switch = %d,%d\n", SL_WRIST_EN, SL_WRIST);
#else
    SL_WRIST = SL_Watch_Wrist_Algo();
    gslog_info("000####SL_Watch_Wrist_Algo() = %d\n", SL_WRIST);
#endif

    if ((sc7a20_data_err_flag == 1) && (SL_WRIST == 2)) {
        SL_WRIST = 0;
        //printf("###\n\n\n\n\n\n\n\n\n\n\n\n\n");
    }
    SL_ACTION = SL_SC7A20_Action_Degree();
    gslog_info("SL_ACTION:%d!\r\n", SL_ACTION);

#if 1
    /*******get int config******/

    extern int lcd_sleep_status();
    if ((SL_STEP_TEMP == SL_STEP) && (SL_ACTION == 0) && (lcd_sleep_status() == 1)) { //step no change
        SL_INT_CNT++;

        gslog_info("SC7A20, SL_INT_CNT= %d  \n", SL_INT_CNT);
        if (SL_INT_CNT > 20) { //1-->0.5s  10--5s
            SL_INT_CNT  = 0;
            //close timer of  0.5s
            // SL_Close_Sc7a20_Timer();

            //os_time_dly(2);//中断调用需要关闭
            //if 5s no step, open sensor int function
            //gslog_info("SL_MCU_SLEEP_OPEN_SC7A20_INT 444  \r\n");
            //SL_MCU_SLEEP_OPEN_SC7A20_INT();
            //open mcu int function-->
            //mcu in to sleep status
            //SL_Pedo_WorkMode_Reset();
            //SL_Turn_Wrist_WorkMode_Reset();please not use this function

            // SL_TIMER_TO_INT_MODE = TRUE;
            gslog_info("OPEN_SC7A20_INT  \r\n", SL_ACTION);
        }
    } else { //step changed
        SL_STEP_TEMP = SL_STEP;
        SL_INT_CNT  = 0;
    }
#endif

    /*******get overturn value******/
    SL_CLOCK_STATUS = SL_Get_Clock_Status(1); //open overturn monitor
    if (SL_CLOCK_STATUS == 1) { //overturn success
        SL_Get_Clock_Status(0);//close overturn monitor
    }
    gslog_info("SL_CLOCK_STATUS:%d!\r\n", SL_CLOCK_STATUS);

    /*******get sway value******/
    if (SL_SWAY_ENABLE == 1) {
        /**this function will disable pedo function**/
        SL_SWAY_STATUS = SL_Get_Phone_Answer_Status(4, 5); //get sway value
    }
    SL_DIRECTION_STATUS = SL_Get_Direction_Status();

#if SL_Sensor_Algo_Release_Enable==0x00
    gslog_info("NOW dat_x=%d dat_y=%d dat_z=%d!\r\n", SC7A20_XYZ_Dat[0], SC7A20_XYZ_Dat[1], SC7A20_XYZ_Dat[2]);
    SL_DEBUG_DATA_LEN = SC7A20_FIFO_NUM;
    for (sl_i = 0; sl_i < SL_DEBUG_DATA_LEN; sl_i++) {
        gslog_info("SC7A20_FIFO_NUM=%d dat_x=%d dat_y=%d dat_z=%d!\r\n", SC7A20_FIFO_NUM, SC7A20_XYZ_Buf[0][sl_i], SC7A20_XYZ_Buf[1][sl_i], SC7A20_XYZ_Buf[2][sl_i]);

        SL_DEBUG_DATA[0][sl_i] = SC7A20_XYZ_Buf[0][sl_i];
        SL_DEBUG_DATA[1][sl_i] = SC7A20_XYZ_Buf[1][sl_i];
        SL_DEBUG_DATA[2][sl_i] = SC7A20_XYZ_Buf[2][sl_i];
        SL_DEBUG_DATA[3][sl_i] = SL_STEP;
//        SL_DEBUG_DATA[4][sl_i]=SL_STEP_MOTION;
//        SL_DEBUG_DATA[4][sl_i]=SL_ACTION;
//        SL_DEBUG_DATA[4][sl_i]=SL_CLOCK_STATUS;
        SL_DEBUG_DATA[4][sl_i] = SL_WRIST;
//		SL_DEBUG_DATA[5][sl_i]=SL_SLEEP_STATUS;
        SL_DEBUG_DATA[7][sl_i] = SL_DISTANCE;

//        SL_DEBUG_DATA[5][sl_i]=SL_SWAY_STATUS;
        SL_DEBUG_DATA[6][sl_i] = SL_KCAL;
//        SL_DEBUG_DATA[7][sl_i]=SL_CADENCE_STEP;
//		SL_DEBUG_DATA[7][sl_i]=hp_buf[sl_i];
//        SL_DEBUG_DATA[8][sl_i]=SL_CADENCE_AMP;
//        SL_DEBUG_DATA[9][sl_i]=SL_CADENCE_DEGREE;

    }
    SL_DEBUG_DATA_LEN = SC7A20_FIFO_NUM;
#endif

    if (SL_STEP_PRE != SL_STEP || SL_KCAL_PRE != SL_KCAL || SL_DISTANCE_PRE != SL_DISTANCE) {
        SL_STEP_PRE = SL_STEP;
        SL_KCAL_PRE = SL_KCAL;
        SL_DISTANCE_PRE = SL_DISTANCE;
        // mr.motion_data_buf[step_HL] = (SL_STEP >> 16)&0xff;
        // mr.motion_data_buf[step_H] = (SL_STEP >> 8)&0xff;
        // mr.motion_data_buf[step_L] = SL_STEP&0xff;
        // app_get_realtime_data(-1,SL_KCAL,cal_H);
        // app_get_realtime_data(-1,SL_DISTANCE,distance_H);
    }
    return SL_STEP;
}


void SL_Pedo_set_data(void)
{
    accel_data[0] = SL_STEP;
    accel_data[1] = SL_DISTANCE;//分米dM->米
    accel_data[2] = SL_KCAL;
    accel_data[3] = SL_CADENCE_STEP;
    accel_data[4] = SL_CADENCE_AMP * 10; //
    accel_data[5] = SL_CADENCE_DEGREE / 5;
    accel_data[6] = SL_WRIST;//翻腕状态
    gslog_info("SC7A20, Wrist= %d , SL_STEP = %d \n", accel_data[6], SL_STEP);
}

#if SL_Sensor_Algo_Release_Enable==0x00
unsigned short sl_sleep_counter = 0;
#endif
#define SL_SLEEP_DEEP_TH   6
#define SL_SLEEP_LIGHT_TH  3
extern unsigned char sl_sleep_sys_hour;
/***Call this function regularly for 1 minute***/
unsigned char SL_MCU_SLEEP_ALGO_FUNCTION(void)
{

    /*******get sleep status value******/
    SL_SLEEP_STATUS = SL_Sleep_GetStatus(sl_sleep_sys_hour);
    SL_SLEEP_ACTIVE = SL_Sleep_Get_Active_Degree();
    SL_SLEEP_ADOM  = SL_Adom_GetStatus();

#if SL_Sensor_Algo_Release_Enable==0x00
    sl_sleep_counter++;
    gslog_info("step=%d! sys_time=%d!\r\n", SL_STEP, sl_sleep_sys_hour);
    gslog_info("T=%d,sleep_status:%d,sleep_adom:%d!\r\n", sl_sleep_counter, SL_SLEEP_STATUS, SL_SLEEP_ADOM);
    gslog_info("SL_SLEEP_ACTIVE:%d!\r\n", SL_SLEEP_ACTIVE);
#endif

    if (SL_SLEEP_STATUS < SL_SLEEP_LIGHT_TH) {
        return 0;//0 1 2 3
    } else if (SL_SLEEP_STATUS < SL_SLEEP_DEEP_TH) {
        return 1;//4 5 6
    } else {
        return 2;//7
    }
}

//////////////////////////////////////////////////////////////////
void silan_motion_module_init(void)
{
    SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_INT_SWAY_INIT();
}

void SL_handle_Sc7a20(void)
{
    static unsigned int   steps_old = 0;

    SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_INT_SWAY_ALGO();
    if (steps_old != SL_STEP) {
        printf("\nsl_step = %d\n", SL_STEP);
        steps_old = SL_STEP;
    }
}

void silan_motion_module_handle(short *data, short point)
{
    sl_algo_input(data, point);

    while (sl_fifo_length()) {
        SL_handle_Sc7a20();
    }
}


void SL_Open_Sc7a20_Timer(void)
{
    // gslog_info("sc7a20_500ms_timer open.\n");
    // if(Sc7a20_Timer_timeout_tid == 0){
    //     Sc7a20_Timer_timeout_tid = sys_timer_add(NULL, SL_handle_Sc7a20_Timer, 1000); //500ms
    // }else{
    //     sys_timer_re_run(Sc7a20_Timer_timeout_tid);//已定义则重置
    // 	}
}

void SL_Close_Sc7a20_Timer(void)
{
    // gslog_info("sc7a20_500ms_timer close.\n");
    // if (Sc7a20_Timer_timeout_tid) {
    //     sys_timer_del(Sc7a20_Timer_timeout_tid);
    //     Sc7a20_Timer_timeout_tid = 0;
    // }
}

void SC7A20_read_data(axis_info_t *sl_accel)
{
    short data[3][32] = {{0}, {0}, {0}};
    u8 fifo_len = 0;
    fifo_len = SL_SC7A20_Read_FIFO();
    /*******get pedo value************/
    SL_STEP = SL_Watch_Kcal_Pedo_Algo(0); //马达或音乐时调用该函数,不打开(0),打开(1)
    fifo_len = SL_SC7A20_GET_FIFO_Buf(&data[0][0], &data[1][0], &data[2][0], 0);

    gslog_info("SC7A20 read_data, fifo_len= %d , SL_STEP = %d \n", fifo_len, SL_STEP);

    for (u8 i = 0; i < fifo_len; ++i) {
        sl_accel[i].x = data[0][i];
        sl_accel[i].y = data[1][i];
        sl_accel[i].z = data[2][i];
        gslog_info("group:%2d,sl_accel_x:%5d,  sl_accel_y:%5d,  sl_accel_z:%5d\n", i, sl_accel[i].x, sl_accel[i].y, sl_accel[i].z);
    }
}
void SC7A20_read_single_data(axis_info_t *sl_accel)
{
    short data[3][32] = {{0}, {0}, {0}};
    u8 fifo_len = 0;
    fifo_len = SL_SC7A20_Read_FIFO();
    /*******get pedo value************/
    SL_STEP = SL_Watch_Kcal_Pedo_Algo(0); //马达或音乐时调用该函数,不打开(0),打开(1)
    fifo_len = SL_SC7A20_GET_FIFO_Buf(&data[0][0], &data[1][0], &data[2][0], 0);

    gslog_info("SC7A20 read_data, fifo_len= %d , SL_STEP = %d \n", fifo_len, SL_STEP);
    if (fifo_len == 0) {
        memset(sl_accel, 0, sizeof(axis_info_t));
    } else {
        sl_accel->x = data[0][0];
        sl_accel->y = data[1][0];
        sl_accel->z = data[2][0];
    }
}
u8 SC7A20_Config(void)
{
    u8 Check_Flag = 0;
    Check_Flag = SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_INT_SWAY_INIT();
    if ((Check_Flag == 0x11) || (Check_Flag == 0x26)) {
        gslog_info("SC7A20_Config Init success, ID= 0x%x ###\n", Check_Flag);
        return   0;
    } else {
        gslog_info("SC7A20_Config Init fail,Check_Flag = %d\n", Check_Flag);
        return -1;
    }
}

char SC7A20_Check()
{
    u8 reg_value = 0;
    SL_SC7A20_I2c_Spi_Read(1, 0x0f, 1, &reg_value);
    if ((reg_value == 0x11) || (reg_value == 0x26)) {
        printf("SC7A20 check success, ID= 0x%x \n", reg_value);
        return 0x01;
    } else {
        gslog_info("SC7A20 check fail, ID= 0x%x \n", reg_value);
        return 0x00;
    }
}

static u8 SC7A20_enable(void)
{
    gslog_info("SC7A20 enable \n");
    SL_MCU_WAKEUP_IN_INT_SERVICE_FUNCTION();
    return 0;
}

u8 SC7A20_disable(void)
{
    gslog_info("SC7A20 Disable \n");
    //SL_Close_Sc7a20_Timer();
    //SL_SC7A20_Driver_Init(1,0x08,1);//reset
    gslog_e("SL_MCU_WAKE_CLOSE_SC7A20_INT 333");
    //SL_MCU_WAKE_CLOSE_SC7A20_INT();
    //SL_SC7A20_Power_Down();
    return 0;
}

u8 SC7A20_LowPower(void)
{
    gslog_info("SC7A20 lowerpower \n");
//		u32 rets = 0;
//	__asm__ volatile("%0 = rets" : "=r"(rets));
//	gslog_e("%s:0x%x ", __func__, rets);
    SL_SC7A20_I2c_Spi_Write(1, 0x20, 0x17); //
    return 0;
}

void SC7A20_ctl(u8 cmd, void *arg)
{
    char res;
    switch (cmd) {
    case GSENSOR_DISABLE:
        res = SC7A20_disable();
        memcpy(arg, &res, 1);
        break;
    case GSENSOR_RESET_INT:
        accel_sw_reset = 1;//复位时，不干扰睡眠。
        res = SC7A20_Config();
        memcpy(arg, &res, 1);
        accel_sw_reset = 0;
        break;
    case GSENSOR_RESUME_INT:
        break;
    case GSENSOR_INT_DET:
        break;
    case READ_GSENSOR_DATA:
#if TCFG_GSENSOR_RESULE_FROM_MCU
        SC7A20_read_data((axis_info_t *)arg);
#else
        extern int get_pt_mode_en();
        //   if(!get_pt_mode_en()){
        //         SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_INT_SWAY_ALGO();
        //         SL_Pedo_set_data();

        //   }
        memcpy(arg, &accel_data, sizeof(accel_data));
#endif
        break;
    case SEARCH_SENSOR:
        res = SC7A20_Check();
        memcpy(arg, &res, 1);
        break;
    case 10:
        unsigned int gsensor_data_buf[8];//cmd = 0;1-7 = data
        memcpy(gsensor_data_buf, arg, 8 * sizeof(unsigned int));

        if (gsensor_data_buf[0] == 0x01) { //person_para
            if ((sl_person_para[0] != gsensor_data_buf[1]) //有改变才重新设置，重设会影响计步
                || (sl_person_para[1] != gsensor_data_buf[2])
                || (sl_person_para[2] != gsensor_data_buf[3])
                || (sl_person_para[3] != gsensor_data_buf[4])) {
                sl_person_para[0] = gsensor_data_buf[1];//身高 cm
                sl_person_para[1] = gsensor_data_buf[2];//体重 kg
                sl_person_para[2] = gsensor_data_buf[3];//年龄 year
                sl_person_para[3] = gsensor_data_buf[4];//性别 Woman = 0，Man = 1
                printf("person_para Set:cm = %d,kg = %d,year = %d,sex = %d  \r\n", sl_person_para[0], sl_person_para[1], sl_person_para[2], sl_person_para[3]);
                SC7A20_Config();
            }
        }

        if ((gsensor_data_buf[0] == 0x02) //Wrist_Para
            && (SL_WRIST_EN != gsensor_data_buf[1])) { //person_para

            printf("Wrist_Para Set:enable = %d  \r\n", gsensor_data_buf[1]);
            SL_WRIST_EN = gsensor_data_buf[1];//抬腕开关
        }

        if ((gsensor_data_buf[0] == 0x03) //计步等级 0 - 5
            && (step_level != gsensor_data_buf[1])) {
            printf("step Set:level = %d  \r\n", gsensor_data_buf[1]);
            step_level = gsensor_data_buf[1];
            SC7A20_Config();
        }

        if ((gsensor_data_buf[0] == 0x04) //播放音乐，震动需要开启避免计步
            && (noise_status != gsensor_data_buf[1])) {
            if (gsensor_data_buf[1]) {
                printf("Gsensor noise check set OPEN 	\r\n");
            } else {
                printf("Gsensor noise check set Close	\r\n");
            }

            noise_status = gsensor_data_buf[1];
        }

        if (gsensor_data_buf[0] == 0x05) { //获取sensor旋转方向 有正负，属于增量
            printf("step SL_DEGREE = %d  \r\n", SL_DEGREE);
            gsensor_data_buf[0] = 0x05;
            gsensor_data_buf[1] = SL_DEGREE;
            if (SL_DEGREE < 0) {
                gsensor_data_buf[1] = 0;//正时针
                gsensor_data_buf[2] = -SL_DEGREE;
            } else {
                gsensor_data_buf[1] = 1;//逆时针
                gsensor_data_buf[2] = SL_DEGREE;
            }
            memcpy(arg, gsensor_data_buf, 8 * sizeof(unsigned int));
        }

        if (gsensor_data_buf[0] == 0x06) { //获取睡眠数据
            gsensor_data_buf[0] = 0x06;
            gsensor_data_buf[1] = SL_MCU_SLEEP_ALGO_FUNCTION();//睡眠状态 0清醒 1浅睡眠 2深睡眠
            gsensor_data_buf[2] = SL_SLEEP_ACTIVE;//运动状态 0 静止
            gsensor_data_buf[3] = SL_SLEEP_ADOM;//佩戴情况

            printf("sensor_get sleep status = %d,  level =%d , adom =%d \r\n", gsensor_data_buf[1], gsensor_data_buf[2], gsensor_data_buf[3]);

            memcpy(arg, gsensor_data_buf, 8 * sizeof(unsigned int));
        }

        if (gsensor_data_buf[0] == 0x07) { //reset step
            printf("###############sensor clear step &&&&&&&&&&&&&&&&\r\n");
            SL_DEGREE = 0;
            SL_STEP 				 = 0;
            SL_STEP_TEMP			 = 0;
            SL_STEP_MOTION		 = 0;
            SL_DISTANCE 			 = 0;
            SL_KCAL 				 = 0;
            SL_CADENCE_STEP 		 = 0;
            SL_CADENCE_AMP	     = 0;
            SL_CADENCE_DEGREE		 = 0;
            today_motion_time      = 0;
            //   memset(mr.motion_data_buf,0x00,sizeof(mr.motion_data_buf));
            SL_Pedo_Kcal_ResetStepCount();
        }

        break;
    default:

        break;
    }
}



#endif

