/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_example_common.h
 *
 * @brief   example code for gh30x
 * 
 * @version  @ref GH30X_EXAMPLE_VER_STRING
 *
 */

#ifndef _GH30X_EXAMPLE_COMMON_H_
#define _GH30X_EXAMPLE_COMMON_H_

/* includes */
// #include "stdio.h"
// #include "stdint.h"
//#include "stdbool.h"
// #include "string.h"

#include "gh30x_example_config.h"
#include "../../Lib_Code/inc/goodix_type.h"

/// version string
#define GH30X_EXAMPLE_VER_STRING           "example code v0.6.2 (For HBD lib v0.7.8.6 and later)\r\n"

#define HBD_CTRL_LIB_FOR_BAND              (1) // Bracelet, watch etc
#define HBD_CTRL_LIB_FOR_EP                (2) // Earphone
#define HBD_CTRL_LIB_FOR_IN_EAR_EP         (3)
#define HBD_CTRL_LIB_FOR_FINGER            (4) // Finger clip instrument
#define HBD_CTRL_LIB_DEV_VERSION           HBD_CTRL_LIB_FOR_BAND

#ifndef HBD_CTRL_LIB_DEV_VERSION
#error HBD_CTRL_LIB_DEV_VERSION is undefine !!!
#endif

/// version string
#define HBD_CTRL_MAJOR_VERSION_NUMBER      (6u)
#define HBD_CTRL_MINOR_VERSION_NUMBER      (9u)
#define HBD_CTRL_REVISION_VERSION_NUMBER   (0u) // add fix version on last
#define ENABLE_SPO2_ABNORMAL_STATE         (0)

typedef unsigned char GU8;
typedef signed char GS8;
typedef unsigned short GU16;
typedef signed short GS16;
typedef int GS32;
typedef unsigned int GU32;
typedef unsigned long long GU64;
typedef signed long long GS64;
typedef float GF32;
typedef unsigned char GBOOL;
typedef char GCHAR;  /**< char type */

#define BYTE_TRUE                          (GU8)1
#define BYTE_FALSE                         (GU8)0
#define GH30X_FUNC_OFFSET_ADT              (0)    
#define GH30X_FUNC_OFFSET_HR               (1)    
#define GH30X_FUNC_OFFSET_HRV              (2)      
#define GH30X_FUNC_OFFSET_SPO2             (3)    
#define GH30X_FUNC_OFFSET_SOFT_ADT         (4)
#define GH30X_FUNC_OFFSET_PROTOCOL         (5)
#define GH30X_FUNC_OFFSET_MAX              (6)  

/* Function mode. */
#define GH30X_NO_FUNCTION                  (0)
#define GH30X_FUNCTION_ADT                 ((0x1) << (GH30X_FUNC_OFFSET_ADT))
#define GH30X_FUNCTION_HR                  ((0x1) << (GH30X_FUNC_OFFSET_HR))
#define GH30X_FUNCTION_HRV                 ((0x1) << (GH30X_FUNC_OFFSET_HRV))
#define GH30X_FUNCTION_SPO2                ((0x1) << (GH30X_FUNC_OFFSET_SPO2))
#define GH30X_FUNCTION_SOFT_ADT            ((0x1) << (GH30X_FUNC_OFFSET_SOFT_ADT))
#define GH30X_FUNCTION_PROTOCOL            ((0x1) << (GH30X_FUNC_OFFSET_PROTOCOL))
#define GH30X_FUNCTION_ALL                 (0xFFFFFFFF)

#define __HBD_ALGORITHM_VERTION_PRO__      0
#define __HBD_ALGORITHM_VERTION_TINY__     1
#define __HBD_ALGORITHM_VERTION_BASIC__    2


/* Hbd functiional state enum. */
typedef enum 
{
    HBD_FUNCTIONAL_STATE_DISABLE = 0, 
    HBD_FUNCTIONAL_STATE_ENABLE = 1,
} EM_HBD_FUNCTIONAL_STATE;

/* Int status enum. */
enum EM_INT_STATUS
{
    INT_STATUS_CHIP_RESET = 0,
    INT_STATUS_NEW_DATA,
    INT_STATUS_FIFO_WATERMARK,
    INT_STATUS_FIFO_FULL,
    INT_STATUS_WEAR_DETECTED,
    INT_STATUS_UNWEAR_DETECTED,
    INT_STATUS_INVALID,
} ;

/* I2c Low Two bit select enum. */
enum EM_HBD_I2C_ID_SEL
{
    HBD_I2C_ID_SEL_1L0L = 0,
    HBD_I2C_ID_SEL_1L0H,
    HBD_I2C_ID_SEL_1H0L,
    HBD_I2C_ID_SEL_1H0H,
    HBD_I2C_ID_INVALID,
};

/* Led pd select enum. */
typedef enum 
{
    HBD_LED_PD_SEL_INTERNAL = 0,
    HBD_LED_PD_SEL_EXTERNAL,
} EM_HBD_LED_PD_SEL;

/* Led logic channel map to hw led enum. */
typedef enum
{
    HBD_LED_LOGIC_CHANNEL_MAP_PHY012 = 0,
    HBD_LED_LOGIC_CHANNEL_MAP_PHY021,
    HBD_LED_LOGIC_CHANNEL_MAP_PHY102,
    HBD_LED_LOGIC_CHANNEL_MAP_PHY120,
    HBD_LED_LOGIC_CHANNEL_MAP_PHY201,
    HBD_LED_LOGIC_CHANNEL_MAP_PHY210,
} EM_HBD_LED_LOGIC_CHANNEL_MAP;

/* sample time enum. */
typedef enum 
{
    HBD_SAMPLE_TIME_32CYCLES = 0,
    HBD_SAMPLE_TIME_64CYCLES,
    HBD_SAMPLE_TIME_128CYCLES,
    HBD_SAMPLE_TIME_256CYCLES,
    HBD_SAMPLE_TIME_512CYCLES,
    HBD_SAMPLE_TIME_1024CYCLES,
    HBD_SAMPLE_TIME_2048CYCLES,
} EM_HBD_SAMPLE_TIME;

/* tia gain enum. */
typedef enum
{
    HBD_TIA_GAIN_0 = 0,
    HBD_TIA_GAIN_1,
    HBD_TIA_GAIN_2,
    HBD_TIA_GAIN_3,
    HBD_TIA_GAIN_4,
    HBD_TIA_GAIN_5,
    HBD_TIA_GAIN_6,
    HBD_TIA_GAIN_7,
} EM_HBD_TIA_GAIN;

/* G-sensor sensitivity(counts/g) enum. */
typedef enum
{
    HBD_GSENSOR_SENSITIVITY_512_COUNTS_PER_G = 0, 
    HBD_GSENSOR_SENSITIVITY_1024_COUNTS_PER_G,    
    HBD_GSENSOR_SENSITIVITY_2048_COUNTS_PER_G,
    HBD_GSENSOR_SENSITIVITY_4096_COUNTS_PER_G,
    HBD_GSENSOR_SENSITIVITY_8192_COUNTS_PER_G,
} EM_HBD_GSENSOR_SENSITIVITY;

/* Hb config struct type. */
typedef struct
{
    EM_HBD_FUNCTIONAL_STATE emHbModeFifoEnable;     // HB fifo enable flag
    EM_HBD_FUNCTIONAL_STATE emHrvModeFifoEnable;    // HRV fifo enable flag
    EM_HBD_FUNCTIONAL_STATE emBpfModeFifoEnable;     // BPF fifo enable flag
    EM_HBD_FUNCTIONAL_STATE emReserve2ModeFifoEnable;     // Reserve fifo enable flag
    EM_HBD_FUNCTIONAL_STATE emReserve3ModeFifoEnable;     // Reserve fifo enable flag
    EM_HBD_FUNCTIONAL_STATE emSpo2ModeFifoEnable;     // Spo2 fifo enable flag
} ST_HB_CONFIG_TYPE;

/* Adt config struct type. Notes: only logic channel0 & channel1 valid in adt mode. */
typedef struct
{
    EM_HBD_FUNCTIONAL_STATE emGINTEnable;                  // GINT enable flag
    EM_HBD_LED_LOGIC_CHANNEL_MAP emLedLogicChannelMap;     // Logic map
    EM_HBD_LED_PD_SEL emLogicChannel0PDSelect;             // Logic Channel0 PD select
    EM_HBD_LED_PD_SEL emLogicChannel1PDSelect;             // Logic Channel1 PD select
    GU8 uchLogicChannel0Current;                           // Logic Channel0 current cofig(1 step = 400uA)
    GU8 uchLogicChannel1Current;                           // Logic Channel1 current cofig(1 step = 400uA)
    EM_HBD_TIA_GAIN emLogicChannel0TiaGain;                // Logic Channel0 tia gain(valid value:0-7)
    EM_HBD_TIA_GAIN emLogicChannel1TiaGain;                // Logic Channel1 tia gain(valid value:0-7)
    EM_HBD_SAMPLE_TIME emSampleTime;                       // Sample time
} ST_ADT_CONFIG_TYPE;

/* Hbd init config struct type. */
typedef struct
{
    ST_HB_CONFIG_TYPE stHbInitConfig;
    ST_ADT_CONFIG_TYPE stAdtInitConfig;
} ST_HBD_INIT_CONFIG_TYPE;

/* Gsensor data struct type. */
typedef struct
{
    GS16 sXAxisVal; // X-Axis Value
    GS16 sYAxisVal; // Y-Axis Value
    GS16 sZAxisVal; // Z-Axis Value
} ST_GS_DATA_TYPE;

/* register struct. */
typedef struct  
{
    GU16 usRegAddr;
    GU16 usRegData;
} ST_REGISTER;

typedef struct  
{
    GU16 usRegAddr;
    GU8 uchLsb;
    GU8 uchMsb;
    GU16 usRegData;
} ST_REGISTER_BIT_FIELD;
 
 /* autoled  channel struct. */
typedef struct
{
    GU32 unTrigerUpperThr;
    GU32 unTrigerLowerThr;
     
    GU32 unTargetUpperThr;
    GU32 unTargetLowerThr; 
    GU32 unCompatibleTakeoffThr; 
    
    EM_HBD_FUNCTIONAL_STATE emAutoledEnable;
    
    GU8 uchGainLimitThr;          //Gain Max 7,will be changed if strong light
	GU8 uchCurrentLimit;          //Energy Saving Setting,Can be changed by diffrent mode such as HB SPO2
	EM_HBD_FUNCTIONAL_STATE emAutoledSaveEnergyEnable;  //If Enable,AutoLED will start from little current, The value changed by Reg 0x2A19
} ST_AUTO_LED_CH_CONFIG;

typedef struct 
{
    GU8 uchHbValue; //hb value
    GU8 uchAccuracyLevel; //hb accuracy level
    GU8 uchWearingState; //1,wear;2,unwear
    GU8 uchWearingQuality; //wear accuracy level
    GU8 uchSNR; //SNR(1-100)
    GU8 uchScene; //cuurent scene
    GU8 uchMotionState; //motion state:0/1/2 means small/medium/big
    GU8 uchSleepFlag;
} ST_HB_RES;

typedef struct 
{
    GU16 usRRvalueArr[4];
    GU8 uchRRvalueCnt;
    GU8 uchHrvConfidentLvl;
} ST_HRV_RES;

/*
 *uliSpo2InvlaidFlag ：      bit0 = SPORT_ABNORMALATION_STATE
                            bit1 = DIRECTION_ABNORMALATION_STATE
                            bit2 = AUTOLED_ABNORMALATION_STATE
                            bit3 = R_VALUE_INVALID_STATE
*/
typedef enum
{
    SPORT_ABNORMALATION_STATE = 0x01,
    DIRECTION_ABNORMALATION_STATE = 0x02,
    AUTOLED_ABNORMALATION_STATE = 0X40,
    R_VALUE_INVALID_STATE =0x80
}EMSpo2InvalidFlag;

typedef struct 
{
    GU8 uchWearingState;//1,wear;2,unwear
    GU8 uchSpo2;//spo2 value
    GF32 fSpo2;// spo2 value (with Decimal)
    GU16 usSpo2RVal;//R value
    GS32 uchSpo2ValidLvl;
    GU8 uchSpo2Confidence;//range 0~100
    GS32 uliSpo2InvlaidFlag;
    GU8 uchHbValue;//heart rate
    GU8 uchHbConfidentLvl;//range 0~100
    GU16 usHrvRRVal[4];//RRI
    GU8 uchHrvcnt;//valid RRI, max 4
    GU8 uchHrvConfidentLvl;
} ST_SPO2_RES;

typedef struct
{
    GU8 uchFrameID;
    GU8 led_gain[3];
    GU8 led_current[3];
    GS32 sAccData[3];
    GS32 nGainAdjFlag[12];
    GS32 nRawdata[12];
    GS32 nCurAdjFlag[12];
    GS32 unSleepFlag;//unused
} ST_ALGO_CALCULATE_INFO;

typedef struct
{
    GU8 uchUpdateFlag;
    GU8 uchResultNum;
    GS32 snResult[16];
} ST_ALGO_RESULT;

/**
 * @brief channel map for function id
 */
typedef enum 
{ 
    GH3011_FUNC_OFFSET_HR = 1               , /**<1 HR FUNC_OFFSET */
    GH3011_FUNC_OFFSET_HRV = 2              , /**<2 HRV FUNC_OFFSET */
    GH3011_FUNC_OFFSET_NADT = 3             , /**<3 NADT FUNC_OFFSET */
    GH3011_FUNC_OFFSET_ADT_CONFIRM = 4      , /**<4 ADT_CONFIRM FUNC_OFFSET */
    GH3011_FUNC_OFFSET_SPO2 = 6             , /**<6 SPO2 FUNC_OFFSET */
    GH3011_FUNC_OFFSET_SOFT_ADT_GREEN = 9   , /**<9 SOFT ADT GREEN FUNC_OFFSET */
    GH3011_FUNC_OFFSET_SOFT_ADT_IR = 15     , /**<15 SOFT ADT IR FUNC_OFFSET */
    GH3011_FUNC_OFFSET_MAX
} EMFunctionID;

/* Hbd function return code definitions list. */
#define HBD_RET_OK                          (0)                /**< There is no error */
#define HBD_RET_GENERIC_ERROR               (-1)               /**< A generic error happens */
#define HBD_RET_PARAMETER_ERROR             (-2)               /**< Parameter error */
#define HBD_RET_COMM_NOT_REGISTERED_ERROR   (-3)               /**< I2c/Spi communication interface not registered error */
#define HBD_RET_COMM_ERROR                  (-4)               /**< I2c/Spi Communication error */
#define HBD_RET_RESOURCE_ERROR              (-5)               /**< Resource full or not available error */
#define HBD_RET_NO_INITED_ERROR             (-6)               /**< No inited error */
#define HBD_RET_LED_CONFIG_ALL_OFF_ERROR    (-7)               /**< Led config all off error */

#define HBD_INIT_CONFIG_DEFAULT_DEF(var) ST_HBD_INIT_CONFIG_TYPE var={\
                                                                      {HBD_FUNCTIONAL_STATE_ENABLE,\
                                                                      HBD_FUNCTIONAL_STATE_ENABLE,\
                                                                      HBD_FUNCTIONAL_STATE_ENABLE,\
                                                                      HBD_FUNCTIONAL_STATE_ENABLE,\
                                                                      HBD_FUNCTIONAL_STATE_ENABLE,\
                                                                      HBD_FUNCTIONAL_STATE_ENABLE},\
                                                                      {HBD_FUNCTIONAL_STATE_DISABLE,\
                                                                      HBD_LED_LOGIC_CHANNEL_MAP_PHY012, \
                                                                      HBD_LED_PD_SEL_INTERNAL, \
                                                                      HBD_LED_PD_SEL_INTERNAL,\
                                                                      0x19, 0x19, HBD_TIA_GAIN_3, HBD_TIA_GAIN_3, HBD_SAMPLE_TIME_128CYCLES}\
                                                                    }

