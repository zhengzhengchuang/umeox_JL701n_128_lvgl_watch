/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_algo_call.h
 *
 * @brief   gh30x algo call
 *
 * @version ref gh30x_example_common.h
 *
 */

#ifndef _GH30X_DEMO_ALGO_CALL_H_
#define _GH30X_DEMO_ALGO_CALL_H_

#include "../../../Drv_Code/inc/gh30x_example_common.h"

#define GH30X_ADT_RESULT_WEAR_OFF 0
#define GH30X_ADT_RESULT_WEAR_ON  1

extern GCHAR* GH30X_GetAlgoCallDemoVersion(void);
extern void GH30X_AlgoVersion(GU8 uchFunctionIDOffset);
extern GS8 GH30X_AlgoInit(GU32 unFunctionID);
extern GS8 GH30X_AlgoDeinit(GU32 unFunctionID);
extern GS8 GH30X_AlgoCalculate(GU32 unFunctionID);
extern void GH30X_AlgoCallConfigInit(const STGh30xFrameInfo * const  pSTGh30xFrameInfo[]);
extern void GH30X_InitAlgoConfigParameters(void);
extern void GH30X_AlgoSensorEnable(GU8 uchAlgoGsensorEnable,GU8 uchAlgoCapEnable,GU8 uchAlgoTempEnable);
extern void GH30X_SoftAdtParaInit(void);
extern void GH30X_WriteAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue);

/**
 * @fn     GS8 GH30X_HbAlgorithmOutputTimeConfig(GS32 nOutputTime)
 *
 * @brief  Hb algorithm output time config
 *
 * @attention   None
 *
 * @param[in]   nOutputTime          valid 7~GS32_MAX
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH30X_RET_OK                      return successfully
 * @retval  #GH30X_RET_ALGO_NO_SUPPORTED       return algorithm no supported error
 */
extern GS8 GH30X_HbAlgorithmOutputTimeConfig(GS32 nLatestOutputTime, GS32 nEarliestOutputTime);

/**
 * @fn     GS8 GH30X_HbAlgorithmScenarioConfig(GU8 uchScenario)
 *
 * @brief  Hb algorithm scenario config
 *
 * @attention   None
 *
 * @param[in]  uchScenario     @ref EMHbaScenesConfigType
 *                             others: fixed 0(default) and return GH30X_RET_PARAMETER_ERROR
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH30X_RET_OK                      return successfully
 * @retval  #GH30X_RET_PARAMETER_ERROR         return paramters error
 * @retval  #GH30X_RET_ALGO_NO_SUPPORTED       return algorithm no supported error
 */
extern GS8 GH30X_HbAlgorithmScenarioConfig(GU8 uchScenario);
extern GU8 GH30X_TimestampSyncGetFrameDataFlag(void);
extern void GH30X_TimestampSyncAccInit(void);
extern void GH30X_TimestampSyncPpgInit(GU32 unFunctionID);
extern void GH30X_TimestampSyncSetPpgIntFlag(GU8 uchPpgIntFlag);
extern void GH30X_TimestampSyncFillAccSyncBuffer(GU32 unTimeStamp,
                                           GS16 usAccX,
                                           GS16 usAccY,
                                           GS16 usAccZ);
extern void GH30X_TimestampSyncFillPpgSyncBuffer(GU32 unTimeStamp,
                                           const STGh30xFrameInfo * const pstFrameInfo);

#endif /* _GH30X_DEMO_ALGO_CALL_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2024, Goodix Co., Ltd. ********/
