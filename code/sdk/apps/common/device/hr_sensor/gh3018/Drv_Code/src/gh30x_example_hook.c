/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_example_hook.c
 *
 * @brief   example code for gh30x
 *
 */

#include <string.h>
#include "math.h"
#include "../inc/gh30x_algo_hook.h"
#include "../inc/gh30x_example_common.h"
#include "../../Algo_Code/common/inc/gh30x_demo_algo_call.h"
#include "../../Algo_Code/common/inc/gh30x_demo_algo_config.h"

#if (__ALGO_LOG_CONFIG__)
#define EXAMPLE_DEBUG_LOG_L1_HOOK EXAMPLE_DEBUG_LOG_L1
#else
#define EXAMPLE_DEBUG_LOG_L1_HOOK(...)
#endif
#define UNUSED_VARIABLE(X)  ((void)(X))

/* hook functions */

/**
 * @fn     void gh3x2x_init_hook_func(void)
 * 
 * @brief  gh3x2x init hook
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh30xInitHookFunc(void)
{
#if (__ADT_ONLY_PARTICULAR_WM_CONFIG__)
    g_usCurrentConfigListFifoWmBak = 0;
#endif

#if (__GOODIX_ALGO_CALL_MODE__)
    GH30X_AlgoCallConfigInit(g_pstGh30xFrameInfo);
#endif

#if __GH_MSG_WITH_ALGO_LAYER_EN__
    if(0 == GH3x2x_GetChipResetRecoveringFlag())
    {
        GH3X2X_SEND_MSG_ALGO_CFG_INIT(g_pSTGh30xFrameInfo, g_uchGh3x2xRegCfgArrIndex);
    }
#endif
}

void Gh30xGetRawdataHookFunc(GU32 *read_buffer_ptr, GU16 length)
{
    /* code implement by user */
}

/**
 * @fn      void gh3x2x_algorithm_get_io_data_hook_func(const STGh30xFrameInfo * const pstFrameInfo)
 * 
 * @brief  get algorithm input and output data
 *
 * @attention   None        
 *
 * @param[in]   pstFrameInfo
 * @param[out]  None
 *
 * @return  None
 */
void Gh30xAlgorithmGetIoDataHookFunc(const STGh30xFrameInfo * const pstFrameInfo)
{
    /* algo calculate */
#if (__GOODIX_ALGO_CALL_MODE__)
    GH30X_AlgoCalculate(pstFrameInfo->unFunctionID);
#endif

#if __GH_MSG_WITH_ALGO_LAYER_EN__
    GH3X2X_SEND_MSG_ALGO_CAL(pstFrameInfo->unFunctionID);
#endif

    /****************** FOLLOWING CODE IS EXAMPLE **********************************/
#ifdef GOODIX_DEMO_PLANFORM
    EXAMPLE_DEBUG_LOG_L2("[IO_DATA]FunId= 0x%X,cnt = %d,flg = 0x%X, x = %d, y = %d, z= %d, ",
                        pstFrameInfo->unFunctionID,
                        pstFrameInfo->punFrameCnt[0],
                        pstFrameInfo->pstFunctionInfo->uchFrameFlag,
                        pstFrameInfo->pusGsensordata[0],
                        pstFrameInfo->pusGsensordata[1],
                        pstFrameInfo->pusGsensordata[2]);

    for (GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt++)
    {
#if __EXAMPLE_DEBUG_LOG_LVL__
        GU32 unTempRawdata = pstFrameInfo->punRawdata[uchChnlCnt];
        GU8 adj_flag = ((unTempRawdata >> 30) & 0x00000001);
        GU8 gain = ((unTempRawdata >> 24) & 0x00000007);
        GU8 bg = ((unTempRawdata >> 27) & 0x00000007);
        GU32 RawdataCode = ((unTempRawdata >> 7) & 0x1FFFF);

        ((void)(unTempRawdata));
        ((void)(adj_flag));
        ((void)(gain));
        ((void)(bg));
        ((void)(RawdataCode));
#endif
        EXAMPLE_DEBUG_LOG_L2("chnl%d: id= 0x%d, adj= %d, gn= %d, bg= %d, cur= %d, raw= %d",
        uchChnlCnt,
        pstFrameInfo->unFunctionID,
        adj_flag,
        gain,
        bg,
        pstFrameInfo->pchDrvCurrentCode[uchChnlCnt],
        RawdataCode);
    }
    EXAMPLE_DEBUG_LOG_L2("\r\n");
#endif
}

void Gh30xFrameDataHookFunc(const STGh30xFrameInfo * const pstFrameInfo)
{
    Gh30xAlgorithmGetIoDataHookFunc(pstFrameInfo);
}

//=============================================================================================================================
//algo pool global variables
//=============================================================================================================================

#if (__HBD_ALGORITHM_USE_DYN_MEM__ == 1)
GU32 *g_punGh301xAlgoMemBufferPointer = NULL;
#else
#if 0
GU32 g_punGh301xAlgoMemBuffer[__HBD_ALGORITHM_MEM_SIZE__] = {0};
GU32 *g_punGh301xAlgoMemBufferPointer = g_punGh301xAlgoMemBuffer;
#endif
#endif

#if (__HBD_ALGORITHM_EXTERNANL_CONFIG_ENABLE__)
goodix_hba_config gsthbdCfg = {
    .mode = HBA_TEST_DYNAMIC,
    .scence = HBA_SCENES_DEFAULT,
    .senseless_mode_step = 0,
    .senseless_mode_duration = 0,
    .hba_latest_output_time = 0,
    .hba_lowerest_confidence = 0,
};
GS32 guiNadtSleepStatus = 0;
GS32 guiNadtSleepOnlyAdtStatus = 0;
#endif
GU32 guchSleepFlag = 0;//application has used, so set global

//=============================================================================================================================
//ADT confirm global variables
//=============================================================================================================================
#if (__HB_NEED_ADT_CONFIRM__)

