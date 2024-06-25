/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_algo_call_hrv.c
 *
 * @brief   gh30x algorithm hrv interface
 *
 * @version ref gh30x_example_common.h
 *
 */

#include <stdio.h>
#include "../../common/inc/gh30x_demo_algo_call.h"
#include "../../common/inc/gh30x_demo_algorithm_calc.h"
#include "../../../Lib_Code/inc/goodix_hba.h"
#include "../../../Lib_Code/inc/goodix_sys_hrv.h"

#if (__USE_GOODIX_HRV_ALGORITHM__)

//hrv
goodix_hba_ret goodix_hrv_init_func(GU32 fs)
{
    GS32 nRet = GX_ALGO_HBA_SUCCESS;

    goodix_hrv_config stHrvConfig = {
                                        0,
                                        100,
                                        {200000, 100000, 30000, 30000}
                                    };

    stHrvConfig.fs = fs;
    GH30X_ALGO_LOG_PARAM("stHrvConfig.fs = %d %d %d %d %d %d\n\r", stHrvConfig.fs, stHrvConfig.need_ipl, stHrvConfig.acc_thr[0], \
        stHrvConfig.acc_thr[1], stHrvConfig.acc_thr[2], stHrvConfig.acc_thr[3]);

    nRet = (GS32)goodix_hrv_init(&stHrvConfig);

    return (goodix_hba_ret)nRet;
}

GS8 GH30xHrvAlgoInit(const STGh30xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH30X_RET_GENERIC_ERROR;

    GH30X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_hrv_init_func(pstFrameInfo ->pstFunctionInfo ->usOutputDataRate) == GX_ALGO_HBA_SUCCESS)        
    {
        chRet = GH30X_RET_OK;
    }
    else
    {
        GH30X_ALGO_LOG_PARAM("hrv init error!\r\n");
    }
    return chRet;
}

GS8 GH30xHrvAlgoDeinit(const STGh30xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH30X_RET_OK;
    goodix_hrv_deinit();

    return chRet;
}

GS8 GH30xHrvAlgoExe(const STGh30xFrameInfo * const pstFrameInfo)
{
    goodix_hrv_input_rawdata stHrvInputRawdata = {0};
    goodix_hrv_result stHrvResult = {0};
    GS32 ret = 0;

    if(0 == pstFrameInfo )
    {
        return GH30X_RET_GENERIC_ERROR;
    }

    stHrvInputRawdata.bit_num = 24;
    stHrvInputRawdata.frame_id = GH30X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    stHrvInputRawdata.ppg_rawdata[0] = (pstFrameInfo->punRawdata[0] & 0x00FFFFFF);
    stHrvInputRawdata.acc_x = pstFrameInfo->pusGsensordata[0];
    stHrvInputRawdata.acc_y = pstFrameInfo->pusGsensordata[1];
    stHrvInputRawdata.acc_z = pstFrameInfo->pusGsensordata[2];

    ret = (GS32)goodix_hrv_calc(&stHrvInputRawdata, &stHrvResult);

    if (ret == GX_HRV_ALGO_UPDATE)
    {
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
        pstFrameInfo->pstAlgoResult->snResult[0] = stHrvResult.rri[0];
        pstFrameInfo->pstAlgoResult->snResult[1] = stHrvResult.rri[1];
        pstFrameInfo->pstAlgoResult->snResult[2] = stHrvResult.rri[2];
        pstFrameInfo->pstAlgoResult->snResult[3] = stHrvResult.rri[3];
        pstFrameInfo->pstAlgoResult->snResult[4] = stHrvResult.rri_confidence;
        pstFrameInfo->pstAlgoResult->snResult[5] = stHrvResult.rri_valid_num;
        pstFrameInfo->pstAlgoResult->usResultBit = 0x3F;
        pstFrameInfo->pstAlgoResult->uchResultNum = GH30x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
        GH30X_ALGO_LOG_PARAM("stHrvResult = %d %d %d %d %d %d\n\r", stHrvResult.rri[0], stHrvResult.rri[1], stHrvResult.rri[2], \
            stHrvResult.rri[3], stHrvResult.rri_confidence, stHrvResult.rri_valid_num);
        GH30X_HrvAlgorithmResultReport(pstFrameInfo->pstAlgoResult, pstFrameInfo->punFrameCnt[0]);
    }

    return GH30X_RET_OK;
}

#endif /* __USE_GOODIX_HRV_ALGORITHM__ */
