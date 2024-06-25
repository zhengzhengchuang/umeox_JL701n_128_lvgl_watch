#ifndef __RAM_SHARE_H__
#define __RAM_SHARE_H__
/*********************************************************************************************
    *   Filename        : ram_share.h

    *   Description     : 本文用于配置与主系统共享的ram大小

    *   Author          : MoZhiYe

    *   Email           : mozhiye@zh-jieli.com

    *   Last modifiled  : 2021-10-21 17:00

    *   Copyright:(c)JIELI  2021-2029  @ , All Rights Reserved.
*********************************************************************************************/

//==========================================================//
//                 P2M & M2P MESSAGE_SIZE                   //
//==========================================================//
#define CONFIG_P2M_MESSAGE_SIZE 			0x40
#define CONFIG_M2P_MESSAGE_SIZE 			0x60

//==========================================================//
//                      P11_VAD_CBUF_SIZE                   //
//==========================================================//
#define VAD_POINT_PER_FRAME 				(160)
#define VAD_FRAME_SIZE 						(VAD_POINT_PER_FRAME * 2)
#define VAD_CBUF_FRAME_CNT 					(6)
#define VAD_CBUF_TAG_SIZE 					(0)
#define VAD_CBUF_FRAME_SIZE 				(VAD_FRAME_SIZE +  VAD_CBUF_TAG_SIZE)
#define CONFIG_P11_CBUF_SIZE 				(VAD_CBUF_FRAME_SIZE * VAD_CBUF_FRAME_CNT)

//==========================================================//
//                      P11_VAD_CONFIG_SIZE                 //
//==========================================================//
#define CONFIG_P2M_AVAD_CONFIG_SIZE 		(20 * 4) //sizeof(int)
#define CONFIG_P2M_DVAD_CONFIG_SIZE 		(20 * 4) //sizeof(int)
#define CONFIG_VAD_CONFIG_SIZE 				(CONFIG_P2M_AVAD_CONFIG_SIZE + CONFIG_P2M_DVAD_CONFIG_SIZE)

//==========================================================//
//              P11 RAM SHARE WITH MAIN SYSTEM              //
//NOTE:                                                     //
//1)该区域用于与主系统共享buf, 上电不进行初始化;            //
//2)该宏用于配置最小值, 实际分配大小为 >= 配置值;           //
//3)如果编译结果 < 配置值, 将会报错;            			//
//==========================================================//
#define CONFIG_P11_HEAP_RAM_SIZE_MIN 		200


#endif /* #ifndef __RAM_SHARE_H__ */
