/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_example_process.c
 *
 * @brief   example code for gh30x
 *
 */
#include "math.h"
#include "../inc/gh30x_example.h"
#include "../inc/gh30x_example_common.h"
#include "../../Algo_Code/common/inc/gh30x_demo_algo_call.h"
#include "../../../ui/lv_watch/comm_sensor/sensor_hr_task.h"

typedef struct
{
    GU32 unPpgRawdataBuf[GH30X_RAWDATA_FIFO_BUF_SAMPLE_POINT_NUM_MAX];
    ST_GS_DATA_TYPE stGsensorBuf[GSENSOR_SOFT_FIFO_BUFFER_MAX_LEN];
} STGh30xRawdataBuf;

union UNGh30xRawdataBuf 
{
    STGh30xRawdataBuf stRawdataBuf;
    #if __SYSTEM_TEST_SUPPORT__
    STGh30xFactoryRawdata stFactoryRawdata;
    GU32 unSystemtest_rawdata_buffer[__SYSTEM_TEST_DATA_CNT_CONFIG__];
    #endif
};

union UNGh30xRawdataBuf g_UNGh30xRawdataBuf;

GU32 g_unGh30xFrameCntForADT;
GU32 g_unGh30xFrameCntForHR;
GU32 g_unGh30xFrameCntForHRV;
GU32 g_unGh30xFrameCntForSPO2;
GU32 g_unGh30xFrameCntForSADT;
GU32 g_unGh30xFrameCntForPRO;
GU32 g_punGh30xFrameRawdata[GH30X_FUNC_CHNL_NUM_MAX];
GU8  g_puchGh30xFrameDrvCurrentCode[GH30X_FUNC_CHNL_NUM_MAX];
GS16 g_psGh30xFrameGsensorData[3];
STGh30xAlgoResult g_stGh30xAlgoResult;
GU32 g_unGh30xDemoFuncMode;
GU8  g_uchGh30xIsAdtMode;
GU8  g_mcu_start_mode;

/// init hook func ptr
pfnNormalHookFunc g_pGh30xInitHookFunc = GH30X_PTR_NULL;
/// get rawdata hook func ptr
pfnGetRawdataHookFunc g_pGh30xGetRawdataHookFunc = GH30X_PTR_NULL;
/// get struct hook func ptr
pfnGetIoStructInfo g_pGh30xGetIoStructInfoHookFunc = GH30X_PTR_NULL;

#if __FUNC_TYPE_ADT_ENABLE__
GU16 g_usGh30xAdtDownThrdRegValue;
GU8  g_uchGh30xMotionDetEn;
GU8  g_uchGh30xSoftEvent;
GU8  g_uchGh30xLivingObjectFlag;
#endif

#if __FUNC_TYPE_ADT_ENABLE__
STGh30xFunctionInfo g_st_ADT_FuncitonInfo;
GU8 g_uch_ADT_ChnlMap[GH30X_ADT_CHNL_NUM];
const STGh30xFrameInfo g_stADT_FrameInfo = 
{
    .unFunctionID = GH30X_FUNCTION_ADT,
    .punFrameCnt = &g_unGh30xFrameCntForADT,
    .punRawdata = g_punGh30xFrameRawdata,
    .pusGsensordata = g_psGh30xFrameGsensorData,
    .pchDrvCurrentCode = g_puchGh30xFrameDrvCurrentCode,
    .pstFunctionInfo = &g_st_ADT_FuncitonInfo,
    .puchChnlMap = g_uch_ADT_ChnlMap,
    .pstAlgoResult   = &g_stGh30xAlgoResult,
    .uchFuntionChnlLimit = GH30X_ADT_CHNL_NUM,
};
#define POINTER_GH30X_ADT_FRAME_INFO (&g_stADT_FrameInfo)
#else
#define POINTER_GH30X_ADT_FRAME_INFO 0
#endif

#if __FUNC_TYPE_HR_ENABLE__
STGh30xFunctionInfo g_st_HR_FuncitonInfo;
GU8 g_uch_HR_ChnlMap[GH30X_HR_CHNL_NUM];
const STGh30xFrameInfo g_stHR_FrameInfo = 
{
    .unFunctionID = GH30X_FUNCTION_HR,
    .punFrameCnt = &g_unGh30xFrameCntForHR,
    .punRawdata = g_punGh30xFrameRawdata,
    .pusGsensordata = g_psGh30xFrameGsensorData,
    .pchDrvCurrentCode = g_puchGh30xFrameDrvCurrentCode,
    .pstFunctionInfo = &g_st_HR_FuncitonInfo,
    .puchChnlMap = g_uch_HR_ChnlMap,
    .pstAlgoResult   = &g_stGh30xAlgoResult,
    .uchFuntionChnlLimit = GH30X_HR_CHNL_NUM,
};
#define POINTER_GH30X_HR_FRAME_INFO (&g_stHR_FrameInfo)
#else
#define POINTER_GH30X_HR_FRAME_INFO 0
#endif

#if __FUNC_TYPE_HRV_ENABLE__
STGh30xFunctionInfo g_st_HRV_FuncitonInfo;
GU8 g_uch_HRV_ChnlMap[GH30X_HRV_CHNL_NUM];
const STGh30xFrameInfo g_stHRV_FrameInfo = 
{
    .unFunctionID = GH30X_FUNCTION_HRV,
    .punFrameCnt = &g_unGh30xFrameCntForHRV,
    .punRawdata = g_punGh30xFrameRawdata,
    .pusGsensordata = g_psGh30xFrameGsensorData,
    .pchDrvCurrentCode = g_puchGh30xFrameDrvCurrentCode,
    .pstFunctionInfo = &g_st_HRV_FuncitonInfo,
    .puchChnlMap = g_uch_HRV_ChnlMap,
    .pstAlgoResult   = &g_stGh30xAlgoResult,
    .uchFuntionChnlLimit = GH30X_HRV_CHNL_NUM,
};
#define POINTER_GH30X_HRV_FRAME_INFO (&g_stHRV_FrameInfo)
#else
#define POINTER_GH30X_HRV_FRAME_INFO 0
#endif

#if __FUNC_TYPE_SPO2_ENABLE__
STGh30xFunctionInfo g_st_SPO2_FuncitonInfo;
GU8 g_uch_SPO2_ChnlMap[GH30X_SPO2_CHNL_NUM];
const STGh30xFrameInfo g_stSPO2_FrameInfo = 
{
    .unFunctionID = GH30X_FUNCTION_SPO2,
    .punFrameCnt = &g_unGh30xFrameCntForSPO2,
    .punRawdata = g_punGh30xFrameRawdata,
    .pusGsensordata = g_psGh30xFrameGsensorData,
    .pchDrvCurrentCode = g_puchGh30xFrameDrvCurrentCode,
    .pstFunctionInfo = &g_st_SPO2_FuncitonInfo,
    .puchChnlMap = g_uch_SPO2_ChnlMap,
    .pstAlgoResult   = &g_stGh30xAlgoResult,
    .uchFuntionChnlLimit = GH30X_SPO2_CHNL_NUM,
};
#define POINTER_GH30X_SPO2_FRAME_INFO (&g_stSPO2_FrameInfo)
#else
#define POINTER_GH30X_SPO2_FRAME_INFO 0
#endif

#if __FUNC_TYPE_SOFT_ADT_ENABLE__
STGh30xFunctionInfo g_st_SOFT_ADT_FuncitonInfo;
GU8 g_uch_SOFT_ADT_ChnlMap[GH30X_SOFT_ADT_CHNL_NUM];
const STGh30xFrameInfo g_stSOFT_ADT_FrameInfo = 
{
    .unFunctionID = GH30X_FUNCTION_SOFT_ADT,
    .punFrameCnt = &g_unGh30xFrameCntForSADT,
    .punRawdata = g_punGh30xFrameRawdata,
    .pusGsensordata = g_psGh30xFrameGsensorData,
    .pchDrvCurrentCode = g_puchGh30xFrameDrvCurrentCode,
    .pstFunctionInfo = &g_st_SOFT_ADT_FuncitonInfo,
    .puchChnlMap = g_uch_SOFT_ADT_ChnlMap,
    .pstAlgoResult   = &g_stGh30xAlgoResult,
    .uchFuntionChnlLimit = GH30X_SOFT_ADT_CHNL_NUM,
};
#define POINTER_GH30X_SOFT_ADT_FRAME_INFO (&g_stSOFT_ADT_FrameInfo)
#else
#define POINTER_GH30X_SOFT_ADT_FRAME_INFO 0
#endif

#if __USE_GOODIX_APP__
STGh30xFunctionInfo g_st_PROTOCOL_FuncitonInfo;
GU8 g_uch_PROTOCOL_ChnlMap[GH30X_PROTOCOL_CHNL_NUM];
const STGh30xFrameInfo g_stPROTOCOL_FrameInfo = 
{
    .unFunctionID = GH30X_FUNCTION_PROTOCOL,
    .punFrameCnt = &g_unGh30xFrameCntForPRO,
    .punRawdata = g_punGh30xFrameRawdata,
    .pusGsensordata = g_psGh30xFrameGsensorData,
    .pchDrvCurrentCode = g_puchGh30xFrameDrvCurrentCode,
    .pstFunctionInfo = &g_st_PROTOCOL_FuncitonInfo,
    .puchChnlMap = g_uch_PROTOCOL_ChnlMap,
    .pstAlgoResult   = &g_stGh30xAlgoResult,
    .uchFuntionChnlLimit = 2,
};
#define POINTER_GH30X_PROTOCOL_FRAME_INFO (&g_stPROTOCOL_FrameInfo)
#else
#define POINTER_GH30X_PROTOCOL_FRAME_INFO 0
#endif

extern uint8_t gh30x_run_mode;
extern bool goodix_app_start_app_mode;

//Function data info struct
const STGh30xFrameInfo *const g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_MAX] =
{
    POINTER_GH30X_ADT_FRAME_INFO,       //offset = 0
    POINTER_GH30X_HR_FRAME_INFO,        //offset = 1
    POINTER_GH30X_HRV_FRAME_INFO,       //offset = 2
    POINTER_GH30X_SPO2_FRAME_INFO,      //offset = 3
    POINTER_GH30X_SOFT_ADT_FRAME_INFO,  //offset = 4
    POINTER_GH30X_PROTOCOL_FRAME_INFO,  //offset = 5
};

const stSampleResetCfg g_stSampleResetCfg =
{
    .usResetEn = 0, // |RESET_CFG_SAMPLE_RATE_EN
                    // |RESET_CFG_INT_TIME_EN
                    // |RESET_CFG_CHNL0_AGC_FORCE_DIS
                    // |RESET_CFG_CHNL1_AGC_FORCE_DIS
                    // |RESET_CFG_CHNL0_DEFAULT_CUREET_EN
                    // |RESET_CFG_CHNL1_DEFAULT_CUREET_EN
                    // |RESET_CFG_CHNL0_TIA_GAIN_EN
                    // |RESET_CFG_CHNL1_TIA_GAIN_EN
                    // |RESET_CFG_FIFO_WATER_LINE_EN
    .usResetSampleRate = 25,
    .uchResetIntTimeIndex = 0,
    .uchResetDrvCurrentChnl0 = 0,
    .uchResetDrvCurrentChnl1 = 0,
    .uchResetTiaGainChnl0 = 0,
    .uchResetTiaGainChnl1 = 0,
    .uchResetFifoWaterLine = 100,
};

void Gh30xSampleResetHandle(stSampleResetCfg* pstSampleResetCfg)
{
    if (pstSampleResetCfg->usResetEn & RESET_CFG_SAMPLE_RATE_EN)
    {
        Gh30xSampleRateSet(pstSampleResetCfg->usResetSampleRate);
    }
    if (pstSampleResetCfg->usResetEn & RESET_CFG_INT_TIME_EN)
    {
        Gh30xIntTimeSet(pstSampleResetCfg->uchResetIntTimeIndex);
    }
    if (pstSampleResetCfg->usResetEn & RESET_CFG_CHNL0_AGC_FORCE_DIS)
    {
        Gh30xSoftAgcSet(0, HBD_FUNCTIONAL_STATE_DISABLE);
        Gh30xHardAgcDisable(0);
    }
    if (pstSampleResetCfg->usResetEn & RESET_CFG_CHNL1_AGC_FORCE_DIS)
    {
        Gh30xSoftAgcSet(1, HBD_FUNCTIONAL_STATE_DISABLE);
        Gh30xHardAgcDisable(1);
    }
    if (pstSampleResetCfg->usResetEn & RESET_CFG_CHNL0_DEFAULT_CUREET_EN)
    {
        Gh30xSetDefaultDrvCurrent(0, pstSampleResetCfg->uchResetDrvCurrentChnl0);
    }
    if (pstSampleResetCfg->usResetEn & RESET_CFG_CHNL1_DEFAULT_CUREET_EN)
    {
        Gh30xSetDefaultDrvCurrent(1, pstSampleResetCfg->uchResetDrvCurrentChnl1);
    }
    if (pstSampleResetCfg->usResetEn & RESET_CFG_CHNL0_TIA_GAIN_EN)
    {
        Gh30xSetDefaultGain(0, pstSampleResetCfg->uchResetTiaGainChnl0);
    }
    if (pstSampleResetCfg->usResetEn & RESET_CFG_CHNL1_TIA_GAIN_EN)
    {
        Gh30xSetDefaultGain(1, pstSampleResetCfg->uchResetTiaGainChnl1);
    }
    if (pstSampleResetCfg->usResetEn & RESET_CFG_FIFO_WATER_LINE_EN)
    {
        Gh30xSetFifoWaterline(pstSampleResetCfg->uchResetFifoWaterLine);
    }
}

void Gh30xSetSoftWearEvent(GU8 uchSoftEvent, GU8 uchLivingObjFlg)
{
    g_uchGh30xSoftEvent |= uchSoftEvent;
    g_uchGh30xLivingObjectFlag = uchLivingObjFlg;
    EXAMPLE_DEBUG_LOG_L2("[%s]uchSoftEvent = 0x%X, uchLivingObjFlg = %d.\r\n", __FUNCTION__, uchSoftEvent, uchLivingObjFlg);
}

void Gh30xClearSoftWearEvent(GU8 uchSoftEvent)
{
    g_uchGh30xSoftEvent &= (~uchSoftEvent);
}

void Gh30xDemoSampleStop(void)
{
    HBD_I2cSendCmd(HBD_CMD_IDLE); 
    HBD_AutoLedReset(HBD_START_STATUS_OFF);
    HBD_I2cSendCmd(HBD_CMD_SLEEP);
}

