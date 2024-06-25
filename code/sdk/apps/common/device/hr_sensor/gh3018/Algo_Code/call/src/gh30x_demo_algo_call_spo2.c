/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_algo_call_spo2.c
 *
 * @brief   gh30x algorithm spo2 interface
 *
 * @version ref gh30x_example_common.h
 *
 */

#include <stdio.h>
#include "../../common/inc/gh30x_demo_algo_call.h"
#include "../../common/inc/gh30x_demo_algorithm_calc.h"
#include "../../common/inc/gh30x_demo_algo_config.h"
#include "../inc/goodix_algo.h"
#include "../../../Lib_Code/inc/goodix_spo2.h"

#if (__USE_GOODIX_SPO2_ALGORITHM__)

goodix_spo2_ret goodix_spo2_init_func(GU32 fs)
{
    goodix_spo2_ret stAlgoRet = GX_ALGO_SPO2_SUCCESS;
    //GU8 uchChMapCnt = 0;
    //GU8 uchChIndex  = 0;

    GCHAR uchSpo2Version[100] = {0};
    goodix_spo2_version((uint8_t*)uchSpo2Version);
    //GH30X_ALGO_LOG_PARAM("spo2 algorithm version : %s\r\n", uchSpo2Version);
    //GH30X_ALGO_LOG_PARAM("spo2 algorithm legal chnl num : %d\r\n", g_stSpo2AlgoChnlMap.uchNum);

#if (__HBD_ALGORITHM_VERTION_PRO__ == __HBD_ALGORITHM_VERTION__)

    goodix_spo2_config stSpo2Config = {
                                        1,                                  // channel number
                                        fs,                                 // sample rate
                                        -162708, -30749, 1035316,           // -21742, -201492, 1077714, // cali a, b, c
                                        1,                                  // heart rate calculate enable
                                        0,                                  // wear mode
                                        10, 5, 3, 3,                        // 30, 15, 4, 3, // 6. Acc相关
                                        1, 65,                              // 0, 0, // ctr相关
    };

#elif (__HBD_ALGORITHM_VERTION_TINY__ == __HBD_ALGORITHM_VERTION__)

    goodix_spo2_config stSpo2Config = {
                                        1,                                  // channel number
                                        fs,                                 // sample rate
                                        -162708, -30749, 1035316,           // -21742, -201492, 1077714, // cali a, b, c
                                        1,                                  // heart rate calculate enable
                                        0,                                  // wear mode
                                        30, 15, 4, 3,                       // 30, 15, 4, 3, // 6. Acc相关
                                        0, 0,                               // ctr相关
                                        30, 20,
                                        10, 4, 6, 4, 5,                     // 出值策略
    };

#elif (__HBD_ALGORITHM_VERTION_BASIC__ == __HBD_ALGORITHM_VERTION__)

    goodix_spo2_config stSpo2Config = {
                                        1,                                  // channel number
                                        fs,                                 // sample rate
                                        0, 0, -134717, -115284, 1097668,    // cali a, b, c
                                        0,                                  // heart rate calculate enable
                                        0,                                  // wear mode
                                        30, 15, 4, 3, 90,                   // Acc相关
                                        1, 65,                              // ctr相关
                                        30, 10, 90,                         // ppg相关
                                        0xFD,                               // 11111101b
                                        95,                                 // low_spo2_thr
                                        10, 25, 4, 6, 10, 5,                // 出值策略
                                        0x03,                               // output_module_key
    };

#endif

#if (__HBD_ALGORITHM_VERTION_PRO__ == __HBD_ALGORITHM_VERTION__)

    GH30X_ALGO_LOG_PARAM("[%s]: S version params = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n", __FUNCTION__,
                            stSpo2Config.valid_chl_num,
                            stSpo2Config.raw_fs,
                            stSpo2Config.cali_coef_a,
                            stSpo2Config.cali_coef_b,
                            stSpo2Config.cali_coef_c,
                            stSpo2Config.hb_en_flg,
                            stSpo2Config.wear_mode,
                            stSpo2Config.acc_thr_max,
                            stSpo2Config.acc_thr_min,
                            stSpo2Config.acc_thr_scale,
                            stSpo2Config.acc_thr_num,
                            stSpo2Config.ctr_en_flg,
                            stSpo2Config.ctr_red_thr);

#elif (__HBD_ALGORITHM_VERTION_TINY__ == __HBD_ALGORITHM_VERTION__)

    GH30X_ALGO_LOG_PARAM("[%s]: Tiny version params = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", __FUNCTION__,
                            stSpo2Config.valid_chl_num,
                            stSpo2Config.raw_fs,
                            stSpo2Config.cali_coef_a,
                            stSpo2Config.cali_coef_b,
                            stSpo2Config.cali_coef_c,
                            stSpo2Config.hb_en_flg,
                            stSpo2Config.wear_mode,
                            stSpo2Config.acc_thr_max,
                            stSpo2Config.acc_thr_min,
                            stSpo2Config.acc_thr_scale,
                            stSpo2Config.acc_thr_num,
                            stSpo2Config.ctr_en_flg,
                            stSpo2Config.ctr_red_thr,
                            stSpo2Config.ppg_jitter_thr,
                            stSpo2Config.ppg_periodity_thr,
                            stSpo2Config.fast_out_time,
                            stSpo2Config.min_stable_time_high,
                            stSpo2Config.min_stable_time_low,
                            stSpo2Config.max_spo2_variation_high,
                            stSpo2Config.max_spo2_variation_low);

#elif (__HBD_ALGORITHM_VERTION_BASIC__ == __HBD_ALGORITHM_VERTION__)

    GH30X_ALGO_LOG_PARAM("[%s]: Basic version params1 = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", __FUNCTION__,
                            stSpo2Config.valid_chl_num,
                            stSpo2Config.raw_fs,
                            stSpo2Config.cali_coef_a4,
                            stSpo2Config.cali_coef_a3,
                            stSpo2Config.cali_coef_a2,
                            stSpo2Config.cali_coef_a1,
                            stSpo2Config.cali_coef_a0,
                            stSpo2Config.hb_en_flg,
                            stSpo2Config.wear_mode,
                            stSpo2Config.acc_thr_max,
                            stSpo2Config.acc_thr_min,
                            stSpo2Config.acc_thr_scale,
                            stSpo2Config.acc_thr_num,
                            stSpo2Config.acc_thr_angle,
                            stSpo2Config.ctr_en_flg,
                            stSpo2Config.ctr_red_thr);

    GH30X_ALGO_LOG_PARAM("[%s]: Basic version params2 = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", __FUNCTION__,
                            stSpo2Config.ppg_jitter_thr,
                            stSpo2Config.ppg_noise_thr,
                            stSpo2Config.ppg_coeff_thr,
                            stSpo2Config.quality_module_key,
                            stSpo2Config.low_spo2_thr,
                            stSpo2Config.fast_out_time,
                            stSpo2Config.slow_out_time,
                            stSpo2Config.min_stable_time_high,
                            stSpo2Config.min_stable_time_low,
                            stSpo2Config.max_spo2_variation_high,
                            stSpo2Config.max_spo2_variation_low,
                            stSpo2Config.ouput_module_key);
#endif

    stAlgoRet = goodix_spo2_init(&stSpo2Config);
    return stAlgoRet;
}