#define ADT_CONFIRM_FILTER_BUF_SIZE  (4)
static const GF32 highPassFilterCoefA[] = { 8192,   - 14310,  6511.2, 8192,   -6490.8, 0 };
static const GF32 highPassFilterCoefB[] = { 7254.2, - 14505,  7254.2, 7341.4, -7341.4, 0 };
static const GF32 LowPassFilterCoefA[]  = { 8192,   - 10315,  4505.3, 8192,   -4191.5, 0 };
static const GF32 LowPassFilterCoefB[]  = { 642.65,   1096.7, 642.65, 2000.3,  2000.3, 0 }; 

ST_IIR_PARAM highPassFilterX;
ST_IIR_PARAM highPassFilterY;
ST_IIR_PARAM highPassFilterZ;
ST_IIR_PARAM lowPassFilter;
static GF32 HPXBufferX[ADT_CONFIRM_FILTER_BUF_SIZE], HPYBufferX[ADT_CONFIRM_FILTER_BUF_SIZE];
static GF32 HPXBufferY[ADT_CONFIRM_FILTER_BUF_SIZE], HPYBufferY[ADT_CONFIRM_FILTER_BUF_SIZE];
static GF32 HPXBufferZ[ADT_CONFIRM_FILTER_BUF_SIZE], HPYBufferZ[ADT_CONFIRM_FILTER_BUF_SIZE];
static GF32 LPXBuffer[ADT_CONFIRM_FILTER_BUF_SIZE], LPYBuffer[ADT_CONFIRM_FILTER_BUF_SIZE];


GU8  g_uchInitFlag = 0;
GU8  g_uchAdtGsCalcThrCntMaxIndex = 0;
GU8  g_uchPpgThrCnt = 0;
GU32 g_ulAdtThrCntArr = 0;
GU32 g_ulAdtPpgThrVal = 1320;
GU16 g_usAdtGsThrVal = 38; // 0.3g
GU8  g_uchAdtGsCalcThrCntMax = 25;
GU8  g_uchAdtGsCalcGreaterThanThrCnt = 23; // gsensor 25 max 23
#endif


//=============================================================================================================================
//function
//=============================================================================================================================


#if (__HBD_DOUBLE_CORE_ENABLE__)
GS32 ipc_trans(GS32 *trans_data, GS32 trans_len , GS32 *recv_data, GS32 rec_len)
{
    EXAMPLE_DEBUG_LOG_L1_HOOK("%s \n\r",__FUNCTION__);
    GS32 ret = 0;
    //add your code
    return ret;
}
#endif

#if 0
GS32 gh3011_algo_memory_init_hook(GS32 * error_code)
{
    EXAMPLE_DEBUG_LOG_L1_HOOK("%s \n\r",__FUNCTION__);
    GS32 ret = 0;
    GS32 size = __HBD_ALGORITHM_MEM_SIZE__ * 4;
    #if (__HBD_DOUBLE_CORE_ENABLE__)
    //ipc_trans(trans_data, trans_len , recv_data, rec_len);
    #else
    ret = goodix_mem_init(g_punGh301xAlgoMemBufferPointer, size);
    #endif
    *error_code = ret;
    return  ret;
}
#endif

void gh3011_algo_memory_deinit_hook(void)
{
    EXAMPLE_DEBUG_LOG_L1_HOOK("%s \n\r",__FUNCTION__);
    #if (__HBD_DOUBLE_CORE_ENABLE__)
    //ipc_trans(trans_data, trans_len , recv_data, rec_len);
    #else
    goodix_mem_deinit();
    #endif
}

