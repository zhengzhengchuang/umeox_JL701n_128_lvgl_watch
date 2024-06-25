
#include "includes.h"
#include "iic_api.h"
#include "sensor_gp_timer.h"
#include "sensor_api.h"
#include "msg.h"
#include "bank_switch.h"


#include "app_config.h"
/* #include "os/os_api.h" */
/* #include "asm/clock.h" */
#include "timer.h"
/* #include "asm/cpu.h" */
/* #include "typedef.h" */
/* #include "generic/typedef.h" */
#include "gpio.h"
/* #include "debug.h" */
#include "hrSensor_manage.h"
/* #include "system/os/os_api.h" */
/* #include "printf.h" */
#include <stdint.h>
#include <stdlib.h>


/*********************************************************************************************************************
*               Copyright(c) 2020, Vcare Corporation. All rights reserved.
**********************************************************************************************************************
* @file     module_heart_vc.c
* @brief    Application example.
* @details
              The resources needed to use this chip include an RTC or a general timer, an external interrupt line,
			and an I2C interface.
			  First of all, confirm the count frequency of the RTC or the general timer, assign it to 'mcuOscData',
			if the count frequency is 30kHz, mcuOscData = 30000; then confirm the frequency of the I2C clock, assign
			it to 'mcuI2cClock', if the frequency of the I2C clock is 400kHz, mcuI2cClock = 400.
			  Add the code to get the RTC or the general timer count value in function 'vcHr02GetRtcCountFromMCU()',
			add I2C read and write function code in 'vcHr02ReadRegisters(uint8_t startAddress, uint8_t *pRegisters,
			uint8_t len)' and 'vcHr02ReadRegisters(uint8_t startAddress, uint8_t *pRegisters, uint8_t len)', but do
			not change the name of the function.
			   How To Use The Chip: Call the program to initialize the peripherals of the main control chip first,
			these peripherals include I2C, an external interrupt, RTC or general timer. Then the chip ID can be read
			to verify whether the chip is working properly or whether I2C communication is normal, the register add-
			ress is 0x00, if your chip is VC31B/VC32S/VC51B/VC52S, the chip ID is 0x21, but also need to verify whe-
			ther your I2C supports continuous multi byte read and write, because the driver code involves continuous
			multi byte read and write. After initializing the peripherals, call the chip initialization function
			'vcHr02Init(&vcHr02,vcMode)', the parameter 'vcHr02' is a defined heart rate data structure, 'vcMode'
			is also defined variable which can be used to switch the chip working mode. After the initialization of
			the chip is completed, the peripheral lights will be on, and the lights will be dimmed when the fingers
			are close to the chip from far to near, the interrupt pin of the chip will have interrupt level.
			  About the function 'vcHr02_process(AlgoSportMode vcSportMode)': It can be used as an interrupt serv-
			ice function or as an independent task, which is added to the task queue by the interrupt trigger, but
			we strongly recommend the second one. The parameter 'vcSportMode' is a defined variable which can be us-
			ed to switch the sport mode in heart rate mode.
*---------------------------------------------------------------------------------------------------------------------
*---------------------------------------------------------------------------------------------------------------------
		   使用芯片需要的外设资源包括一个实时时钟源或者一个通用定时器(计数频率不得低于10K，最佳是在30K左右)，一个外部
		中断，一路I2C。
		   首先，确认实时时钟源或者通用定时器的计数频率，并将这个频率赋值给变量‘mcuOscData’，如果计数频率是30KHz，则
		mcuOscData = 30000；然后确认I2C的时钟频率，并将这个频率赋值给变量‘mcuI2cClock’，如果I2C的时钟频率是400KHz，则
		mcuI2cClock = 400。
		   在函数‘vcHr02GetRtcCountFromMCU()'’里添加获取实时时钟源或通用定时器的计数值的代码，注意不是返回差值（与VC31
		和VC51不同），在函数‘vcHr02ReadRegisters(uint8_t startAddress, uint8_t *pRegisters, uint8_t len)’添加I2C的读函
		数的代码，在函数‘vcHr02ReadRegisters(uint8_t startAddress, uint8_t *pRegisters, uint8_t len)’添加I2C的写函数的
		代码，但不要改变函数名称。读函数中三个变量的意义分别是：寄存器地址、指向寄存器内容存储位置的指针（存寄存器里读
		回来的数据的变量地址）、读的长度，写函数中三个变量的意义分别是：寄存器地址、指向要写入寄存器的内容的指针、写的
		长度。
		   芯片使用：先调用初始化主控芯片外设的代码，初始化实时时钟源或通用定时器，I2C，外部中断，然后可以通过读芯片的
		ID来判断芯片是否正常工作或者I2C通讯是否正常，寄存器地址是0x00，如果使用的芯片是VC31B/VC32S/VC51B/VC52S，芯片的
		ID是0x21，但要确认I2C是否支持连续读写多个字节，因为驱动里涉及连续读写多个字节的操作。外设初始化完成之后，调用
		‘vcHr02Init(&vcHr02,vcMode)’来初始化芯片，变量‘vcHr02’是已经定义的心率数据结构体，变量‘vcMode’也是已经定义的，
		可以通过改变这个变量的值，切换芯片的工作模式，初始化芯片之后，外围的等会亮，并且手指由远到近靠近时，灯会由亮到暗
		调整，芯片的中断引脚也会有中断电平。
		   关于‘vcHr02_process(AlgoSportMode vcSportMode)’函数：可以作为中断服务函数，也可以作为独立任务，通过中断触发
		将它添加到任务队列，但我们强烈推荐第二种方式。‘vcSportMode’是一个已经定义的在心率模式下切换运动模式的变量。

       *若用户是用定时读取方式读取Gsensor数据，用于计步等功能，为了更好的动态心率性能，我们推荐按照我们的中断的时间间隔，
	   在‘vcHr02_process(AlgoSportMode vcSportMode)’函数中对应位置，读取Gsensor数据，中断的时间间隔（佩戴状态下）可以
	   根据FIFO的长度可调（40ms整数倍）。如果用户心率非24小时连续测量方式，即进入心率测量功能界面测量心率，退出界面停止
	   心率测量或定时唤醒心率芯片测量心率方式，则调用‘vcHr02StopSample(vcHr02_t *pvcHr02)’函数让芯片进入休眠时，可以重新
	   使能定时器，定时读取Gsensor数据，因为调用上述函数使心率芯片休眠时，中断脚将不会有中断电平，‘vcHr02_process(AlgoS-
	   portMode_t vcSportMode)’函数将不会执行，如果用户是24小时连续心率测量，即不会休眠，可以忽略。若用户是按照Gsensor中
	   断触发读取Gsensor数据的方式，请将Gsensor读取的数据保存到buffer中，Gsensor每读一次数据就将buffer中的上一次数据用这
	   一次的数据覆盖，在‘vcHr02_process(AlgoSportMode vcSportMode)’函数中传入算法函数中。

* @author
* @date
* @version  V1.9.7
**********************************************************************************************************************
*/
#include "vcHr02Hci.h"

