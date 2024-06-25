/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_algo_call_hr.c
 *
 * @brief   gh30x algorithm hr interface
 *
 * @version ref gh30x_example_common.h
 *
 */

#include <stdio.h>
#include "../../common/inc/gh30x_demo_algo_call.h"
#include "../../common/inc/gh30x_demo_algorithm_calc.h"
#include "../../common/inc/gh30x_demo_algo_config.h"
#include "../../../Lib_Code/inc/goodix_hba.h"


#if (__USE_GOODIX_HR_ALGORITHM__)

goodix_hba_ret goodix_hba_init_func(GU32 fs)
{
    goodix_hba_config stHbCfg;
    goodix_hba_ret stAlgoRet = GX_ALGO_HBA_SUCCESS;

    GCHAR uchHrVersion[100] = {0};
    goodix_hba_version((uint8_t*)uchHrVersion);    

    GH30X_ALGO_LOG_PARAM("hba algorithm version : %s\r\n", uchHrVersion);

    memset(&stHbCfg, 0, sizeof(goodix_hba_config));
    stHbCfg.mode = HBA_TEST_DYNAMIC;
    stHbCfg.scence = HBA_SCENES_DEFAULT;
    stHbCfg.fs = fs;
    stHbCfg.valid_channel_num = 1;
    stHbCfg.back_track_len = 0;
    stHbCfg.hba_latest_output_time = 0;
    stHbCfg.hba_earliest_output_time = 0;

    GH30X_ALGO_LOG_PARAM("[%s]:params = %d,%d,%d,%d,%d,%d,%d,\r\n", __FUNCTION__,
                                                                            stHbCfg.mode,
                                                                            stHbCfg.scence,
                                                                            stHbCfg.fs,
                                                                            stHbCfg.valid_channel_num,
                                                                            stHbCfg.back_track_len,
                                                                            stHbCfg.hba_latest_output_time,
                                                                            stHbCfg.hba_earliest_output_time
                                                                            );

    stAlgoRet = goodix_hba_init(&stHbCfg);

    return stAlgoRet;
}

goodix_hba_ret goodix_hba_deinit_func(void)
{
    goodix_hba_deinit();
    return GX_ALGO_HBA_SUCCESS;
}

GS8 GH30xHrAlgoInit(const STGh30xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH30X_RET_GENERIC_ERROR;

    printf("%s\r\n", __func__);
    if (goodix_hba_init_func(pstFrameInfo->pstFunctionInfo->usOutputDataRate) == GX_ALGO_HBA_SUCCESS)
    {
        chRet = GH30X_RET_OK;
        GH30X_ALGO_LOG_PARAM("[%s]GH30X_HbaInit success!!\r\n", __FUNCTION__);
    }
    else
    {
        GH30X_ALGO_LOG_PARAM("hba init error!\r\n");
    }
    pstFrameInfo->pstAlgoResult->uchUpdateFlag = 0;
    pstFrameInfo->pstAlgoResult->snResult[0] = 0;

    return chRet;
}

GS8 GH30xHrAlgoDeinit(const STGh30xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH30X_RET_GENERIC_ERROR;
    GH30X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);

    if (goodix_hba_deinit_func() == GX_ALGO_HBA_SUCCESS)
    {
        chRet = GH30X_RET_OK;
    }
    else
    {
        GH30X_ALGO_LOG_PARAM("hba deinit error!\r\n");
    }
    return chRet;
}

//call algo 2-level interface

GS8 GH30xHrAlgoExe(const STGh30xFrameInfo * const pstFrameInfo)
{
    GS8 chAlgoRet = 0;
    if(0 == pstFrameInfo )
    {
        return GH30X_RET_GENERIC_ERROR;
    }

    goodix_hba_input_rawdata stRawdata = {0};
    goodix_hba_result stResult = {0};

    stRawdata.frameid = GH30X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    stRawdata.ppg_rawdata[0] = (pstFrameInfo->punRawdata[0] & 0x00FFFFFF);
    stRawdata.enable_flg[0] = 1;
    stRawdata.acc_x = pstFrameInfo->pusGsensordata[0];
    stRawdata.acc_y = pstFrameInfo->pusGsensordata[1];
    stRawdata.acc_z = pstFrameInfo->pusGsensordata[2];
    stRawdata.sleep_flg = 0;

    #if 0
    GH30X_ALGO_LOG_PARAM("[%s]:Rawdata = %d,0x%X,%d\r\n", __FUNCTION__,
                                                                            stRawdata.frameid,
                                                                            stRawdata.ppg_rawdata[0],
                                                                            stRawdata.enable_flg[0]
                                                                            );
    #endif

    /* call algorithm, update result */
    if (goodix_hba_update(&stRawdata, &stResult) == GX_ALGO_HBA_SUCCESS)
    {
        //GH30X_ALGO_LOG_PARAM("[GH30xHrAlgoExe] hba_out_flag:%d \r\n", stResult.hba_out_flag);
        if (stResult.hba_out_flag == 1)
        {
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = (GU8)stResult.hba_out_flag;
            pstFrameInfo->pstAlgoResult->snResult[0] = (GS32)stResult.hba_out;
            pstFrameInfo->pstAlgoResult->snResult[1] = (GS32)stResult.valid_score;
            pstFrameInfo->pstAlgoResult->snResult[2] = GH30X_ALGORITHM_GF32_2_GS32(stResult.hba_snr);
            pstFrameInfo->pstAlgoResult->snResult[3] = (GS32)stResult.valid_level;
            pstFrameInfo->pstAlgoResult->snResult[4] = (GS32)stResult.hba_acc_info;
            pstFrameInfo->pstAlgoResult->snResult[5] = (GS32)stResult.hba_acc_scence;

            printf("[%s]hr = %d,UpdateFlag = %d\r\n",__func__,\
                (int)pstFrameInfo->pstAlgoResult->snResult[0],(int)pstFrameInfo->pstAlgoResult->uchUpdateFlag);

            pstFrameInfo->pstAlgoResult->usResultBit = 0x3F;
            pstFrameInfo->pstAlgoResult->uchResultNum = GH30x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
#if __GH30X_HR_OUTPUT_VALUE_STRATEGY_EN__
            Gh30xHrOutputValueStrategyPro(pstFrameInfo->pstAlgoResult,pstFrameInfo->punFrameCnt[0]);
#endif
            GH30X_HrAlgorithmResultReport(pstFrameInfo->pstAlgoResult,pstFrameInfo->punFrameCnt[0]);
            
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = (GU8)stResult.hba_out_flag;
            pstFrameInfo->pstAlgoResult->snResult[0] = (GS32)stResult.hba_out;
        }
    }
    else
    {
        chAlgoRet = GH30X_RET_RESOURCE_ERROR;
        GH30X_ALGO_LOG_PARAM("no enough ram for algorithm error!\r\n");
        GH30X_ALGO_LOG_PARAM("please feedback to GOODIX!\r\n");
    }
    return chAlgoRet;
}

#endif  /* __USE_GOODIX_HR_ALGORITHM__ */

