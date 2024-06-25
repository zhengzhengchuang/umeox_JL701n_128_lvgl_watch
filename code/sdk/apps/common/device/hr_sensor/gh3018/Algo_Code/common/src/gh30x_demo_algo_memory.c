/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh30x_demo_algorithm.c
 * 
 * @brief   gh30x driver lib demo code for algorithm
 * 
 * @author  Gooidx Iot Team
 * 
 */
#include "../../common/inc/gh30x_demo_algo_call.h"
#include "../../common/inc/gh30x_demo_algo_memory.h"
#include "../../common/inc/gh30x_demo_algo_config.h"
#include "../../common/inc/gh30x_demo_algorithm_calc.h"
#include "../../call/inc/goodix_algo.h"
#include "../../call/inc/goodix_mem.h"
#include "../../../Drv_Code/inc/gh30x_example_common.h"

#if (__GOODIX_ALGO_CALL_MODE__)
#if __USER_DYNAMIC_ALGO_MEM_EN__
GU32 *g_punGh30xAlgoMemBufferPointer = 0;
#else
GU32 g_punGh30xAlgoMemBuffer[GH30X_ALGORITHMS_MEMORY_SIZE_FINAL / 4] = {0};
GU32 *g_punGh30xAlgoMemBufferPointer = g_punGh30xAlgoMemBuffer;
#endif
#else  //without algorithm
GU32 *g_punGh30xAlgoMemBufferPointer = 0;
#endif

void GH30X_MemFree(void* mem_addr)
{
    goodix_mem_free(mem_addr);
}

void* GH30X_MemMalloc(GS32 nSize)
{
    return goodix_mem_malloc(nSize);
}

void Gh3x2xPoolIsNotEnough(void)
{
    while(1)
    {
        GH30X_ALGO_LOG_PARAM("MEMORY ERROR!!!PLEASE RESET HARDWARE!!!\n");
    }
}

#if (__GOODIX_ALGO_CALL_MODE__)

#if __GH30X_MEM_POOL_CHECK_EN__

const GU32 g_unGh3z2xMemCheckAreaStart[17] =
{
    0,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*1/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*2/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*3/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*4/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*5/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*6/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*7/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*8/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*9/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*10/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*11/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*12/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*13/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*14/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*15/16,
    GH30X_ALGORITHMS_MEMORY_SIZE_FINAL,
};

GU32 g_unGh30xLastMemPoolCheckSumArea[16];
GU8  g_uchGh30xMemPollHaveGetCheckSum = 0;
GU16 g_usGh30xMemPollErrorFlag = 0;

GU32 Gh30xGetMemPoolCheckSum(GU8 *lunMemPool, GU32 lunByteSize)
{
    GU32 lunCheckSum = 0;

    for (GU32 lunByteCnt = 0; lunByteCnt < lunByteSize; lunByteCnt += 4)
    {
        lunCheckSum += *((GU32*)(lunMemPool + lunByteCnt));
    }

    return lunCheckSum;
}

void Gh30xMemPoolCheckSumWrite(GU8 *lunMemPool)
{
    for (GU8 uchAreaCnt = 0; uchAreaCnt < 16; uchAreaCnt++)
    {
        g_unGh30xLastMemPoolCheckSumArea[uchAreaCnt] = Gh30xGetMemPoolCheckSum(lunMemPool + ((g_unGh3z2xMemCheckAreaStart[uchAreaCnt] / 4) * 4), \
            (g_unGh3z2xMemCheckAreaStart[uchAreaCnt + 1] / 4) * 4 - (g_unGh3z2xMemCheckAreaStart[uchAreaCnt] / 4) * 4);
    }
    g_uchGh30xMemPollHaveGetCheckSum = 1;
}

GU16 Gh30xMemPoolCheckSumCheck(GU8 *lunMemPool)
{
    GU16 usErrorFlag = 0;

    if (0 == g_usGh30xMemPollErrorFlag)
    {
        for (GU8 uchAreaCnt = 0; uchAreaCnt < 16; uchAreaCnt++)
        {
            if (g_unGh30xLastMemPoolCheckSumArea[uchAreaCnt] != Gh30xGetMemPoolCheckSum(lunMemPool + ((g_unGh3z2xMemCheckAreaStart[uchAreaCnt] / 4) * 4), \
                (g_unGh3z2xMemCheckAreaStart[uchAreaCnt + 1] / 4) * 4 - (g_unGh3z2xMemCheckAreaStart[uchAreaCnt] / 4) * 4))
            {
                usErrorFlag |= (((GU16)1) << uchAreaCnt);
            }
        }
    }

    g_usGh30xMemPollErrorFlag = usErrorFlag;
    return usErrorFlag;
}

void Gh30xCheckMemPollBeforeAlgoCal(void)
{
    if (g_uchGh30xMemPollHaveGetCheckSum)
    {
        if (Gh30xMemPoolCheckSumCheck((GU8*)g_punGh30xAlgoMemBufferPointer))
        {
            GH30X_ALGO_LOG_PARAM("Gh30x mem check error!!!  error flag = 0x%X\r\n", g_usGh30xMemPollErrorFlag);
            while(1);  //error !!!  mem pool is be destroy
        }
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 0  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[0]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 1  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[1]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 2  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[2]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 3  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[3]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 4  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[4]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 5  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[5]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 6  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[6]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 7  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[7]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 8  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[8]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 9  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[9]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 10  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[10]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 11  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[11]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 12  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[12]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 13  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[13]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 14  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[14]);
        GH30X_ALGO_LOG_PARAM("Gh30x mem check ok, check sum 15  = 0x%X\r\n",g_unGh30xLastMemPoolCheckSumArea[15]);
    }
}