/**
 * @fn     void HBD_AlgoRegisterHookFunc(GS32 (*pAlgoMemoryInitHookFunc)(void* mem_addr, GS32 size),
 *                               void (*pAlgoMemoryDeinitHookFunc)(void),
 *                               GS32 (*pAlgoVersionHookFunc)(EMFunctionID function_id,GU8 version[100]),
 *                               void (*pAlgoInitHookFunc)(EMFunctionID function_id,GU8 buffer_len,GS32 *algo_param_buffer),
 *                               void (*pAlgoCalculateHookFunc)(EMFunctionID function_id,ST_ALGO_CALCULATE_INFO *algo_calc_info,ST_ALGO_RESULT *algo_result),
 *                               void (*pAlgoDeinitHookFunc)(EMFunctionID function_id))
 *
 * @brief  algo register hook function callback
 *
 * @attention   None
 *
 * @param[in]   pAlgoMemoryInitHookFunc      pointer to algo memory init hook function
 * @param[in]   pAlgoMemoryDeinitHookFunc    pointer to algo memory deinit hook function
 * @param[in]   pAlgoVersionHookFunc         pointer to algo version function
 * @param[in]   pAlgoInitHookFunc            pointer to algo init hook function
 * @param[in]   pAlgoCalculateHookFunc       pointer to algo calculate hook function
 * @param[in]   pAlgoDeinitHookFunc          pointer to algo deinit hook function
 * @param[out]  None
 *
 * @return  None
 */
void HBD_AlgoRegisterHookFunc(GS32 (*pAlgoMemoryInitHookFunc)(GS32* error_code),
                                 void (*pAlgoMemoryDeinitHookFunc)(void),
                                 void (*pAlgoInitHookFunc)(EMFunctionID function_id,GU8 buffer_len,GS32 *algo_param_buffer),
                                 void (*pAlgoCalculateHookFunc)(EMFunctionID function_id,ST_ALGO_CALCULATE_INFO *algo_calc_info,ST_ALGO_RESULT *algo_result),
                                 void (*pAlgoDeinitHookFunc)(EMFunctionID function_id));

/****************************************************************
* Description: set i2c operation function
* Input:  uchI2cIdLowTwoBitsSelect: i2c low two bits addr selected, see EM_HBD_I2C_ID_SEL
          pI2cWriteFunc: i2c write function pointer
          pI2cReadFunc: i2c read function pointer
* Return: HBD_RET_PARAMETER_ERROR: function pointer parameter set null error,
          HBD_RET_OK: register success
******************************************************************/
GS8 HBD_SetI2cRW(GU8 uchI2cIdLowTwoBitsSelect,
                   GU8 (*pI2cWriteFunc)(GU8 uchDeviceId, const GU8 uchWriteBytesArr[], GU16 usWriteLen),  
                   GU8 (*pI2cReadFunc)(GU8 uchDeviceId, const GU8 uchCmddBytesArr[], GU16 usCmddLen, GU8 uchReadBytesArr[], GU16 usMaxReadLen));

/****************************************************************
* Description: set delay function callback
* Input:  pDelayUsFunc:delay function callback
* Return: None
******************************************************************/
void HBD_SetDelayUsCallback(void (*pDelayUsFunc)(GU16 usUsec));

/****************************************************************
* Description: load new GH30x config
* Input:    uchNewConfigArr: config array ,
            uchForceUpdateConfig: 1: need force update,0 :don't need, 
* Return: HBD_RET_OK:load config success, HBD_RET_COMM_ERROR: load config error,
******************************************************************/
GS8 HBD_LoadNewConfig(GU8 uchNewConfigArr[], GU8 uchForceUpdateConfig);

/****************************************************************
* Description: load new GH30x reg config array
* Input:    stRegConfigArr: reg config array ,
            usRegConfigLen: reg config array len, 
* Return: HBD_RET_OK:load config success, HBD_RET_COMM_ERROR: load config error,
******************************************************************/
GS8 HBD_LoadNewRegConfigArr(const ST_REGISTER stRegConfigArr[], GU16 usRegConfigLen);

/****************************************************************
* Description: Communication operation interface confirm.
* Output:  None
* Return: HBD_RET_OK=GH30x comunicate ok, 
          HBD_RET_COMM_ERROR=GH30x comunicate error,
          HBD_RET_COMM_NOT_REGISTERED_ERROR=GH30x i2c/spi interface not registered,
******************************************************************/
GS8 HBD_CommunicationInterfaceConfirm(void);

/****************************************************************
* Description: simple init hbd configure parameters
* Input:    stHbInitConfigParam: Init Config struct ,see ST_HBD_INIT_CONFIG_TYPE,
* Return: HBD_RET_OK=success, 
          HBD_RET_PARAMETER_ERROR=paramters error,
          HBD_RET_COMM_ERROR=GH30x comunicate error, 
          HBD_RET_COMM_NOT_REGISTERED_ERROR=GH30x i2c interface not registered
******************************************************************/
GS8 HBD_SimpleInit(ST_HBD_INIT_CONFIG_TYPE *stHbdInitConfigParam);

/****************************************************************
* Description: get chip id
* Input:  None
* Output: uchIdData: pointer to CHIP ID data buffer 
          uchIdLen: pointer to CHIP ID data buffer len
* Return: HBD_RET_OK=success, 
          HBD_RET_GENERIC_ERROR = chip state error,read reg fail
******************************************************************/
GS8 HBD_GetChipId(GU8 *uchIdData, GU8 *uchIdLen);

/****************************************************************
* Description: Control use HB algo or not 
* Input:  emState 
* Return: None
******************************************************************/
void HBD_EnableHBAlgo(EM_HBD_FUNCTIONAL_STATE emState);

/****************************************************************
* Description: start hbd
* Input:  None
* Return: HBD_RET_OK=success, 
          HBD_RET_NO_INITED_ERROR=not inited,
******************************************************************/
GS8 HBD_HbDetectStart(void);

/****************************************************************
* Description: start Wear state confirm.
* Input:  None, 
* Return: HBD_RET_OK=success, 
          HBD_RET_NO_INITED_ERROR=not inited,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_GENERIC_ERROR=don't need check wear state, 
******************************************************************/
GS8 HBD_WearStateConfirmStart(void);

/****************************************************************
* Description: start in-ear ep wear detect, only use with hb detect
* Input:  None, 
* Return: HBD_RET_OK=success, 
          HBD_RET_NO_INITED_ERROR=not inited,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_GENERIC_ERROR=wear detect function is disabled.
******************************************************************/
//GS8 HBD_InearEpWearDetectStart(void);

/****************************************************************
* Description: recover in-ear ep detect, only use in autoled int 
* Input:  None, 
* Return: HBD_RET_OK=success, 
          HBD_RET_NO_INITED_ERROR=not inited,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_GENERIC_ERROR=wear detect function is disabled.
******************************************************************/
GS8 HBD_InearEpDetectRecover (void);

/****************************************************************
* Description: start wear detect, only use with hb detect
* Input:  None, 
* Return: HBD_RET_OK=success, 
          HBD_RET_NO_INITED_ERROR=not inited,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_GENERIC_ERROR=wear detect function is disabled.
******************************************************************/
GS8 HBD_SoftWearDetectStart (void);

/****************************************************************
* Description: recover detect, only use in autoled int 
* Input:  None, 
* Return: HBD_RET_OK=success, 
          HBD_RET_NO_INITED_ERROR=not inited,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_GENERIC_ERROR=wear detect function is disabled.
******************************************************************/
GS8 HBD_SoftWearDetectRecover (void);

/****************************************************************
* Description: start hrv
* Input:  None
* Return: HBD_RET_OK=success, 
          HBD_RET_NO_INITED_ERROR=not inited,
******************************************************************/
GS8 HBD_HrvDetectStart(void);

/****************************************************************
* Description: start hbd with hrv
* Input:  None
* Return: HBD_RET_OK=success, 
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_NO_INITED_ERROR=not inited,
******************************************************************/
//GS8 HBD_HbWithHrvDetectStart (void);

/****************************************************************
* Description: stop hbd
* Input:  None
* Return: HBD_RET_OK=success, 
          HBD_RET_NO_INITED_ERROR=fail:don't init success 
******************************************************************/
GS8 HBD_Stop(void);

/****************************************************************
* Description: start adt wear detect
* Input:  None
* Return: HBD_RET_OK=success, 
          HBD_RET_NO_INITED_ERROR=fail:don't init success 
******************************************************************/
GS8 HBD_AdtWearDetectStart(void);

/****************************************************************
* Description: start adt shedding detect
* Input:  None
* Return: HBD_RET_OK=success, 
          HBD_RET_NO_INITED_ERROR=fail:don't init success 
******************************************************************/
GS8 HBD_AdtSheddingDetectStart (void);

/****************************************************************
* Description: start adt wear detect continuous
* Input:  None
* Return: HBD_RET_OK=success, 
          HBD_RET_NO_INITED_ERROR=fail:don't init success 
******************************************************************/
GS8 HBD_AdtWearContinuousDetectStart(void);

/****************************************************************
* Description: check adt has started
* Input:  None
* Return: 1= has started, 
          0= has not started
******************************************************************/
GU8 HBD_IsAdtWearDetectHasStarted(void);

/****************************************************************
* Description: get Int status 
* Input:  None
* Return: Status, see EM_INT_STATUS
******************************************************************/
GU8 HBD_GetIntStatus(void);

/****************************************************************
* Description: calculate hbd_value and wearing detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            pusRRvalue: ptr of RR value output 
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
GU8 HBD_HbCalculateByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                            GU8 *puchHbValue, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, GU16 *pusRRvalue);

/****************************************************************
* Description: calculate hbd_value and wearing detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            pusRRvalue: ptr of RR value output 
            nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen��ptr of rawdata len output
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
GU8 HBD_HbCalculateByFifoIntDebugOutputData(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                                            GU8 *puchHbValue, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, GU16 *pusRRvalue, 
                                            GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen);

/****************************************************************
* Description: calculate hbd_value and wearing detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchAccuracyLevel: ptr of heartbeat accuracy level, max 100
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            pusRRvalue: ptr of RR value output 
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
GU8 HBD_HbCalculateWithLvlByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU8 *puchHbValue, GU8 *puchAccuracyLevel, 
                                    GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, GU16 *pusRRvalue);

/****************************************************************
* Description: calculate hbd_value and wearing detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchAccuracyLevel: ptr of heartbeat accuracy level, max 100
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            pusRRvalue: ptr of RR value output 
            nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen��ptr of rawdata len output
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
#if 0
GS8 HBD_HbCalculateWithLvlByFifoIntDebugOutputData(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                                            GU8 *puchHbValue, GU8 *puchAccuracyLevel, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, GU16 *pusRRvalue, 
                                            GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen);
#endif
                                            
/****************************************************************
* Description: calculate hbd_value and wearing detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   *pstHbRes: ptr of hb calculate result ,seeST_HB_RES
            nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen:ptr of rawdata len output
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
GS8 HBD_HbCalculateByFifoIntEx(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                                            GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen, ST_HB_RES *pstHbRes);

/****************************************************************
* Description: calculate hbd_value and wearing detect by newdata int. must started with HBD_HbWithHrvDetectStart
* Input:  stGsAxisValue: gsensor data
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            pusRRvalue: ptr of RR value output 
            usRRvalueArr: ptr of RR value output 
            puchRRvalueCnt: ptr of RR value count output
            puchHrvConfidentLvl: ptr of RR value confident
* Return: refresh flag, 1:heartbeat refresh, 2:wearingstate refresh,3: heartbeat&wearingstate refresh.
******************************************************************/
GS8 HBD_HbWithHrvCalculateByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                    GU8 *puchHbValue, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, 
                    GU16 usRRvalueArr[4], GU8 *puchRRvalueCnt, GU8 *puchHrvConfidentLvl);

/****************************************************************
* Description: calculate hbd_value and wearing detect by newdata int. must started with HBD_HbWithHrvDetectStart
* Input:  stGsAxisValue: gsensor data
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   pstHbRes:ptr of hb calculate result output,see ST_HB_RES
            pstHrvRes:ptr of hrv calculate result output,see ST_HRV_RES
* Return: refresh flag, 1:heartbeat refresh, 2:wearingstate refresh,3: heartbeat&wearingstate refresh.
******************************************************************/
GS8 HBD_HbWithHrvCalculateByNewdataIntEx(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                                          ST_HB_RES *pstHbRes, ST_HRV_RES *pstHrvRes);

/****************************************************************
* Description: calculate hbd_value,wearing detect and hrv by fifo int. must started with HBD_HbWithHrvDetectStart
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchAccuracyLevel: ptr of heartbeat accuracy level, max 100
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            usRRvalueArr: ptr of RR value output 
            puchRRvalueCnt: ptr of RR value count output
            puchHrvConfidentLvl: ptr of RR value confident
            nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen��ptr of rawdata len output
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
GS8 HBD_HbWithHrvCalculateByFifoIntDbgData(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                            GU8 *puchHbValue, GU8 *puchAccuracyLevel, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, 
                            GU16 usRRvalueArr[4], GU8 *puchRRvalueCnt, GU8 *puchHrvConfidentLvl,
                            GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen);
     
/****************************************************************
* Description: calculate hbd_value,wearing detect and hrv by fifo int. must started with HBD_HbWithHrvDetectStart
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   *pstHbRes:ptr of hb calculate result,see ST_HB_RES
            *pstHrvRes:ptr of hrv calculate result,see ST_HRV_RES
            nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen:ptr of rawdata len output
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
GS8 HBD_HbWithHrvCalculateByFifoIntDbgDataEx(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                            ST_HB_RES *pstHbRes, ST_HRV_RES *pstHrvRes, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen);

/****************************************************************
* Description: Wear state confirm.
* Output:   None,
* Return: wear state, 0: during wear state detect;
                      1: state output wear 
                      2: state output unwaer 
******************************************************************/
//GU8 HBD_WearStateConfirm(void);

/****************************************************************
* Description: Wear state Detect.
* Output:   None,
* Return: wear state, 0: during wear state detect;
                      1: state output wear 
                      2: state output unwaer 
******************************************************************/
GU8 HBD_WearStateConfirmEx (void);

/****************************************************************
* Description: Wear state confirm by new int.
* Output:   None,
* Return: wear state, 0: during wear state detect;
                      1: state output wear 
                      2: state output unwaer 
******************************************************************/
GU8 HBD_WearStateConfirmByNewdataInt(void);

/****************************************************************
* Description: Wear state confirm by new int.
* Output:   None,
* Return: wear state, 0: during wear state detect;
                      1: state output wear 
                      2: state output unwaer 
******************************************************************/
GU8 HBD_WearStateConfirmByFifoInt (void);

/****************************************************************
* Description: enable wearing and setting direction array
* Input:    fDirectionArr: gsensor direction 
* Return: None
******************************************************************/
void HBD_EnableWearing(GF32 fDirectionArr[3]);

/****************************************************************
* Description: calculate hrv_value by newdata int.
* Input:  stGsAxisValue: gsensor data, if want get algo static state value, could set NULL.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pusHrvRRValueArr:ptr of hrv RR value output array
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
GS8 HBD_HrvCalculateByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU16 *pusHrvRRValueArr);

/****************************************************************
* Description: calculate hrv_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pusHrvRRValueArr:ptr of hrv RR value output array
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
GS8 HBD_HrvCalculateByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU16 *pusHrvRRValueArr);

/****************************************************************
* Description: calculate hrv_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pusHrvRRValueArr:ptr of hrv RR value output array
          nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
          pusRawdataOutLen��ptr of rawdata len output
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
GS8 HBD_HrvCalculateByFifoIntDbgRawdata(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                                          GU16 *pusHrvRRValueArr, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen);

/****************************************************************
* Description: calculate hrv_value by newdata int.
* Input:  stGsAxisValue: gsensor data, if want get algo static state value, could set NULL.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pusHrvRRValueArr:ptr of hrv RR value output array
          puchConfidentLvl:ptr of confident level, 0:no confidence, 1:confidence
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
GS8 HBD_HrvCalculateWithLvlByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU16 *pusHrvRRValueArr, GU8 *puchConfidentLvl);

/****************************************************************
* Description: calculate hrv_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pusHrvRRValueArr:ptr of hrv RR value output array
          puchConfidentLvl:ptr of confident level, 0:no confidence, 1:confidence
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
GS8 HBD_HrvCalculateWithLvlByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU16 *pusHrvRRValueArr, GU8 *puchConfidentLvl);

/****************************************************************
* Description: calculate hrv_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pusHrvRRValueArr:ptr of hrv RR value output array
          puchConfidentLvl:ptr of confident level, 0:no confidence, 1:confidence
          nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
          pusRawdataOutLen��ptr of rawdata len output
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
GS8 HBD_HrvCalculateWithLvlByFifoIntDbgRawdata(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                                       GU16 *pusHrvRRValueArr, GU8 *puchConfidentLvl, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen);
									   
/****************************************************************
* Description: calculate hrv_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pstHrvRes:ptr of hrv result
          nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
          pusRawdataOutLen:ptr of rawdata len output
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
GS8 HBD_HrvCalculateByFifoIntEx(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, ST_HRV_RES *pstHrvRes, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen);

/****************************************************************
* Description: calculate wear state by newdata int.
* Input:  stGsAxisValue: gsensor data, if want get algo static state value, could set NULL.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: puchWearState:ptr of wear state value output, 0:default, 1:wear, 2:unwear,
* Return: refresh flag, if wear state value have refresh that return 1.
******************************************************************/
GU8 HBD_InearEpWearDetectCalculateByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU8 *puchWearState);

