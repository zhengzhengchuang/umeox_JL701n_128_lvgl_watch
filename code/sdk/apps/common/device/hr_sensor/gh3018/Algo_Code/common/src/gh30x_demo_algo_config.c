/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_algo_config.c
 *
 * @brief   gh30x algorithm config
 *
 * @version ref gh30x_example_common.h
 *
 */

#include <stdio.h>
#include "../inc/gh30x_demo_algorithm_calc.h"
#include "../inc/gh30x_demo_algo_config.h"
#include "../../call/inc/goodix_algo.h"
#include "../../call/inc/goodix_mem.h"
#include "../../../Lib_Code/inc/goodix_type.h"
#include "../../../Lib_Code/inc/goodix_hba.h"
#include "../../../Lib_Code/inc/goodix_sys_hrv.h"
#include "../../../Lib_Code/inc/goodix_spo2.h"
#include "../../../Lib_Code/inc/goodix_nadt.h"

#if (__GOODIX_ALGO_CALL_MODE__)

/* hb algorithm */
#if (__USE_GOODIX_HR_ALGORITHM__)
//GS32 g_unHrParamsArr[GOODIX_HR_PARAMS_NUM_MAX] = {0, 0, 0, 0, 0};
/// hbd rawdata channel map
//STAlgoChnlMap g_stHbaAlgoChnlMap;
#endif

/* hrv algorithm */
#if (__USE_GOODIX_HRV_ALGORITHM__)
//GS32 g_nHrvParamsArr[GOODIX_HRV_PARAMS_NUM_MAX] = {0, 20, 10, 3, 3};
#endif

/* spo2 algorithm */
#if (__USE_GOODIX_SPO2_ALGORITHM__)
#if 0
GS32 g_nSpo2ParamssArr[GOODIX_SPO2_PARAMS_NUM_MAX] = {15757, -290090, 1118150,
                                                      1,//3. 心率使能标志
                                                      0,//4. 佩戴模式
                                                      30, 15, 4, 3,//6. Acc相关
                                                      0, 0
                                                      };
#endif
/// spo2 rawdata channel map
//STAlgoChnlMap g_stSpo2AlgoChnlMap;
#endif

/* ecg algorithm */
#if (__USE_GOODIX_ECG_ALGORITHM__)
GS32 g_nEcgParamsArr[GOODIX_ECG_PARAMS_NUM_MAX] = {0, 1, 1};
/// ecg frame data cnt
GU32 g_unEcgFrameCnt = 0;
GU8 g_uchClassificationFlag;
GU8 g_unEcgTimeSeconds = 0;
#endif

/* bt algorithm */
#if (__USE_GOODIX_BT_ALGORITHM__)
GS32 g_nBtParamsArr[GOODIX_BT_PARAMS_NUM_MAX] = {0};
#endif

/* af algorithm */
#if (__USE_GOODIX_AF_ALGORITHM__)
GS32 g_nAfParamsArr[GOODIX_AF_PARAMS_NUM_MAX] = {45, 0};
#endif

/// new data flag
GU8 g_uchNewDataFlag = 0;
GU8 g_uchGh30xSleepFlag = 0;

//sensor enable
GU8 g_uchAlgoGsensorEnable = 0;
GU8 g_uchAlgoCapEnable = 0;
GU8 g_uchAlgoTempEnable = 0;

GS32 GH30x_Round(GF32 fValue)
{
    GS32 nRet = 0;
    float fEpsinon = 0.00001;
    if((fValue >= -fEpsinon)&&(fValue <= fEpsinon))
    {
        nRet = 0;
    }
    else if(fValue < -fEpsinon)
    {
        nRet = (GS32)(fValue - 0.5f);
    }
    else if(fValue > fEpsinon)
    {
        nRet = (GS32)(fValue + 0.5f);
    }
    return nRet;
}

GU8 GH30x_BitCount(GU32 n)
{
    GU8 c =0 ; // 计数器
    while (n >0)
    {
        if((n &1) ==1) // 当前位是1
            ++c ; // 计数器加1
        n >>=1 ; // 移位
    }
    return c ;
}