/* Include your INT,I2C,Timer header file */
#include "iic_hw.h"
#include "iic_soft.h"
/* #include "asm/timer.h" */

#if TCFG_VCHR02_DEV_ENABLE
#define BANK_NAME  BANK_SENSOR_TAG
#define log_info(format,...) printf("[vchr02]"format"\n",## __VA_ARGS__)
#define log_e(format,...) printf("<err:>[vchr02]"format"\n",## __VA_ARGS__)
#define log_error log_e

/* extern spinlock_t sensor_iic; */
#define   MDELAY(n)    mdelay(n)

/* 如果Gsensor数据读取代码已经添加，请将这个宏置1 */
#define GsensorEn 0

/* Heart rate value */
int HeartRateValue = 0;

//*******************************************************************************//
//定时器定时频率及变量mcuOscData iic频率及变量mcuI2cClock iic操作不可关定时器中断//
//*******************************************************************************//
/* Timer clock frequency
   Be used to adjust INT frequency */
const uint32_t mcuOscData = 22000;

/* MCU I2C clock frequency */
/* const uint16_t mcuI2cClock = 400; */
const uint16_t mcuI2cClock = 150;//150k

/* Heart rate sample frequency */
const uint8_t vcHr02SampleRate = 25;
/*###############注意: 以上三值保证与sdk配置相同,且与实际值相同###################*/
/*###############注意: mcuOscData提供的值vchr02_time_cnt不可被任何流程中断#############*/
/* INT Flag */
bool vcHr02IRQFlag = 0;

#if VCHR02_SPO2_EN
/* Spo2 value */
float vcSpo2Value = 0.0;
int real_spo = 0;

/* Spo2 data */
float vcRedPPG = 0.0;
float vcIrPPG = 0.0;
#endif

uint16_t GsensorLength = 0;

/* G-Sensor Data */
int16_t xData[40] = {0};
int16_t yData[40] = {0};
int16_t zData[40] = {0};

/* The algorithm of hert rate data struct */
AlgoInputData_t algoInputData;
AlgoOutputData_t algoOutputData;

/* Heart rate data struct */
vcHr02_t vcHr02;

/* Heart rate mode */
vcHr02Mode_t vcMode = VCWORK_MODE_HRWORK;

/* Sport Mode In Heart Rate Mode */
AlgoSportMode vcSportMode = SPORT_TYPE_NORMAL;

struct vcHr02_send vcHr02_sendbuf {
    bool vcFifoReadFlag;
    bool vcPsFlag;
    vcHr02WearStatus_e wearStatus;
    uint8_t sampleData_envValue[3];
    uint16_t sampleData_ppgValue[128];
    uint8_t ppgLength;
    vcHr02Ret_t vcHr02Ret_status;
};
#define VCHR02_DATA_LEN sizeof(vcHr02_send)
u8 vchr02_buf[VCHR02_DATA_LEN * 5];
static cbuffer_t *data_w_cbuf;
static u8 sensor_online = 0;
static u8 sensor_spo2_mode = 0; //1:spo2,0:hr
/*
 * @brief  Initialize the chip working mode and set some parameters.
 *
 *
 **/
