/*********************************************************************************************
    *   Filename        : bank_switch.h

    *   Description     : 本文件存放P11切Bank流程接口(API)和数据结构声明

    *   Author          : MoZhiYe

    *   Email           : mozhiye@zh-jieli.com

    *   Last modifiled  : 2021-11-22 10:00

    *   Copyright:(c)JIELI  2021-2029  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef __BANK_SWITCH__
#define __BANK_SWITCH__

//====================================================//
//                   BANK模块管理                     //
//====================================================//
//demo
#define BANK_DEMO_TAG 	"demo"
#define BANK_CODE_DEMO_INIT_BANK_0 			0
#define BANK_CODE_DEMO_NORMAL_BANK_1 		1
#define BANK_CODE_DEMO_NORMAL_BANK_2 		2
#define BANK_CODE_DEMO_BANK_MAX 			3


#define BANK_SENSOR_TAG 	"sensor"
#define BANK_CODE_SENSOR_INIT_BANK_0    	0
#define BANK_CODE_SENSOR_NORMAL_BANK_1 		1
#define BANK_CODE_SENSOR_NORMAL_BANK_2 		2
#define BANK_CODE_SENSOR_BANK_MAX 			3


#define BANK_SYS_TAG 	"sys"
#define BANK_CODE_SYS_INIT_BANK_0        	0
#define BANK_CODE_SYS_NORMAL_BANK_1 		1

extern int overlay_demo_begin[];
extern int overlay_sensor_begin[];

//others


#define _BANK_CODE_SEG(name, index)     __attribute__((section("."name".code.bank."#index)))
#define __BANK_CODE_SEG_DEFINE(name, index)     _BANK_CODE_SEG(name, index)

#define REGISTER_CODE_SEG(index) \
    _BANK_CODE_SEG(BANK_NAME,index)




struct bank_info {
    char *name;
    u8 total_bank;
    u32 run_addr;
};

void bank_code_switch(char *name, u8 bank_index);

#endif /* #ifndef __BANK_SWITCH__ */
