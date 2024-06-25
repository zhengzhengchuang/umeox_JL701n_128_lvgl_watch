/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_algo_hook.h
 *
 * @brief   interface statement for gh30x algo
 *
 */

#ifndef _GH30X_DEMO_ALGO_HOOK_H_
#define _GH30X_DEMO_ALGO_HOOK_H_

#include <stdio.h>
#include "gh30x_demo_algo_config.h"

#ifdef GOODIX_DEMO_PLANFORM
#include "gh30x_demo_goodix_platform.h"
#endif

#if (__GOODIX_ALGO_CALL_MODE__)

/**
 * @fn     void GH30X_AdtAlgorithmResultReport(STHbAlgoResult stHbAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate adt algorithm.
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
void GH30X_AdtAlgorithmResultReport(STGh30xAlgoResult * pstAlgoResult, GU32 lubFrameId);

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
void GH30X_HrAlgorithmResultReport(STGh30xAlgoResult * pstAlgoResult, GU32 lubFrameId);

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
void GH30X_Spo2AlgorithmResultReport(STGh30xAlgoResult * pstAlgoResult, GU32 lubFrameId);

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
void GH30X_HrvAlgorithmResultReport(STGh30xAlgoResult * pstAlgoResult, GU32 lubFrameId);

/**
 * @fn     void GH30X_EcgAlgorithmResultReport(STEcgAlgoResult stEcgAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
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
void GH30X_EcgAlgorithmResultReport(STGh30xAlgoResult * pstAlgoResult, GU32 lubFrameId);

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
void GH30X_SoftAdtAlgorithmResultReport(STGh30xAlgoResult * pstAlgoResult, GU32 lubFrameId);

/**
 * @fn     void GH30X_SoftAdtIrAlgorithmResultReport(STGh30xAlgoResult * pstAlgoResult, GU32 lubFrameId)
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
void GH30X_SoftAdtIrAlgorithmResultReport(STGh30xAlgoResult * pstAlgoResult, GU32 lubFrameId);

#endif /* __GOODIX_ALGO_CALL_MODE__ */

#endif /* _GH30X_DEMO_ALGO_MEMORY_H_ */