void vcHr02Init(vcHr02_t *pVcHr02, vcHr02Mode_t vcHr02WorkMode)
{
    vcHr02StopSample(pVcHr02);
    vcHr02SoftReset(pVcHr02);
    /* vcSportMotionAlgoTerminate(); */
    pVcHr02->vcSampleRate = vcHr02SampleRate;
    pVcHr02->mcuOscValue = mcuOscData;
    pVcHr02->mcuSclRate = mcuI2cClock;
    pVcHr02->workMode = vcHr02WorkMode;
    vcHr02StartSample(pVcHr02);
    /* HeartRate Mode */
    if (pVcHr02->workMode == VCWORK_MODE_HRWORK) {
#if TCFG_VCHR02_P11_ALGO_ENABLE
        Algo_Init();//lib
#endif
    }
#if VCHR02_SPO2_EN
    /* Spo2 Mode */
    else if (pVcHr02->workMode == VCWORK_MODE_SPO2WORK) {
#if GsensorEn
        vcSportMotionAlgoInit();//need gsensor
#endif
#if TCFG_VCHR02_P11_ALGO_ENABLE
        vcSpo2AlgoInit();//lib
#endif
    }
#endif
    /* Light Crosstalk Test Mode */
    else if (pVcHr02->workMode == VCWORK_MODE_LPDETECTION) {
#if TCFG_VCHR02_P11_ALGO_ENABLE
        Algo_Init();//lib
#endif
    }
    /* Light Crosstalk Test Mode */
    else if (pVcHr02->workMode == VCWORK_MODE_CROSSTALKTEST) {
    }
}


/*
 * @brief  The processing flow under different working modes can be used as
 *         interrupt service function or as an independent task, but it is
 *         recommended to be treated as an independent task.
 *
 **/
#if TCFG_VCHR02_P11_ALGO_ENABLE
void vcHr02_process(AlgoSportMode vcSportMode)
{
    /*{{{*/
    uint8_t algoCallNum = 0;
    uint8_t ppgLength = 0;
    uint8_t vcSportFlag = 0;

    if (vcHr02IRQFlag) {
        vcHr02IRQFlag = false;

        if (vcHr02.workMode == VCWORK_MODE_HRWORK) {
            if (VCHR02RET_UNWEARTOISWEAR == vcHr02GetSampleValues(&vcHr02, &ppgLength)) {
                Algo_Init();//lib
            }

            /* printf("ppgLength:%d\n", ppgLength); */
            if (vcHr02.vcFifoReadFlag || vcHr02.vcPsFlag) {
#if GsensorEn

                /*
                Gsensor数据处理示例:
                1.此处Gsensor采样率为36Hz，定时读取方式；
                2.800mS（心率芯片FIFO长度为20,20×40ms）中断间隔时，定时方式读取Gsensor数据
                  长度众数（800ms读一次Gsensor数据，一段时间内读到29个数据的次数最多）是29；
                3.心率芯片的FIFO长度是20，所以也只要20组Gsensor数据，心率芯片FIFO长度为多少
                  就需要多少组Gsensor数据；

                */
                /* 在下一行添加读取Gsensor数据的代码 */
                /* ReadGsensorFIFO(+-4G11bit or +-8G12bit 256/g) */

                if (GsensorLength >= 29) {
                    GsensorLength = 29;
                } else if (GsensorLength >= 1) {
                    //小于29个数据补足29个；
                    for (uint8_t i = GsensorLength; i < 29; i++) {
                        xData[i] = xData[GsensorLength - 1];
                        yData[i] = yData[GsensorLength - 1];
                        zData[i] = zData[GsensorLength - 1];
                    }
                }

                for (uint8_t i = 0; i < 20; i++) {
                    //cash_num[20]数组为抽样数组；此处将29个数据抽成20个，匹配800mS中断的20个PPG
                    xData[i] = yData[cash_num[i]] >> 5;
                    yData[i] = xData[cash_num[i]] >> 5;
                    zData[i] = zData[cash_num[i]] >> 5;
                }
#endif
                if (vcHr02.vcFifoReadFlag) {
                    vcHr02.vcFifoReadFlag = 0;
                    if (vcHr02.wearStatus == VCHR02WEARST_ISWEAR) {
#if GsensorEn
                        if (GsensorLength < ppgLength) { //此处为了防止丢中断，保证每个PPG数据参与算法运算，丢中断的情况下，Gsensor数据个数补足到ppgLength；
                            for (uint8_t i = GsensorLength; i < ppgLength; i++) {
                                xData[i] = xData[GsensorLength - 1];
                                yData[i] = yData[GsensorLength - 1];
                                zData[i] = zData[GsensorLength - 1];
                            }
                        }
#endif
                        algoInputData.envSample = vcHr02.sampleData.envValue[0];
                        for (algoCallNum = 0; algoCallNum < ppgLength; algoCallNum++) {
                            algoInputData.ppgSample = vcHr02.sampleData.ppgValue[algoCallNum];
                            algoInputData.axes.x = xData[algoCallNum];//The direction vertical with ARM.
                            algoInputData.axes.y = yData[algoCallNum];//The direction parallel with ARM.
                            algoInputData.axes.z = zData[algoCallNum];//The direction upside.
                            Algo_Input(&algoInputData, 1000 / vcHr02SampleRate, vcSportMode, 0, 0);//lib
                        }

                        Algo_Output(&algoOutputData);//lib
                        HeartRateValue = algoOutputData.hrData;
                        /* printf("HeartRateValue :%d\n", HeartRateValue); */

                        if (HeartRateValue == -1) {
                            Algo_Init();//lib
                        }
                    }
                } else {
#if SportMotionEn
                    green_led_off_state_gsensor_abs_sum_diff_func(xData[0], yData[0], zData[0]);
#endif
                    HeartRateValue = -2;
                }
            }
        }
#if VCHR02_SPO2_EN
        else if (vcHr02.workMode == VCWORK_MODE_SPO2WORK) {
            if (VCHR02RET_UNWEARTOISWEAR == vcHr02GetSampleValues(&vcHr02, &ppgLength)) {
                vcSpo2AlgoInit();//lib
#if GsensorEn
                vcSportMotionAlgoInit();//need gsensor
#endif
            }
            /* printf("spo2ppgLength:%d\n", ppgLength); */
            if (vcHr02.vcFifoReadFlag || vcHr02.vcPsFlag) {
#if GsensorEn
                /* ReadGsensorFIFO(+-4G11bit or +-8G12bit 256/g) */
                if (GsensorLength >= 29) {
                    GsensorLength = 29;
                } else if (GsensorLength >= 1) {
                    for (uint8_t i = GsensorLength; i < 29; i++) {
                        xData[i] = xData[GsensorLength - 1];
                        yData[i] = yData[GsensorLength - 1];
                        zData[i] = zData[GsensorLength - 1];
                    }
                }

                for (uint8_t i = 0; i < 20; i++) {
                    xData[i] = yData[cash_num[i]] >> 5;
                    yData[i] = xData[cash_num[i]] >> 5;
                    zData[i] = zData[cash_num[i]] >> 5;
                }
#endif
                if (vcHr02.vcFifoReadFlag) {
                    vcHr02.vcFifoReadFlag = 0;
                    if (ppgLength <= 50) {
                        for (algoCallNum = 0; algoCallNum < ppgLength / 2; algoCallNum++) {
                            vcIrPPG = vcHr02.sampleData.ppgValue[algoCallNum * 2];
                            vcRedPPG = vcHr02.sampleData.ppgValue[algoCallNum * 2 + 1];
                            vcSpo2Value = vcSpo2Calculate(vcRedPPG, vcIrPPG);//lib
#if GsensorEn
                            vcSportFlag = vcSportMotionCalculate(xData[algoCallNum], yData[algoCallNum], zData[algoCallNum]);
#endif
                            if ((!vcSportFlag) && (vcSpo2Value > 0)) {
                                real_spo = vcSpo2Value;
                                /* printf("real_spo:%d\n", real_spo); */
                            }
                        }
                    }
                } else {
#if SportMotionEn
                    green_led_off_state_gsensor_abs_sum_diff_func(xData[0], yData[0], zData[0]);
#endif
                }
            }
        }
#endif
        else if (vcHr02.workMode == VCWORK_MODE_LPDETECTION) {
            vcHr02GetSampleValues(&vcHr02, &ppgLength);
        } else if (vcHr02.workMode == VCWORK_MODE_CROSSTALKTEST) {
            vcHr02GetSampleValues(&vcHr02, &ppgLength);
            /* If Pass: */
            if ((vcHr02.sampleData.maxLedCur >= 100) && (vcHr02.sampleData.preValue[0] <= 2)) {
                //PASS：
                //Display the value of vcHr02.sampleData.maxLedCur and vcHr02.sampleData.preValue[0]
            }
        }
    }
}/*}}}*/
#else //sdk

