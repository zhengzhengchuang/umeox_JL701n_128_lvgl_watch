
#include "app_config.h"
#include "system/includes.h"
#include "update_loader_download.h"

#ifdef CONFIG_256K_FLASH
const int config_update_mode = UPDATE_UART_EN;
#else
const int config_update_mode = UPDATE_BT_LMP_EN | UPDATE_STORAGE_DEV_EN | UPDATE_APP_EN | UPDATE_UART_EN;
#endif

//是否采用双备份升级方案:0-单备份;1-双备份
#if CONFIG_DOUBLE_BANK_ENABLE
const int support_dual_bank_update_en = 1;
#else
const int support_dual_bank_update_en = 0;
#endif

//是否支持外挂flash升级,需要打开Board.h中的TCFG_NOR_FS_ENABLE
const int support_norflash_update_en  = 0;

//支持从外挂flash读取ufw文件升级使能
const int support_norflash_ufw_update_en = 0;

//支持计算推loader数据时的进度
const int support_update_loader_precent = 0;

//支持对btif区域中的蓝牙地址升级前备份，升级后还原的操作
const int support_update_backup_btif_addr = 1;

//支持外挂flash和预留区域一起升级，关闭只升级外挂flash
#if (defined(TCFG_VIRFAT_INSERT_FLASH_ENABLE) && (TCFG_VIRFAT_INSERT_FLASH_ENABLE))
const int support_user_file_update_v2_en = 1;
#else
const int support_user_file_update_v2_en = 0;
#endif

#if OTA_TWS_SAME_TIME_NEW       //使用新的同步升级流程
const int support_ota_tws_same_time_new =  1;
#else
const int support_ota_tws_same_time_new =  0;
#endif

//是否支持升级之后保留vm数据
#if (((defined CONFIG_CPU_BR25) || defined(CONFIG_CPU_BR28)) && SMART_BOX_EN)
const int support_vm_data_keep = 1;
#else
const int support_vm_data_keep = 0;
#endif

const char log_tag_const_v_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  FALSE;
const char log_tag_const_i_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  TRUE;
const char log_tag_const_d_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  FALSE;
const char log_tag_const_w_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  TRUE;
const char log_tag_const_e_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  TRUE;