/****************************************************************
* Description: calculate wear state by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 25. 
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: puchWearState:ptr of wear state value output, 0:default, 1:wear, 2:unwear,
* Return: refresh flag, if wear state value have refresh that return 1.
******************************************************************/
GU16 HBD_InearEpWearDetectCalculateByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU8 *puchWearState);

/****************************************************************
* Description: calculate wear state by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 25. 
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: puchWearState:ptr of wear state value output, 0:default, 1:wear, 2:unwear,
          nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
          pusRawdataOutLen��ptr of rawdata len output
* Return: refresh flag, if wear state value have refresh that return 1.
******************************************************************/
GU16 HBD_InearEpWearDetectByFifoIntDbgRawdata(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU8 *puchWearState, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen);

/****************************************************************
* Description: get fifo count has read
* Input:  None,  
* Return: fifo count has read, 
******************************************************************/
GU8 HBD_GetFifoCntHasRead(void);

/****************************************************************
* Description: Reset chip
* Input:  None
* Return: HBD_RET_OK=success, 
          HBD_RET_COMM_NOT_REGISTERED_ERROR=GH30x i2c interface not registered
******************************************************************/
GS8 HBD_ChipReset(void);

/****************************************************************
* Description: set irq pluse width 
* Input:  uchIrqPluseWidth: irq width(us) setting ,
                            if set 0: fixed 1us and return HBD_RET_PARAMETER_ERROR
* Return: HBD_RET_OK=success, 
          HBD_RET_PARAMETER_ERROR=fail:parameter error
******************************************************************/
GS8 HBD_SetIrqPluseWidth(GU8 uchIrqPluseWidth);

/****************************************************************
* Description: change Hb config
* Input:  uchMode: 0:Hrv reconfig, else Hb reconfig
*         emFifoEnable: see EM_HBD_FUNCTIONAL_STATE
* Return: HBD_RET_OK=success, 
          HBD_RET_PARAMETER_ERROR=fail:parameter error,
******************************************************************/
GS8 HBD_FifoConfig(GU8 uchMode, EM_HBD_FUNCTIONAL_STATE emFifoEnable);

/****************************************************************
* Description: set irq pluse width 
* Input:  uchMode: 0:Hrv config, 
                   1:Hb config
                   2:hsm config
                   3:bpd config
                   4:pfa config
            usFifoCnt: Fifo thr setting (if enable two channel led,thr will auto * 2) 
* Return: HBD_RET_OK=success, 
          HBD_RET_PARAMETER_ERROR=fail:parameter error, usFifoCnt not allow 0, uchMode > 4
******************************************************************/
//GS8 HBD_SetFifoThrCnt(GU8 uchMode, GU16 usFifoCnt);

/****************************************************************
* Description: config autoled channel
* Input:  stAutoLedChannelConfig: channal config
          uchChannel: channel index(1 or 2)
* Return: HBD_RET_OK=success, 
          HBD_RET_PARAMETER_ERROR=paramters error,
******************************************************************/
GS8 HBD_AutoLedChannelConfig(ST_AUTO_LED_CH_CONFIG stAutoLedChannelConfig, GU8 uchChannel);

/****************************************************************
* Description: config autoled bg thr 
* Input:  uchBgUpperThrConfig: bg upper thr config
          uchgLowerThrConfig: bg lower thr config 
* Return: HBD_RET_OK=success
******************************************************************/
GS8 HBD_AutoLedBgThrConfig(GU8 uchBgUpperThrConfig, GU8 uchgLowerThrConfig);

/****************************************************************
* Description: config autoled 
* Input:  stAutoLedCh1Config: channal 1 config
          stAutoLedCh2Config: channal 2 config
          uchBgUpperThrConfig: bg upper thr config
          uchgLowerThrConfig: bg lower thr config 
* Return: HBD_RET_OK=success, 
******************************************************************/
GS8 HBD_AutoLedConfig(ST_AUTO_LED_CH_CONFIG stAutoLedCh1Config, ST_AUTO_LED_CH_CONFIG stAutoLedCh2Config, 
                      GU8 uchBgUpperThrConfig, GU8 uchgLowerThrConfig);

/****************************************************************
* Description: config autoled gain start val
* Input:  emUseConfigValEnable: set HBD_FUNCTIONAL_STATE_ENABLE, use uchGainVal,
                                else use default val;
          uchGainVal: gain val, only 0-7 valid,  
* Return: HBD_RET_OK=success, 
******************************************************************/
GS8 HBD_AutoLedGainModeConfig(EM_HBD_FUNCTIONAL_STATE emUseConfigValEnable, GU8 uchGainVal);

/****************************************************************
* Description: config autoled current thr max val
* Input:  uchAutoledCurrentMaxThrVal: current max thr val;
* Return: HBD_RET_OK=success, 
******************************************************************/
GS8 HBD_AutoLedCurrentMaxThrConfig(GU8 uchAutoledCurrentMaxThrVal);

/****************************************************************
* Description: Get version
* Input:    None,
* Return: library Hbd ctrl version
******************************************************************/
GS8 * HBD_GetHbdVersion(void);

#if (ENABLE_SPO2_ABNORMAL_STATE)
/****************************************************************
* Description: Get SpO2_value good or not
* Input:    None,
* Return : 0-data good(signal good), 1-Detected motion, 2-PI too big, 3-data bad(signal bad)
******************************************************************/
GU8 HBD_GetSpo2AbnormalState (void);
#endif
/****************************************************************
* Description: start SpO2 and Hr
* Input:  None
* Return: HBD_RET_OK=success, 
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_NO_INITED_ERROR=not inited,
******************************************************************/
GS8 HBD_HrSpO2DetectStart (void);

/****************************************************************
* Description: start SpO2
* Input:  None
* Return: HBD_RET_OK=success, 
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_NO_INITED_ERROR=not inited,
******************************************************************/
//GS8 HBD_SpO2DetectStart (void);

/****************************************************************
* Description: calculate SpO2_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   pusSpo2Value : Spo2 Value
            puchSpo2ConfidentLvl: Spo2 Confident Level
            puchHbValue  : HB Value
            puchHbConfidentLvl: HB Confident Level
            pusHrvRRVal1 : HRV RRI value 1
            pusHrvRRVal2 : HRV RRI value 2
            pusHrvRRVal3 : HRV RRI value 3
            pusHrvRRVal4 : HRV RRI value 4
            puchHrvConfidentLvl: HRV Confident Level
            puchHrvcnt : HRV valid cnt
            pusSpo2RVal :spo2 R value
            pliSpo2InvalidFlag:InvalidFlag
            puchWearingState : Wearing State
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
GU8 HBD_Spo2CalculateByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU8 *pusSpo2Value, GU8 *puchSpo2ConfidentLvl, GU8 *puchHbValue, GU8 *puchHbConfidentLvl,
                                                                           GU16 *pusHrvRRVal1,GU16 *pusHrvRRVal2,GU16 *pusHrvRRVal3,GU16 *pusHrvRRVal4, GU8 *puchHrvConfidentLvl, GU8 *puchHrvcnt,GU16 *pusSpo2RVal, GS32 *pliSpo2InvalidFlag, GU8 *puchWearingState);
/****************************************************************
* Description: calculate SpO2_value by newdata int.
* Input:  stGsAxisValue: gsensor data, if want get algo static state value, could set NULL.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   pusSpo2Value : Spo2 Value
            puchSpo2ConfidentLvl: Spo2 Confident Level
            puchHbValue  : HB Value
            puchHbConfidentLvl: HB Confident Level
            pusHrvRRVal1 : HRV RRI value 1
            pusHrvRRVal2 : HRV RRI value 2
            pusHrvRRVal3 : HRV RRI value 3
            pusHrvRRVal4 : HRV RRI value 4
            puchHrvConfidentLvl: HRV Confident Level
            puchHrvcnt : HRV valid cnt
            pusSpo2RVal :spo2 R value
            pliSpo2InvalidFlag:InvalidFlag
            puchWearingState : Wearing State
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
GS8 HBD_Spo2CalculateByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,  GU8 *pusSpo2Value, GU8 *puchSpo2ConfidentLvl, GU8 *puchHbValue, GU8 *puchHbConfidentLvl,
                                                                          GU16 *pusHrvRRVal1,GU16 *pusHrvRRVal2,GU16 *pusHrvRRVal3,GU16 *pusHrvRRVal4, GU8 *puchHrvConfidentLvl, GU8 *puchHrvcnt,GU16 *pusSpo2RVal, GS32 *pliSpo2InvalidFlag, GU8 *puchWearingState);

/****************************************************************
* Description: calculate SpO2_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   pusSpo2Value : Spo2 Value
            puchSpo2ConfidentLvl: Spo2 Confident Level
            puchHbValue  : HB Value
            puchHbConfidentLvl: HB Confident Level
            pusHrvRRVal1 : HRV RRI value 1
            pusHrvRRVal2 : HRV RRI value 2
            pusHrvRRVal3 : HRV RRI value 3
            pusHrvRRVal4 : HRV RRI value 4
            puchHrvConfidentLvl: HRV Confident Level
            puchHrvcnt : HRV valid cnt
            pusSpo2RVal :spo2 R value
            pliSpo2InvalidFlag:InvalidFlag
            puchWearingState : Wearing State
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
GU8 HBD_Spo2CalculateByFifoIntDbgRawdata(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                                        GU8 *pusSpo2Value, GU8 *puchSpo2ConfidentLvl, GU8 *puchHbValue, GU8 *puchHbConfidentLvl,
                                     GU16 *pusHrvRRVal1,GU16 *pusHrvRRVal2,GU16 *pusHrvRRVal3,GU16 *pusHrvRRVal4, GU8 *puchHrvConfidentLvl, 
                                     GU8 *puchHrvcnt,GU16 *pusSpo2RVal, GS32 *pliSpo2InvalidFlag, GU8 *puchWearingState, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen);

/****************************************************************
* Description: calculate hbd_value and wearing detect by newdata int.
* Input:  stGsAxisValue: gsensor data
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            pusRRvalue: ptr of RR value output 
* Return: refresh flag, 1:heartbeat refresh, 2:wearingstate refresh,3: heartbeat&wearingstate refresh.
******************************************************************/
GS8 HBD_HbCalculateByNewdataIntDbg(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                   GU8 *puchHbValue, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, GU16 *pusRRvalue);

/****************************************************************
* Description: calculate hbd_value and wearing detect by newdata int.
* Input:  stGsAxisValue: gsensor data
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: *pstHbRes:ptr of hb calculate result,see ST_HB_RES
* Return: refresh flag, 1:heartbeat refresh, 2:wearingstate refresh,3: heartbeat&wearingstate refresh.
******************************************************************/
GS8 HBD_HbCalculateByNewdataIntDbgEx(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, ST_HB_RES *pstHbRes);

/****************************************************************
* Description: calculate SpO2_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   pusSpo2Value : Spo2 Value
            puchSpo2ConfidentLvl: Spo2 Confident Level
            puchHbValue  : HB Value
            puchHbConfidentLvl: HB Confident Level
            pusHrvRRVal1 : HRV RRI value 1
            pusHrvRRVal2 : HRV RRI value 2
            pusHrvRRVal3 : HRV RRI value 3
            pusHrvRRVal4 : HRV RRI value 4
            puchHrvConfidentLvl: HRV Confident Level
            puchHrvcnt : HRV valid cnt
            pusSpo2RVal :spo2 R value
            pliSpo2InvalidFlag:InvalidFlag
            puchWearingState : Wearing State
            nRawdataOut : 
            pusRawdataOutLen : 
            puchValidLvl : 
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
#if 0
GS8 HBD_Spo2CalculateByFifoIntDbgRawdataInnerUse(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                                                GU8 *pusSpo2Value, GU8 *puchSpo2ConfidentLvl, GU8 *puchHbValue, GU8 *puchHbConfidentLvl,
                                                GU16 *pusHrvRRVal1,GU16 *pusHrvRRVal2,GU16 *pusHrvRRVal3,GU16 *pusHrvRRVal4, GU8 *puchHrvConfidentLvl, 
                                                GU8 *puchHrvcnt,GU16 *pusSpo2RVal, GS32 *pliSpo2InvalidFlag, GU8 *puchWearingState, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen, GU8 *puchValidLvl);
#endif                                              
/****************************************************************
* Description: calculate SpO2_value by fifo int.(new api)
* Input:    stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
            usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
            emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
            pusRawdataOutLen : nRawdataOut length
* Output:   pstSpo2Res : spo2 result struct
            nRawdataOut : rawdata buf
            pusRawdataOutLen : RawdataOut real length
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
GS8 HBD_Spo2CalculateByFifoIntEx(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                                                GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen, ST_SPO2_RES *pstSpo2Res);
/****************************************************************
* Description: calculate Hr_value and SpO2_value by fifo int.(new api)
* Input:    stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
            usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
            emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
            pusRawdataOutLen : RawdataOut real length
* Output:   pstSpo2Res : spo2 result struct
            nRawdataOut : rawdata buf
            pusRawdataOutLen : RawdataOut real length
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
GS8 HBD_HrSpo2CalculateByFifoIntEx(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                                                GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen, ST_SPO2_RES *pstSpo2Res);

/****************************************************************
* Description: config adt confrim
* Input:  usAdtConfirmGsThrVal : gsensor confirm thr
          uchAdtConfirmGsCalcThrCntMax: gsensor confirm thr cnt max 
          uchAdtConfirmGsCalcThrCnt  : gsensor confirm thr cnt
* Return: None
******************************************************************/
void HBD_AdtConfirmConfig(GU16 usAdtConfirmGsThrVal, GU8 uchAdtConfirmGsCalcThrCntMax, GU8 uchAdtConfirmGsCalcThrCnt);

/****************************************************************
* Description: start adt confrim
* Input:  None
* Return: HBD_RET_OK=success, 
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_NO_INITED_ERROR=not inited,
******************************************************************/
GS8 HBD_AdtConfirmStart(void);

/****************************************************************
* Description: adt confirm detect by newdata int.
* Input:  stGsAxisValue: gsensor data
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:  None
* Return: wear flag, 0x00: calc ing 0x11:wear, 0x12:unwear
******************************************************************/
GU8 HBD_AdtConfirmCalculateByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity);

/****************************************************************
* Description: adt confirm detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   None
* Return: wear flag, 0x00: calc ing 0x11:wear, 0x12:unwear
******************************************************************/
GU8 HBD_AdtConfirmCalculateByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity);

/****************************************************************
* Description: adt confirm detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen��ptr of rawdata len output
* Return: wear flag, 0x00: calc ing 0x11:wear, 0x12:unwear
******************************************************************/
GU8 HBD_AdtConfirmCalculateByFifoIntDbgOutputData(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                                        GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen);

/*****************************************************************************************
* Description: set LED current
* Input:  fLed0Current:LED0 current
          fLed1Current:LED1 current
* Return: HBD_RET_PARAMETER_ERROR: out of range,and set current to the closest limit value
          HBD_RET_GENERIC_ERROR: can't set current in this mode
          HBD_RET_OK: set current success
******************************************************************************************/
GS8 HBD_SetLedCurrent(GF32 fLed0Current, GF32 fLed1Current);

/****************************************************************
* Description: get LED current
* output:  pLed0Current:ptr of LED0 current
          pLed1Current:ptr of LED1 current
* Return: HBD_RET_GENERIC_ERROR: can't get current in this mode
          HBD_RET_OK: get current success
******************************************************************/
GS8 HBD_GetLedCurrrent(GF32 *pLed0Current, GF32 *pLed1Current);

/****************************************************************
* Description: Start HBD mode for get rawdata
* Input:  GU16 usSampleRate : sample rate
*         GU8 ucEnableFifo : 1 enable fifo, 0 disable fifo
*         GU8 ucFifoTHR : Fifo int generate threshold
* Return: 0 success, others fail
******************************************************************/
GS8 HBD_StartHBDOnly(GU16 usSampleRate, GU8 ucEnableFifo, GU16 ucFifoTHR);