void vcHr02_process(AlgoSportMode vcSportMode)
{
    uint8_t algoCallNum = 0;
    uint8_t ppgLength = 0;
    uint8_t vcSportFlag = 0;
    if (!sensor_online) {
        return ;
    }

    if (vcHr02IRQFlag) {
        vcHr02IRQFlag = false;
        vcHr02_sendbuf.vcHr02Ret_status = vcHr02GetSampleValues(&vcHr02, &ppgLength);
#if TCFG_VCHR02_P11_ALGO_ENABLE
#else
        vcHr02_sendbuf.ppgLength = ppgLength;
        vcHr02_sendbuf.vcFifoReadFlag = vcHr02.vcFifoReadFlag;
        vcHr02_sendbuf.vcPsFlag = vcHr02.vcPsFlag;
        vcHr02_sendbuf.wearStatus = vcHr02.wearStatus;
        memcpy(vcHr02_sendbuf.sampleData_envValue, vcHr02.sampleData.envValue, 3);
        memcpy(vcHr02_sendbuf.sampleData_ppgValue, vcHr02.sampleData.ppgValue, 128);
        /* log_info("readdata cbuf write:"); */
        if (cbuf_is_write_able(data_w_cbuf, sizeof(vcHr02_sendbuf))) {
            int wlen = cbuf_write(data_w_cbuf, &vcHr02_sendbuf, sizeof(vcHr02_sendbuf));
            if (wlen == 0) {
                printf("vchr02 data_w_cbuf_full\n");
            }
        } else {
            putchar('g');//vchr02 data_w_cbuf_full
            /* return; */
        }
#endif

        if (vcHr02.workMode == VCWORK_MODE_HRWORK) {
            if (VCHR02RET_UNWEARTOISWEAR == vcHr02_sendbuf.vcHr02Ret_status) {
                log_error("need init algo_init()");
#if TCFG_VCHR02_P11_ALGO_ENABLE
                Algo_Init();//lib
#endif
            }

            /* printf("ppgLength:%d\n", ppgLength); */
            if (vcHr02.vcFifoReadFlag || vcHr02.vcPsFlag) {
#if TCFG_VCHR02_P11_ALGO_ENABLE
#if GsensorEn/*{{{*/

                /*
                Gsensor数据处理示例:
                1.此处Gsensor采样率为36Hz，定时读取方式；
                2.800mS（心率芯片FIFO长度为20,20×40ms）中断间隔时，定时方式读取Gsensor数据
                  长度众数（800ms读一次Gsensor数据，一段时间内读到29个数据的次数最多）是29；
                3.心率芯片的FIFO长度是20，所以也只要20组Gsensor数据，心率芯片FIFO长度为多少
                  就需要多少组Gsensor数据；

                */
                /* 在下一行添加读取Gsensor数据的代码 */
                /* ReadGsensorFIFO(+-4G11bit or +-8G12bit 256/g) */

                if (GsensorLength >= 29) {
                    GsensorLength = 29;
                } else if (GsensorLength >= 1) {
                    //小于29个数据补足29个；
                    for (uint8_t i = GsensorLength; i < 29; i++) {
                        xData[i] = xData[GsensorLength - 1];
                        yData[i] = yData[GsensorLength - 1];
                        zData[i] = zData[GsensorLength - 1];
                    }
                }

                for (uint8_t i = 0; i < 20; i++) {
                    //cash_num[20]数组为抽样数组；此处将29个数据抽成20个，匹配800mS中断的20个PPG
                    xData[i] = yData[cash_num[i]] >> 5;
                    yData[i] = xData[cash_num[i]] >> 5;
                    zData[i] = zData[cash_num[i]] >> 5;
                }
#endif/*}}}*/
#endif
                if (vcHr02.vcFifoReadFlag) {
#if TCFG_VCHR02_P11_ALGO_ENABLE
                    vcHr02.vcFifoReadFlag = 0;
                    if (vcHr02.wearStatus == VCHR02WEARST_ISWEAR) {/*{{{*/
#if GsensorEn/*{{{*/
                        if (GsensorLength < ppgLength) { //此处为了防止丢中断，保证每个PPG数据参与算法运算，丢中断的情况下，Gsensor数据个数补足到ppgLength；
                            for (uint8_t i = GsensorLength; i < ppgLength; i++) {
                                xData[i] = xData[GsensorLength - 1];
                                yData[i] = yData[GsensorLength - 1];
                                zData[i] = zData[GsensorLength - 1];
                            }
                        }
#endif/*}}}*/
                        algoInputData.envSample = vcHr02.sampleData.envValue[0];
                        for (algoCallNum = 0; algoCallNum < ppgLength; algoCallNum++) {
                            algoInputData.ppgSample = vcHr02.sampleData.ppgValue[algoCallNum];
                            algoInputData.axes.x = xData[algoCallNum];//The direction vertical with ARM.
                            algoInputData.axes.y = yData[algoCallNum];//The direction parallel with ARM.
                            algoInputData.axes.z = zData[algoCallNum];//The direction upside.
                            Algo_Input(&algoInputData, 1000 / vcHr02SampleRate, vcSportMode, 0, 0);//lib
                        }

                        Algo_Output(&algoOutputData);//lib
                        HeartRateValue = algoOutputData.hrData;
                        /* printf("HeartRateValue :%d\n", HeartRateValue); */

                        if (HeartRateValue == -1) {
                            Algo_Init();//lib
                        }
                    }/*}}}*/
#endif
                } else {
#if SportMotionEn
                    green_led_off_state_gsensor_abs_sum_diff_func(xData[0], yData[0], zData[0]);
#endif
                    HeartRateValue = -2;
                }
            }
        }
#if VCHR02_SPO2_EN
        else if (vcHr02.workMode == VCWORK_MODE_SPO2WORK) {
            if (VCHR02RET_UNWEARTOISWEAR == vcHr02_sendbuf.vcHr02Ret_status) {
                log_error("need init vcSpo2AlgoInit()");
#if TCFG_VCHR02_P11_ALGO_ENABLE
                vcSpo2AlgoInit();//lib
#endif
#if GsensorEn
                vcSportMotionAlgoInit();//need gsensor
#endif
            }
            /* printf("spo2ppgLength:%d\n", ppgLength); */
            if (vcHr02.vcFifoReadFlag || vcHr02.vcPsFlag) {
#if TCFG_VCHR02_P11_ALGO_ENABLE
#if GsensorEn/*{{{*/
                /* ReadGsensorFIFO(+-4G11bit or +-8G12bit 256/g) */
                if (GsensorLength >= 29) {
                    GsensorLength = 29;
                } else if (GsensorLength >= 1) {
                    for (uint8_t i = GsensorLength; i < 29; i++) {
                        xData[i] = xData[GsensorLength - 1];
                        yData[i] = yData[GsensorLength - 1];
                        zData[i] = zData[GsensorLength - 1];
                    }
                }

                for (uint8_t i = 0; i < 20; i++) {
                    xData[i] = yData[cash_num[i]] >> 5;
                    yData[i] = xData[cash_num[i]] >> 5;
                    zData[i] = zData[cash_num[i]] >> 5;
                }
#endif/*}}}*/
#endif
                if (vcHr02.vcFifoReadFlag) {
#if TCFG_VCHR02_P11_ALGO_ENABLE
                    vcHr02.vcFifoReadFlag = 0;
                    if (ppgLength <= 50) {/*{{{*/
                        for (algoCallNum = 0; algoCallNum < ppgLength / 2; algoCallNum++) {
                            vcIrPPG = vcHr02.sampleData.ppgValue[algoCallNum * 2];
                            vcRedPPG = vcHr02.sampleData.ppgValue[algoCallNum * 2 + 1];
                            vcSpo2Value = vcSpo2Calculate(vcRedPPG, vcIrPPG);//lib
#if GsensorEn
                            vcSportFlag = vcSportMotionCalculate(xData[algoCallNum], yData[algoCallNum], zData[algoCallNum]);
#endif
                            if ((!vcSportFlag) && (vcSpo2Value > 0)) {
                                real_spo = vcSpo2Value;
                                /* printf("real_spo:%d\n", real_spo); */
                            }
                        }
                    }/*}}}*/
#endif
                } else {
#if SportMotionEn
                    green_led_off_state_gsensor_abs_sum_diff_func(xData[0], yData[0], zData[0]);
#endif
                }
            }
        }
#endif
        else if (vcHr02.workMode == VCWORK_MODE_LPDETECTION) {
        } else if (vcHr02.workMode == VCWORK_MODE_CROSSTALKTEST) {
            /* If Pass: */
            if ((vcHr02.sampleData.maxLedCur >= 100) && (vcHr02.sampleData.preValue[0] <= 2)) {
                //PASS：
                //Display the value of vcHr02.sampleData.maxLedCur and vcHr02.sampleData.preValue[0]
            }
        }
    }
}