void gh3011_algo_init_hook(EMFunctionID function_id, GU8 buffer_len, GS32 *algo_param_buffer)
{
    EXAMPLE_DEBUG_LOG_L1_HOOK("%s : EMFunctionID = %d\n\r",__FUNCTION__ , function_id);
    GS32 ret = 0;
#if(__HBD_HB_ALGORITHM_ENABLE__)
    goodix_hba_config stHbaConfig;
    memset(&stHbaConfig, 0, sizeof(goodix_hba_config));
#endif
#if(__HBD_HRV_ALGORITHM_ENABLE__)
    goodix_hrv_config stHrvConfig = {
                                        1,
                                        200,
                                        {200000, 100000, 30000, 30000}
                                    };
#endif

#if(__HBD_SPO2_ALGORITHM_ENABLE__)
    goodix_spo2_config stSpo2Config = {
                                        1,100,
                                        -21742,-201492,1077714,//0xffffab12,0xfffcecec,0x1071d2
                                        1,0,
                                        30, 15, 4, 3,//6. Acc相关
                                        0,0,//ctr相关
                                        #if __HBD_ALGORITHM_VERTION__ == __HBD_ALGORITHM_VERTION_BASIC__
                                        30,20,
                                        10,4,6,4,5,//出值策略
                                        #elif __HBD_ALGORITHM_VERTION__ == __HBD_ALGORITHM_VERTION_TINY__
                                        30,20,
                                        10,4,6,4,5,//出值策略
                                        #endif
        };
#endif
#if(__HBD_NADT_ALGORITHM_ENABLE__)
    //NADT ALGO DEFAULT CONFIG
    GS32 liNadtCfg[33] = {
                              0,0,5,4,60,25,0,1,1,40,
                              140,10,180,1,10,1,10,100,1,10,
                              1,4,3,3,1,30,90,3,10,7,
                              0,400,5
                         };
#endif

#if (__HB_NEED_ADT_CONFIRM__)
    GU32 ulNewAdtPpgThrVal = 0;
#endif

    switch(function_id)
    {
#if(__HBD_HB_ALGORITHM_ENABLE__)
    case GH3011_FUNC_OFFSET_HR:
        if (buffer_len == 3)
        {
            stHbaConfig.fs = algo_param_buffer[0];
            stHbaConfig.scence = (hba_scenes_e) algo_param_buffer[1];//user can config scense here
            stHbaConfig.mode = (hba_test_mode)algo_param_buffer[2];//user can config test mode here
            stHbaConfig.valid_channel_num = 1;
            #if (__HBD_ALGORITHM_EXTERNANL_CONFIG_ENABLE__)
            stHbaConfig.scence = gsthbdCfg.scence;
            stHbaConfig.mode = gsthbdCfg.mode;
            stHbaConfig.senseless_mode_step = gsthbdCfg.senseless_mode_step;
            stHbaConfig.senseless_mode_duration = gsthbdCfg.senseless_mode_duration;
            stHbaConfig.hba_latest_output_time = gsthbdCfg.hba_latest_output_time;
            stHbaConfig.hba_earliest_output_time = gsthbdCfg.hba_earliest_output_time;
            #endif
            EXAMPLE_DEBUG_LOG_L1_HOOK("GH3011_FUNC_HR stHbaConfig = %d %d %d %d\n\r",stHbaConfig.fs,stHbaConfig.scence,stHbaConfig.hba_latest_output_time,stHbaConfig.valid_channel_num);
            #if (__HBD_DOUBLE_CORE_ENABLE__)
            //ipc_trans(trans_data, trans_len , recv_data, rec_len);
            UNUSED_VARIABLE(stHbaConfig);
            #else
            ret = (GS32)goodix_hba_init(&stHbaConfig);
            #endif
        }
        break;
#endif
#if(__HBD_HRV_ALGORITHM_ENABLE__)
    case GH3011_FUNC_OFFSET_HRV:
        if (buffer_len == 1)
        {
            stHrvConfig.fs = algo_param_buffer[0];
            EXAMPLE_DEBUG_LOG_L1_HOOK("GH3011_FUNC_OFFSET_HRV stHrvConfig.fs = %d %d %d %d %d %d\n\r",stHrvConfig.fs,stHrvConfig.need_ipl,stHrvConfig.acc_thr[0],stHrvConfig.acc_thr[1],stHrvConfig.acc_thr[2],stHrvConfig.acc_thr[3]);
            #if (__HBD_DOUBLE_CORE_ENABLE__)
            //ipc_trans(trans_data, trans_len , recv_data, rec_len);
            UNUSED_VARIABLE(stHrvConfig);
            #else
            ret = (GS32)goodix_hrv_init(&stHrvConfig);
            #endif
        }
        break;
#endif
#if(__HBD_SPO2_ALGORITHM_ENABLE__)
    case GH3011_FUNC_OFFSET_SPO2:
        if (buffer_len == 6)
        {
            stSpo2Config.raw_fs = algo_param_buffer[0];
            #if (__HBD_ALGORITHM_VERTION_BASIC__ == __HBD_ALGORITHM_VERTION__)
            stSpo2Config.cali_coef_a2 = algo_param_buffer[1];
            stSpo2Config.cali_coef_a1 = algo_param_buffer[2];
            stSpo2Config.cali_coef_a0 = algo_param_buffer[3];
            #else
            stSpo2Config.cali_coef_a = algo_param_buffer[1];
            stSpo2Config.cali_coef_b = algo_param_buffer[2];
            stSpo2Config.cali_coef_c = algo_param_buffer[3];
            #endif

             #if (__HBD_DOUBLE_CORE_ENABLE__)
            //ipc_trans(trans_data, trans_len , recv_data, rec_len);
            UNUSED_VARIABLE(stSpo2Config);
            #else
            ret = (GS32)goodix_spo2_init(&stSpo2Config);
            #endif
        }
        break;
#endif

#if(__HBD_NADT_ALGORITHM_ENABLE__)
    case GH3011_FUNC_OFFSET_NADT:
        if(buffer_len == 4)
        {
            #if (__HBD_DOUBLE_CORE_ENABLE__)
            //ipc_trans(trans_data, trans_len , recv_data, rec_len);
            UNUSED_VARIABLE(liNadtCfg);
            #else
            EXAMPLE_DEBUG_LOG_L1_HOOK("nadt para %d, %d, %d, %d\r\n",algo_param_buffer[0],algo_param_buffer[1],algo_param_buffer[2],algo_param_buffer[3]);
            liNadtCfg[0] = algo_param_buffer[0];
            liNadtCfg[1] = algo_param_buffer[1];
            liNadtCfg[5] = algo_param_buffer[2];
            liNadtCfg[31] = algo_param_buffer[3];
            NADT_Control(NADT_CONFIG_SOFT_AUTOLED_TYPE, &liNadtCfg[0]);
            NADT_Control(NADT_CONFIG_TURNLIGHT_TYPE, &liNadtCfg[2]);
            NADT_Control(NADT_CONFIG_UNWEAR_TIME_TYPE, &liNadtCfg[3]);
            NADT_Control(NADT_CONFIG_DETECT_TIMEOUT_TYPE, &liNadtCfg[4]);
            NADT_Control(NADT_CONFIG_SAMPLE_RATE_TYPE, &liNadtCfg[5]);
            NADT_Control(NADT_CONFIG_SLEEP_STATUS_TYPE, &liNadtCfg[6]);
            NADT_Control(NADT_COFIG_UNWEAR_LEVEL_TYPE, &liNadtCfg[7]);
            NADT_Control(NADT_CONFIG_LIVE_DETECT_EN_TYPE, &liNadtCfg[8]);
            NADT_Control(NADT_CONFIG_HB_LOW_THR_TYPE, &liNadtCfg[9]);
            NADT_Control(NADT_CONFIG_HB_HIGH_THR_TYPE, &liNadtCfg[10]);
            NADT_Control(NADT_CONFIG_GREEN_SIG_LOW_THR_TYPE, &liNadtCfg[11]);
            NADT_Control(NADT_CONFIG_GREEN_SIG_HIGH_THR_TYPE, &liNadtCfg[12]);
            NADT_Control(NADT_CONFIG_BASE_RATIO_EN_TYPE, &liNadtCfg[13]);
            NADT_Control(NADT_CONFIG_BASE_RATIO_THR_TYPE, &liNadtCfg[14]);
            NADT_Control(NADT_CONFIG_LIVE_CONFIRM_EN_TYPE, &liNadtCfg[15]);
            NADT_Control(NADT_CONFIG_IR_SIG_LOW_THR_TYPE, &liNadtCfg[16]);
            NADT_Control(NADT_CONFIG_IR_SIG_HIGH_THR_TYPE, &liNadtCfg[17]);
            NADT_Control(NADT_CONFIG_CTR_EN_TYPE, &liNadtCfg[18]);
            NADT_Control(NADT_CONFIG_CTR_THR_TYPE, &liNadtCfg[19]);
            NADT_Control(NADT_CONFIG_GAIN_EN_TYPE, &liNadtCfg[20]);
            NADT_Control(NADT_CONFIG_GAIN_STABLE_TIME_TYPE, &liNadtCfg[21]);
            NADT_Control(NADT_CONFIG_GREEN_GAIN_THR_TYPE, &liNadtCfg[22]);
            NADT_Control(NADT_CONFIG_IR_GAIN_THR_TYPE, &liNadtCfg[23]);
            NADT_Control(NADT_CONFIG_PERIOD_EN_TYPE, &liNadtCfg[24]);
            NADT_Control(NADT_CONFIG_PERIOD_LOW_THR_TYPE, &liNadtCfg[25]);
            NADT_Control(NADT_CONFIG_PERIOD_HIGH_THR_TYPE, &liNadtCfg[26]);
            NADT_Control(NADT_CONFIG_PERIOD_DIFF_THR_TYPE, &liNadtCfg[27]);
            NADT_Control(NADT_CONFIG_TOTAL_CNT_TYPE, &liNadtCfg[28]);
            NADT_Control(NADT_CONFIG_UNWEAR_CNT_TYPE, &liNadtCfg[29]);
            NADT_Control(NADT_CONFIG_ADT_ONLY_SLEEP_TYPE, &liNadtCfg[30]);
            NADT_Control(NADT_CONFIG_ADT_WEAR_OFF_THR_TYPE, &liNadtCfg[31]);
            NADT_Control(NADT_CONFIG_ADT_WEAR_OFF_CNT_TYPE, &liNadtCfg[32]);
            #if (__HBD_ALGORITHM_EXTERNANL_CONFIG_ENABLE__)
            NADT_Control(NADT_CONFIG_SLEEP_STATUS_TYPE, &guiNadtSleepStatus);
            NADT_Control(NADT_CONFIG_ADT_ONLY_SLEEP_TYPE, &guiNadtSleepOnlyAdtStatus);
            #endif
            NADT_ProcInit();
            #endif
        }
        break;
#endif
#if (__HB_NEED_ADT_CONFIRM__)
    case GH3011_FUNC_OFFSET_ADT_CONFIRM:
        ulNewAdtPpgThrVal = (GU32)algo_param_buffer[0];
        ADTConfirmAlgoInit(ulNewAdtPpgThrVal);
        break;
#endif
    default:
        break;
    }
    if (ret != 0)
    {
        //error log
        EXAMPLE_DEBUG_LOG_L1_HOOK("%s : function[%d] algo init error!!!\n\r",__FUNCTION__,function_id);
    }
}