void Gh30xDemoSampleStart(const ST_REGISTER *pstBaseArray, GU16 usBaseArrayLen,
                                const ST_REGISTER_BIT_FIELD *pstAppendArray, GU16 usAppendArrayLen,
                                stSampleResetCfg *pstSampleResetCfg)
{
    /**** load pstBaseArray ***/
    if (false == goodix_app_start_app_mode)
    {
        gh30x_Load_new_config(pstBaseArray,usBaseArrayLen);
    }

    /**** init channel info ***/
    Gh30xGetChnlInfo();

    /**** load pstAppendArray ***/
    if (0 != pstAppendArray)
    {
        Gh30xLoadBitFieldRegList(pstAppendArray,usAppendArrayLen);
    }

    /**** set pstSampleResetCfg ***/
    if (0 != pstSampleResetCfg)
    {
        Gh30xSampleResetHandle(pstSampleResetCfg);
    }

    /**** init drv and gain ***/
    Gh30xInitChnlDrvAndGainInfo();

    /**** adjust adt threshold ***/
#if __FUNC_TYPE_ADT_ENABLE__
    Gh30xGetAdtDownThrdRegValue();
#endif

    /**** update function driver info ***/
    Gh30xUpdateFunctionDivder();

    /**** sample start ***/
    if (g_uchGh30xMotionDetEn)
    {
        Gh30xDemoSampleStop();
		EXAMPLE_DEBUG_LOG_L1("[%s] enter motion detecting...\r\n", __FUNCTION__);
		gsensor_drv_enter_motion_det_mode();
        //enter motion detecting
    }
    else
    {
        Gh30xHBD_SampleStart(); // start HBD mode sample;
    }
}

void Gh30xFunctionFrameInfoInitAtEnable(GU8 unFunc)
{
    for (GU8 uchFunCnt = 0; uchFunCnt < GH30X_FUNC_OFFSET_MAX; uchFunCnt++)
    {
        if (g_pstGh30xFrameInfo[uchFunCnt])
        {
            if (unFunc & (((GU32)1) << uchFunCnt))
            {
                g_pstGh30xFrameInfo[uchFunCnt]->punFrameCnt[0] = 0;
                g_pstGh30xFrameInfo[uchFunCnt]->pstFunctionInfo->uchDivderCnt = 0;
            }
        }
    }
}

void Gh30xDemoFuncEnable(GU32 unFuncMode)
{
    GU32 unRealFuncModeNeedOpen;
    GU32 unLastFuncMode;

    unLastFuncMode = g_unGh30xDemoFuncMode;
    g_unGh30xDemoFuncMode |= unFuncMode;
    unRealFuncModeNeedOpen = g_unGh30xDemoFuncMode ^ unLastFuncMode;

    EXAMPLE_DEBUG_LOG_L1("[%s] %x, %x\r\n", __FUNCTION__, g_unGh30xDemoFuncMode, unRealFuncModeNeedOpen);
    Gh30xFunctionFrameInfoInitAtEnable(unRealFuncModeNeedOpen);

    #if __USE_GOODIX_APP__
    Gh30xUpdateProtocolSendDataFunc();
    #endif

    GS8 ret = GH30X_AlgoInit(unFuncMode);

    if (GH30X_RET_OK != ret)
    {
        EXAMPLE_DEBUG_LOG_L1("[%s] GH30X_AlgoInit fail!\r\n", __FUNCTION__);
        return;
    }

    if (g_unGh30xDemoFuncMode & (~GH30X_FUNCTION_ADT))
    {
        EXAMPLE_DEBUG_LOG_L1("[%s] from NULL/ADT mode to Main func mode, we need diable ADT function\r\n", __FUNCTION__);
        g_uchGh30xIsAdtMode = 0;
        GH30X_AlgoDeinit(GH30X_FUNCTION_ADT);
    }
    else if (GH30X_FUNCTION_ADT == g_unGh30xDemoFuncMode)
    {
        g_uchGh30xIsAdtMode = 1;
    }
}

void Gh30xDemoFuncDisable(GU32 unFuncMode)
{
    if (g_unGh30xDemoFuncMode & (~GH30X_FUNCTION_ADT)) // from Main func mode to ADT mode, we need reset ADT function
    {
        g_uchGh30xIsAdtMode = 1;
        GH30X_AlgoDeinit(GH30X_FUNCTION_ADT);
        GH30X_AlgoInit(GH30X_FUNCTION_ADT);
    }

    g_unGh30xDemoFuncMode &= (~unFuncMode);

#if __USE_GOODIX_APP__
    Gh30xUpdateProtocolSendDataFunc();
#endif

    GH30X_AlgoDeinit(unFuncMode);
}

void gh30x_module_start(GU32 unFunctionMode)
{
    stSampleResetCfg stSampleResetCfgTemp = {0};

    if (GH30X_FUNCTION_HRV & unFunctionMode)  //if HRV is include, sample rate is 100 Hz
    {
        stSampleResetCfgTemp.usResetEn = RESET_CFG_SAMPLE_RATE_EN | RESET_CFG_FIFO_WATER_LINE_EN;
        stSampleResetCfgTemp.usResetSampleRate = 100;
        stSampleResetCfgTemp.uchResetFifoWaterLine = 50; // 25 * g_uchNeedReadRawdataCount, default 50
    }
    else if (GH30X_FUNCTION_ADT == unFunctionMode)
    {
        stSampleResetCfgTemp.usResetEn = RESET_CFG_SAMPLE_RATE_EN | RESET_CFG_FIFO_WATER_LINE_EN;
        stSampleResetCfgTemp.usResetSampleRate = 25;
        stSampleResetCfgTemp.uchResetFifoWaterLine = 25; // 25 * g_uchNeedReadRawdataCount, default 25
    }
    else
    {
        stSampleResetCfgTemp.usResetEn = RESET_CFG_SAMPLE_RATE_EN | RESET_CFG_FIFO_WATER_LINE_EN;
        stSampleResetCfgTemp.usResetSampleRate = 25;
        stSampleResetCfgTemp.uchResetFifoWaterLine = 50; // 25 * g_uchNeedReadRawdataCount, default 50
    }

    if (goodix_app_start_app_mode && GH30X_FUNCTION_HR == unFunctionMode)
    {
        stSampleResetCfgTemp.usResetEn = RESET_CFG_SAMPLE_RATE_EN | RESET_CFG_FIFO_WATER_LINE_EN;
        stSampleResetCfgTemp.usResetSampleRate = 25;
        stSampleResetCfgTemp.uchResetFifoWaterLine = 25; // 25 * g_uchNeedReadRawdataCount, default 25
    }

    Gh30xFunctionFrameInfoUpdate(unFunctionMode, &stSampleResetCfgTemp);

    EXAMPLE_DEBUG_LOG_L1("[%s] unFunctionMode = 0x%x, %d, %x, %d, %d\r\n", __FUNCTION__, unFunctionMode, g_uchNeedReadRawdataCount, \
        stSampleResetCfgTemp.usResetEn, stSampleResetCfgTemp.usResetSampleRate, stSampleResetCfgTemp.uchResetFifoWaterLine);

    gh30x_run_mode = (uint8_t)unFunctionMode;

    if (GH30X_FUNCTION_SPO2 & unFunctionMode)  //unFunctionMode has sop2, select spo2 cfg array priority
    {
        Gh30xDemoFuncEnable(unFunctionMode);
        Gh30xDemoSampleStart(spo2_reg_config_array, CONFIT_SIZE(spo2_reg_config_array), \
                            g_stGh30xSpo2ExtralCfg, CONFIT_SIZE(g_stGh30xSpo2ExtralCfg), \
                            &stSampleResetCfgTemp);
    }
    else if ((GH30X_FUNCTION_HR | GH30X_FUNCTION_HRV | GH30X_FUNCTION_SOFT_ADT) & unFunctionMode)  //unFunctionMode has
    {
        Gh30xDemoFuncEnable(unFunctionMode);
        Gh30xDemoSampleStart(hb_reg_config_array, CONFIT_SIZE(hb_reg_config_array), \
                            g_stGh30xHrExtralCfg, CONFIT_SIZE(g_stGh30xHrExtralCfg), \
                            &stSampleResetCfgTemp);
    }
    else if (GH30X_FUNCTION_ADT == unFunctionMode)
    {
        Gh30xDemoFuncEnable(unFunctionMode);
        Gh30xDemoSampleStart(hb_adt_confirm_reg_config, CONFIT_SIZE(hb_adt_confirm_reg_config), \
                            g_stGh30xAdtExtralCfg, CONFIT_SIZE(g_stGh30xAdtExtralCfg), \
                            &stSampleResetCfgTemp);
    }
    else
    {
        EXAMPLE_DEBUG_LOG_L1("[%s] unFunctionMode is invalid! Please cheak. unFunctionMode = 0x%X.\r\n", __FUNCTION__, unFunctionMode);
        while(1);
    }
}

void gh30x_module_stop(void)
{
    EXAMPLE_DEBUG_LOG_L1("[%s].\r\n", __FUNCTION__);
    Gh30xDemoFuncDisable(GH30X_FUNCTION_ALL);
    Gh30xDemoSampleStop();
}

void Gh30xFunctionFrameInfoUpdate(GU32 unFunctionMode, stSampleResetCfg *stSampleCfg)
{
    if (NULL == stSampleCfg || GH30X_FUNCTION_ADT == unFunctionMode)
    {
        return;
    }

    for (uint32_t i = GH30X_FUNC_OFFSET_HR; i < GH30X_FUNC_OFFSET_PROTOCOL; i++)
    {
        if (((0x1 << i) & unFunctionMode) && g_pstGh30xFrameInfo[i]->pstFunctionInfo->usOutputDataRate > stSampleCfg->usResetSampleRate)
        {
            EXAMPLE_DEBUG_LOG_L1("[%s] func:%d fs update to %d.\r\n", __FUNCTION__, i, stSampleCfg->usResetSampleRate);
            g_pstGh30xFrameInfo[i]->pstFunctionInfo->usOutputDataRate = stSampleCfg->usResetSampleRate;
        }
    }

    return;
}

void Gh30xFunctionFrameInfoBaseInit(void)
{
    g_unGh30xDemoFuncMode = 0;
    g_uchGh30xIsAdtMode = 0;

    #if __FUNC_TYPE_ADT_ENABLE__
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_ADT]->pstFunctionInfo->uchChnlNum = 1;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_ADT]->pstFunctionInfo->usOutputDataRate = 25;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_ADT]->puchChnlMap[0] = CHNL_MAP_TO_FIFO_PHASE0_DATA;
    #endif

    #if __FUNC_TYPE_HR_ENABLE__
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_HR]->pstFunctionInfo->uchChnlNum = 1;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_HR]->pstFunctionInfo->usOutputDataRate = 25;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_HR]->puchChnlMap[0] = CHNL_MAP_TO_FIFO_PHASE0_DATA;
    #endif

    #if __FUNC_TYPE_HRV_ENABLE__
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_HRV]->pstFunctionInfo->uchChnlNum = 1;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_HRV]->pstFunctionInfo->usOutputDataRate = 100;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_HRV]->puchChnlMap[0] = CHNL_MAP_TO_FIFO_PHASE0_DATA;
    #endif

    #if __FUNC_TYPE_SPO2_ENABLE__
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_SPO2]->pstFunctionInfo->uchChnlNum = 2;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_SPO2]->pstFunctionInfo->usOutputDataRate = 100;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_SPO2]->puchChnlMap[0] = CHNL_MAP_TO_FIFO_PHASE0_DATA;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_SPO2]->puchChnlMap[1] = CHNL_MAP_TO_FIFO_PHASE1_DATA;
    #endif

    #if __FUNC_TYPE_SOFT_ADT_ENABLE__
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_SOFT_ADT]->pstFunctionInfo->uchChnlNum = 2;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_SOFT_ADT]->pstFunctionInfo->usOutputDataRate = 25;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_SOFT_ADT]->puchChnlMap[0] = CHNL_MAP_TO_FIFO_PHASE0_DATA;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_SOFT_ADT]->puchChnlMap[1] = CHNL_MAP_TO_FIFO_PHASE1_DATA;
    #endif

    #if __USE_GOODIX_APP__
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_PROTOCOL]->pstFunctionInfo->uchChnlNum = 2;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_PROTOCOL]->pstFunctionInfo->usOutputDataRate = 200;  //if usOutputDataRate is bigger than chip sample rate, uchDivder is 0 or 1
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_PROTOCOL]->puchChnlMap[0] = CHNL_MAP_TO_FIFO_PHASE0_DATA;
    g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_PROTOCOL]->puchChnlMap[1] = CHNL_MAP_TO_FIFO_PHASE1_DATA;
    #endif
}

void GH30X_RegisterHookFunc(void (*pInitHookFunc)(void),
                                void (*pGetRawdataHookFunc)(GU32 *puchReadDataBuffer, GU16 usReadDataLen),
                                void (*pGetIoStructInfoHookFunc)(const STGh30xFrameInfo *const pstFrameInfo))
{
    g_pGh30xInitHookFunc = pInitHookFunc;
    g_pGh30xGetRawdataHookFunc = pGetRawdataHookFunc;
    g_pGh30xGetIoStructInfoHookFunc = pGetIoStructInfoHookFunc;
}

//============================================================================================================
#if (__GET_RAWDATA_WITHOUT_ALGO_HANDLE)
//STGh30xFactoryRawdata stFactoryRawdata = {0};
#endif

extern GU32 *g_punGh301xAlgoMemBufferPointer;

#if (__HBD_ALGORITHM_USE_DYN_MEM__)
GU8 g_MemReady = 0;
GU8 *g_pMem = NULL;
#endif

/// app mode start flag
bool goodix_app_start_app_mode = false;

/// system test mode start flag
uint8_t goodix_system_test_mode = 0;

/// system test os led num
uint8_t goodix_system_test_os_led_num = 0;

/// adt confirm flag
bool adt_confirm_flag = false; 
/// start flag without adt confirm flag
bool hb_start_without_adt_confirm = false; 


/// gsensor fifo index and len
//static uint16_t gsensor_soft_fifo_buffer_index = 0;
/// gsnesor fifo
//static ST_GS_DATA_TYPE gsensor_soft_fifo_buffer[GSENSOR_SOFT_FIFO_BUFFER_MAX_LEN];

/// gh30x run mode
uint8_t gh30x_run_mode = RUN_MODE_INVALID; 

/// gh30x_hardware adt mode
EMAdtMode gemHardwareAdtMode = WEAR_DETECTION;

//char gcCompileTime[40] = {0};