int read_vchr02_result(void *arg, u16 len)
{
    /* log_info("%s",__func__); */

    if (!sensor_online) {
        return 0;
    }

    if (arg) {
        return cbuf_read(data_w_cbuf, arg, len);
    }
    return cbuf_get_data_len(data_w_cbuf);
}

int vchr02_data_check_len()
{
    log_info("%s", __func__);
    if (!sensor_online) {
        return 0;
    }
    return  cbuf_get_data_len(data_w_cbuf);
}

#endif

/****************************iic*****************************/
/* #if TCFG_VCHR02_USE_IIC_TYPE */
/* #define iic_init(iic)                       hw_iic_init(iic) */
/* #define iic_uninit(iic)                     hw_iic_uninit(iic) */
/* #define iic_start(iic)                      hw_iic_start(iic) */
/* #define iic_stop(iic)                       hw_iic_stop(iic) */
/* #define iic_tx_byte(iic, byte)              hw_iic_tx_byte(iic, byte) */
/* #define iic_rx_byte(iic, ack)               hw_iic_rx_byte(iic, ack) */
/* #define iic_read_buf(iic, buf, len)         hw_iic_read_buf(iic, buf, len) */
/* #define iic_write_buf(iic, buf, len)        hw_iic_write_buf(iic, buf, len) */
/* #define iic_suspend(iic)                    hw_iic_suspend(iic) */
/* #define iic_resume(iic)                     hw_iic_resume(iic) */
/* #else */
/* #define iic_init(iic)                       soft_iic_init(iic) */
/* #define iic_uninit(iic)                     soft_iic_uninit(iic) */
/* #define iic_start(iic)                      soft_iic_start(iic) */
/* #define iic_stop(iic)                       soft_iic_stop(iic) */
/* #define iic_tx_byte(iic, byte)              soft_iic_tx_byte(iic, byte) */
/* #define iic_rx_byte(iic, ack)               soft_iic_rx_byte(iic, ack) */
/* #define iic_read_buf(iic, buf, len)         soft_iic_read_buf(iic, buf, len) */
/* #define iic_write_buf(iic, buf, len)        soft_iic_write_buf(iic, buf, len) */
/* #define iic_suspend(iic)                    soft_iic_suspend(iic) */
/* #define iic_resume(iic)                     soft_iic_resume(iic) */
/* #endif */
/* static struct _vchr02_dev_platform_data *vchr02_iic_info; */
/*  */
/* static u8 vchr02_write_reg(u8 vchr02_reg_addr, u8 *buf, u8 len) */
/* { */
/*     #<{(| os_mutex_pend(&SENSOR_IIC_MUTEX,0); |)}># */
/*     #<{(| printf("%s",__func__); |)}># */
/*     #<{(| spin_lock(&sensor_iic); |)}># */
/*     iic_start(vchr02_iic_info->iic_hdl); */
/*     if (0 == iic_tx_byte(vchr02_iic_info->iic_hdl, VCHR02_WADDR)) { */
/*         iic_stop(vchr02_iic_info->iic_hdl); */
/*         log_e("vchr02 write fail1!\n"); */
/*         return false; */
/*     } */
/*     MDELAY(vchr02_iic_info->iic_delay); */
/*     if (0 == iic_tx_byte(vchr02_iic_info->iic_hdl, vchr02_reg_addr)) { */
/*         iic_stop(vchr02_iic_info->iic_hdl); */
/*         log_e("vchr02 write fail2!\n"); */
/*         return false; */
/*     } */
/*     for (u8 i = 0; i < len; i++) { */
/*         MDELAY(vchr02_iic_info->iic_delay); */
/*         if (0 == iic_tx_byte(vchr02_iic_info->iic_hdl, *(buf + i))) { */
/*             iic_stop(vchr02_iic_info->iic_hdl); */
/*             log_e("vchr02 write fail3!\n"); */
/*             return false; */
/*         } */
/*     } */
/*     iic_stop(vchr02_iic_info->iic_hdl); */
/*     MDELAY(vchr02_iic_info->iic_delay); */
/*     #<{(| os_mutex_post(&SENSOR_IIC_MUTEX); |)}># */
/*     #<{(| spin_unlock(&sensor_iic); |)}># */
/*     return true; */
/* } */
/* AT(.volatile_ram_code) */
/* static u8 vchr02_read_reg(u8 vchr02_reg_addr, u8 *read_data, u8 len) */
/* { */
/*     #<{(| printf("%s",__func__); |)}># */
/*     #<{(| os_mutex_pend(&SENSOR_IIC_MUTEX,0); |)}># */
/*     #<{(| spin_lock(&sensor_iic); |)}># */
/*     iic_start(vchr02_iic_info->iic_hdl); */
/*     if (0 == iic_tx_byte(vchr02_iic_info->iic_hdl, VCHR02_WADDR)) { */
/*         iic_stop(vchr02_iic_info->iic_hdl); */
/*         log_e("vchr02 read fail1!\n"); */
/*         return false; */
/*     } */
/*     MDELAY(vchr02_iic_info->iic_delay); */
/*     if (0 == iic_tx_byte(vchr02_iic_info->iic_hdl, vchr02_reg_addr)) { */
/*         iic_stop(vchr02_iic_info->iic_hdl); */
/*         log_e("vchr0vchr02 fail2!\n"); */
/*         return false; */
/*     } */
/*     MDELAY(vchr02_iic_info->iic_delay); */
/*     iic_start(vchr02_iic_info->iic_hdl); */
/*     if (0 == iic_tx_byte(vchr02_iic_info->iic_hdl, VCHR02_RADDR)) { */
/*         iic_stop(vchr02_iic_info->iic_hdl); */
/*         log_e("vchr02 read fail3!\n"); */
/*         return false; */
/*     } */
/*  */
/*     for (u8 i = 0; i < len - 1; i++) { */
/*         read_data[i] = iic_rx_byte(vchr02_iic_info->iic_hdl, 1); */
/*         MDELAY(vchr02_iic_info->iic_delay); */
/*     } */
/*     read_data[len - 1] = iic_rx_byte(vchr02_iic_info->iic_hdl, 0); */
/*     iic_stop(vchr02_iic_info->iic_hdl); */
/*     MDELAY(vchr02_iic_info->iic_delay); */
/*     #<{(| os_mutex_post(&SENSOR_IIC_MUTEX); |)}># */
/*     #<{(| spin_unlock(&sensor_iic); |)}># */
/*     return true; */
/* } */