void gh3011_algo_calculate_hook(EMFunctionID function_id,
                                ST_ALGO_CALCULATE_INFO *algo_calc_info,
                                ST_ALGO_RESULT *algo_result)
{
    EXAMPLE_DEBUG_LOG_L1_HOOK("%s : EMFunctionID = %d\n\r",__FUNCTION__ , function_id);
    GS32 ret = 0;
#if(__HBD_HB_ALGORITHM_ENABLE__)
    goodix_hba_input_rawdata stHbaInputRawdata = {0};
    goodix_hba_result stHbaResult = {0};
#endif
#if(__HBD_HRV_ALGORITHM_ENABLE__)
    goodix_hrv_input_rawdata stHrvInputRawdata = {0};
    goodix_hrv_result stHrvResult = {0};
#endif
#if(__HBD_SPO2_ALGORITHM_ENABLE__)
    goodix_spo2_input_rawdata stSpo2InputRawdata = {0};
    goodix_spo2_result stSpo2Result = {0};
#endif
#if(__HBD_NADT_ALGORITHM_ENABLE__)
    GS32 lNadtPacketInfoArr[6] = {0};
    GU8 lNadtResult[2] = {0};
#endif
#if(__HB_NEED_ADT_CONFIRM__)
    GS32 lAdtConfirmPacketInfoArr[6] = {0};
    GU8 lAdtConfirmResult = {0};
#endif

    switch(function_id)
    {
#if(__HBD_HB_ALGORITHM_ENABLE__)
    case GH3011_FUNC_OFFSET_HR:
        stHbaInputRawdata.frameid = algo_calc_info->uchFrameID;
        stHbaInputRawdata.ppg_rawdata[0] = algo_calc_info->nRawdata[0];
        stHbaInputRawdata.enable_flg[0] = 1;
        stHbaInputRawdata.acc_x = algo_calc_info->sAccData[0];
        stHbaInputRawdata.acc_y = algo_calc_info->sAccData[1];
        stHbaInputRawdata.acc_z = algo_calc_info->sAccData[2];
        //user can config sleep flag here
        stHbaInputRawdata.sleep_flg = guchSleepFlag;
        #if (__HBD_DOUBLE_CORE_ENABLE__)
        //ipc_trans(trans_data, trans_len , recv_data, rec_len);
        UNUSED_VARIABLE(stHbaInputRawdata);
        UNUSED_VARIABLE(stHbaResult);
        #else
        ret = (GS32)goodix_hba_update(&stHbaInputRawdata, &stHbaResult);
        #endif
        if (stHbaResult.hba_out_flag == 1 && ret == GX_ALGO_HBA_SUCCESS)
        {
            algo_result->uchUpdateFlag = 1;
            algo_result->uchResultNum = 6;
            algo_result->snResult[0] = (GS32)stHbaResult.hba_out;
            algo_result->snResult[1] = (GS32)stHbaResult.valid_score;
            algo_result->snResult[2] = (GS32)stHbaResult.hba_acc_scence;
            algo_result->snResult[3] = (GS32)(stHbaResult.hba_snr * 100);
            algo_result->snResult[4] = (GS32)stHbaResult.valid_score;
            algo_result->snResult[5] = (GS32)stHbaResult.hba_acc_info;
        }
        break;
#endif
#if(__HBD_HRV_ALGORITHM_ENABLE__)
    case GH3011_FUNC_OFFSET_HRV:
        stHrvInputRawdata.bit_num = 24;
        stHrvInputRawdata.frame_id = algo_calc_info->uchFrameID;
        stHrvInputRawdata.ppg_rawdata[0] = algo_calc_info->nRawdata[0];
        stHrvInputRawdata.acc_x = algo_calc_info->sAccData[0];
        stHrvInputRawdata.acc_y = algo_calc_info->sAccData[1];
        stHrvInputRawdata.acc_z = algo_calc_info->sAccData[2];
        algo_result->uchUpdateFlag = 0;
        #if (__HBD_DOUBLE_CORE_ENABLE__)
        //ipc_trans(trans_data, trans_len , recv_data, rec_len);
        UNUSED_VARIABLE(stHrvInputRawdata);
        UNUSED_VARIABLE(stHrvResult);
        #else
        ret = (GS32)goodix_hrv_calc(&stHrvInputRawdata, &stHrvResult);
        #endif
        EXAMPLE_DEBUG_LOG_L1_HOOK("goodix_hrv_calc ret = %d\n\r",ret);
        if (ret == GX_HRV_ALGO_UPDATE)
        {
            algo_result->uchUpdateFlag = 1;
            algo_result->uchResultNum = 6;
            algo_result->snResult[0] = stHrvResult.rri[0];
            algo_result->snResult[1] = stHrvResult.rri[1];
            algo_result->snResult[2] = stHrvResult.rri[2];
            algo_result->snResult[3] = stHrvResult.rri[3];
            algo_result->snResult[4] = stHrvResult.rri_confidence;
            algo_result->snResult[5] = stHrvResult.rri_valid_num;
            EXAMPLE_DEBUG_LOG_L1_HOOK("stHrvResult = %d %d %d %d %d %d\n\r", stHrvResult.rri[0],stHrvResult.rri[1],stHrvResult.rri[2],stHrvResult.rri[3],stHrvResult.rri_confidence,stHrvResult.rri_valid_num);
        }
        break;
#endif
#if(__HBD_SPO2_ALGORITHM_ENABLE__)
    case GH3011_FUNC_OFFSET_SPO2:
        stSpo2InputRawdata.frameid = algo_calc_info->uchFrameID;
        stSpo2InputRawdata.bit_num = 24;
        stSpo2InputRawdata.ppg_rawdata[4] = algo_calc_info->nRawdata[0];
        stSpo2InputRawdata.ppg_rawdata[8] = algo_calc_info->nRawdata[1];
        stSpo2InputRawdata.enable_flg[4] = 1;
        stSpo2InputRawdata.enable_flg[8] = 1;
        stSpo2InputRawdata.cur_adj_flg[4] = algo_calc_info->nCurAdjFlag[0];
        stSpo2InputRawdata.cur_adj_flg[8] = algo_calc_info->nCurAdjFlag[1];
        stSpo2InputRawdata.acc_x = algo_calc_info->sAccData[0];
        stSpo2InputRawdata.acc_y = algo_calc_info->sAccData[1];
        stSpo2InputRawdata.acc_z = algo_calc_info->sAccData[2];
        algo_result->uchUpdateFlag = 0;
        #if (__HBD_DOUBLE_CORE_ENABLE__)
        //ipc_trans(trans_data, trans_len , recv_data, rec_len);
        UNUSED_VARIABLE(stSpo2InputRawdata);
        UNUSED_VARIABLE(stSpo2Result);
        #else
        ret = (GS32)goodix_spo2_calc(&stSpo2InputRawdata, &stSpo2Result);
        #endif
        if (stSpo2Result.final_calc_flg == 1 && ret == GX_ALGO_SPO2_SUCCESS)
        {
            algo_result->uchUpdateFlag = 1;
            algo_result->uchResultNum = 12;
            algo_result->snResult[0] = (GS32)(((float)stSpo2Result.final_spo2/10000 + 0.5f));
            algo_result->snResult[1] = (GS32)stSpo2Result.final_valid_level;
            algo_result->snResult[2] = (GS32)stSpo2Result.final_confi_coeff;
            algo_result->snResult[3] = (GS32)stSpo2Result.final_WeightR;
            algo_result->snResult[4] = (GS32)stSpo2Result.final_hb_mean;
            algo_result->snResult[5] = (GS32)stSpo2Result.final_hb_confi_lvl;
            algo_result->snResult[6] = (GS32)stSpo2Result.final_invalidFlg;
            algo_result->snResult[7] = (GS32)stSpo2Result.final_piIR;
            algo_result->snResult[8] = (GS32)stSpo2Result.final_piRed;
            algo_result->snResult[9] = (GS32)stSpo2Result.final_snrIR;
            algo_result->snResult[10] = (GS32)stSpo2Result.final_snrRed;
            algo_result->snResult[11] = (GS32)stSpo2Result.final_r_val;
            EXAMPLE_DEBUG_LOG_L1_HOOK("stSpo2Result = %d %d\n\r", algo_result->snResult[0],algo_result->snResult[4]);
        }
        break;
#endif
#if(__HBD_NADT_ALGORITHM_ENABLE__)
    case GH3011_FUNC_OFFSET_NADT:
        lNadtPacketInfoArr[0] = algo_calc_info->nRawdata[0];
        lNadtPacketInfoArr[1] = algo_calc_info->nRawdata[1];
        lNadtPacketInfoArr[2] = algo_calc_info->sAccData[0];
        lNadtPacketInfoArr[3] = algo_calc_info->sAccData[1];
        lNadtPacketInfoArr[4] = algo_calc_info->sAccData[2];
        lNadtPacketInfoArr[5] = algo_calc_info->led_current[1];
        #if (__HBD_DOUBLE_CORE_ENABLE__)
        UNUSED_VARIABLE(lNadtPacketInfoArr);
        //ipc_trans(trans_data, trans_len , recv_data, rec_len);
        #else
        NADT_Proc(lNadtPacketInfoArr, lNadtResult);
        EXAMPLE_DEBUG_LOG_L1_HOOK("NADT_Proc lNadtPacketInfoArr = %d , %d, %d, %d, %d, %d\n\r lNadtResult = %d, %d\n\r",\
                            lNadtPacketInfoArr[0],lNadtPacketInfoArr[1],lNadtPacketInfoArr[2],lNadtPacketInfoArr[3],lNadtPacketInfoArr[4],lNadtPacketInfoArr[5],\
                            lNadtResult[0], lNadtResult[1]);
        #endif
        algo_result->snResult[12] = (GS32)lNadtResult[0];
        algo_result->snResult[13] = (GS32)lNadtResult[1];
        break;
#endif
#if (__HB_NEED_ADT_CONFIRM__)
    case GH3011_FUNC_OFFSET_ADT_CONFIRM:
        lAdtConfirmPacketInfoArr[0] = algo_calc_info->nRawdata[0];
        lAdtConfirmPacketInfoArr[1] = algo_calc_info->nRawdata[1];
        lAdtConfirmPacketInfoArr[2] = algo_calc_info->sAccData[0];
        lAdtConfirmPacketInfoArr[3] = algo_calc_info->sAccData[1];
        lAdtConfirmPacketInfoArr[4] = algo_calc_info->sAccData[2];
        lAdtConfirmResult = ADTConfirmAlgoCalc(lAdtConfirmPacketInfoArr);
        algo_result->snResult[0] = (GS32)lAdtConfirmResult;
        algo_result->uchUpdateFlag = 1;
        break;
#endif

    default:
        break;
    }
    if (ret != 0)
    {
        //error log
        EXAMPLE_DEBUG_LOG_L1_HOOK("%s : function[%d] algo calc error!!!\n\r",__FUNCTION__,function_id);
    }
}

