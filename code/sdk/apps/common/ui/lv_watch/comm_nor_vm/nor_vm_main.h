#ifndef __NOR_VM_MAIN_H__
#define __NOR_VM_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nor_vm_cfg.h"
#include "nor_vm_hr.h"
#include "nor_vm_bo.h"
#include "nor_vm_sleep.h"
#include "nor_vm_message.h"
#include "nor_vm_weather.h"
#include "nor_vm_activity.h"
#include "nor_vm_call_log.h"
#include "nor_vm_contacts.h"
#include "../include/ui_conf.h"
#include "../include/ui_menu.h"
#include "../poc_modem/poc_modem_vm.h"

void nor_flash_vm_init(void);
void nor_flash_vm_clear(void);
void *nor_flash_vm_file(nor_vm_type_t nor_vm_type);
uint8_t nor_flash_vm_num_max(nor_vm_type_t nor_vm_type);
#ifdef __cplusplus
}
#endif

#endif