/*
 * @brief  MCU peripherals initialization.
 **/
/* void vcHr02PhyInit(void) */
/* { */
/*****************************************************************************
* 1.If Reset Pin of the chip is connected to MCU, initialize the corresponding
*	pin on the MCU.
*
* 2.Initialize the I2C of MCU.
*
* 3.Initialize the external interrupt of MCU.
******************************************************************************/
//	hrSensor_manager.c
/*     iic_init(vchr02_iic_info->iic_hdl); */
/* #if (TCFG_VCHR02_P11_ALGO_ENABLE)//sdk wkup init */
/*     gpio_set_die(VCHR02_INT_IO, 1); */
/*     gpio_set_direction(VCHR02_INT_IO, 1); */
/*     gpio_set_pull_up(VCHR02_INT_IO, 1); */
/* #endif */
/* } */



/*
 * @brief  Register writing.
 *
 *
 **/
vcHr02Ret_t vcHr02WriteRegisters(uint8_t startAddress, uint8_t *pRegisters, uint8_t len)
{
    /**********************************************************************************
    * @param
    *       startAddress:Register address.
    *       pRegisters  :Point to the data to write.
    *       len         :Byte to be transmitted.
    *
    *
    *       7bit I2C own address:0x33
    *       8bit I2C own address:0x66(the least significant bit is the W/R control bit)
    ************************************************************************************/
    hrsensor_write_nbyte(VCHR02_WADDR, startAddress, pRegisters, len);
    return VCHR02RET_ISOK;
}