/****************************************************************
* Description: Get rawdata in fifo int
* Input:  GU8 ucBufLen : rawdata array length(frames), if ucBufLen < *pucRealDataLen value, data will be lost
* Output: GS32 nRawdataOut: ptr of rawdata array output
* Return: 0 OK, 1 soft autoled error
******************************************************************/
GU8 HBD_GetRawdataByFifoInt(GU16 ucBufLen, GS32 nRawdataOut[][2], GU16 *pucRealDataLen);

/****************************************************************
* Description: Get rawdata in fifo int ,do not auto led
* Input:  GU8 ucBufLen : rawdata array length(frames), if ucBufLen < *pucRealDataLen value, data will be lost
* Output: GS32 nRawdataOut: ptr of rawdata array output
* Return: 0 OK
******************************************************************/
GU8 HBD_GetRawdataByFifoIntWithoutAutoled(GU16 ucBufLen, GS32 nRawdataOut[][2], GU16 *pucRealDataLen);

/****************************************************************
* Description: Get rawdata in new data int
* Output: GU32 *ppg1 
*         GU32 *ppg2
* Return: 0 OK, 1 soft autoled error
******************************************************************/
GU8 HBD_GetRawdataByNewDataInt(GU32 *ppg1, GU32 *ppg2);

/*****************************************************************************************
* Description: let chip enter resume mode when has read fifo ,so must be called after HBD_GetRawdataByFifoInt or HBD_GetRawdataByNewDataInt
* Output: 
* Return: 
*****************************************************************************************/
void HBD_GetRawdataHasDone(void);

/****************************************************************
* Description: Use IR ppg to do a simple check wear state by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen��ptr of rawdata len output
* Return: wear flag, 0x00: calc ing 0x11:wear, 0x12:unwear
******************************************************************/
GU8 HBD_IRSimpleWearCheckByFifoIntDbgOutputData(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                                    GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen);







/* Hbd commhandler return value type. */
typedef enum 
{
    COMM_CMD_ALGO_IN_MCU_HB_START = 0,
    COMM_CMD_ALGO_IN_MCU_HB_STOP,
    COMM_CMD_ALGO_IN_APP_HB_START,
    COMM_CMD_ALGO_IN_APP_HB_STOP,
    COMM_CMD_ALGO_IN_MCU_HRV_START,
    COMM_CMD_ALGO_IN_MCU_HRV_STOP,
    COMM_CMD_ALGO_IN_APP_HRV_START,
    COMM_CMD_ALGO_IN_APP_HRV_STOP,
    COMM_CMD_ADT_SINGLE_MODE_START,
    COMM_CMD_ADT_SINGLE_MODE_STOP,
    COMM_CMD_ADT_CONTINUOUS_MODE_START,
    COMM_CMD_ADT_CONTINUOUS_MODE_STOP,
	COMM_CMD_ALGO_IN_MCU_SPO2_START,
    COMM_CMD_ALGO_IN_MCU_SPO2_STOP,
    COMM_CMD_ALGO_IN_APP_SPO2_START,
    COMM_CMD_ALGO_IN_APP_SPO2_STOP,
    COMM_CMD_ALGO_IN_MCU_BP_START,
    COMM_CMD_ALGO_IN_MCU_BP_STOP,
    COMM_CMD_INVALID,
} EM_COMM_CMD_TYPE;

/****************************************************************
* Description: parse communicate receive data
* Input:    uchCommInterFaceId: this id from HBD_SetSendDataFunc return,
            uchDataBuffArr: data buff ptr of receive data,
			uchLen: data len of receive data,
* Return: see EM_COMM_CMD_TYPE typedef
******************************************************************/
EM_COMM_CMD_TYPE HBD_CommParseHandler(GU8 uchCommInterFaceId, GU8 uchDataBuffArr[], GU8 uchLen);

/****************************************************************
* Description: Set send data function
* Input:    pSendDataFunc: send data func ptr,
* Return: val > 0:communication interface id,
          HBD_RET_RESOURCE_ERROR: have no resource,
******************************************************************/
GS8 HBD_SetSendDataFunc(void (*pSendDataFunc)(GU8 uchDataBuffArr[], GU8 uchLen));

/****************************************************************
* Description: send rawdata Package by newdata INT 
* Input:    stGsAxisValue: gsensor axis value
            emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Return: auto led state flag: 0: success, 1: error
******************************************************************/
GU8 HBD_SendRawdataPackageByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity);


/************************************************************************
*   Macro Definitions
************************************************************************/
#define ENABLE_LOG      0
#define LOG_LEVEL       0
#define DEBUG_LOG_ONLY  0

#if ENABLE_LOG
  #if (!DEBUG_LOG_ONLY)
  #define LOGI(format, ...) printf("[I]: " format "\n", ##__VA_ARGS__)
  #define LOGI2(format, ...) printf("[I2]: " format "\n", ##__VA_ARGS__)
  #else
  #define LOGI(format, ...)
  #define LOGI2(format, ...)
  #endif
  #if (0 < LOG_LEVEL && !DEBUG_LOG_ONLY)
  #define LOGW(format, ...) printf("[W]: " format "\n", ##__VA_ARGS__)
  #else
  #define LOGW(format, ...)
  #endif
  #if (1 < LOG_LEVEL || DEBUG_LOG_ONLY)
  #define LOGD(format, ...) printf("[D][%d]: " format "\n", __LINE__, ##__VA_ARGS__)
  #else
  #define LOGD(format, ...)
  #endif
  #define LOGC(format, ...) 
  #define LOGY(format, ...) printf("[I]: " format "\n", ##__VA_ARGS__)
#else
#define LOGI(format, ...)
#define LOGI2(format, ...)
#define LOGW(format, ...)
#define LOGD(format, ...)
#define LOGC(format, ...)
#define LOGY(format, ...) 
#endif

#define GH30X_RET_READ_FIFO_CONTINUE                (4)             /**< Fifo not read empty,should read it again */
#define GH30X_RET_RAW_DATA_PKG_OVER                 (3)             /**< already read all of rawdata package data */
#define GH30X_RET_RAW_DATA_PKG_CONTINUE             (2)             /**< there is rawdata package data left */
#define GH30X_RET_ALGO_HAS_REFRESHED                (1)             /**< algorithm has refreshed */
#define GH30X_RET_OK                                (0)             /**< there is no error */
#define GH30X_RET_GENERIC_ERROR                     (-1)            /**< a generic error happens */
#define GH30X_RET_PARAMETER_ERROR                   (-2)            /**< parameter error */
#define GH30X_RET_COMM_NOT_REGISTERED_ERROR         (-3)            /**< i2c/spi operation not registered error */
#define GH30X_RET_COMM_ERROR                        (-4)            /**< i2c/spi communication error */
#define GH30X_RET_RESOURCE_ERROR                    (-5)            /**< resource full or not available error */
#define GH30X_RET_NO_INITED_ERROR                   (-6)            /**< no inited error */
#define GH30X_RET_ALGO_NO_SUPPORTED                 (-7)            /**< the algorithm no supported error */

#define GH30X_SOFT_EVENT_WEAR_OFF                   (1 << 0)
#define GH30X_SOFT_EVENT_WEAR_ON                    (1 << 1)

#define GH30X_ADT_MODE_DET_WEAR_ON                  0
#define GH30X_ADT_MODE_DET_WEAR_OFF                 1

//NADT版本号
#define NADT_VER_MAJOR_NUM                          (2u)
#define NADT_VER_MINOR_NUM                          (0u)
#define NADT_VER_REVISION_NUM                       (12u)

//INADT版本号
#define INADT_VER_MAJOR_NUM                         (2u)
#define INADT_VER_MINOR_NUM                         (0u)
#define INADT_VER_REVISION_NUM                      (0u)

#define HB_SAMPLING_RATE_SETTING                    (25)  // sample rate set 25Hz
#define HRV_SAMPLING_RATE_SETTING                   (100) // sample rate set 100Hz
#define HSM_SAMPLING_RATE_SETTING                   (25)  // sample rate set 25Hz
#define BPF_SAMPLING_RATE_SETTING                   (200)  // sample rate set 200Hz
#define PFA_SAMPLING_RATE_SETTING                   (25)  // sample rate set 25Hz
#define SPO2_SAMPLING_RATE_SETTING                  (25) // sample rate set 25Hz

/* GH30x Cmd macro. */
#define HBD_CMD_SCAN                                (0xA1)
#define HBD_CMD_STOP                                (0xA2)
#define HBD_CMD_RESET                               (0xC2)
#define HBD_CMD_SLEEP                               (0xC4)
#define HBD_CMD_TWR_TRG                             (0xCA)
#define HBD_CMD_IDLE                                (0xC0)
#define HBD_CMD_ADT                                 (0xC1)
#define HBD_CMD_RESUME                              (0xC3)

/* GH30x Communicate interface macro */
/// i2c ctrl interface
#define HBD_I2C_DEVICE_ID_BASE                      (0x28)
#define HBD_I2C_CMD_ADDR                            (0xDDDD)
/// spi cs pin ctrl io state
#define HBD_SPI_CMD_WRITE                           (0xF0)
#define HBD_SPI_CMD_READ                            (0xF1)
#define HBD_SPI_CS_CTRL_HIGH                        (0x01)
#define HBD_SPI_CS_CTRL_LOW                         (0x00)
#define HBD_SPI_WAIT_DELAY_X_US                     (20) // 20us
#define HBD_SPI_END_DELAY_X_US                      (10) // 10us
/// send cmd delay
#define HBD_SEND_CMD_DELAY_X_US                     (500) // 500us
#define HBD_SEND_RST_CMD_DELAY_X_US                 (2000) // 2ms

#define HBD_COMMUNICATE_CHECK_MAX_CNT               (3) /// init communicate check max cnt
/// reg addr & value 
#define HBD_FIFO_REG_ADDR                           (0xAAAA)
/// id reg & default value 
#define HBD_ID0_REG_ADDR                            (0x0026)
#define HBD_ID1_REG_ADDR                            (0x0028)
#define HBD_GH3XX_ID0_VAL                           (0x02A0)
#define HBD_GH3XX_ID1_VAL                           (0x0031)
#define HBD_VER_REG_ADDR                            (0x00E0)
#define HBD_VER_VAL_GH300_DEFAULT                   (0x0021)
/// hbd config reg 
#define HBD_TIME_SEL0_REG_ADDR                      (0x0080)
#define HBD_TIME_SEL1_REG_ADDR                      (0x0082)
#define HBD_CONFIG_REG_ADDR                         (0x0084)
#define HBD_LED12_REG_ADDR                          (0x0118)
#define HBD_LED13_REG_ADDR                          (0x011A)
#define HBD_LED17_REG_ADDR                          (0x0122)
#define HBD_LED18_REG_ADDR                          (0x0124)
#define HBD_LED27_REG_ADDR                          (0x0136)
#define HBD_LED_MAXDRIVECUR_REG_ADDR                (0x0180)
#define HBD_AFE_REG_ADDR                            (0x0186)
/// rawdata reg list
#define HBD_LED0_DATA_L_REG_ADDR                    (0x0086)
#define HBD_LED0_DATA_H_REG_ADDR                    (0x0088)
#define HBD_LED1_DATA_L_REG_ADDR                    (0x008A)
#define HBD_LED1_DATA_H_REG_ADDR                    (0x008C)
#define HBD_LED2_DATA_L_REG_ADDR                    (0x008E)
#define HBD_LED2_DATA_H_REG_ADDR                    (0x0090)
/// sample rate reg 
#define HBD_SAMPLE_RATE_REG_ADDR                    (0x0016)
#define HBD_SAMPLE_RATE_CLOCK_FREQ                  (32768u)
/// fifo reg list
#define HBD_FIFO_EN_REG_ADDR                        (0x0048)
#define HBD_FIFO_THR_REG_ADDR                       (0x0044)
#define HBD_FIFO_DATA_CNT_ADDR                      (0x004A)
/// INT reg list & value
#define HBD_IRQ_CTRL_REG_ADDR                       (0x0002)
#define HBD_IRQ_STATUS_REG_ADDR	                    (0x0008)
#define HBD_IRQ_REG_IRQ_MSK                         (0x003F)
#define HBD_IRQ_REG_IRQ_UNMSK                       (0xFFE0)
#define HBD_IRQ_MSK_ADC_VALID                       (1 << 0)
#define HBD_IRQ_MSK_FIFO_THR_FULL                   (1 << 1)
#define HBD_IRQ_MSK_FIFO_FULL                       (1 << 2)
#define HBD_IRQ_MSK_AUTO_LED_FAIL                   (1 << 3)
#define HBD_IRQ_MSK_ADT_SUCCESS                     (1 << 4)
#define HBD_IRQ_MSK_RESET                           (1 << 5)
/// ADT reg list   
#define HBD_ADT_SAMPLE_RATE_REG_ADDR                (0x0018)
#define HBD_ADT_GINT_CTRL0_REG_ADDR                 (0x0020)
#define HBD_ADT_GINT_CTRL1_REG_ADDR                 (0x0022)
#define HBD_ADT_CTRL_REG_ADDR                       (0x00C0)
#define HBD_ADT_CTRL_COARSE_DET_BIT                 (0)
#define HBD_ADT_CTRL_MSK_COARSE_DET                 (1 << 0)
#define HBD_ADT_CTRL_FINE_DET_BIT                   (1)
#define HBD_ADT_CTRL_MSK_FINE_DET                   (1 << 1)
#define HBD_ADT_CTRL_RATE_EN_BIT                    (2)
#define HBD_ADT_CTRL_MSK_RATE_EN                    (1 << 2)
#define HBD_ADT_CTRL_UNWEAR_EN_BIT                  (3)
#define HBD_ADT_CTRL_MSK_UNWEAR_EN                  (1 << 3)
#define HBD_ADT_CTRL_MSK_UNWEAR_FLAG                (1 << 8)
#define HBD_ADT_THR3_REG_ADDR                       (0x00C8)

/*indedx of fifo thr config. */
#define HBD_FIFO_THR_CONFIG_HRV_INDEX               (0u)
#define HBD_FIFO_THR_CONFIG_HB_INDEX                (1u)
#define HBD_FIFO_THR_CONFIG_HSM_INDEX               (2u)
#define HBD_FIFO_THR_CONFIG_BPD_INDEX               (3u)
#define HBD_FIFO_THR_CONFIG_PFA_INDEX               (4u)
#define HBD_FIFO_THR_CONFIG_SPO2_INDEX              (5u)
#define HBD_FIFO_THR_CONFIG_NUM                     (6u)

/* Storage macro */
#define HBD_STORAGE_CONFIG_DATA_REG_MAX_NUM         (64u)
#define HBD_STORAGE_CONFIG_DATA_MAGIC               (0xF55F)
/*indedx of config arr. */
#define HBD_CONFIG_ARR_SERIAL_NUMBER_BYTE1_INDEX    (0u)
#define HBD_CONFIG_ARR_SERIAL_NUMBER_BYTE2_INDEX    (1u)
#define HBD_CONFIG_ARR_SERIAL_NUMBER_BYTE3_INDEX    (2u)
#define HBD_CONFIG_ARR_SERIAL_NUMBER_BYTE4_INDEX    (3u)
#define HBD_CONFIG_ARR_LENGTH_INDEX                 (4u)
#define HBD_CONFIG_ARR_DATA_INDEX                   (5u)
#define HBD_CONFIG_ARR_HEADER_LENGTH                (5u)

/* fifo macro */
#define HBD_FIFO_SATURATE_INC(v, limit)             do{(v)++; if ((v) >= (limit)){(v) = 0;}}while(0)
#define HBD_FIFO_FIXED_ADDITION_NUM                 (5)
#define HBD_READ_FIFO_BYTE_NUM                      (768) // chip fifo max num, max freq 100Hz
#define HBD_READ_FIFO_MAX_CNT                       (255) // HBD_READ_FIFO_BYTE_NUM / 3

/* wear state mcaro */
#define HBD_WEAR_STATE_CHECK_SUM                    (22)
#define HBD_WEAR_STATE_CHECK_THR_SUM                (18)
#define HBD_WEAR_STATE_CHECK_THR_MSK                (0x003FFFFFU)
#define HBD_WEAR_STATE_CHECK_MAX_COUNT              (250)

/* hba scenario mcaro */
#define   HBD_HBA_SCENARIO_CONFIG_DEFAULT_VAL       (0) // fixed to 0
#define   HBD_HBA_SCENARIO_CONFIG_MAX_VAL           (24)