void GH30xSleepFlagSet(GU8 uchSleepFlag)
{
    g_uchGh30xSleepFlag = uchSleepFlag;
}

GU8 GH30xSleepFlagGet(void)
{
    return g_uchGh30xSleepFlag;
}

void GH30X_AlgoSensorEnable(GU8 uchAlgoGsensorEnable,
                            GU8 uchAlgoCapEnable,
                            GU8 uchAlgoTempEnable)
{
    g_uchAlgoGsensorEnable = uchAlgoGsensorEnable;
    g_uchAlgoCapEnable = uchAlgoCapEnable;
    g_uchAlgoTempEnable = uchAlgoTempEnable;
}

/**
 * @fn     void GH30X_SetNewDataFlag(EMChannelMapId emFuncId, GU8 uchFlag)
 *
 * @brief  Set new sampling data flag
 *
 * @attention   None
 *
 * @param[in]   emFuncId    function id
 * @param[in]   uchFlag      new data or not
 * @param[out]  None
 *
 * @return  None
 */
void GH30X_SetNewDataFlag(GU8 emFuncId, GU8 uchFlag)
{
    if (GH30X_IS_NOT_NEW_RAWDATA == uchFlag)
    {
        GH30X_VAL_CLEAR_LEFT_BIT(g_uchNewDataFlag, emFuncId);
    }
    else
    {
        GH30X_VAL_SET_BIT(g_uchNewDataFlag, GH30X_GET_LEFT_SHIFT_VAL(emFuncId));
    }
}

GU16 GH30X_ReadSpo2CorrectionFactor(GU16 usVirtualRegAddr)
{
#if 0
    GU16 usVirtualRegData = 0;
    GU16 usValIndex = 0;

    if(usVirtualRegAddr >= GH30X_VSPO2_CH0_CORFAC0_L_REG_ADDR && usVirtualRegAddr <= GH30X_VSPO2_CH0_CORFAC2_H_REG_ADDR)
    {
        usValIndex = (usVirtualRegAddr - GH30X_VSPO2_CH0_CORFAC0_L_REG_ADDR) / GH30X_REG_ADDR_SIZE;
    }
    else if(usVirtualRegAddr >= GH30X_VSPO2_CH0_CORFAC3_L_REG_ADDR && usVirtualRegAddr <= GH30X_VSPO2_CH3_CORFAC4_H_REG_ADDR)
    {
        usValIndex = (usVirtualRegAddr - GH30X_VSPO2_CH0_CORFAC3_L_REG_ADDR) / GH30X_REG_ADDR_SIZE + 6;
    }
    else
    {
        return usVirtualRegData;
    }
    
   if ((usValIndex % GH30X_REG_ADDR_SIZE) == 0) // low word
   {
       usVirtualRegData = GH30X_GET_LOW_WORD_FROM_DWORD(
                            g_nSpo2ParamssArr[usValIndex / (SP02_CORR_FAC_NUM * 2)][(usValIndex % (SP02_CORR_FAC_NUM * 2)) / GH30X_REG_ADDR_SIZE]);
   }
   else // high word
   {
       usVirtualRegData = GH30X_GET_HIGH_WORD_FROM_DWORD(
                            g_nSpo2ParamssArr[usValIndex / (SP02_CORR_FAC_NUM * 2)][(usValIndex % (SP02_CORR_FAC_NUM * 2)) / GH30X_REG_ADDR_SIZE]);
   }

   
   return usVirtualRegData;
#else
     return 0;
#endif
}

/**
 * @fn     void GH30X_AlgoChnlMapInit(STAlgoChnlMap stAlgoChnlMap)
 *
 * @brief  Init algo channel group struct
 *
 * @attention   
 *
 *
 * @return  None
 */
void GH30X_AlgoChnlMapInit(STAlgoChnlMap *pstAlgoChnlMap)
{
    (*pstAlgoChnlMap).uchFlag = 0;
    (*pstAlgoChnlMap).uchNum = 0;
    for (GU32 num = 0  ; num < ALGO_CHNL_NUM ; num ++)
    {
        (*pstAlgoChnlMap).uchAlgoChnlMap[num] = 0xff;
    }
}