/*
 * @brief  Register reading.
 *
 *
 **/
vcHr02Ret_t vcHr02ReadRegisters(uint8_t startAddress, uint8_t *pRegisters, uint8_t len)
{
    /***********************************************************************************
    * @param
    *       startAddress:Register address.
    *       pRegisters  :Point to the location to save.
    *       len         :Byte to be transmitted.
    *
    *
    *       7bit I2C own address:0x33
    *       8bit I2C own address:0x67(the least significant bit is the W/R control bit)
    ************************************************************************************/
    if (hrsensor_read_nbyte(VCHR02_RADDR, startAddress, pRegisters, len)) {
        /* return VCHR02RET_ISOK; */
    }
    return VCHR02RET_ISOK;
}


/*
 * @brief  Interrupt service function.
 **/
void vcHr02IRQHandler(void)
{
    if (!sensor_online) {
        return ;
    }

    vcHr02IRQFlag = true;
    if (sensor_spo2_mode) {
        sensorhub_irq_add(IRQ_SPO2SENSOR_INDEX);
    } else {
        sensorhub_irq_add(IRQ_HRSENSOR_INDEX);
    }
    putchar('V');//vchr02 data ready
    user_main_post_to_p11_system(P11_SYS_KICK, 0);
}
bool vchr02_chip_check_id()
{
    int i = 0 ;
    uint8_t chip_id = 0;

    if (vcHr02ReadRegisters(0x00, &chip_id, 1)) {
        log_info("--vchr02 id!0x%x\n", chip_id);
        if (chip_id != 0x21) {
            log_error("--------read vchr02 id error!");
            return false;
        }
    }
    log_info("--------read vchr02 id ok!");

    return true;
}

