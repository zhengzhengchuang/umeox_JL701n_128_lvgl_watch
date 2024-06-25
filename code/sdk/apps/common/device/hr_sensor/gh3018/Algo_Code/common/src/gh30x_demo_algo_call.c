/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_algo_call.c
 *
 * @brief   gh30x algo call
 *
 * @version ref gh30x_example_common.h
 *
 */

#include <stdio.h>
#include "../inc/gh30x_demo_algo_memory.h"
#include "../inc/gh30x_demo_algo_call.h"
#include "../inc/gh30x_demo_algo_config.h"
#include "../inc/gh30x_demo_algorithm_calc.h"
#include "../inc/gh30x_demo_algo_version.h"
#include "../../call/inc/goodix_algo.h"
#include "../../call/inc/goodix_mem.h"
#include "../../../Lib_Code/inc/goodix_type.h"
#include "../../../Lib_Code/inc/goodix_hba.h"
#include "../../../Lib_Code/inc/goodix_sys_hrv.h"
#include "../../../Lib_Code/inc/goodix_spo2.h"
#include "../../../Lib_Code/inc/goodix_nadt.h"

#if (__GOODIX_ALGO_CALL_MODE__)
static GU32 g_unAlgoFuncStartedBitmap = 0;
static GU8 g_uchAlgInitFlag[GH30X_FUNC_OFFSET_MAX] = {0};
static const pfnAlgorithmCallFunc g_pfnAlgorithmCallFunc[GH30X_FUNC_OFFSET_MAX][GH30X_ALGO_FUNCTION_NUM_MAX] = {
//adt
#if __USE_GOODIX_ADT_ALGORITHM__
    { GH30xAdtAlgoInit, GH30xAdtAlgoExe, GH30xAdtAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//hr
#if __USE_GOODIX_HR_ALGORITHM__
    { GH30xHrAlgoInit, GH30xHrAlgoExe, GH30xHrAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//hrv
#if __USE_GOODIX_HRV_ALGORITHM__
    { GH30xHrvAlgoInit, GH30xHrvAlgoExe, GH30xHrvAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//spo2
#if __USE_GOODIX_SPO2_ALGORITHM__
    { GH30xSpo2AlgoInit, GH30xSpo2AlgoExe, GH30xSpo2AlgoDeinit },
#else
    { 0, 0, 0 },
#endif
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
    { GH30xSoftAdtAlgoInit, GH30xSoftAdtAlgoExe, GH30xSoftAdtAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
};

GCHAR* GH30X_GetAlgoCallDemoVersion(void)
{
    return GOODIX_ALGO_CALL_VERSION_STRING;
}


void GH30X_AlgoCallConfigInit(const STGh30xFrameInfo * const  pSTGh30xFrameInfo[])
{
    GH30X_AlgoMemConfig(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL);
    GH30X_ALGO_LOG_PARAM("Algorithm mem size = %d\r\n",(int)GH30X_ALGORITHMS_MEMORY_SIZE_FINAL);
}

#if (__ALGO_LOG_CONFIG__)
const char* g_sFunctionName[] = 
{
    "ADT",
    "HR",
    "HRV",
    "SPO2", 
    "ADT",
    "PROTOCOL",
    "UNKOWN_FUNC"
};

const char* Gh30xGetFuncName(GU8 uchFunOffset)
{
    if(uchFunOffset >= GH30X_FUNC_OFFSET_MAX)
    {
        uchFunOffset = GH30X_FUNC_OFFSET_MAX;
    }
    return g_sFunctionName[uchFunOffset];
}
#endif

const GCHAR chNoVersion[10] = "no_ver";

void GH30X_AlgoVersion(GU8 uchFunctionIDOffset)
{
    GCHAR version[100];
    memcpy(version, (GCHAR *)chNoVersion,\
    strlen((const GCHAR *)chNoVersion) + 1);

    switch (uchFunctionIDOffset)
    {
#if __USE_GOODIX_ADT_ALGORITHM__
    case GH30X_FUNC_OFFSET_ADT:
        memcpy(version, GH30xAdtVersion(), strlen((const GCHAR *)GH30xAdtVersion()) + 1);
        break;
#endif
#if __USE_GOODIX_HR_ALGORITHM__
    case GH30X_FUNC_OFFSET_HR:
        goodix_hba_version((uint8_t*)version);
        break;
#endif
#if __USE_GOODIX_HRV_ALGORITHM__
    case GH30X_FUNC_OFFSET_HRV:
        memcpy(version, goodix_hrv_version(), strlen((const GCHAR *)goodix_hrv_version()) + 1);
        break;
#endif
#if __USE_GOODIX_SPO2_ALGORITHM__
    case GH30X_FUNC_OFFSET_SPO2:
        goodix_spo2_version((uint8_t*)version);
        break;
#endif

#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
    case GH30X_FUNC_OFFSET_SOFT_ADT:
       memcpy(version, (GU8*) NADT_GetVersion(), strlen((const GCHAR *)NADT_GetVersion()) + 1);
        break;
#endif

    default:
        break;
    }
    GH30X_ALGO_LOG_PARAM("[%s] %s: %s\r\n", __FUNCTION__, Gh30xGetFuncName(uchFunctionIDOffset), version);
}

#define GH30X_GOODIX_ALGO_USING_BIT (0 * GH30X_FUNCTION_ADT + \
                                    __USE_GOODIX_HR_ALGORITHM__ * GH30X_FUNCTION_HR + \
                                    __USE_GOODIX_HRV_ALGORITHM__ * GH30X_FUNCTION_HRV + \
                                    __USE_GOODIX_SPO2_ALGORITHM__ * GH30X_FUNCTION_SPO2 + \
                                    __USE_GOODIX_SOFT_ADT_ALGORITHM__ * GH30X_FUNCTION_SOFT_ADT)

const GU32 g_punAlgoModeList[] = {
    GH30X_ALGO_FUNCTION_GROUP_0,
    GH30X_ALGO_FUNCTION_GROUP_1,
    GH30X_ALGO_FUNCTION_GROUP_2,
    GH30X_ALGO_FUNCTION_GROUP_3,
    GH30X_ALGO_FUNCTION_GROUP_4
};

void Gh30xCheckOpenedAlgoIsValid(void)
{
    GU32 unEnabledGoodixAlgo = g_unAlgoFuncStartedBitmap&GH30X_GOODIX_ALGO_USING_BIT;
    for (GU8 uchModeCnt = 0; uchModeCnt < sizeof(g_punAlgoModeList)/4; uchModeCnt ++)
    {
        GH30X_ALGO_LOG_PARAM("[%s]:g_punAlgoModeList[%d] = 0x%X, Opened algo combination: 0x%X\r\n",__FUNCTION__, uchModeCnt, g_punAlgoModeList[uchModeCnt], unEnabledGoodixAlgo);
        if (unEnabledGoodixAlgo == (unEnabledGoodixAlgo&g_punAlgoModeList[uchModeCnt]))
        {
            return;
        }
    }
    GH30X_ALGO_LOG_PARAM("[%s]:Can not find algo mode in g_punAlgoModeList. Opened algo combination: 0x%X\r\n", __FUNCTION__, unEnabledGoodixAlgo);
    while(1);
}

GS8 GH30X_AlgoInit(GU32 unFunctionID)
{
    GU8 chRet = GH30X_RET_OK;

    /* set started bit */
    g_unAlgoFuncStartedBitmap |= unFunctionID;

    Gh30xCheckOpenedAlgoIsValid();

    /* init mem pool */
    if (GH30X_GetAlgoMemStatus() != ALGO_MEM_INIT_OK)
    {
        return -1;
    }

    for (GU8 uchFunCnt = 0; uchFunCnt < GH30X_FUNC_OFFSET_MAX; uchFunCnt ++)
    {
        if (((1<<uchFunCnt) & unFunctionID) == (1<<uchFunCnt))
        {
            /* int algo */
            const STGh30xFrameInfo * const pstFrameInfo = g_pstGh30xFrameInfo[uchFunCnt];
            if (1 == g_uchAlgInitFlag[uchFunCnt])
            {
                GH30X_ALGO_LOG_PARAM("[%s]:Function(function id = 0x%X) already init!\r\n", __FUNCTION__, (1<<uchFunCnt));
            }
            else
            {
                if (g_pfnAlgorithmCallFunc[uchFunCnt][GH30X_ALGO_INIT_FUNCTION] != GH30X_PTR_NULL)
                {
                    chRet = g_pfnAlgorithmCallFunc[uchFunCnt][GH30X_ALGO_INIT_FUNCTION](pstFrameInfo);
                    if (chRet != GH30X_RET_OK)
                    {
                        GH30X_ALGO_LOG_PARAM("[%s]:Function(function id = 0x%X) init fail! errcode = %d\r\n", __FUNCTION__, (1<<uchFunCnt), (int)chRet);
                    }
                    else
                    {
                        GH30X_ALGO_LOG_PARAM("[%s]:Function(function id = 0x%X) init success!\r\n", __FUNCTION__, (1<<uchFunCnt));
                        g_uchAlgInitFlag[uchFunCnt] = 1;
                    }
                }
                else
                {
                    GH30X_ALGO_LOG_PARAM("[%s]:Function(function id = 0x%X) init func null!\r\n", __FUNCTION__, (1<<uchFunCnt));
                }
            }
            GH30X_RET_ERROR_CHECK(chRet);
        }
    }
    return chRet;
}

GS8 GH30X_AlgoDeinit(GU32 unFunctionID)
{
    GS32 chRet = GH30X_RET_OK;

    /* clear started bit */
    g_unAlgoFuncStartedBitmap &= (~unFunctionID);

    /* deint algo */
    for (GU8 uchFunCnt = 0; uchFunCnt < GH30X_FUNC_OFFSET_MAX; uchFunCnt ++)
    {
        if (((1<<uchFunCnt) & unFunctionID) == (1<<uchFunCnt))
        {
            /* deint algo */
            const STGh30xFrameInfo * const pstFrameInfo = g_pstGh30xFrameInfo[uchFunCnt];
            if (0 == g_uchAlgInitFlag[uchFunCnt])
            {
                //GH30X_ALGO_LOG_PARAM("[%s]:Function(function id = %d) didn't init!\r\n", __FUNCTION__, (int)uchFunCnt);
            }
            else
            {
                if (g_pfnAlgorithmCallFunc[uchFunCnt][GH30X_ALGO_DEINIT_FUNCTION] != GH30X_PTR_NULL)
                {
                    chRet = g_pfnAlgorithmCallFunc[uchFunCnt][GH30X_ALGO_DEINIT_FUNCTION](pstFrameInfo);
                    if (chRet != GH30X_RET_OK)
                    {
                        GH30X_ALGO_LOG_PARAM("[%s]:Function(function id = 0x%X) deinit fail! errcode = %d\r\n", __FUNCTION__, (1<<uchFunCnt), (int)chRet);
                    }
                    else
                    {
                        GH30X_ALGO_LOG_PARAM("[%s]:Function(function id = 0x%X) deinit success!\r\n", __FUNCTION__, (1<<uchFunCnt));
                        g_uchAlgInitFlag[uchFunCnt] = 0;
                    }
                }
                else
                {
                    GH30X_ALGO_LOG_PARAM("[%s]:Function(function id = 0x%X) deinit func null!\r\n", __FUNCTION__, (1<<uchFunCnt));
                }
            }
            GH30X_RET_ERROR_CHECK(chRet);
        }
    }
    return chRet;
}

GS8 GH30X_AlgoCalculate(GU32 unFunctionID)
{
    if (g_unAlgoFuncStartedBitmap == GH30X_NO_FUNCTION)
    {
        return GH30X_RET_OK;
    }

    GS32 chRet = GH30X_RET_GENERIC_ERROR;

    for (GU8 uchFunCnt = 0; uchFunCnt < GH30X_FUNC_OFFSET_MAX; uchFunCnt ++)
    {
        if (((1<<uchFunCnt) & unFunctionID) == (1<<uchFunCnt))
        {
            /* calc algo */
            if (GH30X_CHECK_BIT_SET(g_unAlgoFuncStartedBitmap, unFunctionID))
            {
                const STGh30xFrameInfo * const pstFrameInfo = g_pstGh30xFrameInfo[uchFunCnt];
                if (GH30xSleepFlagGet())
                {
                    //pstFrameInfo->punFrameFlag[2] |= ((GU32)1)<<3;
                }
                if (0 == g_uchAlgInitFlag[uchFunCnt])
                {
                    //GH30X_ALGO_LOG_PARAM("Function(funcition id = %d) didn't init!\r\n",(int)uchFunCnt);
                }
                else
                {
                    if (g_pfnAlgorithmCallFunc[uchFunCnt][GH30X_ALGO_CALCULATE_FUNCTION] != GH30X_PTR_NULL)
                    {
                        chRet = g_pfnAlgorithmCallFunc[uchFunCnt][GH30X_ALGO_CALCULATE_FUNCTION](pstFrameInfo);
                    }
                }
            }
        }
        else
        {
            chRet = GH30X_RET_OK;
        }
    }
    return chRet;
}

#else

GS8 GH30X_AlgoInit(GU32 unFunctionID)
{
    return 0;
}

GS8 GH30X_AlgoDeinit(GU32 unFunctionID)
{
    return 0;
}

void GH30X_AlgoVersion(GU8 uchFunctionIDOffset)
{
    return;
}

GS8 GH30X_AlgoCalculate(GU32 unFunctionID)
{
    return 0;
}

void GH30X_AlgoCallConfigInit(const STGh30xFrameInfo *const pSTGh30xFrameInfo[])
{
    return;
}

void GH30X_AlgoLog(GCHAR *log_string)
{
    return;
}

#endif

