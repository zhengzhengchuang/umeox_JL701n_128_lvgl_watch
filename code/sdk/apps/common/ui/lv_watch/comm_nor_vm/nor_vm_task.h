#ifndef __NOR_VM_TASK_H__
#define __NOR_VM_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Nor_Vm_Task_Name "nor_vm_task"

enum
{
    nor_vm_weather,
    nor_vm_message,
    nor_vm_call_log,
    nor_vm_contacts,
};

void nor_vm_task_create(void);
int post_nor_vm_task_msg(int *post_msg, u8 len);
void nor_vm_task_msg_handle(int *rev_msg, u8 len);

void nor_vm_task_post(int type, int *para, int len);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