/* auto led mcaro */
#define HBD_AUTO_LED_MAX_CHANNEL_NUM                (2)
#define HBD_AUTO_LED_CHECK_MAX_SUM                  (40) 

#define HBD_AUTO_LED_DC_OK                          (0)
#define HBD_AUTO_LED_DC_CALC_G_MINUS                (1<<0)
#define HBD_AUTO_LED_DC_CALC_G_PLUS                 (1<<1)
#define HBD_AUTO_LED_DC_FAIL                        (1<<7)

#define HBD_AUTO_LED_COMPATIBLE_NONE                (0)
#define HBD_AUTO_LED_COMPATIBLE_CASE1               (1<<0)
#define HBD_AUTO_LED_COMPATIBLE_CASE2               (1<<1)
#define HBD_AUTO_LED_COMPATIBLE_FAIL                (1<<7)

#define HBD_AUTOLED_MOTION_LEVEL_0                  (1<<0)
#define HBD_AUTOLED_MOTION_LEVEL_1                  (1<<1)
#define HBD_AUTOLED_MOTION_LEVEL_2                  (1<<2)
#define HBD_AUTOLED_MOTION_LEVEL_3                  (1<<3)
#define HBD_AUTOLED_MOTION_LEVEL_4                  (1<<4)
#define HBD_AUTOLED_MOTION_LEVEL_5                  (1<<5)

#define HBD_AUTOLED_BG_NORMAL                       (1<<0) // 1
#define HBD_AUTOLED_BG_LEVEL34                      (1<<1) // 2
#define HBD_AUTOLED_BG_LEVEL4                       (1<<2) // 4
#define HBD_AUTOLED_BG_WEAKLIGHT                    (1<<3) // 8
#define HBD_AUTOLED_BG_HIGHLIGHT                    (1<<4) // 16

#define HBD_AUTOLED_ERROR_CURRENT_RAWDATA           (0x1) // CURRENT IS 0

/// reg don't check macro
#define HBD_REG_DONT_NEED_CHECK_BIT                 (0x1000) /**< bit 12 use to indicate reg need check or not. */
#define HBD_GET_REG_REAL_ADRR(x)                    (x & 0x0FFF) /**< reg addr[11:0] valid. */

/* mem use falg. */
#define NO_MEM_USE_CMD                              0
#define HB_MEM_USE_CMD                              1
#define SPO2_MEM_USE_CMD                            2
#define HRV_MEM_USE_CMD                             3
#define HB_HRV_MEM_USE_CMD                          4

/* virtual addr. */
#define COMM_ALGORITHM_VIRTUAL_ADDR                 (0x2000)

/* utils macro */
#define HBD_PTR_NULL                                ((void *) 0)
#define HBD_DELAY_ONE_US_CNT                        (9u)

#define HBD_GET_HIGH_BYTE_FROM_WORD(sValue)         ((GU8)(((sValue) >> 8) & 0xFF))
#define HBD_GET_LOW_BYTE_FROM_WORD(sValue)          ((GU8)((sValue) & 0xFF))

#define HBD_GET_BYTE3_FROM_DWORD(unValue)           ((GU8)((unValue >> 24) & 0x000000FFU))
#define HBD_GET_BYTE2_FROM_DWORD(unValue)           ((GU8)((unValue >> 16) & 0x000000FFU))
#define HBD_GET_BYTE1_FROM_DWORD(unValue)           ((GU8)((unValue >> 8) & 0x000000FFU))
#define HBD_GET_BYTE0_FROM_DWORD(unValue)           ((GU8)((unValue ) & 0x000000FFU))

#define HBD_MAKEUP_WORD(uchHighByte, uchLowByte)                  (((((GU16)uchHighByte) << 8)& 0xFF00) | (((GU16)uchLowByte)& 0x00FF))
#define HBD_MAKEUP_DWORD(uchByte3, uchByte2, uchByte1, uchByte0)  (((((GU32)uchByte3) << 24) & 0xFF000000U) | ((((GU32)uchByte2) << 16) & 0x00FF0000U) |\
                                                                    ((((GU32)uchByte1) << 8) & 0x0000FF00U) |(((GU32)uchByte0) & 0x000000FFU))
#define HBD_MAKEUP_DWORD2(usHighWord, usLowWord)                  (((((GU32)usHighWord) << 16) & 0xFFFF0000U) | (((GU32)usLowWord) & 0x0000FFFFU))

#define NADT_MATH_ROUND(v)                          (v >= 0) ? (v + 0.5f) : (v - 0.5f)
#define NADT_MATH_ROUND1(v)                         round(v)
#define NADT_MATH_ABSF(v)                           fabs(v)

/************************************************************************
*   Type Definitions
************************************************************************/

/* Analog config reg list. */
typedef struct
{
    GU16 usChannelMapConfigRegVal;     // reg 0x84
    GU16 usLed01CurrentConfigRegVal;   // reg 0x118
    GU16 usGainConfigRegVal;           // reg 0x136
    GU16 usTimeSel0RegVal;             // reg 0x80
    GU16 usTimeSel1RegVal;             // reg 0x82
    GU16 usHbdAfeRegVal;               // reg 0x186
} ST_ANALOG_REG_LIST;

/* fifo thr config . */
typedef struct
{
    GU8 uchFifoThrCntConfigByFunc;    // Fifo threshold set by function
    GU16 usFifoThrCntConfigVal;       // Fifo threshold cnt
} ST_FIFO_THR_CONFIG;

/* soft autoled struct. */
typedef struct
{
    ST_AUTO_LED_CH_CONFIG stAutoLedChannel[2];
    GU8 uchBgStrongLightThr;      //Default 5
    GU8 uchBgWeakStrongLightThr;  //Default 4
    GU8 uchBgUpperThr;
    GU8 uchgLowerThr;
} ST_AUTO_LED_CONFIG;

/* soft autoled rawdata buffer struct. */
typedef struct
{
	GU32 unRawdataBuffer[HBD_AUTO_LED_CHECK_MAX_SUM]; //Rawdata
    GU16 unBufferindex;                               //Index
} ST_AUTO_LED_RAWDATA;

/* soft autoled calc gsensor std */
typedef struct 
{
    GF32 fStd; /* std */
    GF32 fAvg; /* avg */
    GU32 uMax; /* max */
    GU32 uMin; /* min */
    GU32 uDif; /* diff */
} ST_RECURSION_CALC_STD;

typedef struct 
{
    GF32 fAvg; /* avg */
    GU32 uMax; /* max */
    GU32 uMin; /* min */
    GU32 uDif; /* diff */
} ST_RECURSION_CALC_NOSTD;

typedef struct 
{
    GU16 Sat_CheckCnt;  /* num of data checked*/
    GU8 Sat_cnt[HBD_AUTO_LED_MAX_CHANNEL_NUM]; /* sat num of each channel*/
    GU8 Sat_flag; /* sat flag*/
    GU8 Sat_adj_flag; /* sat adj flag*/
} ST_AUTO_LED_Sat;

/* soft autoled motion detection thr sturct */
typedef struct
{
    GU16 uGsensorNormal;    /* different Gsensor Normal to 512,default 512 */
    GU16 uDailyACCStdThr;   /* default 200, <uDailyACCStdThr, small ACC disturbance*/
    GU16 uWalkACCStdThr;    /* (uDailyACCStdThr uWalkACCStdThr],default 450, walk or jog */
    GU16 uRestACCDiffThr;   /* Resting state,default 0.4, uRestACCDiffThr*uGsensorNormal */
    GU16 uTrotACCAvgThr;    /* [uTrotACCAvgThr unlimit), default 1.4, 1.4*uGsensorNormal,jog --> trot */
    GU16 uDashACCMaxThr;    /* [uDashACCMaxThr unlimit), default 5, 5*uGsensorNormal */
}ST_MOTION_JUDGE_THR;

typedef struct
{
    GU16 uchCnt;            /* Trigger Counter,change mode*/
    GU8 uchLevel;           /* Trigger Level */
    GU8 uchCntThr;          /* Trigger change mode threshold, Default 5s */
}ST_MOTION_PROC_FLAG;

/* soft autoled ideal target judge & threshold sturct */
typedef struct
{
    GU8 uchFirstTrigger;     /* First Adjust */
    GU16 uchTriggerCnt;       /* Trigger counter */
    GU8 uchLastIdealTrigger; /* Last time Flag */
} ST_IDEAL_ADJUST_CH;

typedef struct
{
    ST_IDEAL_ADJUST_CH stIdealAdjustCh[2]; /* Channel flag */
    GU8 uchTriggerDuration;  /* Trigger Duration, default 7s, pulse 9-25/min,-->7s */
}ST_IDEAL_ADJUST;

/* soft autoled ideal target judge & threshold sturct */
typedef struct
{
    GU16 uchBGLevelCnt; /* stable time to adjust,unit s */
    GU8 uchLastBGLevel; /* Last time Ideal Flag */
    GU8 uchUpperlevel; /*0-Normal 1-49000 2-45000 3-30000 */
} ST_BG_ADJUST_CH;

typedef struct
{
    ST_BG_ADJUST_CH stBGAdjCh[2];  /* channel Info */
    GU8 uchBGReduceGainDuration;   /* Trigger Duration, default 7s, pulse 9-25/min,-->7s */
    GU8 uchBGRecoverGainDuration;  /* Trigger Duration, default 7s, pulse 9-25/min,-->7s */
    GU8 uchTurnOnNotReturnFail;    /* If enable, will not return autoled fail at high light */
    GU8 uchTakeoffFactorAtBGhigher;/* At high BG level, takeoff threshold multi factor */
}ST_BG_ADJUST;

/* Target adjusting threshold struct */
//typedef struct 
//{
//    GF32 fTriggerIdealAdjustRatio; /* trigger ideal adjusting ratio in motion */
//    GU32 luIdealTarget;            /* Ideal target in motion */
//}ST_RAWDATA_ADJUST_THR; //Motion proc

/* Sample information */
typedef struct 
{
    GU16 unSampleFre;          //sample frequency
    GU16 unSampleTim;          //Time of per frame
    GU16 unSampleFifoDuration; //Fifo time
    GU16 unAutoLedCalcLen;     //Frames number of AutoLed 
}ST_SAMPLE_INFO;

typedef struct 
{
    GU16 unWrtRegThres;        //Threshold
    GU16 unWrtRegNormDuration[2]; //channel counter
}ST_AUTOLED_WRITE_REG_CHECK;

typedef struct 
{
    GF32 fMaxRatio;
    GF32 fMinRatio;
}ST_RAWDATA_AVG_IN_MOTIONIN;

/* config mode enum. */
enum EM_CONFIG_MODE
{
    CONFIG_ARR_FROM_LIB_INIT_LOAD = 0,
    CONFIG_ARR_FROM_COMPILE_LOAD,
    CONFIG_ARR_FROM_RUN_LOAD,
    CONFIG_ARR_MODE_INVALID,
};

/* chip id enum. */
enum EM_CHIP_ID
{
    CHIP_ID_GH300_3A = 0x01,
    CHIP_ID_GH300_CREE = 0x02,
    CHIP_ID_GH301_3A = 0x04,
    CHIP_ID_GH301_CREE = 0x05,
};

/* init status flag enum. */ 
enum EM_HBD_INIT_STATUS
{
    HBD_INIT_STATUS_NO_INIT = 0,
    HBD_INIT_STATUS_INITED,
    HBD_INIT_STATUS_INVALID,
};

/* start status flag enum. */
enum EM_HBD_START_STATUS
{
    HBD_START_STATUS_OFF = 0,
    HBD_START_STATUS_HB_MODE,
    HBD_START_STATUS_HRV_MODE,
    HBD_START_STATUS_HSM_MODE,
    HBD_START_STATUS_BPF_MODE,
    HBD_START_STATUS_PFA_MODE,
    HBD_START_STATUS_BAND_WEAR_CONFIRM_MODE,
    HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE,
    HBD_START_STATUS_SPO2_MODE,
    HBD_START_STATUS_GET_DATA_MODE,
    HBD_START_STATUS_ADT_WEAR_MODE, 
    HBD_START_STATUS_ADT_UNWEAR_MODE,    
    HBD_START_STATUS_INVALID,
};

/* device type enum. */
enum  EM_DEVICE_TYPE
{
    DEVICE_BAND = 2,
    DEVICE_EP = 3,
    DEVICE_FINGER = 4,
    DEVICE_INVALID,
};

typedef struct
{
    GU8 uchFifoMode;
    GU8 uchU8Mode;
    GS32 nRawDataArr[10];
    
    GU16 usRawdataFifoCnt;
    GU16 usIndex;
    GF32 fIndexInc;
    GS16 sGsensorRightShift;
    ST_GS_DATA_TYPE *stGsAxisValueArr;
    GU16 usGsDataNum;
    
    void *pnRawdataOut;
    GU16 *pusRawdataOutLen;
    GU16 usDbgRawdataMaxLen;
    GU8  uchDbgPkgLen;
    
    GU8 *puchSpo2Value;
    GU8 *puchSpo2ConfidentLvl;
    GU16 *pusSpo2RVal;
    GS32 *pliSpo2InvalidFlag;
    GU8 *puchValidLvl;
    GU8 *puchHbValue;
    GU8 *puchHbConfidentLvl;
    GU16 *pusHrvRRVal1;
    GU16 *pusHrvRRVal2;
    GU16 *pusHrvRRVal3;
    GU16 *pusHrvRRVal4;
    GU8 *puchHrvcnt;
    GU8 *puchHrvConfidentLvl;
} ST_SPO2_CAL_PARAM;

#define HOOK_FUNC_CALL(func, argv) do {                     \
                                        if ((func) != 0)    \
                                        {                   \
                                            func##argv;     \
                                        }                   \
                                      } while (0)

/* function ptr typedef */
typedef GU8 (*pfnI2cWrite)(GU8 uchDeviceId, const GU8 uchWriteBytesArr[], GU16 usWriteLen); // i2c write function typde
typedef GU8 (*pfnI2cRead)(GU8 uchDeviceId, const GU8 uchCmddBytesArr[], GU16 usCmddLen, GU8 uchReadBytesArr[], GU16 usMaxReadLen); // i2c read function typde
typedef void (*pfnDelayUs)(GU16 usUsec); // DelayUs function typde

/* algo function ptr typedef */
typedef GS32 (*pfnAlgoMemoryInitHookFunc)(GS32 * error_code);
typedef void (*pfnAlgoMemoryDeinitHookFunc)(void);
typedef void (*pfnAlgoInitHookFunc)(EMFunctionID function_id,GU8 buffer_len,GS32 *algo_param_buffer);
typedef void (*pfnAlgoCalculateHookFunc)(EMFunctionID function_id,ST_ALGO_CALCULATE_INFO *algo_calc_info,ST_ALGO_RESULT *algo_result);
typedef void (*pfnAlgoDeinitHookFunc)(EMFunctionID function_id);

/* functions declaration. */
/* from ctrl lib. */
extern void HBD_I2cSendCmd(GU8 uchCmd);
extern void HBD_I2cWriteReg(GU16 usAddr, GU16 usValue);
extern GU16 HBD_I2cReadReg(GU16 usAddr);
extern GU32 HBD_I2cReadRawdataReg(GU16 usAddr);
extern void HBD_I2cReadFifo(GU8 *puchDataBuffer, GU16 usLen);
extern void HBD_DelayUs(GU16 usUsCnt);
extern void HBD_ChangeToHbModeRegVal(void);
extern void HBD_StoreHbModeRegVal(void);
extern void HBD_ConfigArrTranslate(GU8 uchDataBuffArr[], GU16 usLen);
extern GS8 HBD_LoadConfigWithMode(GU8 uchConfigArr[], GU8 uchStoreMode, GU8 uchForceUpdateConfig);
extern GS8 HBD_AutoLedReset(GU8 uchMode);
extern GU8 HBD_AutoLedCalcTriger(GU32 unRawdata[2], GU8 uchLen, GU32 *punRawdataAvgArr);
extern GU8 HBD_AutoLedGetCurrentAndGain(GU8 *puchGainArr, GU8 *puchGainShiftArr, GU16 *pusLastGainMsk, 
                          GU8 *puchCurrentArr, GU8 *puchCurrentShift, GU32 *punLastCurrentMsk);
extern GU8 HBD_AutoLedCalcGainCurrent(GU8 uchFlag, GU8 uchGainArr[2], GU8 uchGainShiftArr[2], GU16 usLastGainMsk, 
                                            GU8 uchCurrentArr[2], GU8 uchCurrentShift[2], GU32 unLastCurrentMsk, GU32 unRawdataAvgArr[2]);