REGISTER_CODE_SEG(BANK_CODE_SENSOR_INIT_BANK_0)
u8 vcHr02DevInit(cbuffer_t  *hrcbuf, cbuffer_t  *spo2cbuf)
{
    if (!hrcbuf || !spo2cbuf) {
        ASSERT(0);
    }
    data_w_cbuf = hrcbuf;
    cbuf_init(data_w_cbuf, vchr02_buf, sizeof(vchr02_buf));
    printf("\nvchr02_buf size:%d,addr:0x%x,0x%x\n", sizeof(vchr02_buf), vchr02_buf, data_w_cbuf->begin);


    if (vchr02_chip_check_id()) {
        sensor_online = 1;
    } else {
        sensor_online = 0;
        return false;
    }

#if (TCFG_VCHR02_P11_ALGO_ENABLE)//sdk wkup init
#ifdef CONFIG_CPU_BR23
    io_ext_interrupt_init(VCHR02_INT_IO, 1, vcHr02IRQHandler);
#elif defined(CONFIG_CPU_BR28)||defined(CONFIG_CPU_BR27)
    // br28外部中断回调函数，按照现在的外部中断注册方式
    // io配置在板级，定义在板级头文件，这里只是注册回调函数
    port_edge_wkup_set_callback(vcHr02IRQHandler);
#endif
#endif
    return true;
}
u8 vchr02_enable()
{
    if (!sensor_online) {
        return false;
    }
    vcHr02Init(&vcHr02, vcMode);
    return true;

}

REGISTER_CODE_SEG(BANK_CODE_SENSOR_NORMAL_BANK_1)
int vchr02_io_ctl(u8 cmd, void *arg)
{
    /* printf("%s cmd=%d\n",__func__,cmd); */
//	log_i("%s",__func__);

    int *data;
    int *msg = (int *)arg;
    int len;
    int ret = 0;

    /* if (!sensor_online) { */
    /* return -1; */
    /* } */

    switch (cmd) {
    case HR_SENSOR_ENABLE:
        vcMode = VCWORK_MODE_HRWORK;
        vchr02_enable();//先init
        printf("vchr02_enable");
        sensor_spo2_mode = 0;
        //timer init
        break;
    case SPO2_SENSOR_ENABLE:
        vcMode = VCWORK_MODE_SPO2WORK;
        vchr02_enable();//先init
        sensor_spo2_mode = 1;
        break;
    case HR_SENSOR_DISABLE:
    case SPO2_SENSOR_DISABLE:
        vcHr02StopSample(&vcHr02);
        //close timer
        cbuf_clear(data_w_cbuf);
        break;
    case HR_SENSOR_CHECK_DATA:
    case SPO2_SENSOR_CHECK_DATA:
        return vchr02_data_check_len();
        break;
    case HR_SENSOR_READ_DATA:
    case SPO2_SENSOR_READ_DATA:
        //返回心率传感器数据
        //返回血氧传感器数据
        data = (void *)(msg[0]);
        len  = (int)(msg[1]);
        ret = read_vchr02_result(data, len);
        break;
    case HR_SEARCH_SENSOR:
        if (vchr02_chip_check_id() == true) {
            char res = 1;
            memcpy(arg, &res, 1);
        } else {
            char res = 0;
            memcpy(arg, &res, 1);
        }
        break;
    case HR_SENSOR_IRQ:
    case SPO2_SENSOR_IRQ:
        vcHr02_process(vcSportMode);//vcSportMode主机有效
        break;

    default:
        break;
    }
    return ret;
}
REGISTER_HR_SENSOR(hrSensor) = {
    .logo = "vchr02",
    .heart_rate_sensor_init = vcHr02DevInit,
    .heart_rate_sensor_check = NULL,
    .heart_rate_sensor_ctl = vchr02_io_ctl,
} ;



/****************************timer3*****************************/

/*
 * @brief  Get the count value of RTC or general timer of MCU.
 *
 *
 **/
extern volatile u32 vchr02_cur_time_tick;//set 20k
uint32_t vcHr02GetRtcCountFromMCU(void)
{
    /* return RTC_GetCounter(); */
    return vchr02_cur_time_tick;
}



#if 0
/*
 * @brief  main test.
 **/
static struct _vchr02_dev_platform_data vchr02_iic_info_test = {
    .iic_hdl = 0,
    .iic_delay = 0
};

void vchr02_test()
{
    /*-----------INSERT BOARD HARDWARE AND SOFTWARE INITIAL FUNCTION------------*/
    /* accInit();          	//Accelerometer initial +-8G 12bit */
    /* vcHr02PhyInit();      	//vcHr02 board initial */
    /*     vcHr02Init(&vcHr02,vcMode); */
    /* #ifdef CONFIG_CPU_BR23 */
    /*         io_ext_interrupt_init(MPU9250_INT_IO, 1, vcHr02IRQHandler); */
    /* #elif defined(CONFIG_CPU_BR28)||defined(CONFIG_CPU_BR27) */
    /*         // br28外部中断回调函数，按照现在的外部中断注册方式 */
    /*         // io配置在板级，定义在板级头文件，这里只是注册回调函数 */
    /*         port_edge_wkup_set_callback(vcHr02IRQHandler); */
    /* #endif */

    /* vcMode = VCWORK_MODE_HRWORK; */
    vcMode = VCWORK_MODE_SPO2WORK;
    if (vcHr02DevInit(NULL.NULL)) {
        vchr02_enable();
        /* timerx_init(); */
        /* vchr02_chip_check_id();//read id */
        log_info("vchr02 init ok!");
        while (1) {
            vcHr02_process(vcSportMode);
            /* wdt_clear(); */
        }
    } else {
        log_info("vchr02 init fail!");
    }
}
#endif
#endif