/****************************************************************
* Description: Get hbd lib compile time
* Input:    None,
* Return: hbd lib compile time string
******************************************************************/
#if 0
GS8 * GH3011_GetHbdCompileTime(void)
{
    GU8 uchIndex = 0;
    GU8 strlen = 0;
    GS8 *str = (GS8*)__DATE__;
    while('\0' != str[strlen] && 39 > uchIndex)
    {
        gcCompileTime[uchIndex++] = str[strlen++];
        if(39 <= uchIndex)
        {
            break;
        }
    }
    gcCompileTime[uchIndex++] = ' ';
    str = (GS8*)__TIME__;
    strlen = 0;
    while('\0' != str[strlen] && 39 > uchIndex)
    {
        gcCompileTime[uchIndex++] = str[strlen++];
        if(39 <= uchIndex)
        {
            break;
        }
    }
    gcCompileTime[uchIndex] = '\0';
    
    return (GS8 *)gcCompileTime;
}
#endif

/// gh30x module init, include gsensor init
static bool first_init_flag = true;
int gh30x_module_init(void)
{
    GS8 init_err_flag = HBD_RET_OK;
    GCHAR *hbd_ver = Gh30xHBDVersionGet();
    
    printf("%s", hbd_ver);
    printf(GH30X_EXAMPLE_VER_STRING);
    printf("SDK code version: %s\r\n", HBD_GetHbdVersion());

    if (first_init_flag)
    {
#if (__GH30X_COMMUNICATION_INTERFACE__ == GH30X_COMMUNICATION_INTERFACE_SPI)
        hal_gh30x_spi_init(); // spi init
        HBD_SetI2cRW(HBD_I2C_ID_SEL_1L0L, gh30x_i2c_write_exchange_to_spi, gh30x_i2c_read_exchange_to_spi); // register i2c exchange to spi api
#elif (__GH30X_COMMUNICATION_INTERFACE__ == GH30X_COMMUNICATION_INTERFACE_I2C)
        hal_gh30x_i2c_init(); // i2c init
        HBD_SetI2cRW(HBD_I2C_ID_SEL_1L0L, hal_gh30x_i2c_write, hal_gh30x_i2c_read); // register i2c RW func api
#else
        EXAMPLE_DEBUG_LOG_L1("[gh30x_module_init] communication interface undefine!");
#endif

#if (__PLATFORM_DELAY_US_CONFIG__)
        HBD_SetDelayUsCallback(hal_gh30x_delay_us);
#endif
        GH30X_RegisterHookFunc(Gh30xInitHookFunc, Gh30xGetRawdataHookFunc, Gh30xFrameDataHookFunc);
    }

    /* log all version string */
    for (GS32 i = 0; i < GH30X_FUNC_OFFSET_MAX; i++)
    {
        GH30X_AlgoVersion(i);
    }

    /* init gh30x */
    init_err_flag = HBD_SimpleInit(&gh30x_init_config);
    if (HBD_RET_OK != init_err_flag)
    {
        EXAMPLE_DEBUG_LOG_L1("gh30x init error[%s]\r\n", dbg_ret_val_string[DEBUG_HBD_RET_VAL_BASE + init_err_flag]);
        return GH30X_EXAMPLE_ERR_VAL;
    }

    /* init gsensor */
    init_err_flag = gsensor_drv_init();
    if (GH30X_EXAMPLE_ERR_VAL == init_err_flag)  
    {
        EXAMPLE_DEBUG_LOG_L1("gsensor init error\r\n");
        return GH30X_EXAMPLE_ERR_VAL;
    }

    /* init hardware */
    if (first_init_flag)
    {
        hal_gsensor_int1_init(); // gsensor int pin init
        hal_gh30x_int_init(); // gh30x int pin init

        #if (__GH30X_IRQ_PLUSE_WIDTH_CONFIG__)
        HBD_SetIrqPluseWidth(255); // set Irq pluse width (255us)
        #endif

        #if (__USE_GOODIX_APP__) || (__UART_WITH_GOODIX_TOOLS__)
        gh30x_comm_pkg_init(); // comm pkg init
        #endif
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_INIT();
    }

    /* init variable */
    g_uchGh30xMotionDetEn = 0;
    g_uchGh30xSoftEvent = 0;
    g_uchGh30xLivingObjectFlag = 0;
    Gh30xFunctionFrameInfoBaseInit();

    /* init algo memory etc */
    //HOOK_FUNC_CALL(g_pGh30xInitHookFunc, ());
    if(g_pGh30xInitHookFunc)
        g_pGh30xInitHookFunc();

    first_init_flag = false;
    printf("gh30x module init ok\n");

    return GH30X_EXAMPLE_OK_VAL;  
}

void CtrlGh30xModuleStop(void)
{
    gh30x_module_stop();

    return;
}

void CtrlGh30xModuleStart(uint8_t work_type)
{
    EMGh30xRunMode start_run_mode;

    gh30x_module_stop();

#if 1
    if(work_type == SensorWorkHr)
        start_run_mode = HR_WORK_MODE;
    else if(work_type == SensorWorkBo)
        start_run_mode = BO_WORK_MODE;
    else
        start_run_mode = HR_WORK_MODE;
#else

    if(work_type == SensorWorkHr)
        start_run_mode = \
            GH30X_FUNCTION_ADT;
    else if(work_type == SensorWorkBo)
        start_run_mode = \
            GH30X_FUNCTION_ADT;
    else
        start_run_mode = \
            GH30X_FUNCTION_ADT;
#endif

    gh30x_module_start(start_run_mode);

    printf("%s:%d\n", \
        __func__, start_run_mode);

    return;
}
#if 0
/// gh30x module start, with adt 
void gh30x_module_start(EMGh30xRunMode start_run_mode)
{
    #if ((__HB_DET_SUPPORT__) && (__HB_NEED_ADT_CONFIRM__))
    if (start_run_mode == RUN_MODE_ADT_HB_DET)
    {
        hb_start_without_adt_confirm = false;
    }
    #endif
    gh30x_run_mode = (uint8_t)start_run_mode;
    g_uchGh30xRunMethod = 0;
    EXAMPLE_DEBUG_LOG_L1("gh30x module start, mode [%s]\r\n", dbg_rum_mode_string[gh30x_run_mode]);
    if (gh30x_run_mode == RUN_MODE_SPO2_DET)
    {
        SEND_MCU_SPO2_UNWEAR_EVT(NULL, 0); // send start cmd with unwear evt data
    }
    gsensor_enter_normal_and_clear_buffer();
    gh30x_start_adt_with_mode((uint8_t)start_run_mode);
}


/// gh30x module start, without adt 
void gh30x_module_start_without_adt(EMGh30xRunMode start_run_mode)
{
    #if ((__HB_DET_SUPPORT__) && (__HB_NEED_ADT_CONFIRM__))
    if (start_run_mode == RUN_MODE_ADT_HB_DET)
    {
        hb_start_without_adt_confirm = true;
    }
    #endif
    gh30x_run_mode = (uint8_t)start_run_mode;
    g_uchGh30xRunMethod = 1;
    EXAMPLE_DEBUG_LOG_L1("gh30x module start without adt, mode [%s]\r\n", dbg_rum_mode_string[gh30x_run_mode]);
    gsensor_enter_clear_buffer_and_enter_fifo();
    gh30x_start_func_with_mode((uint8_t)start_run_mode);
}
#endif

#if 0
/// gh30x module stop
void gh30x_module_stop(void)
{
    gsensor_enter_normal_and_clear_buffer();
    gh30x_stop_func();
    gh30x_run_mode = RUN_MODE_INVALID;
    EXAMPLE_DEBUG_LOG_L1("gh30x module stop\r\n");
    #if (__USE_GOODIX_APP__)
    goodix_app_start_app_mode = false; // if call stop, clear app mode
    #endif
}
#endif

#if (__HBD_ALGORITHM_USE_DYN_MEM__)
/****************************************************************
* Description: Get memory size of hbd_ctrl lib need
* Input: GU8 uchMemoryLevel   0, Maximum memory required, full function supoort
*                             1, Less than level 0 memory required, HBwithHRV function will unsupport.
* Return: size in bytes
******************************************************************/
GU32 HBD_GetMemRequired(GU8 uchMemoryLevel)
{
    GU32 nMemoryRequired = 64;//prepare some bytes for align
    nMemoryRequired += (__HBD_ALGORITHM_MEM_SIZE__ * 4);
    if(1 == uchMemoryLevel)
    {
        nMemoryRequired -= (2 * 1024 + 512);
    }

    return nMemoryRequired;
}

/****************************************************************
* Description: Align memory addr
* Input: GU8 *pMem   pointer of
******************************************************************/
#define HBD_MEM_ADDR_ALIGN_BYTES (4)
static GU32* HBD_MemAddrAlign(void *pMem)
{
    GU32 memaddr = (GU32)pMem;
    GU8 index = 0;

    for(; index < HBD_MEM_ADDR_ALIGN_BYTES; ++index)
    {
        if(0 == (memaddr % HBD_MEM_ADDR_ALIGN_BYTES))
        {
            break;
        }
        ++memaddr;
    }
    return (GU32 *)memaddr;
}
                                                                    
/****************************************************************
* Description: Get memory size of hbd_ctrl lib need
* Input: GU8 *pMem   pointer of
******************************************************************/
void HBD_SetMemPtr(GU8 *pMem)
{
    g_punGh301xAlgoMemBufferPointer = HBD_MemAddrAlign(pMem);
}

void gh30x_malloc_memory(void)
{
    if(0 == g_MemReady)
    {
        GS32 nMemSize = HBD_GetMemRequired(0);
        
        g_pMem = hal_gh30x_memory_malloc(nMemSize);
        if(NULL != g_pMem)
        {            
            g_MemReady = 1;
        }
    }
    if(0 != g_pMem)
    {
        HBD_SetMemPtr(g_pMem);
    }
}

void gh30x_free_memory(void)
{
    g_MemReady = 0;
    if(NULL != g_pMem)
    {
        hal_gh30x_memory_free(g_pMem);
        g_pMem = NULL;
    }    
}
#endif

/// gh30x reset evt handler
void gh30x_reset_evt_handler(void)
{
    GS8 reinit_ret = HBD_RET_OK;
    GU8 reinit_cnt = __RESET_REINIT_CNT_CONFIG__;

    gsensor_enter_normal_and_clear_buffer();
    // reinit
    do 
    {
        reinit_ret = HBD_SimpleInit(&gh30x_init_config);
        reinit_cnt --;
    } while (reinit_ret != HBD_RET_OK);

    if ((reinit_ret == HBD_RET_OK) && (gh30x_run_mode != RUN_MODE_INVALID)) // if reinit ok, restart last mode
    {
        #if 0
        #if (__USE_GOODIX_APP__)
        if (goodix_app_start_app_mode)
        {
            SEND_GH30X_RESET_EVT();
        }
        else
        #endif
        {
            gh30x_start_adt_with_mode(gh30x_run_mode);
        }
        #endif
    }
    EXAMPLE_DEBUG_LOG_L1("got gh30x reset evt, reinit [%s]\r\n", dbg_ret_val_string[DEBUG_HBD_RET_VAL_BASE + reinit_ret]);
}

/// gh30x unwear evt handler
void gh30x_unwear_evt_handler(void)
{
    EXAMPLE_DEBUG_LOG_L1("got gh30x hardware wear off evt.\r\n");
    Gh30xAdtWearDetectStart(GH30X_ADT_MODE_DET_WEAR_ON);
    handle_wear_status_result(WEAR_STATUS_UNWEAR, 0);
}

/// gh30x wear evt handler
void gh30x_wear_evt_handler(void)
{
    EXAMPLE_DEBUG_LOG_L1("got gh30x hardware wear on evt.\r\n");
    g_uchGh30xMotionDetEn = 0;
    Gh30xAdtWearDetectStart(GH30X_ADT_MODE_DET_WEAR_OFF);
    handle_wear_status_result(WEAR_STATUS_WEAR, 0);
}

/// calc unwear status handle
void gh30x_handle_calc_unwear_status(void)
{
#if 0
    gsensor_enter_normal_and_clear_buffer();
    if (gh30x_run_mode == RUN_MODE_ADT_HB_DET)
    {
        #if (__HB_START_WITH_GSENSOR_MOTION__)
        gsensor_drv_enter_motion_det_mode();
        EXAMPLE_DEBUG_LOG_L1("calc unwear status, start gsensor motion\r\n");
        #else
        gh30x_start_adt_with_mode(gh30x_run_mode);
        EXAMPLE_DEBUG_LOG_L1("calc unwear status, start adt detect\r\n");
        #endif
    }
    else
    {
        gh30x_start_adt_with_mode(gh30x_run_mode);
        EXAMPLE_DEBUG_LOG_L1("calc unwear status, start adt detect\r\n");
    }
    handle_wear_status_result(WEAR_STATUS_UNWEAR, 0);
#endif
}