extern GU8 HBD_AutoLedbyNewDataInt(GU32 unRawdata[2]);
extern void HBD_SendRawdataPackageDbg(GU32 unCh1Ppg, GU32 unCh2Ppg, GS16 sAccX,GS16 sAccY, GS16 sAccZ, 
                               GU8 uchCh0Current, GU8 uchCh1Current, GU8 uchCh2Current, GU8 uchIndex);

extern void HBAEnableWearingDetect (GU8 ubEnableWearingDetect);
extern void HBAConfigGsensor(GF32 fDirectionArr[3], GU8 ubResolutionLeftShiftBits, GU8 ubScalePercentageArr[3]);
extern GU8* MemStackPush(GU32 ulSize, GU32 ulAlign);
extern void MemStackPop(void* pBuf);

extern void HSMInit(void);
extern void HSMDeInit(void);
extern GU8 HSMCalculate(GS32 siPacketInfoArr[], GS32 fHrvOutArr[]);
extern void HSMGetSleepStaInfo(GS16 sSleepStatus[], GS32 lDataLen, GS32 fallsleepTime, GS32 outSleepTime, GS32 lSleepInfo[]);
extern const GU8 *BpVersion(void);
extern GU8 BpInit(GU16 nFs);
extern void BpDeInit(void);
extern GU8 BpCalculate(GS32 nRawdataArr[6], GS32 nResult[34]);
extern const GU8 *AFVersion(void);
extern GU8 AFInit(void);
extern GU8 AFDeInit(void);
extern GU8 AF_Detection_Module(GU16 nRRIArr[6], GU32 nResult[3]);
extern void PFAInit(void);
extern void PFADeInit(void);
extern GU8 PFACalculate(GS32 siPacketInfoArr[], GS32 fHrvOutArr[]);
//extern void HBAStopLostPackageProcess(void);
//extern void HBAControl(GS32 lOptTye, GS32 lConfigValue[]);
//extern void NADTControl(GS32 lOptTye, GS32 lConfigValue[]);
extern GU8 ADTCalculate(GS32 lRawData[], GS32 lAdtInfo[]);
//extern GS32 HBAControlGetNonFastLastBpmOutTime(void);
//extern void SpO2DeInit(void);
// extern void GmAlgMemDestroy(void);
// extern GS32 GmAlgMemInit(void* pMemAddr, GS32 nSize);
// extern void* GmAlgMemMalloc(GU32 unSize);
// extern void  GmAlgMemFree(void* pmemaddr);
//extern GS32 goodix_mem_init(void* mem_addr, GS32 size);
//extern void goodix_mem_deinit(void);

/* variables declaration from ctrl lib. */
extern GU16 g_usIrqPluseWidth; 
extern EM_HBD_FUNCTIONAL_STATE g_emHbFifoEnableFlag;
extern EM_HBD_FUNCTIONAL_STATE g_emHrvFifoEnableFlag;
extern EM_HBD_FUNCTIONAL_STATE g_emBpfFifoEnableFlag;
extern EM_HBD_FUNCTIONAL_STATE g_emSpo2FifoEnableFlag;

extern GU16 g_usReadRawdataRegList[2];
extern GU8 g_uchNeedReadRawdataCount;
extern GU8 g_uchPackageSN; 
extern GU8 g_uchHbdStarted;
extern GU8 g_uchHbdInited;
extern GU8 g_uchI2cDeviceId;
//extern ST_ANALOG_REG_LIST g_stHbRegList;
//extern ST_ANALOG_REG_LIST g_stAdtRegList;
extern GU8 g_uchAdtHasStarted;
extern GU8 g_chWearIsEnableFlag;
//extern GU8 g_uchHbaScenario;
//extern ST_FIFO_THR_CONFIG g_stFifoThrConfigList[HBD_FIFO_THR_CONFIG_NUM];
extern GU8 g_uchHbaCalcFastOutput;
extern ST_AUTO_LED_CONFIG g_stAutoLedConfig;
extern GU8 g_uchKeepLedWorking;
extern GU8 g_uchHbaTestMode;
extern GU8 g_uchHbaScenario;

#define GH30X_VAL_CLEAR_BIT(x, b)        ((x) &= (~(b)))
#define GH30X_VAL_SET_BIT(x, b)          ((x) |= (b))
#define GH30X_VAL_GET_BIT(x, b)          ((x) &= (b))

#define CONFIT_SIZE(config_array) (config_array##_len)

#define GH30X_ALGO_RESULT_MAX_NUM   8

#define CHNL_MAP_TO_FIFO_PHASE0_DATA  0x00      /** map to fifo's phase0 data (fifo first data is phase0 data) */
#define CHNL_MAP_TO_FIFO_PHASE1_DATA  0x01      /** map to fifo's phase1 data  */

#define RESET_CFG_SAMPLE_RATE_EN          (((GU16)1) << 0)
#define RESET_CFG_INT_TIME_EN             (((GU16)1) << 1)
#define RESET_CFG_CHNL0_AGC_FORCE_DIS     (((GU16)1) << 2)
#define RESET_CFG_CHNL1_AGC_FORCE_DIS     (((GU16)1) << 3)
#define RESET_CFG_CHNL0_DEFAULT_CUREET_EN (((GU16)1) << 4)
#define RESET_CFG_CHNL1_DEFAULT_CUREET_EN (((GU16)1) << 5)
#define RESET_CFG_CHNL0_TIA_GAIN_EN       (((GU16)1) << 6)
#define RESET_CFG_CHNL1_TIA_GAIN_EN       (((GU16)1) << 7)
#define RESET_CFG_FIFO_WATER_LINE_EN      (((GU16)1) << 8)

typedef struct
{
    GU8  uchLogicLedIndex;   //range: 0~2   0xFF means this channle do not exist
    GU8  uchLedColor;        //range:0~2    0xFF means this channle do not exist
    GU8  uchDrvCurrentCode;
    GU8  uchDrvCurrentChangeFlag;   //0: no change   1: change
    GU8  uchGain;
    GU8  uchGainChangeFlag;
} STGh30xChnlInfo;

typedef struct
{
    GU16 usResetEn;
    GU16 usResetSampleRate;
    GU8 uchResetIntTimeIndex;
    GU8 uchResetDrvCurrentChnl0;
    GU8 uchResetDrvCurrentChnl1;
    GU8 uchResetTiaGainChnl0;
    GU8 uchResetTiaGainChnl1;
    GU16 uchResetFifoWaterLine;
}stSampleResetCfg;

typedef struct
{
    GU8  uchChnlNum;
    GU8  uchDivder;                  
    GU16 usOutputDataRate;        
    GU8  uchDivderCnt;
    GU8  uchFrameFlag;
} STGh30xFunctionInfo;

typedef struct
{
    GU8 uchUpdateFlag;
    GU8 uchResultNum;
    GU16 usResultBit;
    GS32 snResult[GH30X_ALGO_RESULT_MAX_NUM];
} STGh30xAlgoResult;

typedef struct
{
    GU32 unFunctionID ;
    GU32 *punFrameCnt;
    GU32 *punRawdata;
    GS16 *pusGsensordata;
    GU8  *pchDrvCurrentCode;
    STGh30xFunctionInfo *pstFunctionInfo;
    GU8  *puchChnlMap;
    STGh30xAlgoResult * pstAlgoResult;
    GU8  uchFuntionChnlLimit;   //max chnl num
} STGh30xFrameInfo;

extern const STGh30xFrameInfo *const g_pstGh30xFrameInfo[];

#define GH30X_PROTOCOL_SEND_ALGO_RESULT_BYTE_NUM 21

extern GU32 g_unGh30xProtocolSendDataFunc;  //0: disable  >0: GH30X_FUNCTION_HR / GH30X_FUNCTION_HRV / GH30X_FUNCTION_SPO2 
extern GU8 g_uchGh30xProtocolSendAlgoResult[];

#define GH30X_PTR_NULL ((void *) 0)

typedef void (*pfnNormalHookFunc)(void);                                                 /**< normal hook type */
typedef void (*pfnGetRawdataHookFunc)(GU32 *punDataBuffer, GU16 usLen);                  /**< get rawdata hook type */
typedef void (*pfnGetIoStructInfo)(const STGh30xFrameInfo * const pstFrameInfo);

extern GU8 g_uchGh30xLastCmd;   //real chip cmd
extern volatile GU8 g_uchNewIntFlag; 
extern STGh30xChnlInfo g_pstGh30xChnlInfo[];
extern GU16 g_usGh30xAdtDownThrdRegValue;
extern GU8  g_uchGh30xLivingObjectFlag;
extern GU8  g_uchGh30xSoftEvent;

extern void Gh30xGetAdtDownThrdRegValue(void);
extern GS8 Gh30xHBD_SampleStart (void);
extern GU16 Gh30xGetFifoRawdata(GU8 *puchRawdataFifo, GU8 *uchNeedForceReadAgain);
void Gh30xSampleRateSet(GU16 usSampleRate);
void Gh30xIntTimeSet(GU8 uchIntTimeRegisterValue);
void Gh30xSoftAgcSet(GU8 uchChnlId, EM_HBD_FUNCTIONAL_STATE emState);
void Gh30xHardAgcDisable(GU8 uchChnlId);
void Gh30xSetDefaultDrvCurrent(GU8 uchChnlId, GU8 uchDrvCurrentCode);
GU8 Gh30xGetDrvCurrent(GU8 uchChnlId);
void Gh30xSetDefaultGain(GU8 uchChnlId, GU8 uchGainCode);
GU8 Gh30xGetGain(GU8 uchChnlId);
void Gh30xSetFifoWaterline(GU16 usWaterline);
void Gh30xInitChnlDrvAndGainInfo(void);
void Gh30xUpdateChnlDrvInfo(void);
void Gh30xUpdateChnlGainInfo(GU8 uchChnlId, GU8 uchFifoHighByte);
void Gh30xInitHookFunc(void);
void Gh30xGetRawdataHookFunc(GU32 *read_buffer_ptr, GU16 length);
void Gh30xFrameDataHookFunc(const STGh30xFrameInfo * const pstFrameInfo);

/// dbg output rawdata macro
#if (__ALGO_CALC_WITH_DBG_DATA__)
    #define __DBG_OUTPUT_RAWDATA__          (1)
#else
    #define __DBG_OUTPUT_RAWDATA__          (0)
#endif

/// define all dbg mcu mode api
#if (__USE_GOODIX_APP__)
#define BLE_MODULE_SEND_HB(val)           
#define BLE_MODULE_SEND_RRI(val, cnt)                                   
#define SEND_GH30X_RESET_EVT()                                          comm_send_app_cmd_reset(ble_module_send_pkg)
#define SEND_AUTOLED_FAIL_EVT()                                         comm_send_app_cmd_auto_led_fail(ble_module_send_pkg)
#define SEND_MCU_HB_MODE_WEAR_STATUS(sta, data, len)                    send_mcu_hb_mode_wear_status_pkg(sta, data, len)
#ifdef __HBD_API_EX__
#define SEND_MCU_HB_MODE_RESULT(hb, lv, sta, wq, vb, ret, rr, current, data, len, scenario, snr, motion, sleepflag)    \
                                                                        send_mcu_hb_mode_result_pkg(hb, lv, sta, wq, vb, ret, rr, current, data, len, scenario, snr, motion, sleepflag)
#else
#define SEND_MCU_HB_MODE_RESULT(hb, lv, sta, wq, vb, ret, rr, current, data, len)    \
                                                                        send_mcu_hb_mode_result_pkg(hb, lv, sta, wq, vb, ret, rr, current, data, len)
#endif
#define SEND_MCU_HRV_MODE_RESULT(rr_arr, cnt, lvl, hb, current, data, len)       send_mcu_hrv_mode_result_pkg(rr_arr, cnt, lvl, hb, current, data, len)
#define SEND_MCU_SPO2_UNWEAR_EVT(data, len)                             send_mcu_spo2_mode_unwear_pkg(data, len)
#ifdef __HBD_API_EX__
#define SEND_MCU_SPO2_MODE_RESULT(pstSpo2Res, abnsta, current, data, len)       send_mcu_spo2_mode_result_pkg(pstSpo2Res, abnsta, current, data, len)
#else
#define SEND_MCU_SPO2_MODE_RESULT(spo2, spo2_lv, hb, hb_lv, rr, rr_lv, rr_c, spo2_r, sta, vd, abnsta, current, data, len)     \
                                                                        send_mcu_spo2_mode_result_pkg(spo2, spo2_lv, hb, hb_lv, rr, rr_lv, rr_c, spo2_r, sta, vd, abnsta, current, data, len)
#endif
#define SEND_MCU_BPF_MODE_RESULT(bpf_res, current, data, len)           send_mcu_bpf_mode_result_pkg(bpf_res, current, data, len)

#else // (__USE_GOODIX_APP__)

#define BLE_MODULE_SEND_HB(val)                                         ble_module_send_heartrate(val)
#define BLE_MODULE_SEND_RRI(val, cnt)                                   do { ble_module_add_rr_interval(val, cnt); ble_module_send_heartrate(0); } while (0)
#define SEND_GH30X_RESET_EVT()
#define SEND_AUTOLED_FAIL_EVT()
#define SEND_MCU_HB_MODE_WEAR_STATUS(sta, data, len)
#ifdef __HBD_API_EX__
#define SEND_MCU_HB_MODE_RESULT(hb, lv, sta, wq, vb, ret, rr, current, data, len, scenario, snr, motion, sleepflag)
#else
#define SEND_MCU_HB_MODE_RESULT(hb, lv, sta, wq, vb, ret, rr, current, data, len)
#endif
#define SEND_MCU_HRV_MODE_RESULT(rr_arr, cnt, lvl, hb, current, data, len)
#define SEND_MCU_SPO2_UNWEAR_EVT(data, len)
#ifdef __HBD_API_EX__
#define SEND_MCU_SPO2_MODE_RESULT(pstSpo2Res, abnsta, current, data, len)
#else
#define SEND_MCU_SPO2_MODE_RESULT(spo2, spo2_lv, hb, hb_lv, rr, rr_lv, rr_c, spo2_r, sta, vd, abnsta, current, data, len)
#endif
#define SEND_MCU_BPF_MODE_RESULT(bpf_res, current, data, len)
#endif

/// define all fifo int timeout timer api
#if (__FIFO_INT_TIMEOUT_CHECK__)
#define HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_INIT()                         hal_gh30x_fifo_int_timeout_timer_init()
#define HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START()                        hal_gh30x_fifo_int_timeout_timer_start()
#define HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP()                         hal_gh30x_fifo_int_timeout_timer_stop()
#else
#define HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_INIT()                         
#define HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START()                      
#define HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP() 
#endif

/// ok and err val for some api ret
#define GH30X_EXAMPLE_OK_VAL                (1)
#define GH30X_EXAMPLE_ERR_VAL               (0)

/// autoled error val
#define GH30X_AUTOLED_ERR_VAL               (1)

#define LED_COLOR_GREEN                     (0)
#define LED_COLOR_RED                       (1)
#define LED_COLOR_IR                        (2)
#define LED_COLOR_INVALID                   (0xFF)

/// interface type 
#define GH30X_COMMUNICATION_INTERFACE_I2C   (1)
#define GH30X_COMMUNICATION_INTERFACE_SPI   (2)

/// autoled bg thr default
#define AUTOLED_BG_THR_MAX_DEFAULT          (6)
#define AUTOLED_BG_THR_MIN_DEFAULT          (2)

/// run mode
#define RUN_MODE_INVALID                    (0)
#define RUN_MODE_ADT_HB_DET                 (2)
#define RUN_MODE_HRV_DET                    (3)
#define RUN_MODE_GETRAWDATA_DET             (5)
#define RUN_MODE_SPO2_DET                   (7)
#define RUN_MODE_HARDWARE_ADT_DET           (8)

#define RUN_MODE_HR_SPO2_DET                (10)
#define RUN_MODE_FACTORY_DET                (11)

/// wear status
#define WEAR_STATUS_DETECTING               (0)
#define WEAR_STATUS_WEAR                    (1)
#define WEAR_STATUS_UNWEAR                  (2)
#define WEAR_STATUS_ALMOST_UNWEAR           (3)

/// adt confirm status
#define ADT_CONFRIM_STATUS_DETECTING        (0)
#define ADT_CONFRIM_STATUS_WEAR             (0x11)
#define ADT_CONFRIM_STATUS_UNWEAR           (0x12)

