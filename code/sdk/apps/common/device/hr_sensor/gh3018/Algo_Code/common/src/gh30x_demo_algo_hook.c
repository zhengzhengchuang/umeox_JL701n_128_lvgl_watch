/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_algo_hook.c
 *
 * @brief   example code for gh30x algo
 *
 */

#include <stdio.h>
#include "../../common/inc/gh30x_demo_common.h"
#include "../../common/inc/gh30x_demo_algo_call.h"
#include "../../common/inc/gh30x_demo_algo_config.h"
#include "../../common/inc/gh30x_demo_algo_hook.h"
#include "../../../ui/lv_watch/comm_func/hr_func.h"
#include "../../../ui/lv_watch/comm_func/bo_func.h"
#include "../../../ui/lv_watch/comm_sensor/sensor_hr_task.h"

#if (__GOODIX_ALGO_CALL_MODE__)

/**
 * @fn     void GH30X_AlgoLog(char *log_string)
 * 
 * @brief  for debug version, log
 *
 * @attention   this function must define that use debug version lib
 *
 * @param[in]   log_string      pointer to log string
 * @param[out]  None
 *
 * @return  None
 */

void GH30X_AlgoLog(GCHAR *log_string)
{
    example_dbg_log(log_string);
}

/**
 * @fn     void GH30X_AdtAlgorithmResultReport(STHbAlgoResult stHbAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate hb algorithm.
 *
 * @attention   None
 *
 * @param[in]   stHbAlgoRes             hb algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH30X_AdtAlgorithmResultReport(STGh30xAlgoResult *pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_ADT_ALGORITHM__)

    //printf("[%s]snResult = %d.\r\n", __func__, pstAlgoResult->snResult[0]);
    if (GH30X_ADT_RESULT_WEAR_OFF == pstAlgoResult->snResult[0])
    {
        Gh30xClearSoftWearEvent(GH30X_SOFT_EVENT_WEAR_ON);
        Gh30xSetSoftWearEvent(GH30X_SOFT_EVENT_WEAR_OFF, 0);
    }
    else
    {
        Gh30xClearSoftWearEvent(GH30X_SOFT_EVENT_WEAR_OFF);
        Gh30xSetSoftWearEvent(GH30X_SOFT_EVENT_WEAR_ON, 0);
    }
#endif
}

/**
 * @fn     void GH30X_HrAlgorithmResultReport(STHbAlgoResult stHbAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate hb algorithm.
 *
 * @attention   None
 *
 * @param[in]   stHbAlgoRes             hb algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH30X_HrAlgorithmResultReport(STGh30xAlgoResult *pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_HR_ALGORITHM__)

    /* code implement by user */
//    GH30X_ALGO_LOG_PARAM("[HR]hba=%d, valid_score=%d, hba_snr=%d, valid_level=%d, hba_acc_info=%d, hba_acc_scence=%d\r\n", \
//                pstAlgoResult->snResult[0], pstAlgoResult->snResult[1], pstAlgoResult->snResult[2], pstAlgoResult->snResult[3], \
//                pstAlgoResult->snResult[4], pstAlgoResult->snResult[5]);
//    GOODIX_PLANFROM_HR_RESULT_REPORT_ENTITY();
#endif

    int hr = pstAlgoResult->snResult[0];
    int ry = pstAlgoResult->snResult[3];
    
    if(hr > 0)
    {
        //过滤5秒内的数据

        u8 FilterCnt = \
            GetHrSensorDataFilterCnt();
        if(FilterCnt == 0)
        {
            SetHrRealVal((u8)hr);
            SetHrVmCtxCache((u8)hr);
        }else
        {
            FilterCnt--;
            SetHrSensorDataFilterCnt(\
                FilterCnt);
        }
    }else
    {
        SetHrRealVal(0);
        SetHrSensorDataFilterCnt(\
            Hr_Data_Filter_Cnt);
    }
}