void gh3011_algo_deinit_hook(EMFunctionID function_id)
{
    EXAMPLE_DEBUG_LOG_L1_HOOK("%s : EMFunctionID = %d\n\r",__FUNCTION__ , function_id);
    switch(function_id)
    {
#if(__HBD_HB_ALGORITHM_ENABLE__)
    case GH3011_FUNC_OFFSET_HR:
        #if (__HBD_DOUBLE_CORE_ENABLE__)
        //ipc_trans(trans_data, trans_len , recv_data, rec_len);
        #else
        goodix_hba_deinit();
        #endif
        break;
#endif
#if(__HBD_HRV_ALGORITHM_ENABLE__)
    case GH3011_FUNC_OFFSET_HRV:
        #if (__HBD_DOUBLE_CORE_ENABLE__)
        //ipc_trans(trans_data, trans_len , recv_data, rec_len);
        #else
        goodix_hrv_deinit();
        #endif
        break;
#endif
#if(__HBD_SPO2_ALGORITHM_ENABLE__)
    case GH3011_FUNC_OFFSET_SPO2:
        #if (__HBD_DOUBLE_CORE_ENABLE__)
        //ipc_trans(trans_data, trans_len , recv_data, rec_len);
        #else
        goodix_spo2_deinit();
        #endif
        break;
#endif
#if(__HBD_NADT_ALGORITHM_ENABLE__)
    case GH3011_FUNC_OFFSET_NADT:
        #if (__HBD_DOUBLE_CORE_ENABLE__)
        //ipc_trans(trans_data, trans_len , recv_data, rec_len);
        #else
        NADT_ProcDeInit();
        #endif
        break;
#endif
    default:
        break;
    }
}

