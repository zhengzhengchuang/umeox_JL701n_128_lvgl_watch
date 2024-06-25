/*********************************************************************************************
    *   Filename        : btcontroller_modules.h

    *   Description     : Lto 优化Macro 定义

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2018-12-19 16:38

    *   Copyright:(c)JIELI  2011-2017  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _BTCONTROLLER_H_
#define _BTCONTROLLER_H_

#include "hci_transport.h"
#include "btcontroller_mode.h"

/*
 *-------------------Module SUPPORT
 *  brief : 运行时优化（LTO）下，代码空间优化；
 */
#define BT_MODULE_CLASSIC                   BIT(0)
#define BT_MODULE_LE                        BIT(1)

extern const int config_btctler_modules;
#define BT_MODULES_IS_SUPPORT(x)            (config_btctler_modules & (x))
/*-----------------------------------------------------------*/
extern const int config_stack_modules;
#define STACK_MODULES_IS_SUPPORT(x)         (config_stack_modules & (x))

/*
 *-------------------Mode SELECT
 *  brief : 运行时优化（LTO）下，代码空间优化；
 */
extern const int config_btctler_mode;
#define BT_MODE_IS(x)            (config_btctler_mode & (x))

/*-----------------------------------------------------------*/

extern const int config_btctler_hci_standard;
#define BT_HCI_STANDARD_IS_SUPPORT(x)        (config_btctler_hci_standard)


/********************************************************************************/
/*
 *                   API
 *
 */
void hci_controller_init(void);

int le_controller_set_mac(void *addr);

void bt_reset_cap(u8 sel_l, u8 sel_r);

void bt_set_tx_power(u8 txpower);

extern const int CONFIG_TEST_DUT_CODE;
extern const int CONFIG_TEST_FCC_CODE;
extern const int CONFIG_TEST_DUT_ONLY_BOX_CODE;
extern const int CONFIG_BREDR_INQUIRY;

extern const int CONFIG_INQUIRY_PAGE_OFFSET_ADJUST ;

extern const int CONFIG_LMP_NAME_REQ_ENABLE ;
extern const int CONFIG_LMP_PASSKEY_ENABLE ;
extern const int CONFIG_LMP_MASTER_ESCO_ENABLE ;
extern const int config_btctler_bredr_master ;
extern const int config_bredr_afh_user ;
extern const int config_bredr_master_afh ;
extern const int CONFIG_ESCO_MUX_RX_BULK_ENABLE ;
extern const int config_bt_temperature_pll_trim ;
extern const int CONFIG_WIFI_DETECT_ENABLE;
extern const int ESCO_FORWARD_ENABLE;


#endif
