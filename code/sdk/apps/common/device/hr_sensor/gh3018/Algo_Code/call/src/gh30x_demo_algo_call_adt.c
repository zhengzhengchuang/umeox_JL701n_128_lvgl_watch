/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_algo_call_adt.c
 *
 * @brief   gh30x algorithm adt interface
 *
 * @version ref gh30x_example_common.h
 *
 */

#include <stdio.h>
#include "../../common/inc/gh30x_demo_algo_call.h"
#include "../../common/inc/gh30x_demo_algorithm_calc.h"
#include "../../common/inc/gh30x_demo_algo_config.h"

#if (__USE_GOODIX_ADT_ALGORITHM__)

#define GH30X_WEAR_ON_CNT_MAX  4
#define ADT_CHECK_ALGO_VERSION "GH_ADT_CONFIRM_V1.0"

GU8 g_uchGh30xWearOnCnt;
GU8 g_uchGh30xWearOffCnt;

GU8 *GH30xAdtVersion(void)
{
    return (GU8*)ADT_CHECK_ALGO_VERSION;
}

GS8  GH30xAdtAlgoInit(const STGh30xFrameInfo *const pstFrameInfo)
{
    g_uchGh30xWearOnCnt = 0;
    g_uchGh30xWearOffCnt = 0;
    return GH30X_RET_OK;
}

GS8 GH30xAdtAlgoDeinit(const STGh30xFrameInfo *const pstFrameInfo)
{
    return GH30X_RET_OK;
}

GS8 GH30xAdtAlgoExe(const STGh30xFrameInfo *const pstFrameInfo)
{
    GU32 unRawdata = (pstFrameInfo->punRawdata[0] >> 7) & 0x1FFFF ;
    GU32 unDownTrd = g_usGh30xAdtDownThrdRegValue *2;
    GU8  uchOutputFlag = 0;
    GU8  uchResult = GH30X_ADT_RESULT_WEAR_OFF;

    // GH30X_ALGO_LOG_PARAM("[%s]rawdata = %d, unDownTrd = %d.\r\n",__FUNCTION__, unRawdata, unDownTrd);

#if (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_FINGER)
    if (unRawdata < unDownTrd)
    {
        if(g_uchGh30xWearOnCnt < GH30X_WEAR_ON_CNT_MAX)
        {
            g_uchGh30xWearOnCnt ++;
            g_uchGh30xWearOffCnt = 0;
        }
    }
    else
    {
        if (g_uchGh30xWearOffCnt < GH30X_WEAR_ON_CNT_MAX)
        {
            g_uchGh30xWearOnCnt = 0;
            g_uchGh30xWearOffCnt ++;
        }
    }
#else
    if(unRawdata > unDownTrd)
    {
        if(g_uchGh30xWearOnCnt < GH30X_WEAR_ON_CNT_MAX)
        {
            g_uchGh30xWearOnCnt ++;
            g_uchGh30xWearOffCnt = 0;
        }
    }
    else
    {
        if (g_uchGh30xWearOffCnt < GH30X_WEAR_ON_CNT_MAX)
        {
            g_uchGh30xWearOnCnt = 0;
            g_uchGh30xWearOffCnt ++;
        }
    }
#endif

    if(g_uchGh30xWearOnCnt >= GH30X_WEAR_ON_CNT_MAX)
    {
        uchOutputFlag = 1;
        uchResult = GH30X_ADT_RESULT_WEAR_ON;
    }
    else if(g_uchGh30xWearOffCnt >= GH30X_WEAR_ON_CNT_MAX)
    {
        uchOutputFlag = 1;
        uchResult = GH30X_ADT_RESULT_WEAR_OFF;
    }
    else if (pstFrameInfo->punFrameCnt[0] >= (2*25 - 1))  // time out
    {
        uchOutputFlag = 1;
        uchResult = GH30X_ADT_RESULT_WEAR_OFF;
    }

    if(uchOutputFlag)
    {
        // GH30X_ALGO_LOG_PARAM("[%s]uchResult = %d\r\n",__FUNCTION__, uchResult);
        pstFrameInfo->pstAlgoResult->usResultBit = 0x7;
        pstFrameInfo->pstAlgoResult->uchResultNum = GH30x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
        pstFrameInfo->pstAlgoResult->snResult[0] = uchResult;
        pstFrameInfo->pstAlgoResult->snResult[1] = unRawdata;
        pstFrameInfo->pstAlgoResult->snResult[2] = unDownTrd;
        GH30X_AdtAlgorithmResultReport(pstFrameInfo->pstAlgoResult, pstFrameInfo->punFrameCnt[0]);
    }

    return GH30X_RET_OK;
}

#endif  /* __USE_GOODIX_ADT_ALGORITHM__ */