//algm version buffer
#if 0
static GU8 guchVersionBuffer[100] = {0};
#endif

#if(__HBD_NADT_ALGORITHM_ENABLE__)
/****************************************************************
* Description: Get Nadt version
* Input:    None,
* Return:  Nadt version
******************************************************************/
GS8 * HBD_GetNadtVersion(void)
{
    return (GS8 *)NADT_GetVersion();
}

#if (__HBD_ALGORITHM_EXTERNANL_CONFIG_ENABLE__)
/****************************************************************
* Description: Set NADT 
* Input:  nAdtOnlyEnable 0:ADT and NADT in sleep mode
                          1:ADT only in sleep mode   
******************************************************************/
void HBD_AdtOnlySleepTypeEnable(GS32 nAdtOnlyEnable)
{
    guiNadtSleepOnlyAdtStatus = nAdtOnlyEnable;
    return;
}

/****************************************************************
* Description: Set NADT 
* Input:  ST_NADT_CONFIG stConfig  config struct for NADT
******************************************************************/
void NADT_ParamConfigure(ST_NADT_CONFIG stConfig)
{ 
    guiNadtSleepStatus = stConfig.nSleepInState;
    return;
}
#endif

#endif

#if(__HBD_HB_ALGORITHM_ENABLE__)
/****************************************************************
* Description: Get hba algorithm version
* Input:    None,
* Return: library hba algorithm version
******************************************************************/
#if 0
GS8 * HBD_GetHbaVersion (void)
{
    memset(&guchVersionBuffer, 0, 100);
    goodix_hba_version(guchVersionBuffer);
    return (GS8 *)guchVersionBuffer;
}
#endif

#if (__HBD_ALGORITHM_EXTERNANL_CONFIG_ENABLE__)

