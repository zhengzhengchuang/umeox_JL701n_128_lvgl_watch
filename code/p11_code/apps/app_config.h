#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__
/*********************************************************************************************
    *   Filename        : app_config.h

    *   Description     : 本文用于配置功能工程的功能使能开关

    *   Author          : MoZhiYe

    *   Email           : mozhiye@zh-jieli.com

    *   Last modifiled  : 2021-04-17 09:00

    *   Copyright:(c)JIELI  2021-2029  @ , All Rights Reserved.
*********************************************************************************************/
#include "ram_share.h"

//==========================================================//
//                      串口打印使能开关                    //
//==========================================================//
#define CONFIG_UART_DEBUG_ENABLE   0

//==========================================================//
//                 		VAD使能开关                   		//
//==========================================================//
#define CONFIG_VAD_ENABLE          0

//==========================================================//
//                 		LPCTMU使能开关                   	//
//==========================================================//
#define CONFIG_LPCTMU_ENABLE       0

//==========================================================//
//                 		soft iic                   	//
//==========================================================//
#define TCFG_SW_I2C0_CLK_PORT      IO_PORTB_04  //spft IIC CLK
#define TCFG_SW_I2C0_DAT_PORT      IO_PORTB_05  //spft IIC DAT
#define TCFG_SW_I2C0_DELAY_CNT     1           //软件IIC延时参数，影响通讯时钟频率
//==========================================================//
//                 		bmp280                   	//
//==========================================================//
#define TCFG_BMP280_DEV_ENABLE     0
#define TCFG_BMP280_USER_IIC_TYPE  0  //0:软件IIC  1:硬件IIC


//==========================================================//
//                 		sensor alg使能开关                   		//
//==========================================================//
#define CONFIG_SENSOR_ALG_ENABLE            	0



//==========================================================//
//                 		gSensor                   	//
//==========================================================//
#define TCFG_GSENSOR_ENABLE        0
#define TCFG_SC7A20_EN             0
#define TCFG_SC7A20E_EN            0
#define TCFG_SC7A20H_EN            0
#define TCFG_MSA310_EN             0
#define TCFG_GSENOR_USER_IIC_TYPE  0  //0:软件IIC  1:硬件IIC

//*********************************************************************************//
//                                  hr-sensor配置                                   //
//*********************************************************************************//
#define TCFG_HR_SENSOR_ENABLE      0  //hrSensor使能
#define TCFG_SPO2_SENSOR_ENABLE    0  //SPO2sensor使能
//hrs3603
#define TCFG_HRS3603_DEV_ENABLE    0
//vchr02
#define TCFG_VCHR02_DEV_ENABLE       0//模块time不能停
#define TCFG_VCHR02_P11_ALGO_ENABLE  0//algo 在sdk操作
//hrs3300 无
#define TCFG_HRS3300_EN            0
#define TCFG_HR_SENSOR_USE_IIC_TYPE   0  //0:软件IIC  1:硬件IIC

#define TCFG_HR_SENSOR_READ_BY_INT   0  //0:定时器读数据  1:中断读数据
#endif /* #ifndef __APP_CONFIG_H__ */