void Gh30xUpdataMemPollChkSumAfterAlgoCal(void)
{
    Gh30xMemPoolCheckSumWrite((GU8*)g_punGh30xAlgoMemBufferPointer);

    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*1/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*2/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*3/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*4/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*5/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*6/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*7/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*8/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*9/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*10/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*11/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*12/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*13/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*14/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*15/16)/4 - 1] = 0x3333;   //test
    //g_punGh30xAlgoMemBuffer[(GH30X_ALGORITHMS_MEMORY_SIZE_FINAL*16/16)/4 - 1] = 0x3333;   //test
            
}
#endif

#if __USER_DYNAMIC_ALGO_MEM_EN__
void* Gh30xAlgoMemApply(GU32 unMemSize)
{
    return hal_gh30x_memory_malloc(unMemSize);
}

void Gh30xAlgoMemFree(void* MemAddr)
{
    hal_gh30x_memory_free(MemAddr);
}
#else
void* Gh30xAlgoMemApply(GU32 unMemSize)
{
    return &g_punGh30xAlgoMemBuffer[0];
}

void Gh30xAlgoMemFree(void* MemAddr)
{

}
#endif

/// algorithm memory is inited or not
GU8 g_uchAlgoMemStatus = ALGO_MEM_NO_INIT;

/// buffer for algorithm, this addr is import from outer 
//void* g_unAlgorithmMemBuffer = 0;

/// buffer size(byte) for algorithm, this size is import from outer
GU32 g_unAlgorithmMemBufferSize = 0;

/**
 * @fn     GS8 GH30X_AlgoMemConfig( GU32 unMemSize)
 *
 * @brief  Config algorithm memory.
 *
 * @attention   Mem can only be static,such as global array
 *
 * @param[in]  unMemSize       memory size(unit:byte)
 *
 * @return  error code
 * @retval  #GH30X_RET_OK                      return successfully
 * @retval  #GH30X_RET_RESOURCE_ERROR          return algorithm mem config error
 */
GS8 GH30X_AlgoMemConfig( GU32 unMemSize)
{
    GS8 chRet = GH30X_RET_OK;
    if (unMemSize > 0)
    {
        GH30X_AlgoDeinit(0xFFFFFFFF);
        g_unAlgorithmMemBufferSize = unMemSize;
        chRet = GH30X_RET_OK;
        GH30X_ALGO_LOG_PARAM("Algorithm mem Config complete!Size:%d\r\n", (int)unMemSize);
        GH30X_ALGO_LOG_PARAM("Algorithm mem init start...\r\n");
        chRet = GH30X_AlgoMemInit();
    }
    else
    {
        GH30X_ALGO_LOG_PARAM("Algorithm mem config fail!\r\n");
        chRet = GH30X_RET_RESOURCE_ERROR;
    }
    return chRet;
}

/**
 * @fn     GS8 GH30X_AlgoMemInit(void)
 *
 * @brief  algorithm memory init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH30X_RET_OK                      return successfully
 * @retval  #GH30X_RET_RESOURCE_ERROR          return algorithm mem init error
 */
GS8 GH30X_AlgoMemInit(void)
{
    GS8 chRet = GH30X_RET_OK;

    GH30X_ALGO_LOG_PARAM("Algorithm mem init start...\r\n");
    if (ALGO_MEM_NO_INIT == g_uchAlgoMemStatus)
    {
        g_punGh30xAlgoMemBufferPointer = (GU32*)Gh30xAlgoMemApply(g_unAlgorithmMemBufferSize);
        if ((g_punGh30xAlgoMemBufferPointer != 0) && (g_unAlgorithmMemBufferSize > 0))
        {
            if (MEMPOOL_IS_OK_E != goodix_mem_init((void*)g_punGh30xAlgoMemBufferPointer, g_unAlgorithmMemBufferSize))
            {
                chRet = GH30X_RET_RESOURCE_ERROR;
                GH30X_ALGO_LOG_PARAM("Algorithm mem init error!\r\n");
            }
            else
            {
                g_uchAlgoMemStatus = ALGO_MEM_INIT_OK;
                chRet = GH30X_RET_OK;
                GH30X_ALGO_LOG_PARAM("Algorithm mem init success.Addr:0x%x,Size:%d\r\n", (unsigned int)g_punGh30xAlgoMemBufferPointer, \
                                                                                        (int)g_unAlgorithmMemBufferSize);
            }
        }
        else
        {
            GH30X_ALGO_LOG_PARAM("Algorithm mem para error!\r\n");
            chRet = GH30X_RET_RESOURCE_ERROR;
        }
    }
    return chRet;
}

/**
* @fn     void GH30X_AlgoMemDeinit(void)
*
* @brief  algorithm memory deinit
*
* @attention   None
*
* @param[in]   None
* @param[out]  None
*
* @return  None
*/
void GH30X_AlgoMemDeinit(void)
{
    if (ALGO_MEM_INIT_OK == g_uchAlgoMemStatus)
    {
        goodix_mem_deinit();  //destroy all memory
        if (g_punGh30xAlgoMemBufferPointer)
        {
            Gh30xAlgoMemFree((void*)g_punGh30xAlgoMemBufferPointer);
            g_punGh30xAlgoMemBufferPointer = 0;
        }
        g_uchAlgoMemStatus = ALGO_MEM_NO_INIT;
        GH30X_ALGO_LOG_PARAM("Algorithm mem deinit!\r\n");
    }    
}

GU8 GH30X_GetAlgoMemStatus(void)
{
    return g_uchAlgoMemStatus;
}

GU8 Gh30xGetHrAlgoSupportChnl(void)
{
    return __HR_ALGORITHM_SUPPORT_CHNL_NUM__;
}

GU8 Gh30xGetSpo2AlgoSupportChnl(void)
{
    return __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__;
}

#endif