/****************************************************************
* Description: Hb algorithm scenario config 
* Input:    uchScenario: 0~24,see hba_scenes_e
                         others: fixed 0(default) and return HBD_RET_PARAMETER_ERROR
* Return: HBD_RET_OK=success, 
          HBD_RET_PARAMETER_ERROR=paramters error,
******************************************************************/
GS8 HBD_HbAlgoScenarioConfig (hba_scenes_e uchScenario)
{
    GS8 chRet = HBD_RET_OK;
    if (uchScenario > HBA_SCENES_SIZE)
    {
        gsthbdCfg.scence = HBA_SCENES_DEFAULT; 
        chRet = HBD_RET_PARAMETER_ERROR;
    }
    else
    {
        gsthbdCfg.scence = uchScenario;
    }
    return chRet;
}

/****************************************************************
* Description: Hb algorithm test mode config ,before HBD_HbDetectStart
* Input:    emHbaTestMode: test mode, see hba_test_mode
            usSenselessModeStep
            usSenselessModeDuration
******************************************************************/
void HBD_HbaTestModeConfig (hba_test_mode emHbaTestMode, GU16 usSenselessModeStep, GU16 usSenselessModeDuration)
{
    gsthbdCfg.mode = emHbaTestMode;
    gsthbdCfg.senseless_mode_step = usSenselessModeStep;
    gsthbdCfg.senseless_mode_duration = usSenselessModeDuration;
}

/****************************************************************
* Description: get current Hb algorithm test mode
* Input:    emCurHbaTestMode: test mode, see EM_HBA_TEST_MODE
******************************************************************/
GU8 HBD_GetHbaTestMode (void)
{
    hba_test_mode emCurHbaTestMode;
    emCurHbaTestMode = gsthbdCfg.mode;
    return (GU8) emCurHbaTestMode;
}

/****************************************************************
* Description: Hb algorithm output time config, only nonFast mode valid 
* Input:    nHbaLatestOutputTime
            nHbaEarliestOutputTime
* Return: HBD_RET_OK=success, 
******************************************************************/
GS8 HBD_HbAlgoOutputTimeConfig (GS32 nHbaLatestOutputTime, GS32 nHbaEarliestOutputTime)
{
    gsthbdCfg.hba_latest_output_time = nHbaLatestOutputTime;
    gsthbdCfg.hba_earliest_output_time = nHbaEarliestOutputTime;
    return HBD_RET_OK;
}
#endif
#endif

/****************************************************************
* Description: Hb algorithm sleep flag config,before hb calculate     
* Input:    uchSleepFlg: 0:not sleep; 1:sleep
******************************************************************/
void HBD_HbaSleepFlagConfig (GU8 uchSleepFlg)
{
    guchSleepFlag = uchSleepFlg;
}

/****************************************************************
* Description: Hb algorithm get sleep flag
* Return:    guchSleepFlag: 0:not sleep; 1:sleep
******************************************************************/
GU8 HBD_HbaGetSleepFlag(void)
{
    return guchSleepFlag;
}

#if(__HBD_HRV_ALGORITHM_ENABLE__)
/****************************************************************
* Description: Get hrv algorithm version
* Input:    None,
* Return: library hrv algorithm version
******************************************************************/
GS8 * HBD_GetHrvVersion(void)
{
    return (GS8 *)goodix_hrv_version();
}
#endif

#if(__HBD_SPO2_ALGORITHM_ENABLE__)
/****************************************************************
* Description: Get SPO2 algorithm version
* Input:    None,
* Return: library SPO2 algorithm version
******************************************************************/
#if 0
GU8 * HBD_GetSpo2Version (void)
{
    memset(&guchVersionBuffer, 0, 100);
    goodix_spo2_version(guchVersionBuffer);
    return (GU8 *)guchVersionBuffer;
}
#endif
#endif


#if (__HB_NEED_ADT_CONFIRM__)

/////////////////////////////////////
//  
//        adt confirm algorithm
//
/////////////////////////////////////
/// init
void IIRFilter2Init(ST_IIR_PARAM *pstIIRInfo, const GF32 *pfCoefA, const GF32 *pfCoefB, GF32 *pfXbuff, GF32 *pfYbuff, GS32 lLen, GF32 fThr)
{
    pstIIRInfo->pfCoefA = pfCoefA;
    pstIIRInfo->pfCoefB = pfCoefB;
    pstIIRInfo->pfXbuff = pfXbuff;
    pstIIRInfo->pfYbuff = pfYbuff;
    pstIIRInfo->lLen = lLen; //unused;
    pstIIRInfo->fThr = fThr;
    memset(pfXbuff, 0, (lLen + 1) * sizeof(GF32));
    memset(pfYbuff, 0, (lLen + 1) * sizeof(GF32));
}

static GF32 HPFilter(const GF32 b[], const  GF32 a[], GF32 x[], GF32 y[], GF32 dDataIn)
{
    GF32 dSum2 = 0;
    GF32 dSum1 = b[0] * dDataIn;

    dSum1 += b[1] * x[0] + b[2] * x[1];
    dSum2 += a[1] * y[0] + a[2] * y[1];
    if (1 == a[0])
    {
        return dSum1 - dSum2;
    }
    else
    {
        return (GF32)(NADT_MATH_ROUND((dSum1 - dSum2) / a[0]));
    }
}