// hb algorithm scenario macro
#define HBA_SCENARIO_DEFAULT                (0)
#define HBA_SCENARIO_ROUTINE                (1)
#define HBA_SCENARIO_RUN                    (2)
#define HBA_SCENARIO_CLIBM                  (3)
#define HBA_SCENARIO_BIKE                   (4)
#define HBA_SCENARIO_IRREGULAR              (5)

/// fifo thr config type 
#define FIFO_THR_CONFIG_TYPE_HRV            (0)
#define FIFO_THR_CONFIG_TYPE_HB             (1)
#define FIFO_THR_CONFIG_TYPE_BPF            (3)
#define FIFO_THR_CONFIG_TYPE_SPO2           (5)

/// gsensor soft fifo buffer len, should >FIFO_THR_CNT_CONFIG
#define GSENSOR_SOFT_FIFO_BUFFER_MAX_LEN    (70)
/// generate fake gsensor data for test
#define USE_FAKE_GS_DATA_FOR_TEST           (0)

#define FAKE_GSENSOR_DATA_EQUE_0            (1)

/// mutli pkg magic byte
#define MUTLI_PKG_HEADER_LEN                (4)
#define MUTLI_PKG_MAGIC_0_VAL               (0x47)
#define MUTLI_PKG_MAGIC_1_VAL               (0x44)
#define MUTLI_PKG_MAGIC_2_VAL               (0x0A)
#define MUTLI_PKG_CMD_VERIFY(cmd)           ((cmd & 0xF0) == 0x80)
#define MUTLI_PKG_UART_MAX_LEN              (20)

/// mcu pkg 
#define MCU_PKG_HB_TYPE_VAL                 (0)
#define MCU_PKG_HRV_TYPE_VAL                (1)
#define MCU_PKG_BPF_TYPE_VAL                (3)
#define MCU_PKG_SPO2_TYPE_VAL               (5)

#define MCU_PKG_HB_ALGO_RESULT_LEN          (15)
#define MCU_PKG_HRV_ALGO_RESULT_LEN         (14)
#define MCU_PKG_SPO2_WEAR_RESULT_LEN        (21)
#define MCU_PKG_SPO2_ALGO_RESULT_LEN        (21)
#define MCU_PKG_BPF_ALGO_RESULT_LEN         (88)

/// spo2 mcu unwear 
#define SPO2_MCU_UNWEAR_VAL                 (255)

/// fix spo2 mcu val
#define FIXED_SPO2_ALGO_RES(val)            do { if ((val) == SPO2_MCU_UNWEAR_VAL) { (val) = 0; } } while (0)

/// mcu rawdata cnt
#define DBG_MCU_MODE_PKG_LEN                (6)

/// mcu rawdata frame len
#define DBG_MCU_PKG_RAW_FRAME_LEN           (13)

/// mcu pkg header len
#define DBG_MCU_PKG_HEADER_LEN              (8)

/// hrv output res max len
#define HRV_MODE_RES_MAX_CNT                (8)

/// comm type val 
#define COMM_TYPE_INVALID_VAL               (0xFF)

/// hbd ret val base
#define DEBUG_HBD_RET_VAL_BASE              (7)

#define NADT_MATH_ROUND(v)                  (v >= 0) ? (v + 0.5f) : (v - 0.5f)
#define NADT_MATH_ROUND1(v)                 round(v)
#define NADT_MATH_ABSF(v)                   fabs(v)

typedef enum
{
    FACTOR_INVALID_STATE,
    FACTORY_GREEN_IR_STATE,
    FACTORY_IR_RED_STATE
}EMFactoryState;

typedef struct
{
    EMFactoryState last_state;
    EMFactoryState now_state;
}STFactoryModeCfg;

typedef enum
{
    WEAR_DETECTION,
    SHEDDING_DETECTION
}EMAdtMode;

typedef struct
{
    STFactoryModeCfg factory_mode_cfg;
}STRunModeCfg;

#if (__GET_RAWDATA_WITHOUT_ALGO_HANDLE)
#define GH30X_RAW_DATA_LEN  30  //25Hz, add 5 buffer;

typedef struct
{
    int threshold;

    double m_vecA[7];
    double m_vecB[7];
    double m_vecDataIn[6];
    double m_vecDataOut[6];    
    char m_bFirstDataFlag;
}HighPassFilter;
#endif

#if (__SYSTEM_TEST_SUPPORT__)
typedef struct {
    GU16    gre1_num;
    GU16    gre1_led_curr;
    GU16    red_num;
    GU16    red_led_curr;
    GU16    ir_num;
    GU16    ir_led_curr;
    GU32    gre1Buf[GH30X_RAW_DATA_LEN];
    GU32    redBuf[GH30X_RAW_DATA_LEN];
    GU32    IrBuf[GH30X_RAW_DATA_LEN];
    GU32    unTempBufForStatistic[100];
    HighPassFilter HighPassFilters;
} STGh30xFactoryRawdata;
#endif

/* debug log control, if enable debug, example_dbg_log must define in platforms code */
#if (__EXAMPLE_DEBUG_LOG_LVL__) // debug level > 0
/// dbg run mode string
extern const char dbg_rum_mode_string[][27];
/// dbg communicate cmd string
extern const char dbg_comm_cmd_string[][35];
/// dbg ret val string
extern const char dbg_ret_val_string[][35];
/// lv1 log string
#define EXAMPLE_DEBUG_LOG_L1(...)       do {\
                                                char dbg_log_str[__EXAMPLE_LOG_DEBUG_SUP_LEN__] = {0};\
                                                snprintf(dbg_log_str, __EXAMPLE_LOG_DEBUG_SUP_LEN__, \
                                                        "[gh30x]: "__VA_ARGS__);\
                                                example_dbg_log(dbg_log_str);\
                                            } while (0)


#if (__EXAMPLE_DEBUG_LOG_LVL__ > 1) // debug level > 1
/// lv2 log string
#define EXAMPLE_DEBUG_LOG_L2(...)                { EXAMPLE_DEBUG_LOG_L1(__VA_ARGS__); }
#define EXAMPLE_LOG_RAWDARA(str, dat, len)      do { \
                                                        EXAMPLE_DEBUG_LOG_L2(str);\
                                                        uint8_t log_index = 0; \
                                                        for (log_index = 0; log_index < len; log_index++) \
                                                        {\
                                                            EXAMPLE_DEBUG_LOG_L2("raw,%ld,%ld,%ld,%ld,%ld,%ld\r\n", \
                                                                        dat[log_index * DBG_MCU_MODE_PKG_LEN + 0], \
                                                                        dat[log_index * DBG_MCU_MODE_PKG_LEN + 1], \
                                                                        dat[log_index * DBG_MCU_MODE_PKG_LEN + 2], \
                                                                        dat[log_index * DBG_MCU_MODE_PKG_LEN + 3], \
                                                                        dat[log_index * DBG_MCU_MODE_PKG_LEN + 4], \
                                                                        dat[log_index * DBG_MCU_MODE_PKG_LEN + 5]); \
                                                        }\
                                                  } while (0)
#else
#define   EXAMPLE_DEBUG_LOG_L2(...)
#define   EXAMPLE_LOG_RAWDARA(str, dat, len)
#endif
#else   // debug level <= 0
#define   EXAMPLE_DEBUG_LOG_L1(...)
#define   EXAMPLE_DEBUG_LOG_L2(...)
#define   EXAMPLE_LOG_RAWDARA(str, dat, len)
#endif

//// get bytes macro
#define   GET_BYTE3_FROM_DWORD(val)     ((uint8_t)((val >> 24) & 0x000000FFU))
#define   GET_BYTE2_FROM_DWORD(val)     ((uint8_t)((val >> 16) & 0x000000FFU))
#define   GET_BYTE1_FROM_DWORD(val)     ((uint8_t)((val >> 8) & 0x000000FFU))
#define   GET_BYTE0_FROM_DWORD(val)     ((uint8_t)((val ) & 0x000000FFU))
#define   GET_HIGH_BYTE_FROM_WORD(val)  ((uint8_t)(((val) >> 8) & 0xFF))
#define   GET_LOW_BYTE_FROM_WORD(val)   ((uint8_t)((val) & 0xFF))

typedef unsigned char    (* Write_fun)(unsigned short reg,unsigned short value);
typedef unsigned short (* Read_fun)(unsigned short reg);
typedef void (*Delay_us_fun)(unsigned short us_cnt);

/**
old struct to check system
* @member    nLpNoise:the noise level
* @member    nNoiseRawDataMin:set rawdata max
* @member    nNoiseRawDataMin:set rawdata min
* @member    nNoiseRawDataAvr:output the avarage noise
*/
typedef struct 
{
    short int nLpNoise;//set noise Level
    int nNoiseRawDataMax;//set rawdata max
    int nNoiseRawDataMin;//set rawdata min
    int nNoiseRawDataAvr;
}HBDTEST_ROMAHBData;

/**
struct contain communicate func
* @member    WR_Fun:func write register
* @member    RD_Fun:func read register
*/
typedef struct
{
    Write_fun WR_Fun;
    Read_fun RD_Fun;
}ROMAHBD_Interfcae;

//add 20201113
/**
struct contain standar
* @member    _ctrStd:min ctr
* @member    _leakStd:max leak
* @member    _ratio:max leak/ctr ratio
* @member    _noiseStd:max noise
*/
typedef struct
{
    float _ctrStd;//min ctr
    float _leakStd;//max leak
    float _ratio;//max leak/ctr ratio
	float _noiseStd;//max noise
}HBDTEST_ROMAStd;

/**
struct save the result of test
* @member    _flag:indicate CTR has been calculate if =1
* @member    _CTR:save the result of CTR test
* @member    _leak:save the result of leak test
* @member    _leakRatio:save the result of leakratio test
*/
typedef struct
{
    unsigned char _flag;
    float _CTR;
    float _leak;
    float _leakRatio;
	int _noise;
}HBDTEST_ROMATSTRes;

/**
struct used in cal
* @member    _ledResisLst:the resistance in config
* @member    _ledCurrLst:the current in config
*/
typedef struct
{
    float _ledCurr;
    int _ledResis;
}HBDTEST_ROMAConfigParam;

/// the set of front three
typedef struct
{
    HBDTEST_ROMAStd _std;
    HBDTEST_ROMATSTRes _res;
    HBDTEST_ROMAConfigParam _param;
}HBDTEST_ROMALEDCheckData;

/// errorcode enum
typedef enum
{
    EN_NORMAL_OK=0,
    EN_PARAM_FAIL,
    EN_COMM_READ_FAIL,
    EN_COMM_WRITE_FAIL,
    EN_OTP_CHECK_FAIL,
    EN_CTR_UNDER_STD,
    EN_RAWDATA_OUT_RANGE,
    EN_NOISE_UNDER_STD,
    EN_LEAK_UNDER_STD,
    EN_LEAKRATIO_UNDER_STD,
    EN_SELFTST_ERROR_ALL
}enSelfTestError;

extern GU32 g_unGh30xDemoFuncMode;
extern GU8 g_uchGh30xIsAdtMode;
///gh30x run method
extern uint8_t g_uchGh30xRunMethod;

extern STFactoryModeCfg gstFactoryModeCfg;
	
/// config var 

// init
extern ST_HBD_INIT_CONFIG_TYPE gh30x_init_config; 

// hb
extern const ST_REGISTER hb_adt_confirm_reg_config[];
extern const uint8_t hb_adt_confirm_reg_config_len;
extern const ST_REGISTER hb_reg_config_array[];
extern const uint8_t hb_reg_config_array_len;
extern const ST_REGISTER hb_wear_confirm_reg_config_array[];
extern const uint8_t hb_wear_confirm_reg_config_array_len;
extern const ST_REGISTER_BIT_FIELD g_stGh30xAdtExtralCfg[];
extern const uint8_t g_stGh30xAdtExtralCfg_len;
extern const ST_REGISTER_BIT_FIELD g_stGh30xHrExtralCfg[];
extern const uint8_t g_stGh30xHrExtralCfg_len;
extern const ST_REGISTER_BIT_FIELD g_stGh30xSpo2ExtralCfg[];
extern const uint8_t g_stGh30xSpo2ExtralCfg_len;

// spo2
extern const ST_REGISTER spo2_reg_config_array[];
extern const uint8_t spo2_reg_config_array_len;

// hrv
extern const ST_REGISTER hrv_reg_config_array[];
extern const uint8_t hrv_reg_config_array_len;

// getrawdata
extern const ST_REGISTER getrawdata_reg_config_array[];
extern const uint8_t getrawdata_reg_config_array_len;

//adt
extern const ST_REGISTER adt_reg_config_array[];
extern const uint8_t adt_reg_config_array_len;

// system test
extern const ST_REGISTER systemtest_led0_reg_config_array[];
extern const uint8_t systemtest_led0_reg_config_array_len;			

extern const ST_REGISTER systemtest_led1_reg_config_array[];
extern const uint8_t systemtest_led1_reg_config_array_len;

extern const ST_REGISTER systemtest_led2_reg_config_array[];
extern const uint8_t systemtest_led2_reg_config_array_len;

// factory
extern const ST_REGISTER factory_green_ir_reg_config_array[];
extern const uint8_t factory_green_ir_reg_config_array_len;

extern const ST_REGISTER factory_ir_red_reg_config_array[];
extern const uint8_t factory_ir_red_reg_config_array_len;

#if (__SYSTEM_TEST_SUPPORT__)
extern HBDTEST_ROMAHBData systemtest_led0_os_result;
extern HBDTEST_ROMAHBData systemtest_led1_os_result;
extern HBDTEST_ROMAHBData systemtest_led2_os_result;
extern HBDTEST_ROMALEDCheckData led0std,led1std,led2std;
extern unsigned char ledmask[3];
extern uint8_t goodix_system_test_mode;
void gh30x_systemtest_init(uint8_t mode);
#endif

/**
    * @function        HBDTEST_Comm_Check
    * @description    check whether communicate is normal
    * @input             OTP reg wr rd fun
    * @return            enSelfTestStatus:the status of check
*/
extern unsigned char HBDTEST_Comm_Check(ROMAHBD_Interfcae *ROMAHBD_Str);

/**
    * @function        HBDTEST_OTP_Check
    * @description    check OTPdata crc function.
    * @input                 OTP reg wr rd fun.
    * @return            enSelfTestStatus:the status of check
    * @note
    */
extern unsigned char HBDTEST_OTP_Check( ROMAHBD_Interfcae *ROMAHBD_Str,unsigned char *retbuf );

/**
    * @function        HBDTEST_Check_Rawdata_Noise
    * @description    check Rawdata and noise function.
    * @input                 Rawdata and check thold value.
    * @output                check result.
    * @return               enSelfTestStatus:the status of check
    * @note
    */
extern unsigned char HBDTEST_Check_Rawdata_Noise( int *_inDatabuff,const unsigned char lenth,const HBDTEST_ROMAHBData *_inRawdataStr );

/**
    * @function        HBDTEST_Check_LeakandRatio
    * @description    add function use to check rawdata in (Dark best) and calculate leak and leakratio.
    * @input                 _inDatabuff:rawdata.
    * @input                lenth: rawdata buffer lenth
    * @input             std: the standar to check
    * @input             data: the check struct
    * @output            data->_res.leak: the result of leak test
    * @output             data->_res.leakratio : the result of leakratio test
    * @return            enSelfTestStatus:the status of check
    * @note
    */
extern unsigned char HBDTEST_Check_LeakandRatio(int *_inDatabuff,const unsigned char lenth,HBDTEST_ROMALEDCheckData* data);

/**
    * @function        HBDTEST_Check_CTRandNoise
    * @description    add function use to check rawdata and calculate CTR.
    * @input                 _inDatabuff:rawdata.
    * @input                lenth: rawdata buffer lenth
    * @input             data: the check struct
    * @output            data->_res._CTR: the result of CTR test
    * @return                enSelfTestStatus:the status of check
    * @note
    */
extern unsigned char HBDTEST_Check_CTRandNoise(int *_inDatabuff,const unsigned char lenth,HBDTEST_ROMALEDCheckData* data);


/**
    * @function        HBDTEST_Get_TestlibVersion
    * @description    get the version of test lib.
    * @return                the version of test lib.
    * @note
    */
extern const char* HBDTEST_Get_TestlibVersion(void);

/**
    * @function        HBDTEST_set_delayFunc
    * @description    get the version of test lib.
    * @return                the version of test lib.
    * @note
    */
extern void HBDTEST_set_delayFunc(Delay_us_fun func);

