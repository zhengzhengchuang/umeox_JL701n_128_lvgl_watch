/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_algo_call_nadt.c
 *
 * @brief   gh30x algorithm nadt interface
 *
 * @version ref gh30x_example_common.h
 *
 */

#include <stdio.h>
#include "../../common/inc/gh30x_demo_algo_call.h"
#include "../../common/inc/gh30x_demo_algorithm_calc.h"
#include "../inc/goodix_mem.h"
#include "../../../Lib_Code/inc/goodix_nadt.h"

#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)

const GS32 liNadtCfg[32] = {
                        1,				// NADT_CONFIG_SOFT_AUTOLED_TYPE            0
                        5,				// NADT_CONFIG_TURNLIGHT_TYPE               1
                        4,				// NADT_CONFIG_UNWEAR_TIME_TYPE             2
                        60,				// NADT_CONFIG_DETECT_TIMEOUT_TYPE          3
                        25,				// NADT_CONFIG_SAMPLE_RATE_TYPE             4
                        0,				// NADT_CONFIG_SLEEP_STATUS_TYPE            5
                        1,				// NADT_COFIG_UNWEAR_LEVEL_TYPE             6
                        1,				// NADT_CONFIG_LIVE_DETECT_EN_TYPE          7
                        40,				// NADT_CONFIG_HB_LOW_THR_TYPE              8
                        140,			// NADT_CONFIG_HB_HIGH_THR_TYPE             9
                        10,				// NADT_CONFIG_GREEN_SIG_LOW_THR_TYPE       10
                        180,			// NADT_CONFIG_GREEN_SIG_HIGH_THR_TYPE      11
                        1,				// NADT_CONFIG_BASE_RATIO_EN_TYPE           12
                        10,				// NADT_CONFIG_BASE_RATIO_THR_TYPE          13
                        1,				// NADT_CONFIG_LIVE_CONFIRM_EN_TYPE         14
                        10,				// NADT_CONFIG_IR_SIG_LOW_THR_TYPE          15
                        100,			// NADT_CONFIG_IR_SIG_HIGH_THR_TYPE         16
                        1,				// NADT_CONFIG_CTR_EN_TYPE                  17
                        10,				// NADT_CONFIG_CTR_THR_TYPE                 18
                        1,				// NADT_CONFIG_GAIN_EN_TYPE                 19
                        4,				// NADT_CONFIG_GAIN_STABLE_TIME_TYPE        20
                        3,				// NADT_CONFIG_GREEN_GAIN_THR_TYPE          21
                        3,				// NADT_CONFIG_IR_GAIN_THR_TYPE             22
                        1,				// NADT_CONFIG_PERIOD_EN_TYPE               23
                        30,				// NADT_CONFIG_PERIOD_LOW_THR_TYPE          24
                        90,				// NADT_CONFIG_PERIOD_HIGH_THR_TYPE         25
                        3,				// NADT_CONFIG_PERIOD_DIFF_THR_TYPE         26
                        10,				// NADT_CONFIG_TOTAL_CNT_TYPE               27
                        7,				// NADT_CONFIG_UNWEAR_CNT_TYPE              28
                        0,				// NADT_CONFIG_ADT_ONLY_SLEEP_TYPE          29
                        5230,			// NADT_CONFIG_ADT_WEAR_OFF_THR_TYPE        30
                        5				// NADT_CONFIG_ADT_WEAR_OFF_CNT_TYPE        31
};

void NADT_Control_S(GS32 lOptTye, GS32 lConfigValue[])
{
    GH30X_ALGO_LOG_PARAM("[NADT_Control]: %d, %d\r\n", lOptTye, lConfigValue[0]);
    NADT_Control(lOptTye, lConfigValue);
}

GS8 GH30xSoftAdtAlgoInit(const STGh30xFrameInfo * const pstFrameInfo)
{
    NADT_ProcInit();

    for (GU8 uchParmCnt = 0; uchParmCnt < 32; uchParmCnt++)
    {
        NADT_Control_S(uchParmCnt, (GS32*)(&(liNadtCfg[uchParmCnt])));
    }

    return GH30X_RET_OK;
}

GS8 GH30xSoftAdtAlgoExe(const STGh30xFrameInfo *const pstFrameInfo)
{
    GS32 lNadtPacketInfoArr[6] = {0};
    GU8 lNadtResult[2] = {0};

    lNadtPacketInfoArr[0] = (pstFrameInfo->punRawdata[0] & 0x00FFFFFF);
    lNadtPacketInfoArr[1] = (pstFrameInfo->punRawdata[1] & 0x00FFFFFF);
    lNadtPacketInfoArr[2] = pstFrameInfo->pusGsensordata[0];
    lNadtPacketInfoArr[3] = pstFrameInfo->pusGsensordata[1];
    lNadtPacketInfoArr[4] = pstFrameInfo->pusGsensordata[2];
    lNadtPacketInfoArr[5] = pstFrameInfo->pchDrvCurrentCode[1];

    NADT_Proc(lNadtPacketInfoArr, lNadtResult);

    pstFrameInfo->pstAlgoResult->snResult[0] = (GS32)lNadtResult[0];
    pstFrameInfo->pstAlgoResult->snResult[1] = (GS32)lNadtResult[1];

    if (lNadtResult[0])
    {
//        GH30X_ALGO_LOG_PARAM("NADT_Proc lNadtPacketInfoArr = %d , %d, %d, %d, %d, %d\n\r lNadtResult = %d, %d\n\r",\
//                        lNadtPacketInfoArr[0],lNadtPacketInfoArr[1],lNadtPacketInfoArr[2],lNadtPacketInfoArr[3],lNadtPacketInfoArr[4],lNadtPacketInfoArr[5],\
//                        lNadtResult[0], lNadtResult[1]);
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
        pstFrameInfo->pstAlgoResult->usResultBit = 0x03;
        pstFrameInfo->pstAlgoResult->uchResultNum = GH30x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
        GH30X_SoftAdtAlgorithmResultReport(pstFrameInfo->pstAlgoResult, GH30X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt)));
    }

    return GH30X_RET_OK;
}

GS8 GH30xSoftAdtAlgoDeinit(const STGh30xFrameInfo *const pstFrameInfo)
{
    NADT_ProcDeInit();
    return GH30X_RET_OK;
}

#endif /* __USE_GOODIX_SOFT_ADT_ALGORITHM__ */
