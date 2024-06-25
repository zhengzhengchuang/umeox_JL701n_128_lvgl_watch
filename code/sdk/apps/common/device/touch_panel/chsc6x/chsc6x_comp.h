#ifndef __CHSC6X_COMP_H__
#define __CHSC6X_COMP_H__

#include "app_config.h"

#if TCFG_TOUCH_PANEL_ENABLE

#if TCFG_TP_CHSC6X_ENABLE

struct ts_fw_infos {
    unsigned short chsc6x_cfg_version;  //customer read 
    unsigned short chsc6x_boot_version; //customer read 
    unsigned short chsc6x_vendor_id;    //customer read 
    unsigned short chsc6x_project_id;   //customer read 
    unsigned short chsc6x_chip_id;      //customer read 
    unsigned short chsc6x_chip_type;    //customer read 
    unsigned short chsc6x_rpt_lcd_x;    //customer read must after chsc6x_get_chip_info
    unsigned short chsc6x_rpt_lcd_y;    //customer read must after chsc6x_get_chip_info
    unsigned short chsc6x_max_pt_num;   //customer read must after chsc6x_get_chip_info
};


/* FUNC In your systerm init process,Must call this interface function to detec if the TP IC is Chipsemi corp'. 
/* PARM pfw_infos: to get top 5 fw info in struct ts_fw_infos.
/* PARM update_ret_flag: point value=1 update succeed; point value=0 update failed, If opend CHSC6X_AUTO_UPGRADE macro.
/* RETURN 1:is chsc chip, 0:is not chsc chip
*/
extern int chsc6x_tp_dect(struct ts_fw_infos *pfw_infos, unsigned char *update_ret_flag);


/* FUNC You can call this interfacce function to realize upgrade TP Firmware by OTA. 
/* PARM pfw_infos: to get top 6 fw infos in struct ts_fw_infos, after ota upgrade.
/* PARM p_fw_upd: array address of the upgrade firmware array 
/* PARM fw_len: total size of the upgrade firmware array 
/* RETURN NULL
*/
extern void chsc6x_ota_upgrade_tp_fw(struct ts_fw_infos *pfw_infos, unsigned char* p_fw_upd, unsigned int fw_len);


/* FUNC: get fw info in struct ts_fw_infos you can call this func anytime.
/* PARM pfw_infos: can get all fw infos in struct ts_fw_infos, after call this interface.
/* RETURN NULL
*/
extern void chsc6x_get_chip_info(struct ts_fw_infos *infos);

#endif

#endif

#endif