goodix_spo2_ret goodix_spo2_deinit_func(void)
{
    goodix_spo2_deinit();
    return GX_ALGO_SPO2_SUCCESS;
}

GS8 GH30xSpo2AlgoInit(const STGh30xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH30X_RET_GENERIC_ERROR;

    //printf("%s\n", __func__);
    if (goodix_spo2_init_func(pstFrameInfo ->pstFunctionInfo -> usOutputDataRate) == GX_ALGO_SPO2_SUCCESS)
    {
        chRet = GH30X_RET_OK;
    }
    else
    {
        GH30X_ALGO_LOG_PARAM("spo2 init error!\r\n");
    }
    return chRet;
}


GS8 GH30xSpo2AlgoDeinit(const STGh30xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH30X_RET_GENERIC_ERROR;

    GH30X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_spo2_deinit_func() == GX_ALGO_SPO2_SUCCESS)
    {
        chRet = GH30X_RET_OK;
    }
    else
    {
        GH30X_ALGO_LOG_PARAM("spo2 deinit error!\r\n");
    }
    return chRet;
}


GS8 GH30xSpo2AlgoExe(const STGh30xFrameInfo * const pstFrameInfo)
{
    if (0 == pstFrameInfo )
    {
        return GH30X_RET_GENERIC_ERROR;
    }
    goodix_spo2_input_rawdata stRawdata = {0};
    goodix_spo2_result stResult;
    goodix_spo2_ret emAlgoRet = GX_ALGO_SPO2_SUCCESS;

    stRawdata.frameid = GH30X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    stRawdata.bit_num = 24;
    stRawdata.ppg_rawdata[4] = (pstFrameInfo->punRawdata[0] & 0x00FFFFFF);  //IR
    stRawdata.ppg_rawdata[8] = (pstFrameInfo->punRawdata[1] & 0x00FFFFFF);  //red
    stRawdata.enable_flg[4] = 1;
    stRawdata.enable_flg[8] = 1;
    stRawdata.cur_adj_flg[4] = ((pstFrameInfo->punRawdata[0] >> 30) & 0x00000001);
    stRawdata.cur_adj_flg[8] = ((pstFrameInfo->punRawdata[1] >> 30) & 0x00000001);
    stRawdata.acc_x = pstFrameInfo->pusGsensordata[0];
    stRawdata.acc_y = pstFrameInfo->pusGsensordata[1];
    stRawdata.acc_z = pstFrameInfo->pusGsensordata[2];
    stRawdata.wear_on_flag = 1;
    
    /* call algorithm, update result */
    memset(&stResult, 0, sizeof(goodix_spo2_result));

    emAlgoRet = goodix_spo2_calc(&stRawdata, &stResult);
    if (emAlgoRet == GX_ALGO_SPO2_SUCCESS || emAlgoRet == GX_ALGO_SPO2_FRAME_UNCOMPLETE || emAlgoRet == GX_ALGO_SPO2_WIN_UNCOMPLETE)
    {
        if (stResult.final_calc_flg == 1)
        {
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = (GU8)stResult.final_calc_flg;
            pstFrameInfo->pstAlgoResult->snResult[0] = (GS32)(GH30x_Round((float)stResult.final_spo2/10000));
            pstFrameInfo->pstAlgoResult->snResult[1] = (GS32)stResult.final_r_val;
            pstFrameInfo->pstAlgoResult->snResult[2] = (GS32)stResult.final_confi_coeff;
            pstFrameInfo->pstAlgoResult->snResult[3] = (GS32)stResult.final_valid_level;
            pstFrameInfo->pstAlgoResult->snResult[4] = (GS32)stResult.final_hb_mean;
            pstFrameInfo->pstAlgoResult->snResult[5] = (GS32)stResult.final_invalidFlg;
            pstFrameInfo->pstAlgoResult->snResult[6] = (GS32)stResult.final_hb_confi_lvl;
            pstFrameInfo->pstAlgoResult->usResultBit = 0x7F;
            pstFrameInfo->pstAlgoResult->uchResultNum = GH30x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
					  GH30X_ALGO_LOG_PARAM("spo2 = %d, R = %d \r\n", pstFrameInfo->pstAlgoResult->snResult[0], pstFrameInfo->pstAlgoResult->snResult[1]);
            GH30X_Spo2AlgorithmResultReport(pstFrameInfo->pstAlgoResult,pstFrameInfo->punFrameCnt[0]);
        }
    }
    else
    {
        GH30X_ALGO_LOG_PARAM("error code = 0x%x, please feedback to GOODIX!\r\n", (int)emAlgoRet);
    }
    return (GS8)emAlgoRet;
}

#endif /* __USE_GOODIX_SPO2_ALGORITHM__ */