/**
 * @fn     void GH30X_Spo2AlgorithmResultReport(STGh30xAlgoResult * pstAlgoResult)
 *
 *
 * @brief  This function will be called after calculate spo2 algorithm.
 *
 * @attention   None
 *
 * @param[in]   stSpo2AlgoRes           spo2 algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH30X_Spo2AlgorithmResultReport(STGh30xAlgoResult *pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_SPO2_ALGORITHM__)

    /* code implement by user */
//    GH30X_ALGO_LOG_PARAM("[SPO2]spo2=%d, r_val=%d, confi_coeff=%d, valid_level=%d, hb_mean=%d, invalidFlg=%d, hb_confi_lvl=%d\r\n", \
//                pstAlgoResult->snResult[0], pstAlgoResult->snResult[1], pstAlgoResult->snResult[2], pstAlgoResult->snResult[3], \
//                pstAlgoResult->snResult[4], pstAlgoResult->snResult[5], pstAlgoResult->snResult[6]);
//    GOODIX_PLANFROM_SPO2_RESULT_REPORT_ENTITY();
#endif
    int bo = pstAlgoResult->snResult[0];
    int ry = pstAlgoResult->snResult[3];

    printf("%s:bo = %d, ry = %d\n", __func__, bo, ry);

    if(bo > 0)
    {
        //过滤5秒内的数据

        u8 FilterCnt = \
            GetHrSensorDataFilterCnt();
        if(FilterCnt == 0)
        {
            SetBoRealVal((u8)bo);
            SetBoVmCtxCache((u8)bo);
        }else
        {
            FilterCnt--;
            SetHrSensorDataFilterCnt(\
                FilterCnt);
        }
    }else
    {
        SetBoRealVal(0);
        SetHrSensorDataFilterCnt(\
            Hr_Data_Filter_Cnt);
    }
}

/**
 * @fn     void GH30X_HrvAlgorithmResultReport(STHrvAlgoResult stHrvAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate hrv algorithm.
 *
 * @attention   None
 *
 * @param[in]   stHrvAlgoRes            hrv algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH30X_HrvAlgorithmResultReport(STGh30xAlgoResult *pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_HRV_ALGORITHM__)
    /* code implement by user */
//    GH30X_ALGO_LOG_PARAM("[HRV]rri0=%d, rri1=%d, rri2=%d, rri3=%d, rri_valid=%d, rri_num=%d\r\n", \
//                pstAlgoResult->snResult[0], pstAlgoResult->snResult[1], pstAlgoResult->snResult[2], pstAlgoResult->snResult[3], \
//                pstAlgoResult->snResult[4], pstAlgoResult->snResult[5]);
//    GOODIX_PLANFROM_HRV_RESULT_REPORT_ENTITY();
#endif
}

/**
 * @fn     void GH30X_SoftAdtAlgorithmResultReport(STGh30xAlgoResult * pstAlgoResult, GU32 lubFrameId)
 *
 * @brief  This function will be called after calculate ecg algorithm.
 *
 * @attention   None
 *
 * @param[in]   stEcgAlgoRes            ecg algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH30X_SoftAdtAlgorithmResultReport(STGh30xAlgoResult *pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
    printf("[ResultReport] result0 = %d, result1 = %d.\r\n", pstAlgoResult->snResult[0], pstAlgoResult->snResult[1]);
    if (2 == pstAlgoResult->snResult[0])  //wear off
    {
        if (1 == pstAlgoResult->snResult[1])   //no-living
        {
            printf("[ResultReport] no-living!\r");
            Gh30xSetSoftWearEvent(GH30X_SOFT_EVENT_WEAR_OFF, 1);
        }
        else
        {
            printf("[ResultReport] wear off !\r");
            Gh30xSetSoftWearEvent(GH30X_SOFT_EVENT_WEAR_OFF, 0);
        }
    }
    else if (1 == pstAlgoResult->snResult[0])  //wear on
    {
        printf("[ResultReport] wear on !\r");
        Gh30xSetSoftWearEvent(GH30X_SOFT_EVENT_WEAR_ON, 0);
    }
#endif
}

#endif // end else #if (__DRIVER_LIB_MODE__==__DRV_LIB_WITHOUT_ALGO__)