#if (__HB_DET_SUPPORT__)
/// fifo evt hb mode calc
#if 0
static void gh30x_fifo_evt_hb_mode_calc(GS32 *dbg_rawdata_ptr)
{
    GU16 dbg_rawdata_len = __ALGO_CALC_DBG_BUFFER_LEN__;
    #if (__HB_NEED_ADT_CONFIRM__)
    if (adt_confirm_flag)
    {
        GU8 adt_confirm_res = HBD_AdtConfirmCalculateByFifoIntDbgOutputData(gsensor_soft_fifo_buffer, gsensor_soft_fifo_buffer_index, 
                                    __GS_SENSITIVITY_CONFIG__, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, &dbg_rawdata_len);
        EXAMPLE_DEBUG_LOG_L1("adt confirm calc, gs_len=%d, result=0x%x\r\n", gsensor_soft_fifo_buffer_index, adt_confirm_res);
        if (adt_confirm_res != ADT_CONFRIM_STATUS_DETECTING)
        {
            adt_confirm_flag = false;
            HBD_Stop();
            if (adt_confirm_res == ADT_CONFRIM_STATUS_WEAR)
            {
                gh30x_module_start_without_adt((EMGh30xRunMode)gh30x_run_mode);
                HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
                //close here because do not need store adt confirm data in csv
                //SEND_MCU_HB_MODE_WEAR_STATUS(WEAR_STATUS_WEAR, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
                handle_wear_status_result(WEAR_STATUS_WEAR, 0);
            }
            else if (adt_confirm_res == ADT_CONFRIM_STATUS_UNWEAR)
            {
                //close here because do not need store adt confirm data in csv
                //SEND_MCU_HB_MODE_WEAR_STATUS(WEAR_STATUS_UNWEAR, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
                gh30x_handle_calc_unwear_status();
            }
        }
        EXAMPLE_LOG_RAWDARA("adt confirm calc:\r\n", dbg_rawdata_ptr, dbg_rawdata_len);
    }
    else
    #endif
#ifdef __HBD_API_EX__
    {
        ST_HB_RES stHbRes = {0};
        GU8 voice_broadcast = 0;
        GU16 rr_value = 0;
        GU8 hb_res = 0;
        GU8 currentarr[3] = {0};
#if (__ALGO_CALC_WITH_DBG_DATA__)
        GU16 current = 0;
        current = HBD_I2cReadReg(0x0122);
        currentarr[0] = current & 0xFF;
        currentarr[1] = (current >> 8) & 0xFF;
        current = HBD_I2cReadReg(0x0124);
        currentarr[2] = current & 0xFF;
#endif
        hb_res = HBD_HbCalculateByFifoIntEx(gsensor_soft_fifo_buffer, gsensor_soft_fifo_buffer_index, __GS_SENSITIVITY_CONFIG__, 
                        (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, &dbg_rawdata_len, &stHbRes);
        (void)hb_res;
        (void)currentarr;
        handle_hb_mode_result(&stHbRes, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
        EXAMPLE_DEBUG_LOG_L1("hb calc, gs_len=%d, result=%d,%d,%d,%d\r\n", gsensor_soft_fifo_buffer_index, stHbRes.uchHbValue, stHbRes.uchAccuracyLevel, stHbRes.uchWearingState, stHbRes.uchWearingQuality);
        if (stHbRes.uchWearingState == WEAR_STATUS_UNWEAR)
        {
            SEND_MCU_HB_MODE_RESULT(stHbRes.uchHbValue, stHbRes.uchAccuracyLevel, WEAR_STATUS_UNWEAR, stHbRes.uchWearingQuality, voice_broadcast, hb_res, rr_value, currentarr, 
                                    (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len, stHbRes.uchScene, stHbRes.uchSNR, stHbRes.uchMotionState, HBD_HbaGetSleepFlag());
            HBD_Stop();
            gh30x_handle_calc_unwear_status();
        }
        else
        {
            HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
            SEND_MCU_HB_MODE_RESULT(stHbRes.uchHbValue, stHbRes.uchAccuracyLevel, WEAR_STATUS_WEAR, stHbRes.uchWearingQuality, voice_broadcast, hb_res, rr_value, currentarr, 
                                    (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len, stHbRes.uchScene, stHbRes.uchSNR, stHbRes.uchMotionState, HBD_HbaGetSleepFlag());
        }
        BLE_MODULE_SEND_HB(stHbRes.uchHbValue);
        EXAMPLE_LOG_RAWDARA("hb calc:\r\n", dbg_rawdata_ptr, dbg_rawdata_len);
    }
#else //__HBD_API_EX__
    {
        GU8 hb_value = 0;
        GU8 hb_value_lvl = 0;
        GU8 wearing_state = 0;
        GU8 wearing_quality = 0;
        GU8 voice_broadcast = 0;
        GU16 rr_value = 0;
        GU8 hb_res = 0;
        GU8 currentarr[3] = {0};
#if (__ALGO_CALC_WITH_DBG_DATA__)
        GU16 current = 0;
        current = HBD_I2cReadReg(0x0122);
        currentarr[0] = current & 0xFF;
        currentarr[1] = (current >> 8) & 0xFF;
        current = HBD_I2cReadReg(0x0124);
        currentarr[2] = current & 0xFF;
#endif
        hb_res = HBD_HbCalculateWithLvlByFifoIntDebugOutputData(gsensor_soft_fifo_buffer, gsensor_soft_fifo_buffer_index, __GS_SENSITIVITY_CONFIG__, 
                        &hb_value, &hb_value_lvl, &wearing_state, &wearing_quality, &voice_broadcast, &rr_value, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, &dbg_rawdata_len);
        (void)hb_res;
        (void)currentarr;
        //handle_hb_mode_result(hb_value, hb_value_lvl, wearing_state, rr_value, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
        EXAMPLE_DEBUG_LOG_L1("hb calc, gs_len=%d, result=%d,%d,%d,%d\r\n", gsensor_soft_fifo_buffer_index, hb_value, hb_value_lvl, wearing_state, rr_value);
        if (wearing_state == WEAR_STATUS_UNWEAR)
        {
            SEND_MCU_HB_MODE_RESULT(hb_value, hb_value_lvl, WEAR_STATUS_UNWEAR, wearing_quality, voice_broadcast, hb_res, rr_value, currentarr, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
            HBD_Stop();
            gh30x_handle_calc_unwear_status();
        }
        else
        {
            HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
            SEND_MCU_HB_MODE_RESULT(hb_value, hb_value_lvl, WEAR_STATUS_WEAR, wearing_quality, voice_broadcast, hb_res, rr_value, currentarr, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
        }
        BLE_MODULE_SEND_HB(hb_value);
        EXAMPLE_LOG_RAWDARA("hb calc:\r\n", dbg_rawdata_ptr, dbg_rawdata_len);
    }
#endif //__HBD_API_EX__
}
#endif
#endif

#if (__GET_RAWDATA_ONLY_SUPPORT__)
/// fifo evt getrawdata mode
static void gh30x_fifo_evt_getrawdata_mode_only(GS32 *dbg_rawdata_ptr)
{
    GU16 dbg_rawdata_len = __ALGO_CALC_DBG_BUFFER_LEN__;
    GS32 rawdata[__GET_RAWDATA_BUF_LEN__ ][2];
    GU8 currentarr[3] = {0};

    GU16 current = 0;
    current = HBD_I2cReadReg(0x0122);
    currentarr[0] = current & 0xFF;
    currentarr[1] = (current >> 8) & 0xFF;
    current = HBD_I2cReadReg(0x0124);
    currentarr[2] = current & 0xFF;

    GU8 nRes = 0;
    nRes = HBD_GetRawdataByFifoInt((GU8) __GET_RAWDATA_BUF_LEN__, (GS32 (*)[2])rawdata, &dbg_rawdata_len);
    HBD_GetRawdataHasDone();
    GU8 i = 0;
    static GU8 uchPackId = 0;
    for (i = 0; i < dbg_rawdata_len && NULL != dbg_rawdata_ptr; i++)
    {
        dbg_rawdata_ptr[i * 6] = rawdata[i][0];
        dbg_rawdata_ptr[i * 6 + 1] = rawdata[i][1];
        dbg_rawdata_ptr[i * 6 + 2] = 0;
        dbg_rawdata_ptr[i * 6 + 3] = 0;
        dbg_rawdata_ptr[i * 6 + 4] = 0;
        dbg_rawdata_ptr[i * 6 + 5] = uchPackId++;
    }
    (void)currentarr;
    GU8 wearing_state = WEAR_STATUS_WEAR;
    if(1 == nRes)
    {
        wearing_state = WEAR_STATUS_UNWEAR;
    }
    handle_getrawdata_mode_result(wearing_state, rawdata, dbg_rawdata_len);
    EXAMPLE_DEBUG_LOG_L1("get rawdata only, rawdata_len=%d\r\n", dbg_rawdata_len);
    EXAMPLE_DEBUG_LOG_L1("rawdata:%ld,%ld\n",(rawdata[0][0]&0x1ffff),(rawdata[0][1]&0x1ffff));

    if (wearing_state == WEAR_STATUS_UNWEAR)
    {
        SEND_MCU_HB_MODE_RESULT(0, 0, WEAR_STATUS_UNWEAR, 0, 0, 0, 0, currentarr, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len,0,0,0,0);
        HBD_Stop();
        gh30x_handle_calc_unwear_status();
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
        SEND_MCU_HB_MODE_RESULT(0, 0, WEAR_STATUS_WEAR, 0, 0, 0, 0, currentarr, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len,0,0,0,0);
    }
    EXAMPLE_LOG_RAWDARA("get rawdata only:\r\n", dbg_rawdata_ptr, dbg_rawdata_len);
}
#endif

#if(__FACTORY_DET_SUPPORT__)
static void gh30x_fifo_evt_factory_mode_calc(GS32 *dbg_rawdata_ptr)
{
#if (__GET_RAWDATA_WITHOUT_ALGO_HANDLE)
    GU16 usRawdataLen = 0;
    GS32 puchRawdataBuffer[GH30X_RAW_DATA_LEN][2] = {0};
    GU8 currentarr[3] = {0};
    GU16 current = 0;
    current = HBD_I2cReadReg(0x0122);
    currentarr[0] = current & 0xFF;
    currentarr[1] = (current >> 8) & 0xFF;
    current = HBD_I2cReadReg(0x0124);
    currentarr[2] = current & 0xFF;

    HBD_GetRawdataByFifoInt(GH30X_RAW_DATA_LEN, puchRawdataBuffer, &usRawdataLen);  //read FIFO
    if(usRawdataLen > GH30X_RAW_DATA_LEN)
    {
        EXAMPLE_DEBUG_LOG_L1("rawdata overflow, real len = %d \r\n", usRawdataLen);
        usRawdataLen = GH30X_RAW_DATA_LEN;
    }

    for (GU8 uchIndex = 0; uchIndex < usRawdataLen; uchIndex++)
    {
        puchRawdataBuffer[uchIndex][0] &= 0xFFFFFF;
        puchRawdataBuffer[uchIndex][1] &= 0xFFFFFF;
    }

    if(usRawdataLen > 0)
    {
        if(gstFactoryModeCfg.now_state  == FACTORY_GREEN_IR_STATE)
        {
            g_UNGh30xRawdataBuf.stFactoryRawdata.gre1_num = usRawdataLen;
            g_UNGh30xRawdataBuf.stFactoryRawdata.ir_num = usRawdataLen;

            g_UNGh30xRawdataBuf.stFactoryRawdata.gre1_led_curr = currentarr[0];
            g_UNGh30xRawdataBuf.stFactoryRawdata.ir_led_curr = currentarr[1];

            for (GU8 uchIndex = 0; uchIndex < usRawdataLen; uchIndex++)
            {
                g_UNGh30xRawdataBuf.stFactoryRawdata.gre1Buf[uchIndex] = puchRawdataBuffer[uchIndex][0] & 0xFFFFFF;    //green
                g_UNGh30xRawdataBuf.stFactoryRawdata.IrBuf[uchIndex] = puchRawdataBuffer[uchIndex][1] & 0xFFFFFF;   //ir
            }
        }
        else if(gstFactoryModeCfg.now_state  == FACTORY_IR_RED_STATE)
        {
            g_UNGh30xRawdataBuf.stFactoryRawdata.ir_num = usRawdataLen;
            g_UNGh30xRawdataBuf.stFactoryRawdata.red_num = usRawdataLen;
            g_UNGh30xRawdataBuf.stFactoryRawdata.ir_led_curr = currentarr[0];
            g_UNGh30xRawdataBuf.stFactoryRawdata.red_led_curr = currentarr[1];

            for (GU8 uchIndex = 0; uchIndex < usRawdataLen; uchIndex++)
            {
                g_UNGh30xRawdataBuf.stFactoryRawdata.IrBuf[uchIndex] = puchRawdataBuffer[uchIndex][0] & 0xFFFFFF;   //ir
                g_UNGh30xRawdataBuf.stFactoryRawdata.redBuf[uchIndex] = puchRawdataBuffer[uchIndex][1] & 0xFFFFFF;    //red
            }
            handle_factory_mode_result(&g_UNGh30xRawdataBuf.stFactoryRawdata);
            memset(&g_UNGh30xRawdataBuf.stFactoryRawdata, 0, sizeof(g_UNGh30xRawdataBuf.stFactoryRawdata));
        }
        else
        {
            return;
        }
        gh30x_module_stop();
        gh30x_module_start_without_adt(GH30X_RUN_MODE_FACTORY);
    }
#endif
}
#endif

#if (__HR_SPO2_DET_SUPPORT__)
static void gh30x_fifo_evt_hr_spo2_mode_calc(GS32 *dbg_rawdata_ptr)
{
    GU16 dbg_rawdata_len = __ALGO_CALC_DBG_BUFFER_LEN__;
    ST_SPO2_RES stSpo2Res = {0};
    GU8 abnormal_state = 0;
    (void)abnormal_state;
    GU8 currentarr[3] = { 0 };

    GU16 current = 0;
    current = HBD_I2cReadReg(0x0122);
    currentarr[0] = current & 0xFF;
    currentarr[1] = (current >> 8) & 0xFF;
    current = HBD_I2cReadReg(0x0124);
    currentarr[2] = current & 0xFF;

    HBD_HrSpo2CalculateByFifoIntEx(  gsensor_soft_fifo_buffer, gsensor_soft_fifo_buffer_index, __GS_SENSITIVITY_CONFIG__,
                                (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, &dbg_rawdata_len, &stSpo2Res);

    handle_spo2_mode_result(&stSpo2Res, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
    EXAMPLE_DEBUG_LOG_L1("spo2 calc, gs_len=%d, result=%d,%f,%d,%d,%d,%d,%d\r\n", gsensor_soft_fifo_buffer_index, 
                        stSpo2Res.uchSpo2, stSpo2Res.fSpo2, stSpo2Res.uchHbValue, stSpo2Res.uchHbConfidentLvl, 
                        stSpo2Res.usSpo2RVal,stSpo2Res.uliSpo2InvlaidFlag, stSpo2Res.uchWearingState);

    if (stSpo2Res.uchWearingState == WEAR_STATUS_UNWEAR)
    {
        SEND_MCU_SPO2_UNWEAR_EVT((GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);	
        HBD_Stop();
        gh30x_handle_calc_unwear_status();
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
        FIXED_SPO2_ALGO_RES(stSpo2Res.uchSpo2);
        SEND_MCU_SPO2_MODE_RESULT(&stSpo2Res, abnormal_state, currentarr, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
    }
    EXAMPLE_LOG_RAWDARA("spo2 calc:\r\n", dbg_rawdata_ptr, dbg_rawdata_len);

}

#endif


#if (__SPO2_DET_SUPPORT__)
#ifdef __HBD_API_EX__
#if 0
/// fifo evt spo2 mode calc
static void gh30x_fifo_evt_spo2_mode_calc(GS32 *dbg_rawdata_ptr)
{
    GU16 dbg_rawdata_len = __ALGO_CALC_DBG_BUFFER_LEN__;
    ST_SPO2_RES stSpo2Res = {0};
    GU8 abnormal_state = 0;
    (void)abnormal_state;    
    GU8 currentarr[3] = { 0 };
    #if (__ALGO_CALC_WITH_DBG_DATA__)
    GU16 current = 0;
    current = HBD_I2cReadReg(0x0122);
    currentarr[0] = current & 0xFF;
    currentarr[1] = (current >> 8) & 0xFF;
    current = HBD_I2cReadReg(0x0124);
    currentarr[2] = current & 0xFF;
    #endif
    HBD_Spo2CalculateByFifoIntEx(gsensor_soft_fifo_buffer, gsensor_soft_fifo_buffer_index, __GS_SENSITIVITY_CONFIG__,
                                (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, &dbg_rawdata_len, &stSpo2Res);
    handle_spo2_mode_result(&stSpo2Res, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
    EXAMPLE_DEBUG_LOG_L1("spo2 calc, gs_len=%d, result=%d,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", gsensor_soft_fifo_buffer_index, 
                        stSpo2Res.uchSpo2, stSpo2Res.fSpo2, stSpo2Res.uchHbValue, stSpo2Res.uchHbConfidentLvl, 
                        stSpo2Res.usHrvRRVal[0], stSpo2Res.usHrvRRVal[1], stSpo2Res.usHrvRRVal[2], stSpo2Res.usHrvRRVal[3], 
                        stSpo2Res.uchHrvConfidentLvl, stSpo2Res.uchHrvcnt, stSpo2Res.usSpo2RVal, stSpo2Res.uchWearingState);

    if (stSpo2Res.uchWearingState == WEAR_STATUS_UNWEAR)
    {
        SEND_MCU_SPO2_UNWEAR_EVT((GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);						
        HBD_Stop();
        gh30x_handle_calc_unwear_status();
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
        FIXED_SPO2_ALGO_RES(stSpo2Res.uchSpo2);
        SEND_MCU_SPO2_MODE_RESULT(&stSpo2Res, abnormal_state, currentarr, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
    }
    EXAMPLE_LOG_RAWDARA("spo2 calc:\r\n", dbg_rawdata_ptr, dbg_rawdata_len);
}
#endif
#else
/// fifo evt spo2 mode calc
static void gh30x_fifo_evt_spo2_mode_calc(GS32 *dbg_rawdata_ptr)
{
    GU16 dbg_rawdata_len = __ALGO_CALC_DBG_BUFFER_LEN__;
    GU8 spo2_value = 0;
    GU8 spo2_lvl = 0;
    GU8 hb_value = 0;
    GU8 hb_lvl = 0;
    GU16 hrv_val[4] = {0};
    GU8 hrv_lvl = 0;
    GU8 hrv_cnt = 0; 
    GU16 spo2_r_value = 0;
    GU8 wearing_state = 0;
    GU8 valid_lvl = 0;
    GU8 abnormal_state = 0;
    GS32 pliSpo2InvalidFlag = 0;
    (void)abnormal_state;    
    GU8 currentarr[3] = { 0 };

    GU16 current = 0;
    current = HBD_I2cReadReg(0x0122);
    currentarr[0] = current & 0xFF;
    currentarr[1] = (current >> 8) & 0xFF;
    current = HBD_I2cReadReg(0x0124);
    currentarr[2] = current & 0xFF;

    HBD_Spo2CalculateByFifoIntDbgRawdataInnerUse(gsensor_soft_fifo_buffer, gsensor_soft_fifo_buffer_index, __GS_SENSITIVITY_CONFIG__, 
                        &spo2_value, &spo2_lvl, &hb_value, &hb_lvl, &hrv_val[0], &hrv_val[1], &hrv_val[2], &hrv_val[3], &hrv_lvl, &hrv_cnt,
                        &spo2_r_value, &pliSpo2InvalidFlag, &wearing_state, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, &dbg_rawdata_len, &valid_lvl);
    handle_spo2_mode_result(spo2_value, spo2_lvl, hb_value, hb_lvl, hrv_val, hrv_lvl, hrv_cnt, spo2_r_value, wearing_state, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
    //EXAMPLE_DEBUG_LOG_L1("fspo2 : %f\r\n", fSpo2Value);
    EXAMPLE_DEBUG_LOG_L1("spo2 calc, gs_len=%d, result=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", gsensor_soft_fifo_buffer_index, spo2_value, spo2_lvl, hb_value, hb_lvl, hrv_val[0], hrv_val[1], hrv_val[2], 
                                            hrv_val[3], hrv_lvl, hrv_cnt, spo2_r_value, wearing_state);
    
    if (wearing_state == WEAR_STATUS_UNWEAR)
    {
        SEND_MCU_SPO2_UNWEAR_EVT((GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);						
        HBD_Stop();
        gh30x_handle_calc_unwear_status();
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
        FIXED_SPO2_ALGO_RES(spo2_value);
        SEND_MCU_SPO2_MODE_RESULT(spo2_value, spo2_lvl, hb_value, hb_lvl, hrv_val, hrv_lvl, hrv_cnt, spo2_r_value, 
                                  wearing_state, valid_lvl, abnormal_state, currentarr, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
    }
    EXAMPLE_LOG_RAWDARA("spo2 calc:\r\n", dbg_rawdata_ptr, dbg_rawdata_len);
}
#endif //#ifdef __HBD_API_EX__
#endif //#if (__SPO2_DET_SUPPORT__)

#if (__HRV_DET_SUPPORT__)
/// fifo evt hrv mode calc
#ifdef __HBD_API_EX__
#if 0
static void gh30x_fifo_evt_hrv_mode_calc(GS32 *dbg_rawdata_ptr)
{
    GU16 dbg_rawdata_len = __ALGO_CALC_DBG_BUFFER_LEN__;
    ST_HB_RES stHbRes = {0};
    ST_HRV_RES stHrvRes = {0};
    GU16 current = 0;
    GU8 currentarr[3] = { 0 };
    #if (__ALGO_CALC_WITH_DBG_DATA__)
    current = HBD_I2cReadReg(0x0122);
    currentarr[0] = current & 0xFF;
    currentarr[1] = (current >> 8) & 0xFF;
    current = HBD_I2cReadReg(0x0124);
    currentarr[2] = current & 0xFF;
    #endif
    //hrv_rr_value_fresh_cnt = HBD_HrvCalculateWithLvlByFifoIntDbgRawdata(gsensor_soft_fifo_buffer, gsensor_soft_fifo_buffer_index, __GS_SENSITIVITY_CONFIG__, hrv_rr_value_array, &hrv_lvl, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, &dbg_rawdata_len);
    HBD_HbWithHrvCalculateByFifoIntDbgDataEx(gsensor_soft_fifo_buffer, gsensor_soft_fifo_buffer_index, __GS_SENSITIVITY_CONFIG__, 
                                             &stHbRes, &stHrvRes, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, &dbg_rawdata_len);

    if (stHbRes.uchWearingState == WEAR_STATUS_UNWEAR)
    {
        stHrvRes.usRRvalueArr[0] = 255;
        stHrvRes.usRRvalueArr[1] = 255;
        stHrvRes.usRRvalueArr[2] = 255;
        stHrvRes.usRRvalueArr[3] = 255;
        SEND_MCU_HRV_MODE_RESULT(stHrvRes.usRRvalueArr, 0, 0, 255, currentarr, 0, 0);
        HBD_Stop();
        gh30x_handle_calc_unwear_status();
        //gh30x_unwear_evt_handler();
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
        handle_hrv_mode_result(stHrvRes.usRRvalueArr, stHrvRes.uchRRvalueCnt, stHrvRes.uchHrvConfidentLvl, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
        //handle_hb_mode_result(hb_value, 0, WEAR_STATUS_WEAR, 0, 0, 0);
        handle_hb_mode_result(&stHbRes, 0, 0);
        SEND_MCU_HRV_MODE_RESULT(stHrvRes.usRRvalueArr, stHrvRes.uchRRvalueCnt, stHrvRes.uchHrvConfidentLvl, stHbRes.uchHbValue, currentarr, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
        EXAMPLE_DEBUG_LOG_L1("hrv calc, result=%d,%d,%d,%d,%d,%d\r\n", stHrvRes.usRRvalueArr[0], stHrvRes.usRRvalueArr[1], stHrvRes.usRRvalueArr[2], stHrvRes.usRRvalueArr[3], stHrvRes.uchRRvalueCnt, stHrvRes.uchHrvConfidentLvl); //just print 4 result
    }
    BLE_MODULE_SEND_RRI(stHrvRes.usRRvalueArr, stHrvRes.uchRRvalueCnt);
}
#endif
#else //#ifdef __HBD_API_EX__
static void gh30x_fifo_evt_hrv_mode_calc(GS32 *dbg_rawdata_ptr)
{
    GU16 dbg_rawdata_len = __ALGO_CALC_DBG_BUFFER_LEN__;
    GU16 hrv_rr_value_array[HRV_MODE_RES_MAX_CNT] = {0};
    GU8 hrv_rr_value_fresh_cnt = 0;
    GU8 hrv_lvl = 0;
    GU8 hb_value = 0;
    GU8 hb_value_lvl = 0;
    GU8 wearing_state = 0;
    GU8 wearing_quality = 0;
    GU8 voice_broadcast = 0;
    GU16 current = 0;
    GU8 currentarr[3] = { 0 };

    current = HBD_I2cReadReg(0x0122);
    currentarr[0] = current & 0xFF;
    currentarr[1] = (current >> 8) & 0xFF;
    current = HBD_I2cReadReg(0x0124);
    currentarr[2] = current & 0xFF;

    HBD_HbWithHrvCalculateByFifoIntDbgData(gsensor_soft_fifo_buffer, gsensor_soft_fifo_buffer_index, __GS_SENSITIVITY_CONFIG__, \
                                                                    &hb_value, &hb_value_lvl, &wearing_state, &wearing_quality, &voice_broadcast, \
                                                                    hrv_rr_value_array, &hrv_rr_value_fresh_cnt, &hrv_lvl, \
                                                                    (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, &dbg_rawdata_len);
    if (wearing_state == WEAR_STATUS_UNWEAR)
    {
        hrv_rr_value_array[0] = 255;
        hrv_rr_value_array[1] = 255;
        hrv_rr_value_array[2] = 255;
        hrv_rr_value_array[3] = 255;
        SEND_MCU_HRV_MODE_RESULT(hrv_rr_value_array, 0, 0, 255, currentarr, 0, 0);
        gh30x_unwear_evt_handler();
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
        handle_hrv_mode_result(hrv_rr_value_array, hrv_rr_value_fresh_cnt, hrv_lvl, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
        handle_hb_mode_result(hb_value, 0, WEAR_STATUS_WEAR, 0, 0, 0);
        SEND_MCU_HRV_MODE_RESULT(hrv_rr_value_array, hrv_rr_value_fresh_cnt, hrv_lvl, hb_value, currentarr, (GS32 (*)[DBG_MCU_MODE_PKG_LEN])dbg_rawdata_ptr, dbg_rawdata_len);
        EXAMPLE_DEBUG_LOG_L1("hrv calc, result=%d,%d,%d,%d,%d\r\n", hrv_rr_value_array[0], hrv_rr_value_array[1], hrv_rr_value_array[2], hrv_rr_value_array[3], 
                                                                    hrv_rr_value_fresh_cnt); //just print 4 result
    }
    BLE_MODULE_SEND_RRI(hrv_rr_value_array, hrv_rr_value_fresh_cnt);
}
#endif //#ifdef __HBD_API_EX__
#endif

void Gh30xNormalizeGsensorSensitivity(ST_GS_DATA_TYPE gsensor_buffer[], GU16 gsensor_point_num)
{
    GU8 uchRightShift;
    uchRightShift = __GS_SENSITIVITY_CONFIG__;
    for(GU16 uchCnt = 0; uchCnt < gsensor_point_num; uchCnt++)
    {
        gsensor_buffer[uchCnt].sXAxisVal >>= uchRightShift;
        gsensor_buffer[uchCnt].sYAxisVal >>= uchRightShift;
        gsensor_buffer[uchCnt].sZAxisVal >>= uchRightShift;
    }
}


GU16 GH30xGetFrameNum(GU8 *puchRawdataBuf, GU16 usRawdataSamplePointNum)
{
    if(1 == g_uchNeedReadRawdataCount)
    {
        return usRawdataSamplePointNum;
    }
    if(2 == g_uchNeedReadRawdataCount)
    {
        return usRawdataSamplePointNum/2;
    }
    return 0;
}

float GH30xCalGsensorStep(GU16 usGsDataNum, GU16 usFrameNum)
{
    float fGsensorStep = 0;
    
    if ((usFrameNum > 1) && (usGsDataNum > 0)) // calc gs index
    {
        fGsensorStep = ((GF32)(usGsDataNum)) / (usFrameNum);
    }
    
    return  fGsensorStep;

}

void GH30xGetFrameDataAndProcess(GU8 *puchRawdataBuf, GU16 usRawdataSamplePointNum,
                                        GS16 *pusGsValueArr, GU16 usGsDataNum,
                                        const STGh30xFrameInfo * const pstFrameInfo,
                                        float fGensorStep, GU16 usFrameNum)
{
    GU8 uchTempIncompeletFlag;
    GU8  puchMapArray[2];
    GU8  uchChnlNum;
    GU8  uchCompeletMask;
    float fGsensorIndex = 0;
    GU16 usFrameCntAfterDiver = 0;
    GU16 g_frame_data_cnt = 0;

    uchTempIncompeletFlag = 0;
    uchChnlNum = pstFrameInfo->pstFunctionInfo->uchChnlNum;

    memset(puchMapArray, 0xFF, 2);
    for (GU8 uchChnlCnt = 0; uchChnlCnt < uchChnlNum; uchChnlCnt++)
    {
        GU8 uchMap;
        uchMap = pstFrameInfo->puchChnlMap[uchChnlCnt];
        if (uchMap < 2)
        {
            puchMapArray[uchMap] = uchChnlCnt;
        }
    }
    uchCompeletMask = ((1 << uchChnlNum) - 1);

    //set incomplete flag
    for (GU16 usSearchStartPosi = 0; usSearchStartPosi < usRawdataSamplePointNum; usSearchStartPosi++)
    {
        GU8 uchMapAtCurrentPosi;
        GU8 uchChnlId;

        uchMapAtCurrentPosi = CHNL_MAP_TO_FIFO_PHASE0_DATA;
        if((2 == g_uchNeedReadRawdataCount) && (1 == (usSearchStartPosi % 2)))
        {
            uchMapAtCurrentPosi = CHNL_MAP_TO_FIFO_PHASE1_DATA;
        }

        uchChnlId =  puchMapArray[uchMapAtCurrentPosi];
        if (0xFF != uchChnlId)
        {                       
            uchTempIncompeletFlag |= (1 << uchChnlId);
            // fill rawdata
            pstFrameInfo->punRawdata[uchChnlId] = ((GU32*)puchRawdataBuf)[usSearchStartPosi];
        }

        if (uchCompeletMask == (uchCompeletMask & uchTempIncompeletFlag))  //got one frame
        {
            if ((0 == pstFrameInfo->pstFunctionInfo->uchDivder) || (0 == pstFrameInfo->pstFunctionInfo->uchDivderCnt))
            {
                //fill Flag Bit0: new start flag
                if (0 == pstFrameInfo->punFrameCnt[0])
                {
                    pstFrameInfo->pstFunctionInfo->uchFrameFlag |= 1;
                }
                else
                {
                    pstFrameInfo->pstFunctionInfo->uchFrameFlag &= (~1);
                }
                for (GU8 uchChnlCnt = 0; uchChnlCnt < uchChnlNum; uchChnlCnt ++)
                {
                    //fill Flag Bit1~4: led color
                    GU8 uchLogicChnlId = pstFrameInfo->puchChnlMap[uchChnlCnt];
                    GU8 uchleftMoveBitNum = (1 + uchChnlCnt*2);
                    pstFrameInfo->pstFunctionInfo->uchFrameFlag &= (~(3 << uchleftMoveBitNum));
                    pstFrameInfo->pstFunctionInfo->uchFrameFlag |= (g_pstGh30xChnlInfo[uchLogicChnlId].uchLedColor << uchleftMoveBitNum);
                    //EXAMPLE_DEBUG_LOG_L1("color = %d\r\n", )
                    //fill drv current
                    pstFrameInfo->pchDrvCurrentCode[uchChnlCnt] = g_pstGh30xChnlInfo[uchLogicChnlId].uchDrvCurrentCode;
                }
                //file gsenor data
                pstFrameInfo->pusGsensordata[0] = pusGsValueArr[((GU16)fGsensorIndex)*3];
                pstFrameInfo->pusGsensordata[1] = pusGsValueArr[((GU16)fGsensorIndex)*3 + 1];
                pstFrameInfo->pusGsensordata[2] = pusGsValueArr[((GU16)fGsensorIndex)*3 + 2];

                //get algo io data hook
                memset(pstFrameInfo->pstAlgoResult, 0, sizeof(STGh30xAlgoResult));
                //HOOK_FUNC_CALL(g_pGh30xGetIoStructInfoHookFunc, (pstFrameInfo));
                if(g_pGh30xGetIoStructInfoHookFunc)
                    g_pGh30xGetIoStructInfoHookFunc(pstFrameInfo);
                #if __USE_GOODIX_APP__
                //save algo result for protocol
                GH30xRecordAlgoResult(pstFrameInfo);
                //cache rawdata to ble buffer
                Gh30xCopyRawdataToBLeBuf(usFrameCntAfterDiver, pstFrameInfo);
                #endif

                // timestamp increase
                (*(pstFrameInfo->punFrameCnt))++;

                usFrameCntAfterDiver++;
            }
            uchTempIncompeletFlag = 0;
            if(pstFrameInfo->pstFunctionInfo->uchDivder)
            {
                if(pstFrameInfo->pstFunctionInfo->uchDivder <= (pstFrameInfo->pstFunctionInfo->uchDivderCnt + 1))
                {
                    pstFrameInfo->pstFunctionInfo->uchDivderCnt = 0;
                }
                else
                {
                    pstFrameInfo->pstFunctionInfo->uchDivderCnt++;
                }
            }
            g_frame_data_cnt++;

            //calculate next fGsensorIndex
            fGsensorIndex += fGensorStep;
            if(fGsensorIndex > (usGsDataNum - 1))
            {
                fGsensorIndex = (usGsDataNum - 1);
            }
        }
    }

    #if __USE_GOODIX_APP__
    //copy algo result to ble buffer and start send data
    send_mcu_result_pkg(g_frame_data_cnt, pstFrameInfo);
    #endif
}

void GH30xFunctionProcess(GU8 *puchRawdataBuf, GU16 usRawdataSamplePointNum, 
                            GS16 *pusGsValueArr, GU16 usGsDataNum, 
                            const STGh30xFrameInfo * const pstFrameInfo)
{
    GU16 usFrameNum;
    float fGsensorStep;

    //EXAMPLE_DEBUG_LOG_L1("[GH30xFunctionProcess] %d++++++\r", pstFrameInfo->pstFunctionInfo->uchDivder);

    if(0 == pstFrameInfo)
    {
        return ;
    }
    if(0 == pstFrameInfo->pstFunctionInfo->uchChnlNum)
    {
        return;
    }
        
    /***  cal how many frames in this fifo  ***/
    usFrameNum = GH30xGetFrameNum(puchRawdataBuf, usRawdataSamplePointNum);

    /***  cal g-sensor step ***/
    fGsensorStep = GH30xCalGsensorStep(usGsDataNum, usFrameNum);


    //EXAMPLE_DEBUG_LOG_L1("[GetFrameNum]FunctionID: %d FrameNum: %d, usGsDataNum: %d, GsensorStep: %.3f\r\n",(int)pstFrameInfo->unFunctionID, usFrameNum, usGsDataNum, fGsensorStep);

    /***  get one frame data and process***/
    GH30xGetFrameDataAndProcess(puchRawdataBuf,usRawdataSamplePointNum,
                                pusGsValueArr,usGsDataNum,
                                pstFrameInfo,
                                fGsensorStep,usFrameNum);
    

}



void Gh30xUpdateFunctionDivder(void)
{
    GU16 usRegValue = HBD_I2cReadReg(HBD_SAMPLE_RATE_REG_ADDR);
    usRegValue = HBD_I2cReadReg(HBD_SAMPLE_RATE_REG_ADDR);

    for (GU8 uchFunCnt = 0; uchFunCnt < GH30X_FUNC_OFFSET_MAX; uchFunCnt++)
    {
        if (g_pstGh30xFrameInfo[uchFunCnt])
        {
            GU32 unTemp;
            GU16 usSampleRateAfterDiv = g_pstGh30xFrameInfo[uchFunCnt]->pstFunctionInfo->usOutputDataRate;

            g_pstGh30xFrameInfo[uchFunCnt]->punFrameCnt[0] = 0;

            if (usSampleRateAfterDiv)
            {
                unTemp = (GU32)32768 * 2 / usRegValue / usSampleRateAfterDiv + 1;   //half adjust
                g_pstGh30xFrameInfo[uchFunCnt]->pstFunctionInfo->uchDivder = ((GU8)(unTemp / 2));
                EXAMPLE_DEBUG_LOG_L1("[%s]FUN_ID = 0x%X, ChipSampleRate = %d, usSampleRateAfterDiv = %d, uchDivder = %d.\r\n",\
                                    __FUNCTION__,\
                                    g_pstGh30xFrameInfo[uchFunCnt]->unFunctionID,\
                                    ((((GU32)32768 * 2 / (GU32)usRegValue) + 1) / 2),\
                                    usSampleRateAfterDiv,\
                                    g_pstGh30xFrameInfo[uchFunCnt]->pstFunctionInfo->uchDivder);
            }
            else
            {
                EXAMPLE_DEBUG_LOG_L1("[%s]FUN_ID = 0x%X. ChipSampleRate = %d, usOutputDataRate is zero. \r\n",\
                                    __FUNCTION__,\
                                    g_pstGh30xFrameInfo[uchFunCnt]->unFunctionID,\
                                    ((((GU32)32768 * 2 / (GU32)usRegValue) + 1) / 2));
            }
        }
    }
}




void Gh30xDemoFunctionProcess(GU8* puchReadFifoBuffer, GU16 usFifoBuffLen, ST_GS_DATA_TYPE *pstGsAxisValueArr, GU16 usGsDataNum)
{
    // copy raw data to send buffer
#if __USE_GOODIX_APP__
    GH30xFunctionProcess(puchReadFifoBuffer, usFifoBuffLen, (GS16*)pstGsAxisValueArr,usGsDataNum, \
        g_pstGh30xFrameInfo[GH30X_FUNC_OFFSET_PROTOCOL]);
#endif

    for (GU8 uchFunCnt = 0; uchFunCnt < GH30X_FUNC_OFFSET_MAX; uchFunCnt ++)
    {
        if (g_pstGh30xFrameInfo[uchFunCnt])
        {
            GU8 uchExeFlag = 0;
            if((g_unGh30xDemoFuncMode) & (((GU32)1)<< uchFunCnt))
            {
                uchExeFlag = 1;
                if((GH30X_FUNC_OFFSET_ADT == uchFunCnt)&&(0 == g_uchGh30xIsAdtMode)) 
                {
                    uchExeFlag = 0;
                }
            }
            if(uchExeFlag)
            {
                GH30xFunctionProcess(puchReadFifoBuffer, usFifoBuffLen, (GS16*)pstGsAxisValueArr, usGsDataNum, \
                    g_pstGh30xFrameInfo[uchFunCnt]);
            }
        }
    }
}



/// gh30x fifo evt handler
void gh30x_fifo_evt_handler(void)
{
    GU8 uchNeedForceReadAgain = 1;
    GU16 usFifoSamplePointNum;

    while (uchNeedForceReadAgain)
    {
        GU16 usGsensorNum = 0;
        GU8 uchAutoLedFail = 0;

        gsensor_drv_get_fifo_data(g_UNGh30xRawdataBuf.stRawdataBuf.stGsensorBuf, &usGsensorNum, GSENSOR_SOFT_FIFO_BUFFER_MAX_LEN);
        Gh30xNormalizeGsensorSensitivity(g_UNGh30xRawdataBuf.stRawdataBuf.stGsensorBuf, usGsensorNum);
        //#if (__ALGO_CALC_WITH_DBG_DATA__)
        #if 0
        GU16 current = 0;
        current = HBD_I2cReadReg(0x0122);
        currentarr[0] = current & 0xFF;
        currentarr[1] = (current >> 8) & 0xFF;
        current = HBD_I2cReadReg(0x0124);
        currentarr[2] = current & 0xFF;
        #endif
        Gh30xUpdateChnlDrvInfo();  //get every channel drv current
        usFifoSamplePointNum = Gh30xGetFifoRawdata((GU8*)(g_UNGh30xRawdataBuf.stRawdataBuf.unPpgRawdataBuf), &uchNeedForceReadAgain);

        //HOOK_FUNC_CALL(g_pGh30xGetRawdataHookFunc, (g_UNGh30xRawdataBuf.stRawdataBuf.unPpgRawdataBuf, usFifoSamplePointNum));
        if(g_pGh30xGetRawdataHookFunc)
            g_pGh30xGetRawdataHookFunc(g_UNGh30xRawdataBuf.stRawdataBuf.unPpgRawdataBuf, usFifoSamplePointNum);
#ifdef __FUNC_TYPE_AGC_ENABLE__
        //auto led
        uchAutoLedFail = HBD_AutoLedbyFifoInt(g_UNGh30xRawdataBuf.stRawdataBuf.unPpgRawdataBuf, usFifoSamplePointNum / g_uchNeedReadRawdataCount);
#endif

        if (uchAutoLedFail)
        {
            EXAMPLE_DEBUG_LOG_L1("[%s] AGC fail !!!\r\n", __FUNCTION__);
            Gh30xSetSoftWearEvent(GH30X_SOFT_EVENT_WEAR_OFF, 0);
        }

        if (0 == uchNeedForceReadAgain)
        {
            #if 0
            for(int x = 0; x < 200; x ++)
            {
                hal_gh30x_delay_us(1000);
            }
            #endif
            #if 1
            HBD_I2cSendCmd(HBD_CMD_RESUME);
            #endif
        }

        Gh30xDemoFunctionProcess((GU8*)(g_UNGh30xRawdataBuf.stRawdataBuf.unPpgRawdataBuf),usFifoSamplePointNum, 
                                        g_UNGh30xRawdataBuf.stRawdataBuf.stGsensorBuf, usGsensorNum);
    }

    EXAMPLE_DEBUG_LOG_L1("[%s] g_unGh30xDemoFuncMode: 0x%x, g_uchGh30xSoftEvent:0x%x\r\n", __FUNCTION__, g_unGh30xDemoFuncMode, g_uchGh30xSoftEvent);

    if (GH30X_FUNCTION_ADT & g_unGh30xDemoFuncMode || GH30X_FUNCTION_SOFT_ADT & g_unGh30xDemoFuncMode)  // if  ADT fucntion is opened
    {
        if (g_uchGh30xSoftEvent & GH30X_SOFT_EVENT_WEAR_OFF)  //wear off
        {
            if (g_uchGh30xLivingObjectFlag)
            {
                g_uchGh30xMotionDetEn = 1;
            }
            else
            {
                if (GH30X_FUNCTION_ADT == g_unGh30xDemoFuncMode)
                {
                    HBD_I2cSendCmd(HBD_CMD_IDLE);  //stop sample
                    Gh30xAdtWearDetectStart(GH30X_ADT_MODE_DET_WEAR_ON);
                }
            }
            handle_wear_status_result(WEAR_STATUS_UNWEAR, g_uchGh30xLivingObjectFlag);
        }
        if (g_uchGh30xSoftEvent & GH30X_SOFT_EVENT_WEAR_ON)  //wear on
        {
            if (g_uchGh30xLivingObjectFlag)
            {

            }
            else
            {
                if (GH30X_FUNCTION_ADT == g_unGh30xDemoFuncMode)
                {
                    HBD_I2cSendCmd(HBD_CMD_IDLE);  //stop sample
                    Gh30xAdtWearDetectStart(GH30X_ADT_MODE_DET_WEAR_OFF);
                }
            }
            g_uchGh30xMotionDetEn = 0;
            handle_wear_status_result(WEAR_STATUS_WEAR, g_uchGh30xLivingObjectFlag);
        }
    }
    g_uchGh30xSoftEvent = 0;
}

/// gh30x newdata evt handler
void gh30x_new_data_evt_handler(void)
{
    ST_GS_DATA_TYPE gsensor_data;

    gsensor_drv_get_data(&gsensor_data); // get gsensor data
#if (USE_FAKE_GS_DATA_FOR_TEST)
    gh30x_get_sin_gsensor_data(1.5, 1, 0);
    gsensor_data.sXAxisVal = gsensor_soft_fifo_buffer[0].sXAxisVal;
    gsensor_data.sYAxisVal = gsensor_soft_fifo_buffer[0].sYAxisVal;
    gsensor_data.sZAxisVal = gsensor_soft_fifo_buffer[0].sZAxisVal;
#endif

#if (__USE_GOODIX_APP__)
    EXAMPLE_DEBUG_LOG_L1("[gh30x_new_data_evt_handler] gh30x_run_mode:%d, app_mode:%d\r\n", gh30x_run_mode, goodix_app_start_app_mode);
    if ((goodix_app_start_app_mode) && ((gh30x_run_mode == RUN_MODE_ADT_HB_DET) || (gh30x_run_mode == RUN_MODE_HRV_DET) || (gh30x_run_mode == RUN_MODE_SPO2_DET) || (gh30x_run_mode == RUN_MODE_GETRAWDATA_DET)))
    {
        if (GH30X_AUTOLED_ERR_VAL == HBD_SendRawdataPackageByNewdataInt(&gsensor_data, __GS_SENSITIVITY_CONFIG__))
        {
            SEND_AUTOLED_FAIL_EVT();
        }
    }
    else
#endif
    {
#if (__USE_GOODIX_APP__)
        goodix_app_start_app_mode = false; // if call stop, clear app mode
#endif

#if (__SYSTEM_TEST_SUPPORT__)
        if (goodix_system_test_mode)
        {
            if(!ledmask[goodix_system_test_os_led_num])
            {
                if(goodix_system_test_os_led_num<2)
                {
                    goodix_system_test_os_led_num++;
                    gh30x_systemtest_os_start(goodix_system_test_os_led_num);
                }
                else
                {
                    gh30x_systemtest_part2_handle(0);
                }
            }

            EXAMPLE_DEBUG_LOG_L1("got gh30x new data evt, put data to system test module.\r\n");
            GU8 os_test_ret = gh30x_systemtest_os_calc(goodix_system_test_os_led_num);
            if (os_test_ret != 0xFF) // test has done
            {
                EXAMPLE_DEBUG_LOG_L1("system test os[led %d] ret: %d!\r\n", goodix_system_test_os_led_num, os_test_ret);
                if (goodix_system_test_os_led_num < 2&&(!os_test_ret))
                {
                    goodix_system_test_os_led_num++;
                    gh30x_systemtest_os_start(goodix_system_test_os_led_num);
                    EXAMPLE_DEBUG_LOG_L1("system test change to test next led:%d!\r\n", goodix_system_test_os_led_num);
                }
                else
                {
//                    goodix_system_test_mode = false;
//                    EXAMPLE_DEBUG_LOG_L1("system test has done!\r\n");
                    HBD_Stop();
                    gh30x_systemtest_part2_handle(os_test_ret);
                }
            }
        }
        else
#endif
        {
            EXAMPLE_DEBUG_LOG_L1("got gh30x new data evt, shouldn't reach here!!\r\n");
            gh30x_handle_calc_unwear_status();
        }
    }
}

/// gh30x fifo full evt handler
void gh30x_fifo_full_evt_handler(void)
{
    HBD_Stop();
    EXAMPLE_DEBUG_LOG_L1("got gh30x fifo full evt, func[%s],  shouldn't reach here!!\r\n", dbg_rum_mode_string[gh30x_run_mode]);
}

/// gh30x int msg handler
void gh30x_int_msg_handler(void)
{
	GU8 gh30x_irq_status;

    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();

    if (0 == g_uchNewIntFlag)
    {
        EXAMPLE_DEBUG_LOG_L1("Invalid INT event, this time will be skip.\r\n");
        return;
    }
    g_uchNewIntFlag = 1;

    gh30x_irq_status = HBD_GetIntStatus();
    printf("irq_status = %d chip_state = %x\r\n", gh30x_irq_status, \
        HBD_I2cReadReg(HBD_IRQ_CTRL_REG_ADDR) & 0x00E0);

    if (gh30x_irq_status == INT_STATUS_FIFO_WATERMARK)
    {
        gh30x_fifo_evt_handler();
    }
    else if (gh30x_irq_status == INT_STATUS_NEW_DATA)
    {      
        gh30x_new_data_evt_handler();
    }
    else if (gh30x_irq_status == INT_STATUS_WEAR_DETECTED)
    {
        gh30x_wear_evt_handler();
    }
    else if (gh30x_irq_status == INT_STATUS_UNWEAR_DETECTED)
    {
        gh30x_unwear_evt_handler();
    }
    else if (gh30x_irq_status == INT_STATUS_CHIP_RESET) // if gh30x reset, need reinit
    {
        gh30x_reset_evt_handler();
    }
	else if (gh30x_irq_status == INT_STATUS_FIFO_FULL) // if gh30x fifo full, need restart
    {
        gh30x_fifo_full_evt_handler();
    }
}

/// gh30x fifo int timeout msg handler
void gh30x_fifo_int_timeout_msg_handler(void)
{
    GU8 gh30x_irq_status_1;
	GU8 gh30x_irq_status_2;

    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();

	EXAMPLE_DEBUG_LOG_L1("fifo int time out!!!\r\n");

	gh30x_irq_status_1 = HBD_GetIntStatus();
	gh30x_irq_status_2 = HBD_GetIntStatus();
    if ((gh30x_irq_status_1 == INT_STATUS_FIFO_WATERMARK) && (gh30x_irq_status_2 == INT_STATUS_INVALID))
	{
		gh30x_fifo_evt_handler();
	}
	else
    {
        gh30x_reset_evt_handler();
    }
}

void gh30x_mcu_start_mode_set(GU8 mcu_start_mode)
{
    g_mcu_start_mode = mcu_start_mode;
}

GU8 gh30x_mcu_start_mode_get(void)
{
    return g_mcu_start_mode;
}

/// communicate parse handler
void gh30x_communicate_parse_handler(GS8 communicate_type, GU8 *buffer, GU8 length) 
{
    EM_COMM_CMD_TYPE comm_cmd_type  = HBD_CommParseHandler(communicate_type, buffer, length); // parse recv data
    if (communicate_type == COMM_TYPE_INVALID_VAL)
    {
        EXAMPLE_DEBUG_LOG_L1("comm_type error, pelase check inited or not, @ref gh30x_module_init!!!\r\n");
    }
    else
    {
        EXAMPLE_DEBUG_LOG_L1("parse: cmd[%x-%s], comm_type[%d], length[%d]\r\n", buffer[0], dbg_comm_cmd_string[(uint8_t)comm_cmd_type], communicate_type, length);
    }
    if (comm_cmd_type < COMM_CMD_INVALID)
    {
        handle_goodix_communicate_cmd(comm_cmd_type);
        if ((comm_cmd_type == COMM_CMD_ALGO_IN_APP_HB_START) 
            || (comm_cmd_type == COMM_CMD_ALGO_IN_APP_HRV_START) 
            || ( comm_cmd_type == COMM_CMD_ALGO_IN_APP_SPO2_START)) // handle all app mode cmd
        {
            GU8 app_start_mode = RUN_MODE_INVALID;
            goodix_app_start_app_mode = true;
            if (comm_cmd_type == COMM_CMD_ALGO_IN_APP_HB_START)
            {
                app_start_mode = RUN_MODE_ADT_HB_DET;
            }
            else if (comm_cmd_type == COMM_CMD_ALGO_IN_APP_HRV_START)
            {
                app_start_mode = RUN_MODE_HRV_DET;
            }
            else if (comm_cmd_type == COMM_CMD_ALGO_IN_APP_SPO2_START)
            {
                app_start_mode = RUN_MODE_SPO2_DET;
            }

            HBD_FifoConfig(0, HBD_FUNCTIONAL_STATE_DISABLE);
            HBD_FifoConfig(1, HBD_FUNCTIONAL_STATE_DISABLE);

            if (RUN_MODE_ADT_HB_DET == app_start_mode || RUN_MODE_HRV_DET == app_start_mode || RUN_MODE_SPO2_DET == app_start_mode)
            {
                gh30x_module_stop();
                gh30x_mcu_start_mode_set(app_start_mode);
                gh30x_module_start(GH30X_FUNCTION_HR);
            }

            HBD_FifoConfig(0, HBD_FUNCTIONAL_STATE_ENABLE);
            HBD_FifoConfig(1, HBD_FUNCTIONAL_STATE_ENABLE);
        }
        else if ((comm_cmd_type == COMM_CMD_ADT_SINGLE_MODE_START)
            || (comm_cmd_type == COMM_CMD_ALGO_IN_MCU_HRV_START)
            || (comm_cmd_type == COMM_CMD_ALGO_IN_MCU_SPO2_START)) // handle mcu mode cmd
        {
            GU8 mcu_start_mode = RUN_MODE_INVALID;
            goodix_app_start_app_mode = false;
            if (comm_cmd_type == COMM_CMD_ADT_SINGLE_MODE_START)
            {
                mcu_start_mode = RUN_MODE_ADT_HB_DET;
            }
            else if (comm_cmd_type == COMM_CMD_ALGO_IN_MCU_HRV_START)
            {
                mcu_start_mode = RUN_MODE_HRV_DET;
            }
            else if (comm_cmd_type == COMM_CMD_ALGO_IN_MCU_SPO2_START)
            {
                mcu_start_mode = RUN_MODE_SPO2_DET;
            }
            //gh30x_module_start((EMGh30xRunMode)mcu_start_mode);
            if (RUN_MODE_ADT_HB_DET == mcu_start_mode || RUN_MODE_HRV_DET == mcu_start_mode || RUN_MODE_SPO2_DET == mcu_start_mode)
            {
                gh30x_module_stop();
                gh30x_mcu_start_mode_set(mcu_start_mode);
                gh30x_module_start(GH30X_FUNCTION_ADT);
            }
        }
        else // handle all stop cmd
        {
            goodix_app_start_app_mode = false;
            gh30x_module_stop();
        }
    }
}

/// enter normal mode and clear fifo buffer
void gsensor_enter_normal_and_clear_buffer(void)
{
    gsensor_drv_enter_normal_mode();  
	//gsensor_drv_clear_buffer(gsensor_soft_fifo_buffer, &gsensor_soft_fifo_buffer_index, GSENSOR_SOFT_FIFO_BUFFER_MAX_LEN);
}

/// clear fifo buffer and enter fifo mode
void gsensor_enter_clear_buffer_and_enter_fifo(void)
{
    gsensor_drv_enter_normal_mode();
    //gsensor_drv_clear_buffer(gsensor_soft_fifo_buffer, &gsensor_soft_fifo_buffer_index, GSENSOR_SOFT_FIFO_BUFFER_MAX_LEN);
    gsensor_drv_enter_fifo_mode();
}

/// motion detect irq handler
void gsensor_motion_has_detect(void)
{ 
    gsensor_drv_enter_normal_mode();  
    Gh30xHBD_SampleStart();  //start HBD mode sample;
    //gh30x_start_adt_with_mode(gh30x_run_mode);
    EXAMPLE_DEBUG_LOG_L1("got gsensor motion evt, start IR HBD sample.\r\n");
}

/// get data into fifo buffer
void gsensor_read_fifo_data(void)
{
    //gsensor_drv_get_fifo_data(gsensor_soft_fifo_buffer, &gsensor_soft_fifo_buffer_index, GSENSOR_SOFT_FIFO_BUFFER_MAX_LEN);
}

/// stop gh30x func
void gh30x_stop_func(void)
{
    #if (__HB_NEED_ADT_CONFIRM__)
    hb_start_without_adt_confirm = false;
    adt_confirm_flag = false;
    #endif
    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();
    HBD_Stop();
    #if (__HBD_ALGORITHM_USE_DYN_MEM__)
    gh30x_free_memory();
    #endif
}

/// gh30x start func fix adt confirm
#if 0
void gh30x_start_func_whithout_adt_confirm(uint8_t start_run_mode)
{
    #if ((__HB_DET_SUPPORT__) && (__HB_NEED_ADT_CONFIRM__))
    if (start_run_mode == RUN_MODE_ADT_HB_DET)
    {
        hb_start_without_adt_confirm = true;
    }
    #endif
    gsensor_enter_normal_and_clear_buffer();
    gh30x_start_func_with_mode(start_run_mode);
    gh30x_run_mode = start_run_mode;
    EXAMPLE_DEBUG_LOG_L1("gh30x module start without adt confirm, mode [%s]\r\n", dbg_rum_mode_string[gh30x_run_mode]);
}
#endif


#if (__SYSTEM_TEST_SUPPORT__)
/// gh30x module system test os check
void gh30x_module_system_test_os_start(void)
{
    handle_before_system_os_test();
    gh30x_module_stop();
    goodix_system_test_os_led_num = 0;
    gh30x_systemtest_os_start(goodix_system_test_os_led_num);
    EXAMPLE_DEBUG_LOG_L1("system test os check start\r\n");
}
#endif


//gh30x module system test
void gh30x_systemtest_start(EMGh30xTestItem mode)
{	
#if (__SYSTEM_TEST_SUPPORT__)
    #if __PLATFORM_DELAY_US_CONFIG__
    EXAMPLE_DEBUG_LOG_L1("delay has been out");
    HBDTEST_set_delayFunc(&hal_gh30x_delay_us);
    #endif
    uint8_t ret=0;
    if(!mode)
    {
        handle_system_test_result(EN_PARAM_FAIL,0);
        EXAMPLE_DEBUG_LOG_L1("system test check fail, ret = %d\r\n", EN_PARAM_FAIL);
    }
	if(mode & 0x1)
    {
        gh30x_module_stop();
        ret = gh30x_systemtest_comm_check();
        if(ret)
        {
            handle_system_test_result(ret,0);
            return;
        }
        EXAMPLE_DEBUG_LOG_L1("system test comm check, ret = %d\r\n", ret);
    }
    if(mode & 0x2)
    {
        gh30x_module_stop();
        ret = gh30x_systemtest_otp_check();
        if(ret)
        {
            handle_system_test_result(ret,0);
            return;
        }
        EXAMPLE_DEBUG_LOG_L1("system test otp check, ret = %d\r\n", ret);
    }
    if(mode & 0xc)
    {
        goodix_system_test_mode = (mode & 0x1c) >> 1;
        EXAMPLE_DEBUG_LOG_L1("begin goodix_system_test_mode is %d,mode is %d!\r\n",goodix_system_test_mode,mode);
        HBDTEST_ROMALEDCheckData *hbdatalst[]={&led0std,&led1std,&led2std};
        for(int i=0; i<3; i++)
        {
            gh30x_systemtest_param_set(i,&hbdatalst[i]->_param);
        }
        gh30x_module_system_test_os_start();
        EXAMPLE_DEBUG_LOG_L1("come to second part.\n");
    }
    else
    {
        handle_system_test_result(0,0);
    }
#else
    EXAMPLE_DEBUG_LOG_L1("__SYSTEM_TEST_SUPPORT__ disabled in config\r\n");
#endif
}

#if (__SYSTEM_TEST_SUPPORT__)
void gh30x_systemtest_part2_handle(GU8 ret)
{
    EXAMPLE_DEBUG_LOG_L1("now goodix_system_test_mode is %d!\r\n",goodix_system_test_mode);
    for(GS16 i=0;i<3;i++)
    {
        if(goodix_system_test_mode & (1<<i) )
        {
            goodix_system_test_mode^=(1<<i);
            if(i==2&& goodix_system_test_mode&0x8)
            {
                goodix_system_test_mode^=(1<<3);
            }
            break;
        }
    }
    EXAMPLE_DEBUG_LOG_L1("now goodix_system_test_mode is %d!\r\n",goodix_system_test_mode);
    if(goodix_system_test_mode&&(!ret))
    {
        gh30x_module_system_test_os_start();
    }
    else
    {
        goodix_system_test_mode=0;
        EXAMPLE_DEBUG_LOG_L1("system part2 test has done!\r\n");
        HBD_Stop();
        handle_system_test_result(ret,0);
    }
}
#endif

#if (USE_FAKE_GS_DATA_FOR_TEST)
///generate fake G-sensor sin data
void gh30x_get_sin_gsensor_data(float Hz, uint8_t range, uint8_t fifomode)
{
    static int ntime = 0;
    int ms = 1;
    int nGH30Xclk = 32768;
    int nSampleRate = nGH30Xclk / HBD_I2cReadReg(0x0016);
    if(fifomode)
    {
        switch(gh30x_run_mode)
        {
            case RUN_MODE_ADT_HB_DET:
                ms = 1000 * (__HB_FIFO_THR_CNT_CONFIG__ * 1.05) / nSampleRate;
                break;
            case RUN_MODE_SPO2_DET:
                ms = 1000 * (__SPO2_FIFO_THR_CNT_CONFIG__ * 1.05) / nSampleRate;
                break;
            case RUN_MODE_HRV_DET:
                ms = 1000 * (__HRV_FIFO_THR_CNT_CONFIG__ * 1.05) / nSampleRate;
                break;
            default:
                ms = 1000;
                break;
        }
    }
    
    int nPeriod = nSampleRate / Hz;
    int i = 0;
    float fResult;
    int datenum = nSampleRate * ms / 1000;    
    if(0 != (nSampleRate * ms) % 1000)
    {
        ++datenum;
    }
    gsensor_soft_fifo_buffer_index = 0;
    for(;i<datenum;++i)
    {
        if(ntime >= nPeriod)
        {
            ntime = 0;
        }
#if (FAKE_GSENSOR_DATA_EQUE_0)
        fResult = sinf(2 * 3.14 * Hz * ntime / nSampleRate);
#else
        fResult = 0;
#endif
        gsensor_soft_fifo_buffer[i].sXAxisVal = (GS16)(fResult * range * 512);
        gsensor_soft_fifo_buffer[i].sYAxisVal = gsensor_soft_fifo_buffer[i].sXAxisVal;
        gsensor_soft_fifo_buffer[i].sZAxisVal = gsensor_soft_fifo_buffer[i].sXAxisVal;
        ++gsensor_soft_fifo_buffer_index;
        ++ntime;
    }
}
#endif





const double ra_arry[8]={ 1, - 5.41747489061475, 12.2546238621685, - 14.8137955670650, 10.0918989051780, - 3.67327825092135, 0.558034825523797 };
const double rb_arry[8]={ 0.747017285960491, - 4.48210371576294, 11.2052592894074, - 14.9403457192098, 11.2052592894074, - 4.48210371576294, 0.747017285960491};//impulse response

const char *strVersionString="V1.1.0" ;

Delay_us_fun delay_us_out=NULL;

//static HighPassFilter HighPassFilters;

const char* HBDTEST_Get_TestlibVersion()
{
    return strVersionString;
}
    
static unsigned char crc_high_first(unsigned char *ptr, int len)
{
    int i;
    unsigned char crc = 0x00;

    while (len--)
    {
        crc ^= *ptr++;
        for (i = 8; i>0; --i)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ 0x31;
            }
            else
            {
                crc = (crc << 1);
            }
            crc = crc & 0xff;
        }
    }
    return(crc);
}

unsigned char OTP_Test( unsigned char *_inbuff,const unsigned char lenth )
{
    if(lenth<64)
        return 2;//err

    unsigned char CP_crc = 0;

    if (_inbuff[14])//ft_otp_version
    {
        if (0x01 == _inbuff[14])
        {

        }
        else
        {
            _inbuff[36] = 0;
            _inbuff[60] = 0;
        }
        CP_crc = crc_high_first(_inbuff, 61);
        if (CP_crc == _inbuff[61])
        {
            return EN_NORMAL_OK;//ok
        }
        else
        {
            return EN_OTP_CHECK_FAIL;//err
        }
    }
        
    return EN_OTP_CHECK_FAIL;//err
}

static void delay1us(void)
{
        for (char i = 0; i < 9; i++)
        {
        }
}

static void delay_x_us(unsigned short us)
{
        while (us--)
        {
                delay1us();
        }
}

unsigned char HBDTEST_Comm_Check(ROMAHBD_Interfcae *ROMAHBD_Str)
{
    if((ROMAHBD_Str->RD_Fun(0x26)&0xff0)!=0x2A0)
    {
        return EN_COMM_READ_FAIL;
    }
    unsigned short temp_value=ROMAHBD_Str->RD_Fun(0x186);
    ROMAHBD_Str->WR_Fun(0x186,0x446);
    if((ROMAHBD_Str->RD_Fun(0x186)&0x7f8)>>3!=0x88)
    {
        return EN_COMM_WRITE_FAIL;
    }
    ROMAHBD_Str->WR_Fun(0x186,temp_value);
    return EN_NORMAL_OK;
}

unsigned char HBDTEST_OTP_Check( ROMAHBD_Interfcae *ROMAHBD_Str,unsigned char *retbuf )
{
    unsigned char Data[64]={0};
    unsigned short temp_value=0;
    
    if((ROMAHBD_Str->WR_Fun==NULL)||(ROMAHBD_Str->RD_Fun==NULL))
        return EN_COMM_READ_FAIL;
    
    for (char i = 0; i < 64; i+=2)
    {
        ROMAHBD_Str->WR_Fun(0x0012, 0x0D00);
        ROMAHBD_Str->WR_Fun(0x006E, 0x0000);
        ROMAHBD_Str->WR_Fun(0x64, i);
        ROMAHBD_Str->WR_Fun(0x6A, 0x01);
        if(delay_us_out)
        {
            delay_us_out(10);
        }
        else
        {
            delay_x_us(10);
        }
        ROMAHBD_Str->WR_Fun(0x6A, 0x00);
        temp_value=ROMAHBD_Str->RD_Fun(0x6C);
        Data[i] = temp_value;
        Data[i+1] = temp_value>>8;
    }
    
    if(retbuf!=NULL)
    {
        for(char i=0;i<64;i++)
        {
            retbuf[i]=Data[i];
        }
    }
     
    return OTP_Test(Data,64);    
}
#if __SYSTEM_TEST_SUPPORT__
static void CalcNoise( int* pRawdata, short int nCount, HBDTEST_ROMAHBData* pHBDataBuff )
{
//    rawdataVec rawdataVector;
//    for (int i = 0; i < nCount; ++i)
//    {
//        int temp = pRawdata[i];
//        rawdataVector.push_back(temp);
//    }
//    sort(rawdataVector.begin(), rawdataVector.end());
    int tmp=0;
    for( int m=0;m<nCount;m++ )//pp sort
    {
        for( int n=m+1;n<nCount;n++ )
        {
            if( pRawdata[n]<pRawdata[m] )
            {
                tmp=pRawdata[n];
                pRawdata[n]=pRawdata[m];
                pRawdata[m]=tmp;
            }
        }
    }

    int nSum = 0;
    for( int i=0;i<nCount;i++ )
    {
        nSum+=pRawdata[i];
    }
    
//    for (rawdataVec::const_iterator it = rawdataVector.begin(); it != rawdataVector.end(); ++it)
//    {
//        nSum += *it;
//    }
    pHBDataBuff->nNoiseRawDataAvr = nSum / nCount;
    pHBDataBuff->nNoiseRawDataMax = pRawdata[nCount-1];;
    pHBDataBuff->nNoiseRawDataMin = pRawdata[0];
    double fSum = 0.0f;

    for (unsigned char j = 0; j < nCount; ++j)//std^2
    {
        fSum += pow(pRawdata[j] - pHBDataBuff->nNoiseRawDataAvr, 2);
    }
    fSum /= nCount;

    int LpNorise1;
    LpNorise1 = (int)(6 * sqrt(fSum));//sigma??N1
    //LpNorise2 = rawdataVector.at(nCount - 2) - rawdataVector.at(1);//?????N2
    pHBDataBuff->nLpNoise = LpNorise1;// min(LpNorise1, LpNorise2);
}

double design(const double val) 
{
    if (g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_bFirstDataFlag) 
    {
        g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.threshold=400;
        for(char i=0;i<7;i++)
        {
            g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecA[i]=ra_arry[i];
            g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecB[i]=rb_arry[i];
        }
        for(char i=0;i<6;i++)
        {
            g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecDataIn[i]=val;
            g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecDataOut[i]=0.0;
        }
    
//        double skip = val - m_vecDataIn[5];
//        std::transform(m_vecDataIn.begin(), m_vecDataIn.end(), m_vecDataIn.begin(), [skip](T v)->T{
//            return (v + skip);
//        });
        g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_bFirstDataFlag = 0;
    }

    double sum = 0;
    for (int i = 0; i < 6; ++i) 
    {
        sum += (g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecB[i+1] * g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecDataIn[6 -i - 1]);
    }
    double dataBuffer1 = g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecB[0] * val + sum;

    sum = 0;
    for (int j = 0; j < 6; ++j) 
    {
        sum += (g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecA[j+1] * g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecDataOut[6 - j - 1]);
    }
    double dataBuffer2 = sum;

    double dataOut = (dataBuffer1 - dataBuffer2) / g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecA[0];

    for(int j = 0; j < 6; j++)
    {
        if(j)
        {
            g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecDataIn[j-1]=g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecDataIn[j];
            g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecDataOut[j-1]=g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecDataOut[j];
        }
    }
    g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecDataIn[5]=val;
    g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_vecDataOut[5]=dataOut;

    //m_vecDataIn.erase(m_vecDataIn.begin());
    //m_vecDataIn.push_back(val);

    //m_vecDataOut.erase(m_vecDataOut.begin());
    //m_vecDataOut.push_back(dataOut);

    return dataOut;
}

unsigned char HBDTEST_Check_Rawdata_Noise( int *_inDatabuff,const unsigned char lenth,const HBDTEST_ROMAHBData *_inRawdataStr )
{
    if((lenth==0)||(lenth>100))
        return EN_PARAM_FAIL;//err
 
    HBDTEST_ROMAHBData ROMAHBStr;
    
    //int temp_data[100] = { 0 };

    g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_bFirstDataFlag = 1;
    for (char i = 0; i < lenth; i++)
    {
        g_UNGh30xRawdataBuf.stFactoryRawdata.unTempBufForStatistic[i] = (int)design(_inDatabuff[i]);
    }
    CalcNoise((GS32 *)g_UNGh30xRawdataBuf.stFactoryRawdata.unTempBufForStatistic, lenth, &ROMAHBStr);
    int nLpNoise_L1 = ROMAHBStr.nLpNoise;
    CalcNoise(_inDatabuff, lenth, &ROMAHBStr);
    ROMAHBStr.nLpNoise = nLpNoise_L1;
                
    if( (_inRawdataStr->nNoiseRawDataMax<ROMAHBStr.nNoiseRawDataAvr)||(_inRawdataStr->nNoiseRawDataMin>ROMAHBStr.nNoiseRawDataAvr))
    {
        return EN_RAWDATA_OUT_RANGE;//err
    }
    if( (_inRawdataStr->nLpNoise<ROMAHBStr.nLpNoise)||(0==ROMAHBStr.nLpNoise) )
    {
        return EN_NOISE_UNDER_STD;//err
    }
    return EN_NORMAL_OK;
}


unsigned char HBDTEST_Check_CTRandNoise(int *_inDatabuff,const unsigned char lenth,HBDTEST_ROMALEDCheckData* data)
{
    if((lenth == 0)||(lenth>100))
        return EN_PARAM_FAIL;//err
    HBDTEST_ROMAHBData ROMAHBStr;
    
    //static int temp_data[100] = { 0 };

    g_UNGh30xRawdataBuf.stFactoryRawdata.HighPassFilters.m_bFirstDataFlag = 1;
    for (char i = 0; i < lenth; i++)
    {
        g_UNGh30xRawdataBuf.stFactoryRawdata.unTempBufForStatistic[i] = (int)design(_inDatabuff[i]);
    }
    CalcNoise((GS32 *)g_UNGh30xRawdataBuf.stFactoryRawdata.unTempBufForStatistic, lenth, &ROMAHBStr);
    int nLpNoise_L1 = ROMAHBStr.nLpNoise;
    CalcNoise(_inDatabuff, lenth, &ROMAHBStr);
    ROMAHBStr.nLpNoise = nLpNoise_L1;
	data->_res._noise = ROMAHBStr.nLpNoise;
//    if( (_inRawdataStr->nNoiseRawDataMax<ROMAHBStr.nNoiseRawDataAvr)||(_inRawdataStr->nNoiseRawDataMin>ROMAHBStr.nNoiseRawDataAvr))
//    {
//        return 1;//err
//    }
    data->_res._CTR = ROMAHBStr.nNoiseRawDataAvr * 800000.0f / 65536.0f / data->_param._ledCurr / data->_param._ledResis;
    if(data->_res._CTR < data->_std._ctrStd)
    {
        return EN_CTR_UNDER_STD;
    }
	if ((data->_std._noiseStd < ROMAHBStr.nLpNoise) || (0 == ROMAHBStr.nLpNoise))
    {
        return EN_NOISE_UNDER_STD;//err
    }
    data->_res._flag|=0x1;
    return EN_NORMAL_OK;
}
#endif

unsigned char HBDTEST_Check_LeakandRatio(int *buff,const unsigned char lenth,HBDTEST_ROMALEDCheckData* data)
{
    int sum=0;
    for(int i = 0;i < lenth;i++)
    {
        sum+=buff[i];
    }
    data->_res._leak=((float)sum) / lenth * 800000.0f / 65536.0f / data->_param._ledCurr / data->_param._ledResis;
    if(data->_res._leak > data->_std._leakStd)
    {
        return EN_LEAK_UNDER_STD;
    }
    if(data->_res._flag&0x1)
    {
        data->_res._leakRatio=data->_res._leak / data->_res._CTR;
        if(data->_res._leakRatio > data->_std._ratio)
        {
            return EN_LEAKRATIO_UNDER_STD;
        }
    }
    return EN_NORMAL_OK;
}

void HBDTEST_set_delayFunc(Delay_us_fun func)
{
    delay_us_out=func;
}




/********END OF FILE********* Copyright (c) 2003 - 2020, Goodix Co., Ltd. ********/