void comm_send_ext_packet(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen), uint8_t data_array[], uint8_t data_len);
void comm_send_app_cmd(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen), uint8_t dev_state, uint8_t data_array[], uint8_t data_len);
void comm_send_app_cmd_auto_led_fail(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen));
void comm_send_app_cmd_reset(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen));
void comm_send_app_cmd_battery_low(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen));
void comm_send_app_cmd_hb_algo_val(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen), 
                            uint8_t hb_aval, uint8_t wear_sval, uint8_t wear_qval, uint8_t sc_bval, 
                            uint8_t lvl_lval, uint8_t func_rval, uint16_t rri_rval);
void comm_send_app_cmd_hrv_algo_val(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen),
                            uint16_t rri1_rval, uint16_t rri2_rval, uint16_t rri3_rval, uint16_t rri4_rval,
                            uint8_t lvl_lval, uint8_t cnt_cval);
void comm_send_app_cmd_spo2_algo_val(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen),
                            uint8_t spo2_rval,  uint8_t spo2_reliable_rval,uint8_t hb_aval,uint8_t hb_reliable_rval,
							uint16_t rri1_rval, uint16_t rri2_rval, uint16_t rri3_rval, uint16_t rri4_rval,
                            uint8_t lvl_lval, uint8_t cnt_cval, uint16_t spo2R_rval);							
uint16_t comm_packaging_rawdata_packet(uint8_t *packet_buff, uint16_t comm_max_len, 
                                     uint8_t algo_type, uint16_t total_len, uint8_t packet_index, uint8_t algo_result_len,
                                     uint8_t data_array[], uint16_t data_len);

/// porting api

/// i2c
void hal_gh30x_i2c_init(void);
uint8_t hal_gh30x_i2c_write(uint8_t device_id, const uint8_t write_buffer[], uint16_t length);
uint8_t hal_gh30x_i2c_read(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length, uint8_t read_buffer[], uint16_t read_length);

/// spi
void hal_gh30x_spi_init(void);
uint8_t hal_gh30x_spi_write(const uint8_t write_buffer[], uint16_t length);
uint8_t hal_gh30x_spi_read(uint8_t read_buffer[], uint16_t length);
void hal_gh30x_spi_cs_set_low(void);
void hal_gh30x_spi_cs_set_high(void);

/// delay
void hal_gh30x_delay_us(uint16_t us_cnt);

/// gsensor driver
int8_t gsensor_drv_init(void);
void gsensor_drv_enter_normal_mode(void);
void gsensor_drv_enter_fifo_mode(void);
void gsensor_drv_enter_motion_det_mode(void);
void gsensor_drv_get_fifo_data(ST_GS_DATA_TYPE gsensor_buffer[], uint16_t *gsensor_buffer_index, uint16_t gsensor_max_len);
void gsensor_drv_clear_buffer(ST_GS_DATA_TYPE gsensor_buffer[], uint16_t *gsensor_buffer_index, uint16_t gsensor_buffer_len);
void gsensor_drv_get_data(ST_GS_DATA_TYPE *gsensor_data_ptr);
void gsensor_drv_int1_handler(void);

/// int
void hal_gh30x_int_init(void);
void hal_gsensor_int1_init(void);

/// handle result
#ifdef __HBD_API_EX__
void handle_hb_mode_result(ST_HB_RES *stHbRes, GS32 rawdata_ptr[][DBG_MCU_MODE_PKG_LEN], GU16 rawdata_len);
#else
void handle_hb_mode_result(GU8 hb_val, GU8 hb_lvl_val, GU8 wearing_state_val, GU16 rr_val, GS32 rawdata_ptr[][DBG_MCU_MODE_PKG_LEN], GU16 rawdata_len);
#endif

#if (__SPO2_DET_SUPPORT__ || __HR_SPO2_DET_SUPPORT__)
#ifdef __HBD_API_EX__
void handle_spo2_mode_result(ST_SPO2_RES *pstSpo2Res, GS32 rawdata_ptr[][DBG_MCU_MODE_PKG_LEN], GU16 rawdata_len);
#else
void handle_spo2_mode_result(GU8 spo2_val, GU8 spo2_lvl_val, GU8 hb_val, GU8 hb_lvl_val, GU16 rr_val[4], GU8 rr_lvl_val, GU8 rr_cnt, 
                                GU16 spo2_r_val, GU8 wearing_state_val, GS32 rawdata_ptr[][DBG_MCU_MODE_PKG_LEN], GU16 rawdata_len);
#endif
#endif
//void handle_getrawdata_mode_result(GU8 wearing_state_val, GS32 rawdata[__GET_RAWDATA_BUF_LEN__ ][2], GU16 rawdata_len);
//void handle_hrv_mode_result(GU16 rr_val_arr[HRV_MODE_RES_MAX_CNT], GU8 rr_val_cnt, GU8 rr_lvl, GS32 rawdata_ptr[][DBG_MCU_MODE_PKG_LEN], GU16 rawdata_len);

void handle_algo_result_update(GU32 function_id, GU16 value);
void handle_wear_status_result(uint8_t wearing_state_val, GU8 uchLivingFlag);
void handle_system_test_result(uint8_t test_res,uint8_t led_num);
void handle_before_system_os_test(void);
#if (__FACTORY_DET_SUPPORT__)
void handle_factory_mode_result(STGh30xFactoryRawdata* pstGh30xFactoryRawdata);
#endif

#if (__HARDWARE_ADT_DET_SUPPORT__)
/// handle hardware adt mode wear status
void handle_hardware_adt_status_result(GU8 wearing_state_val);
#endif

/// ble 
void ble_module_send_heartrate(uint32_t heartrate); // send value via heartrate profile
void ble_module_add_rr_interval(uint16_t rr_interval_arr[], uint8_t cnt); // add value to heartrate profile
uint8_t ble_module_send_data_via_gdcs(uint8_t string[], uint8_t length); // send value via through profile

// ble repeat send timer
void ble_module_repeat_send_timer_start(void);
void ble_module_repeat_send_timer_stop(void);
void ble_module_repeat_send_timer_init(void);

// fifo int timeout
void hal_gh30x_fifo_int_timeout_timer_start(void);
void hal_gh30x_fifo_int_timeout_timer_stop(void);
void hal_gh30x_fifo_int_timeout_timer_init(void);

// uart
uint8_t uart_module_send_data(uint8_t string[], uint8_t length);

// handle cmd
void handle_goodix_communicate_cmd(EM_COMM_CMD_TYPE cmd_type);

// log
void example_dbg_log(char *log_string);

//For dynamic memory lib only
void *hal_gh30x_memory_malloc(GU32 size);
void hal_gh30x_memory_free(void *pmem);

/// ble pkg api
void uart_module_handle_recv_byte(uint8_t recv_byte);
void uart_module_send_pkg(uint8_t string[], uint8_t length);
void gh30x_comm_pkg_init(void);
void gh30x_app_cmd_parse(uint8_t *buffer, uint8_t length); // ble recv data parse
void gh30x_uart_cmd_parse(uint8_t *buffer, uint8_t length); // uart recv data parse
void ble_module_send_pkg(uint8_t string[], uint8_t length);
void send_mcu_rawdata_packet_repeat(void);
void send_mcu_hb_mode_wear_status_pkg(uint8_t wear_status, GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint8_t rawdata_dbg_len);
#ifdef __HBD_API_EX__
void send_mcu_hb_mode_result_pkg(uint8_t hb_val, uint8_t hb_lvl, uint8_t wear_status, uint8_t wearing_q, uint8_t vb_val, uint8_t clac_ret, uint16_t rr_val, uint8_t current[3],
                                        GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint8_t rawdata_dbg_len,uint8_t hb_senario, uint8_t hb_snr, uint8_t motion_state, uint8_t sleep_flag);
#else
void send_mcu_hb_mode_result_pkg(uint8_t hb_val, uint8_t hb_lvl, uint8_t wear_status, uint8_t wearing_q, uint8_t vb_val, uint8_t clac_ret, uint16_t rr_val, uint8_t current[3],
                                        GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint8_t rawdata_dbg_len);
#endif
void send_mcu_hrv_mode_result_pkg(uint16_t rr_val_arr[], uint8_t rr_cnt, uint8_t rr_lvl, uint8_t hb_val, uint8_t current[3], GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint8_t rawdata_dbg_len);
void send_mcu_spo2_mode_unwear_pkg(GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint8_t rawdata_dbg_len);
#ifdef __HBD_API_EX__
void send_mcu_spo2_mode_result_pkg(void *pstRes, GU8 abnormal_state, GU8 current[3], GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], GU16 rawdata_dbg_len);
#else
void send_mcu_spo2_mode_result_pkg(uint8_t spo2_val, uint8_t spo2_lvl_val, uint8_t hb_val, uint8_t hb_lvl_val, uint16_t rr_val[4], uint8_t rr_lvl_val, uint8_t rr_cnt, 
							uint16_t spo2_r_val, uint8_t wearing_state_val, uint8_t valid_lvl_val, uint8_t abnormal_state, uint8_t current[3], GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint16_t rawdata_dbg_len);
#endif
void send_mcu_bpf_mode_result_pkg(void *pstRes, uint8_t current[3], GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint16_t rawdata_dbg_len);

/// ctrl api
#if (__HBD_ALGORITHM_USE_DYN_MEM__)
/****************************************************************
* Description: Get memory size of hbd_ctrl lib need
* Input: GU8 uchMemoryLevel   0, Maximum memory required, full function supoort
*                             1, Less than level 0 memory required, HBwithHRV function will unsupport.
* Return: size in bytes
******************************************************************/
GU32 HBD_GetMemRequired(GU8 uchMemoryLevel);
                                                                    
/****************************************************************
* Description: Get memory size of hbd_ctrl lib need
* Input: GU8 *pMem   pointer of memory which for hbd_ctrl lib
******************************************************************/
void HBD_SetMemPtr(GU8 *pMem);
void gh30x_malloc_memory(void);
void gh30x_free_memory(void);
#endif
GS8 gh30x_Load_new_config(const ST_REGISTER *config_ptr, uint16_t len);
void gh30x_adt_wear_detect_start(const ST_REGISTER *config_ptr, uint16_t config_len);
#if (__HB_NEED_ADT_CONFIRM__)
void gh30x_adt_confirm_start(void);
#endif

#if (__FACTORY_DET_SUPPORT__)
void gh30x_factory_start(void);
#endif

#if (__HB_DET_SUPPORT__)
void gh30x_hb_start(void);
#endif

#if (__HR_SPO2_DET_SUPPORT__)
void gh30x_hr_spo2_start(void);
#endif
#if (__HRV_DET_SUPPORT__)
//void gh30x_hrv_start(void);
#endif
#if (__SPO2_DET_SUPPORT__)
//void gh30x_spo2_start(void);
#endif
#if (__HARDWARE_ADT_DET_SUPPORT__)
void gh30x_set_hardware_adt_mode(EMAdtMode emHardwareAdtMode);
void gh30x_hardware_adt_start(void);
#endif
#if (__GET_RAWDATA_ONLY_SUPPORT__)
void gh30x_getrawdata_start(void);
#endif
uint8_t gh30x_i2c_write_exchange_to_spi(uint8_t device_id, const uint8_t write_buffer[], uint16_t length);
uint8_t gh30x_i2c_read_exchange_to_spi(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length, uint8_t read_buffer[], uint16_t read_length);
#if (__SYSTEM_TEST_SUPPORT__)
uint8_t gh30x_systemtest_otp_check(void);
void gh30x_systemtest_os_start(uint8_t led_num);
uint8_t gh30x_systemtest_os_calc(uint8_t led_num);
uint8_t gh30x_systemtest_comm_check(void);
void gh30x_systemtest_param_set(uint8_t led_num, HBDTEST_ROMAConfigParam* param);
void gh30x_systemtest_part2_handle(uint8_t ret);
#endif

/// process api
//void gh30x_start_adt_with_mode(uint8_t adt_run_mode);
//void gh30x_start_func_with_mode(uint8_t func_run_mode);
//void gh30x_start_func_whithout_adt_confirm(uint8_t start_run_mode);
void gh30x_stop_func(void);

void gsensor_enter_clear_buffer_and_enter_fifo(void);
void gsensor_read_fifo_data(void);
void gsensor_enter_normal_and_clear_buffer(void);
void gsensor_motion_has_detect(void);
void gh30x_get_sin_gsensor_data(float Hz, uint8_t range, uint8_t fifomode);

void gh30x_communicate_parse_handler(int8_t communicate_type, uint8_t *buffer, uint8_t length);
void gh30x_int_msg_handler(void); // gh30x int msg handler
void gh30x_fifo_int_timeout_msg_handler(void);

/// hbd ctrl extern api & var
extern void HBD_DelayUs(GU16 usUsCnt);
extern void HBD_I2cSendCmd(GU8 uchCmd);
extern void HBD_I2cWriteReg(GU16 usAddr, GU16 usValue);
extern GU16 HBD_I2cReadReg(GU16 usAddr);
extern GU32 HBD_I2cReadRawdataReg(GU16 usAddr);
extern GU16 g_usReadRawdataRegList[2];
extern void HBD_I2cWriteRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb, GU16 usValue);
extern GU16 HBD_I2cReadRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb);

void Gh30xFunctionFrameInfoUpdate(GU32 unFunctionMode, stSampleResetCfg *stSampleCfg);
void Gh30xFunctionFrameInfoBaseInit(void);
void Gh30xGetChnlInfo(void);
void Gh30xInitChnlDrvAndGainInfo(void);
GU8 HBD_AutoLedbyFifoInt(GU32 *punDataBuffer, GU16 usRawdatabufferCnt);
void Gh30xUpdateFunctionDivder(void);
GS8 HBD_AdtWearDetectStart (void);
GS8 HBD_AdtSheddingDetectStart (void);
void Gh30xSetSoftWearEvent(GU8 uchSoftEvent, GU8 uchLivingObjFlg);
void Gh30xClearSoftWearEvent(GU8 uchSoftEvent);
GS8 Gh30xAdtWearDetectStart (GU8 uchDetWearOnOrOff);
void Gh30xLoadAdtSampleParm(void);
void Gh30xLoadBitFieldRegList(const ST_REGISTER_BIT_FIELD * pstCfg, GU16 usLen);
void send_mcu_result_pkg(GU16 usFrameNum, const STGh30xFrameInfo * const pstFrameInfo);
void GH30xRecordAlgoResult(const STGh30xFrameInfo * const pstFrameInfo);
void Gh30xCopyRawdataToBLeBuf(GU16 usFrameCnt,  const STGh30xFrameInfo * const pstFrameInfo);
GCHAR *Gh30xHBDVersionGet(void);
void Gh30xRawdata24BitTo32Bit(GU8 *puchRawdataFifo, GU16 usSamplePointNum, GU16 usSamplePointNumMax);
void Gh30xUpdateProtocolSendDataFunc(void);

void comm_send_ext_packet(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen), uint8_t data_array[], uint8_t data_len);
void comm_send_app_cmd(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen), uint8_t dev_state, uint8_t data_array[], uint8_t data_len);
void comm_send_app_cmd_auto_led_fail(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen));
void comm_send_app_cmd_reset(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen));
void comm_send_app_cmd_battery_low(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen));
void comm_send_app_cmd_hb_algo_val(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen), 
                            uint8_t hb_aval, uint8_t wear_sval, uint8_t wear_qval, uint8_t sc_bval, 
                            uint8_t lvl_lval, uint8_t func_rval, uint16_t rri_rval);
void comm_send_app_cmd_hrv_algo_val(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen),
                            uint16_t rri1_rval, uint16_t rri2_rval, uint16_t rri3_rval, uint16_t rri4_rval,
                            uint8_t lvl_lval, uint8_t cnt_cval);
void comm_send_app_cmd_spo2_algo_val(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen),
                            uint8_t spo2_rval,  uint8_t spo2_reliable_rval,uint8_t hb_aval,uint8_t hb_reliable_rval,
							uint16_t rri1_rval, uint16_t rri2_rval, uint16_t rri3_rval, uint16_t rri4_rval,
                            uint8_t lvl_lval, uint8_t cnt_cval, uint16_t spo2R_rval);							
uint16_t comm_packaging_rawdata_packet(uint8_t *packet_buff, uint16_t comm_max_len, 
                                     uint8_t algo_type, uint16_t total_len, uint8_t packet_index, uint8_t algo_result_len,
                                     uint8_t data_array[], uint16_t data_len);

#endif /* _GH30X_EXAMPLE_COMMON_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2024, Goodix Co., Ltd. ********/