/**
 * @fn     void GH30X_HbaAlgoChnlMapDefultSet(void)
 
 *
 * @brief  Set algo channel group Defult value
 *
 * @attention   
 *
 *
 * @return  None
 */
#if 0
void GH30X_HbaAlgoChnlMapDefultSet(void)
{
#if (__USE_GOODIX_HR_ALGORITHM__)
    if (!g_stHbaAlgoChnlMap.uchFlag)
    {
        GH30X_AlgoChnlMapInit(&g_stHbaAlgoChnlMap);
        g_stHbaAlgoChnlMap.uchNum = __HR_ALGORITHM_SUPPORT_CHNL_NUM__;
        for (GU8 cnt = 0; cnt < __HR_ALGORITHM_SUPPORT_CHNL_NUM__; cnt++)
        {
            g_stHbaAlgoChnlMap.uchAlgoChnlMap[ALGO_GREEN_CHNL_POS + cnt] = cnt;
        }
    }
#endif
}
#endif

/**
 * @fn     void GH30X_HrvAlgoChnlMapDefultSet(void)
 
 *
 * @brief  Set algo channel group Defult value
 *
 * @attention   
 *
 *
 * @return  None
 */
void GH30X_HrvAlgoChnlMapDefultSet(void)
{
#if (0)
    if (!g_stHrvAlgoChnlMap.uchFlag)
    {
        GH30X_AlgoChnlMapInit(&g_stHrvAlgoChnlMap);
        g_stHrvAlgoChnlMap.uchNum = 1;
        g_stHrvAlgoChnlMap.uchAlgoChnlMap[ALGO_GREEN_CHNL_POS + 0] = 0;
    }
#endif
}

void GH30X_BpAlgoChnlMapDefultSet(void)
{
#if (__USE_GOODIX_BP_ALGORITHM__)
    if (!g_stBpAlgoChnlMap.uchFlag)
    {
        GH30X_AlgoChnlMapInit(&g_stBpAlgoChnlMap);
        g_stBpAlgoChnlMap.uchNum = 1;
        g_stBpAlgoChnlMap.uchAlgoChnlMap[ALGO_GREEN_CHNL_POS + 0] = 1;
        g_stBpAlgoChnlMap.uchEcgAlgoChnlMap = 0;
    }
#endif
}

#if 0
void GH30X_WriteAlgParametersWithVirtualReg(GU8 uchParasIndex, GU32 unFunctionID, GS32* nAlgParametersBuffer, GU8 nBufferMaxLen, GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    GU16 usValIndex = (usVirtualRegAddr - (unFunctionID * GH30X_VREG_FUNCTION_OFFSET + GH30X_ADT_ALG_CFG_ADDR)) / GH30X_REG_ADDR_SIZE;
    if ((GS16)((GS16)usValIndex / (GS16)GH30X_REG_ADDR_SIZE - (GS16)uchParasIndex < 0) ||\
        (usValIndex / GH30X_REG_ADDR_SIZE - uchParasIndex >= nBufferMaxLen))
    {
        return;
    }
    if ((usValIndex % GH30X_REG_ADDR_SIZE) != 0) // high word
    {
        nAlgParametersBuffer[usValIndex / GH30X_REG_ADDR_SIZE - uchParasIndex] = \
            GH30X_MAKEUP_DWORD2(usVirtualRegValue, GH30X_GET_LOW_WORD_FROM_DWORD(nAlgParametersBuffer[usValIndex / GH30X_REG_ADDR_SIZE - uchParasIndex]));

    }
    else // low word
    {
        nAlgParametersBuffer[usValIndex / GH30X_REG_ADDR_SIZE - uchParasIndex] = \
            GH30X_MAKEUP_DWORD2(GH30X_GET_HIGH_WORD_FROM_DWORD(nAlgParametersBuffer[usValIndex / GH30X_REG_ADDR_SIZE - uchParasIndex]), usVirtualRegValue);
    }
}
#endif