GF32 IIRFilterEx(ST_IIR_PARAM *pstIIRInfo, GF32 fDataIn)
{
    GF32 *pfXbuff = pstIIRInfo->pfXbuff;
    GF32 *pfYBuff = pstIIRInfo->pfYbuff;
    const GF32 *pfCoefA = pstIIRInfo->pfCoefA;
    const GF32 *pfCoefB = pstIIRInfo->pfCoefB;
    GF32 fDiff = fDataIn - pfXbuff[0];

    if ((fDiff == 0 && (fDataIn > 16000 || fDataIn == 0)) || NADT_MATH_ABSF(fDiff) >= pstIIRInfo->fThr)
    {
        pfXbuff[0] += fDiff;
        pfXbuff[1] += fDiff;
    }

    GF32 fDataOut = 0;
    GF32 dDataIn1 = fDataIn;
    for (int i = 0; i < 2; i++)
    {
        fDataOut = HPFilter(pfCoefB + i * 3, pfCoefA + i * 3, pfXbuff + i * 2, pfYBuff + i * 2, dDataIn1);

        pfXbuff[i * 2 + 1] = pfXbuff[i * 2];
        pfXbuff[i * 2] = dDataIn1;
        pfYBuff[i * 2 + 1] = pfYBuff[i * 2];
        pfYBuff[i * 2] = fDataOut;
        dDataIn1 = fDataOut;
    }

    return (GF32)(NADT_MATH_ROUND(fDataOut));
}

void ADTConfirmAlgoInit(GU32 ulNewAdtPpgThrVal)
{

    IIRFilter2Init(&highPassFilterX, highPassFilterCoefA, highPassFilterCoefB, HPXBufferX, HPYBufferX, 3, 4001);
    IIRFilter2Init(&highPassFilterY, highPassFilterCoefA, highPassFilterCoefB, HPXBufferY, HPYBufferY, 3, 4001);
    IIRFilter2Init(&highPassFilterZ, highPassFilterCoefA, highPassFilterCoefB, HPXBufferZ, HPYBufferZ, 3, 4001);
    IIRFilter2Init(&lowPassFilter,  LowPassFilterCoefA,  LowPassFilterCoefB,  LPXBuffer, LPYBuffer, 3, 4001);
    g_ulAdtPpgThrVal = ulNewAdtPpgThrVal;
    g_uchInitFlag = 1;
    g_uchAdtGsCalcThrCntMaxIndex = 0;
    g_uchPpgThrCnt = 0;
    g_ulAdtThrCntArr = 0;
}

/// calc
GU8 ADTConfirmAlgoCalc(GS32 *plRawdataBuff)
{
    GU8 uchIndex = 0;
    GU8 uchRet = 0x00;
    GF32 fAccx = 0;
    GF32 fAccy =0;
    GF32 fAccz = 0;
    GF32 fAccAmpVal = 0;
    GF32 fAccAmpLpfVal = 0;
    GU8 uchIndexMax = 1;
    GU8 uchWearThrCnt = 0;
    GU32 ulAdtNowFlag = 0;
    GU32 ulPpgRawdataVal = (GU32)((plRawdataBuff[0] >> 7) & 0x0001FFFF); // just use 17bits

    if (g_uchAdtGsCalcThrCntMaxIndex < g_uchAdtGsCalcThrCntMax)
    {
        if (g_uchInitFlag == 1)
        {
            uchIndexMax = 26;
            g_uchInitFlag = 0;
        }
        for (uchIndex = 0; uchIndex < uchIndexMax; uchIndex++)
        {
            fAccx = IIRFilterEx(&highPassFilterX, plRawdataBuff[2]); // gx data input hp filter
            fAccy = IIRFilterEx(&highPassFilterY, plRawdataBuff[3]); // gy data input hp filter
            fAccz = IIRFilterEx(&highPassFilterZ, plRawdataBuff[4]); // gz data input hp filter
            fAccAmpVal = sqrt(fAccx*fAccx + fAccy*fAccy + fAccz*fAccz);
            fAccAmpLpfVal = IIRFilterEx(&lowPassFilter, fAccAmpVal); // gs input lp filter
        }
        g_uchAdtGsCalcThrCntMaxIndex++;
        if (ulPpgRawdataVal < g_ulAdtPpgThrVal)
        {
            g_uchPpgThrCnt ++;
        }
        if (fAccAmpLpfVal > (GF32)g_usAdtGsThrVal)
        {
            ulAdtNowFlag = 1;
        }
        g_ulAdtThrCntArr = (g_ulAdtThrCntArr << 1) | ulAdtNowFlag;
        //EXAMPLE_DEBUG_LOG_L1("fAccAmpLpfVal = %f  g_usAdtGsThrVal = %d\n\r ulPpgRawdataVal = %d g_ulAdtPpgThrVal = %d\n\r",fAccAmpLpfVal, g_usAdtGsThrVal, ulPpgRawdataVal, g_ulAdtPpgThrVal);
        if (g_uchAdtGsCalcThrCntMaxIndex >= g_uchAdtGsCalcThrCntMax)
        {
            for (uchIndex = 0; uchIndex < g_uchAdtGsCalcThrCntMax; uchIndex++)
            {
                if ((g_ulAdtThrCntArr & (1 << uchIndex)) != 0)
                {
                    uchWearThrCnt++;
                }
            }
            if ((uchWearThrCnt >= g_uchAdtGsCalcGreaterThanThrCnt) && (g_uchPpgThrCnt < 5))
            {
                uchRet = 0x11;
            }
            else
            {
                uchRet = 0x12;
            }
        }
    }
    return uchRet;
}
/****************************************************************
* Description: config adt confrim
* Input:  usAdtConfirmGsThrVal : gsensor confirm thr
          uchAdtConfirmGsCalcThrCntMax: gsensor confirm thr cnt max 
          uchAdtConfirmGsCalcThrCnt  : gsensor confirm thr cnt
* Return: None
******************************************************************/
void HBD_AdtConfirmConfig(GU16 usAdtConfirmGsThrVal, GU8 uchAdtConfirmGsCalcThrCntMax, GU8 uchAdtConfirmGsCalcThrCnt)
{
    g_usAdtGsThrVal = usAdtConfirmGsThrVal;
    g_uchAdtGsCalcThrCntMax = uchAdtConfirmGsCalcThrCntMax;
    g_uchAdtGsCalcGreaterThanThrCnt = uchAdtConfirmGsCalcThrCnt;
}

#endif

/* need  write this function in algm call */