void GH30X_WriteAlgChnlMapWithVirtualReg(GU32 unFunctionID, STAlgoChnlMap* pstAlgChnlMap, GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    GU16 usVirtualRegAddrOffset = usVirtualRegAddr - (unFunctionID * GH30X_VREG_FUNCTION_OFFSET + GH30X_ADT_ALG_CFG_ADDR);
    switch (usVirtualRegAddrOffset)
    {
    case GH30X_VREG_ALGO_CHNL_MAP_OFFSET:
        GH30X_AlgoChnlMapInit(pstAlgChnlMap);
        pstAlgChnlMap->uchNum = GH30X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
        if (pstAlgChnlMap->uchNum)
        {
            pstAlgChnlMap->uchFlag = 1;
        }
        break;
    case GH30X_VREG_ALGO_CHNL_MAP_OFFSET + GH30X_REG_ADDR_SIZE:
        pstAlgChnlMap->uchEcgAlgoChnlMap = (GU8)(usVirtualRegValue & 0x000000FF);
        break;
    default:
        pstAlgChnlMap->uchAlgoChnlMap[usVirtualRegAddrOffset - GH30X_VREG_ALGO_CHNL_MAP_OFFSET - 2 * GH30X_REG_ADDR_SIZE]\
            = GH30X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
        pstAlgChnlMap->uchAlgoChnlMap[usVirtualRegAddrOffset - GH30X_VREG_ALGO_CHNL_MAP_OFFSET - 2 * GH30X_REG_ADDR_SIZE + 1]\
            = GH30X_GET_HIGH_BYTE_FROM_WORD(usVirtualRegValue);
    }
}

#if 0
void GH30X_WriteHrvAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_HRV_ALGORITHM__) // hrv
    GH30X_WriteAlgParametersWithVirtualReg(0, GH30X_FUNC_OFFSET_HRV, (GS32*)g_nHrvParamsArr, GOODIX_HRV_PARAMS_NUM_MAX, usVirtualRegAddr, usVirtualRegValue);
#endif
}
#endif

#if 0
void GH30X_WriteSpo2AlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_SPO2_ALGORITHM__) // spo2
    if (usVirtualRegAddr < GH30X_SPO2_ALGO_CHNL_NUM_ADDR)
    {
        GH30X_AlgoChnlMapInit(&g_stSpo2AlgoChnlMap);
        GH30X_WriteAlgParametersWithVirtualReg(0, GH30X_FUNC_OFFSET_SPO2, (GS32*)g_nSpo2ParamssArr, GOODIX_SPO2_PARAMS_NUM_MAX, usVirtualRegAddr,usVirtualRegValue);
    }
    else
    {
        GH30X_WriteAlgChnlMapWithVirtualReg(GH30X_FUNC_OFFSET_SPO2, &g_stSpo2AlgoChnlMap, usVirtualRegAddr, usVirtualRegValue);
    }
#endif
}
#endif

void GH30X_WriteEcgAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{

}

void GH30X_WriteBtAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{

}

void GH30X_WriteAfAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{

}

void GH30X_WriteBpAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_BP_ALGORITHM__) // fpbp

#endif
}

/**
 * @fn     GU16 GH30X_ReadAlgorithmConfigWithVirtualReg(GU16 usVirtualRegAddr)
 *
 * @brief  Read algorithm param config with virtual reg
 *
 * @attention   Virtual reg addr has del control bits, so reg addr is [0:11] valid.
 *
 * @param[in]   usVirtualRegAddr        virtual reg addr
 * @param[out]  None
 *
 * @return  virtual reg val
 */
GU16 GH30X_ReadAlgorithmConfigWithVirtualReg(GU16 usVirtualRegAddr)
{
    GU16 usVirtualRegData = 0;

    return usVirtualRegData;
}

void GH30X_WriteAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
}

#endif

/********END OF FILE********* Copyright (c) 2003 - 2024, Goodix Co., Ltd. ********/
